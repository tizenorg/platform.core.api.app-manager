/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <glib.h>

#include <aul.h>
#include <dlog.h>
#include <cynara-client.h>

#include "package-manager.h"

#include "app_manager.h"
#include "app_manager_internal.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "CAPI_APPFW_APP_MANAGER"

#define SMACK_LABEL_LEN 255

typedef struct _app_event_info {
	int req_id;
	app_event_type_e event_type;
	app_event_state_e event_state;
	struct _app_event_info *next;
} app_event_info;

struct _app_manager_event_s {
	int req_id;
	pkgmgr_client *pc;
	app_manager_app_enable_cb event_cb;
	void *user_data;
	app_event_info *head;
};

static GHashTable *__app_cb_table = NULL;

static void __remove_app_event_info(app_event_info *head)
{
	if (head == NULL)
		return;

	app_event_info *current = head;

	if (current->next != NULL)
		__remove_app_event_info(current->next);

	free(current);
	return;
}

static void __free_app_manager_event(gpointer data)
{
	struct _app_manager_event_s *app_event = (struct _app_manager_event_s *)data;
	if (app_event == NULL) {
		LOGE("failed to get app_event");
		return;
	}

	pkgmgr_client_free(app_event->pc);
	__remove_app_event_info(app_event->head);

	free(app_event);
	app_event = NULL;
}

static void __initialize_app_cb_table(void)
{
	__app_cb_table = g_hash_table_new_full(g_int_hash, g_int_equal, free, __free_app_manager_event);
}

static int __get_request_id(void)
{
	static int internal_req_id = 1;

	return internal_req_id++;
}

static int __add_app_cb_table(struct _app_manager_event_s *app_event)
{
	if (__app_cb_table == NULL)
		__initialize_app_cb_table();

	if (g_hash_table_contains(__app_cb_table, &app_event->req_id)) {
		LOGE("required key[%d] already exists", app_event->req_id);
		return APP_MANAGER_ERROR_REQUEST_FAILED;
	}

	int *key = malloc(sizeof(int));
	*key = app_event->req_id;

	g_hash_table_insert(__app_cb_table, key, app_event);

	return APP_MANAGER_ERROR_NONE;
}

static int __remove_app_cb_table(const int key)
{
	if (!g_hash_table_remove(__app_cb_table, &key))
		return APP_MANAGER_ERROR_REQUEST_FAILED;

	if (g_hash_table_size(__app_cb_table) == 0) {
		g_hash_table_destroy(__app_cb_table);
		__app_cb_table = NULL;
	}

	return APP_MANAGER_ERROR_NONE;
}

static int __add_app_event_info(app_event_info **head, int req_id,
				app_event_type_e event_type,
				app_event_state_e event_state)
{
	app_event_info *event_info;
	app_event_info *current;
	app_event_info *prev;

	event_info = (app_event_info *)calloc(1, sizeof(app_event_info));
	if (event_info == NULL) {
		LOGE("calloc failed");
		return APP_MANAGER_ERROR_OUT_OF_MEMORY;
	}

	event_info->req_id = req_id;
	event_info->event_type = event_type;
	event_info->event_state = event_state;
	event_info->next = NULL;

	if (*head == NULL)
		*head = event_info;
	else {
		current = prev = *head;
		while(current) {
			prev = current;
			current = current->next;
		}
		prev->next = event_info;
	}

	return APP_MANAGER_ERROR_NONE;
}

static int __find_app_event_info(app_event_info **head, int req_id,
				app_event_type_e *event_type,
				app_event_state_e *event_state)
{
	app_event_info *tmp;

	tmp = *head;

	if (tmp == NULL) {
		LOGE("head is null");
		return APP_MANAGER_ERROR_REQUEST_FAILED;
	}

	while (tmp) {
		if (tmp->req_id == req_id) {
			*event_type = tmp->event_type;
			return APP_MANAGER_ERROR_NONE;
		}
		tmp = tmp->next;
	}
	return APP_MANAGER_ERROR_REQUEST_FAILED;
}

static int _convert_app_status_type(int app_status_type)
{
	int result = 0;

	if (app_status_type == 0)
		return PKGMGR_CLIENT_STATUS_ALL;

	if ((app_status_type & APP_MANAGER_APP_STATUS_TYPE_ENABLE)
			== APP_MANAGER_APP_STATUS_TYPE_ENABLE)
		result += PKGMGR_CLIENT_STATUS_ENABLE_APP;

	if ((app_status_type & APP_MANAGER_APP_STATUS_TYPE_DISABLE)
			== APP_MANAGER_APP_STATUS_TYPE_DISABLE)
		result += PKGMGR_CLIENT_STATUS_DISABLE_APP;

	return result;
}

static int __app_manager_get_event_type(const char *key, app_event_type_e *event_type)
{
	if (key == NULL)
		return APP_MANAGER_ERROR_INVALID_PARAMETER;

	if (strcasecmp(key, "disable_app") == 0 ||
			strcasecmp(key, "disable_global_app_for_uid") == 0)
		*event_type = APP_MANAGER_EVENT_DISABLE_APP;
	else if (strcasecmp(key, "enable_app") == 0 ||
			strcasecmp(key, "enable_global_app_for_uid") == 0)
		*event_type = APP_MANAGER_EVENT_ENABLE_APP;
	else
		return APP_MANAGER_ERROR_INVALID_PARAMETER;

	return APP_MANAGER_ERROR_NONE;
}

static int _app_event_handler(uid_t target_uid, int req_id, const char *pkg_type,
				const char *pkgid, const char *appid, const char *key,
				const char *val, const void *pmsg, void *data)
{
	struct _app_manager_event_s *app_event = (struct _app_manager_event_s *)data;
	app_event_type_e event_type = -1;
	app_event_state_e event_state = -1;
	int ret = -1;

	if (strcasecmp(key, "start") == 0) {
		ret = __app_manager_get_event_type(val, &event_type);
		if (ret != APP_MANAGER_ERROR_NONE)
			return APP_MANAGER_ERROR_INVALID_PARAMETER;

		ret = __add_app_event_info(&(app_event->head), req_id, event_type,
				APP_MANAGER_EVENT_STATE_STARTED);
		if (ret != APP_MANAGER_ERROR_NONE)
			return APP_MANAGER_ERROR_OUT_OF_MEMORY;

		if (app_event->event_cb)
			app_event->event_cb(appid, event_type,
					APP_MANAGER_EVENT_STATE_STARTED,
					app_event->user_data);

	} else if (strcasecmp(key, "end") == 0) {
		if (__find_app_event_info(&(app_event->head), req_id, &event_type,
				&event_state) == 0) {
			if (strcasecmp(val, "ok") == 0) {
				if (app_event->event_cb)
					app_event->event_cb(appid, event_type,
							APP_MANAGER_EVENT_STATE_COMPLETED,
							app_event->user_data);

			} else if (strcasecmp(val, "fail") == 0) {
				if (app_event->event_cb)
					app_event->event_cb(appid, event_type,
							APP_MANAGER_EVENT_STATE_FAIL,
							app_event->user_data);
			} else
				return APP_MANAGER_ERROR_INVALID_PARAMETER;
		}
	} else
		return APP_MANAGER_ERROR_INVALID_PARAMETER;

	return APP_MANAGER_ERROR_NONE;
}

static const char* app_manager_error_to_string(app_manager_error_e error)
{
	switch (error) {
	case APP_MANAGER_ERROR_NONE:
		return "Successful";
	case APP_MANAGER_ERROR_INVALID_PARAMETER:
		return "Invalid parameter";
	case APP_MANAGER_ERROR_OUT_OF_MEMORY:
		return "Out of memory";
	case APP_MANAGER_ERROR_IO_ERROR:
		return "IO error";
	case APP_MANAGER_ERROR_NO_SUCH_APP:
		return "No such application";
	case APP_MANAGER_ERROR_DB_FAILED:
		return "DB error";
	case APP_MANAGER_ERROR_INVALID_PACKAGE:
		return "Invalid package";
	case APP_MANAGER_ERROR_NOT_SUPPORTED:
		return "Not supported";
	case APP_MANAGER_ERROR_PERMISSION_DENIED:
		return "Permission denied";
	default:
		return "Unknown";
	}
}

int app_manager_error(app_manager_error_e error, const char* function, const char *description)
{
	if (description)
		LOGE("[%s] %s(0x%08x) : %s", function, app_manager_error_to_string(error), error, description);
	else
		LOGE("[%s] %s(0x%08x)", function, app_manager_error_to_string(error), error);

	return error;
}

int app_manager_check_privilege(char *privilege)
{
	cynara *p_cynara;
	int fd;
	int ret;

	char client[SMACK_LABEL_LEN + 1] = "";
	char uid[10] = {0,};
	char *client_session = "";

	if (privilege == NULL) {
		LOGE("invalid parameter");
		return APP_MANAGER_ERROR_INVALID_PARAMETER;
	}

	ret = cynara_initialize(&p_cynara, NULL);
	if (ret != CYNARA_API_SUCCESS) {
		LOGE("cynara_initialize [%d] failed!", ret);
		return APP_MANAGER_ERROR_IO_ERROR;
	}

	fd = open("/proc/self/attr/current", O_RDONLY);
	if (fd < 0) {
		LOGE("open [%d] failed!", errno);
		ret = APP_MANAGER_ERROR_IO_ERROR;
		goto out;
	}

	ret = read(fd, client, SMACK_LABEL_LEN);
	if (ret < 0) {
		LOGE("read [%d] failed!", errno);
		close(fd);
		ret = APP_MANAGER_ERROR_IO_ERROR;
		goto out;
	}

	close(fd);

	snprintf(uid, 10, "%d", getuid());

	ret = cynara_check(p_cynara, client, client_session, uid, privilege);
	if (ret != CYNARA_API_ACCESS_ALLOWED) {
		LOGE("cynara access check [%d] failed!", ret);

		if (ret == CYNARA_API_ACCESS_DENIED)
			ret = APP_MANAGER_ERROR_PERMISSION_DENIED;
		else
			ret = APP_MANAGER_ERROR_IO_ERROR;

		goto out;
	}

	ret = APP_MANAGER_ERROR_NONE;

out:
	if (p_cynara)
		cynara_finish(p_cynara);

	return ret;
}
#if 1
API int app_manager_set_app_enable_cb(int status_type, app_manager_app_enable_cb callback, void *user_data, app_manager_event_h *manager)
{
	int ret = APP_MANAGER_ERROR_NONE;
	pkgmgr_client *pc = NULL;
	int app_status_type = 0;
	struct _app_manager_event_s *app_event_s = NULL;

	if (callback == NULL)
		return APP_MANAGER_ERROR_INVALID_PARAMETER;

	ret = app_manager_check_privilege(PRIVILEGE_PKGMGR_INFO);
	if (ret != APP_MANAGER_ERROR_NONE) {
		if (ret == APP_MANAGER_ERROR_PERMISSION_DENIED)
			return app_manager_error(APP_MANAGER_ERROR_PERMISSION_DENIED, __FUNCTION__, NULL);
		else
			return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}

	app_event_s = (struct _app_manager_event_s *)calloc(1, sizeof(struct _app_manager_event_s));
	if (app_event_s == NULL)
		return APP_MANAGER_ERROR_OUT_OF_MEMORY;

	pc = pkgmgr_client_new(PC_LISTENING);
	if (pc == NULL) {
		ret = APP_MANAGER_ERROR_OUT_OF_MEMORY;
		goto catch;
	}

	app_status_type = _convert_app_status_type(status_type);
	ret = pkgmgr_client_set_status_type(pc, app_status_type);
	if (ret != PKGMGR_R_OK) {
		if (ret == PKGMGR_R_EINVAL)
			ret = APP_MANAGER_ERROR_INVALID_PARAMETER;
		else
			ret = APP_MANAGER_ERROR_IO_ERROR;
		goto catch;
	}

	app_event_s->req_id = __get_request_id();
	app_event_s->pc = pc;
	app_event_s->user_data = user_data;
	app_event_s->event_cb = callback;
	ret = pkgmgr_client_listen_app_status(pc, _app_event_handler, app_event_s);
	ret = __add_app_cb_table(app_event_s);
	if (ret != APP_MANAGER_ERROR_NONE) {
		LOGE("failed to add callback info table");
		goto catch;
	}

	*manager = app_event_s;
	return APP_MANAGER_ERROR_NONE;

catch:
	pkgmgr_client_free(pc);
	if (app_event_s) {
		free(app_event_s);
		app_event_s = NULL;
	}

	*manager = NULL;
	return ret;
}

API int app_manager_unset_app_enable_cb(app_manager_event_h manager)
{
	int ret = APP_MANAGER_ERROR_NONE;

	if (manager == NULL || manager->pc == NULL)
		return APP_MANAGER_ERROR_INVALID_PARAMETER;

	ret = app_manager_check_privilege(PRIVILEGE_PKGMGR_INFO);
	if (ret != APP_MANAGER_ERROR_NONE) {
		if (ret == APP_MANAGER_ERROR_PERMISSION_DENIED)
			return app_manager_error(APP_MANAGER_ERROR_PERMISSION_DENIED, __FUNCTION__, NULL);
		else
			return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}


	return __remove_app_cb_table(manager->req_id);
}
#else
API int app_manager_set_app_status_cb(app_manager_event_h *manager, app_manager_app_status_cb callback, int status_type, void *user_data)
{
	int ret = APP_MANAGER_ERROR_NONE;
	pkgmgr_client *pc = NULL;
	int app_status_type = 0;
	struct _app_manager_event_s *app_event_s = NULL;

	if (callback == NULL)
		return APP_MANAGER_ERROR_INVALID_PARAMETER;

	ret = app_manager_check_privilege(PRIVILEGE_PKGMGR_INFO);
	if (ret != APP_MANAGER_ERROR_NONE) {
		if (ret == APP_MANAGER_ERROR_PERMISSION_DENIED)
			return app_manager_error(APP_MANAGER_ERROR_PERMISSION_DENIED, __FUNCTION__, NULL);
		else
			return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}

	app_event_s = (_app_manager_event_s *)calloc(1, sizeof(struct _app_manager_event_s));
	if (app_event_s == NULL)
		return APP_MANAGER_ERROR_OUT_OF_MEMORY;

	pc = pkgmgr_client_new(PC_LISTENING);
	if (pc == NULL) {
		ret = APP_MANAGER_ERROR_OUT_OF_MEMORY;
		goto catch;
	}

	app_status_type = _convert_app_status_type(status_type);
	ret = pkgmgr_client_set_status_type(pc, app_status_type);
	if (ret != PKGMGR_R_OK) {
		if (ret == PKGMGR_R_EINVAL)
			ret = APP_MANAGER_ERROR_INVALID_PARAMETER;
		else
			ret = APP_MANAGER_ERROR_IO_ERROR;
		goto catch;
	}

	app_event_s->req_id = __get_request_id();
	app_event_s->pc = pc;
	app_event_s->user_data = user_data;
	app_event_s->event_cb = callback;
	ret = pkgmgr_client_listen_app_status(pc, _app_event_handler, app_event_s);
	ret = __add_app_cb_table(app_event_s);
	if (ret != APP_MANAGER_ERROR_NONE) {
		LOGE("failed to add callback info table");
		goto catch;
	}

	*manager = app_event_s;
	return APP_MANAGER_ERROR_NONE;

catch:
	pkgmgr_client_free(pc);
	if (app_event_s) {
		free(app_event_s);
		app_event_s = NULL;
	}

	*manager = NULL;
	return ret;
}

API int app_manager_unset_app_status_cb(app_manager_event_h manager)
{
	int ret = APP_MANAGER_ERROR_NONE;

	if (manager == NULL || manager->pc == NULL)
		return APP_MANAGER_ERROR_INVALID_PARAMETER;

	ret = app_manager_check_privilege(PRIVILEGE_PKGMGR_INFO);
	if (ret != APP_MANAGER_ERROR_NONE) {
		if (ret == APP_MANAGER_ERROR_PERMISSION_DENIED)
			return app_manager_error(APP_MANAGER_ERROR_PERMISSION_DENIED, __FUNCTION__, NULL);
		else
			return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}


	return __remove_app_cb_table(manager->req_id);
}
#endif
API int app_manager_set_app_context_event_cb(app_manager_app_context_event_cb callback, void *user_data)
{
	int retval = app_context_set_event_cb(callback, user_data);

	if (retval != APP_MANAGER_ERROR_NONE)
		return app_manager_error(retval, __FUNCTION__, NULL);
	else
		return APP_MANAGER_ERROR_NONE;
}

API void app_manager_unset_app_context_event_cb(void)
{
	app_context_unset_event_cb();
}

API int app_manager_foreach_app_context(app_manager_app_context_cb callback, void *user_data)
{
	int retval = app_context_foreach_app_context(callback, user_data);

	if (retval != APP_MANAGER_ERROR_NONE)
		return app_manager_error(retval, __FUNCTION__, NULL);
	else
		return APP_MANAGER_ERROR_NONE;
}

API int app_manager_foreach_running_app_context(app_manager_app_context_cb callback, void *user_data)
{
	int retval = app_context_foreach_running_app_context(callback, user_data);

	if (retval != APP_MANAGER_ERROR_NONE)
		return app_manager_error(retval, __FUNCTION__, NULL);
	else
		return APP_MANAGER_ERROR_NONE;
}

API int app_manager_get_app_context(const char *app_id, app_context_h *app_context)
{
	int retval = app_context_get_app_context(app_id, app_context);

	if (retval != APP_MANAGER_ERROR_NONE)
		return app_manager_error(retval, __FUNCTION__, NULL);
	else
		return APP_MANAGER_ERROR_NONE;
}

API int app_manager_resume_app(app_context_h app_context)
{
	char *app_id;
	int retval = APP_MANAGER_ERROR_NONE;

	if (app_context == NULL)
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);

	if (app_context_get_app_id(app_context, &app_id) != APP_MANAGER_ERROR_NONE)
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, "failed to get the application ID");

	if (aul_app_is_running(app_id) == 0) {
		if (app_id) {
			free(app_id);
			app_id = NULL;
		}
		return app_manager_error(APP_MANAGER_ERROR_APP_NO_RUNNING, __FUNCTION__, NULL);
	}

	retval = aul_resume_app(app_id);

	if (app_id)
		free(app_id);

	if (retval == AUL_R_EINVAL)
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	else if (retval == AUL_R_EILLACC)
		return app_manager_error(APP_MANAGER_ERROR_PERMISSION_DENIED, __FUNCTION__, NULL);
	else if (retval < 0)
		return app_manager_error(APP_MANAGER_ERROR_REQUEST_FAILED, __FUNCTION__, NULL);

	return APP_MANAGER_ERROR_NONE;
}

API int app_manager_foreach_app_info(app_manager_app_info_cb callback, void *user_data)
{
	int retval;

	retval = app_info_foreach_app_info(callback, user_data);

	if (retval != APP_MANAGER_ERROR_NONE)
		return app_manager_error(retval, __FUNCTION__, NULL);
	else
		return APP_MANAGER_ERROR_NONE;
}

API int app_manager_get_app_info(const char *app_id, app_info_h *app_info)
{
	int retval;

	retval = app_info_create(app_id, app_info);

	if (retval != APP_MANAGER_ERROR_NONE)
		return app_manager_error(retval, __FUNCTION__, NULL);
	else
		return APP_MANAGER_ERROR_NONE;
}

API int app_manager_get_app_id(pid_t pid, char **app_id)
{
	char buffer[256] = {0, };
	char *app_id_dup = NULL;

	if (app_id == NULL)
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);

	if (aul_app_get_appid_bypid(pid, buffer, sizeof(buffer)) != AUL_R_OK)
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, "Invalid process ID");

	app_id_dup = strdup(buffer);
	if (app_id_dup == NULL)
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);

	*app_id = app_id_dup;

	return APP_MANAGER_ERROR_NONE;
}

API int app_manager_terminate_app(app_context_h app_context)
{
	int retval;
	pid_t pid = 0;

	if (app_context == NULL)
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);

	if (app_context_get_pid(app_context, &pid) != APP_MANAGER_ERROR_NONE)
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, "failed to get the process ID");

	retval = aul_terminate_pid(pid);
	if (retval == AUL_R_EINVAL) {
		LOGE("[%s] APP_MANAGER_ERROR_INVALID_PARAMETER(0x%08x) : Invalid param", __FUNCTION__, APP_MANAGER_ERROR_INVALID_PARAMETER);
		return APP_MANAGER_ERROR_INVALID_PARAMETER;
	} else if (retval == AUL_R_EILLACC) {
		LOGE("[%s] APP_MANAGER_ERROR_PERMISSION_DENIED(0x%08x) : Permission denied", __FUNCTION__, APP_MANAGER_ERROR_PERMISSION_DENIED);
		return APP_MANAGER_ERROR_PERMISSION_DENIED;
	} else if (retval < 0) {
		return APP_MANAGER_ERROR_REQUEST_FAILED;
	}

	return APP_MANAGER_ERROR_NONE;
}

API int app_manager_request_terminate_bg_app(app_context_h app_context)
{
	int retval = APP_MANAGER_ERROR_NONE;
	pid_t pid = 0;

	if (app_context == NULL)
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);

	if (app_context_get_pid(app_context, &pid) != APP_MANAGER_ERROR_NONE)
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, "failed to get the process ID");

	retval = aul_terminate_bgapp_pid(pid);
	if (retval == AUL_R_EINVAL) {
		LOGE("[%s] APP_MANAGER_ERROR_INVALID_PARAMETER(0x%08x) : Invalid param", __FUNCTION__, APP_MANAGER_ERROR_INVALID_PARAMETER);
		return APP_MANAGER_ERROR_INVALID_PARAMETER;
	} else if (retval == AUL_R_EILLACC) {
		LOGE("[%s] APP_MANAGER_ERROR_PERMISSION_DENIED(0x%08x) : Permission denied", __FUNCTION__, APP_MANAGER_ERROR_PERMISSION_DENIED);
		return APP_MANAGER_ERROR_PERMISSION_DENIED;
	} else if (retval < 0) {
		return APP_MANAGER_ERROR_REQUEST_FAILED;
	}

	return APP_MANAGER_ERROR_NONE;
}

API int app_manager_is_running(const char *app_id, bool *running)
{
	if (app_id == NULL) {
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : invalid package", __FUNCTION__, APP_MANAGER_ERROR_INVALID_PARAMETER);
		return APP_MANAGER_ERROR_INVALID_PARAMETER;
	}

	if (running == NULL) {
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : invalid output param", __FUNCTION__, APP_MANAGER_ERROR_INVALID_PARAMETER);
		return APP_MANAGER_ERROR_INVALID_PARAMETER;
	}

	*running = aul_app_is_running(app_id);

	return APP_MANAGER_ERROR_NONE;
}

API int app_manager_get_shared_data_path(const char *app_id, char **path)
{
	int r;
	int retval = aul_get_app_shared_data_path_by_appid(app_id, path);

	switch (retval) {
	case AUL_R_OK:
		r = APP_MANAGER_ERROR_NONE;
		break;
	case AUL_R_ENOAPP:
		r = app_manager_error(APP_MANAGER_ERROR_NO_SUCH_APP, __FUNCTION__, NULL);
		break;
	case AUL_R_EINVAL:
		r = app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
		break;
	case AUL_R_ERROR:
		r = app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
		break;
	case AUL_R_EREJECTED:
		r = app_manager_error(APP_MANAGER_ERROR_NOT_SUPPORTED, __FUNCTION__, NULL);
		break;
	default:
		r = app_manager_error(APP_MANAGER_ERROR_REQUEST_FAILED, __FUNCTION__, NULL);
		break;
	}

	return r;
}

API int app_manager_get_shared_resource_path(const char *app_id, char **path)
{
	int r;
	int retval = aul_get_app_shared_resource_path_by_appid(app_id, path);

	switch (retval) {
	case AUL_R_OK:
		r = APP_MANAGER_ERROR_NONE;
		break;
	case AUL_R_ENOAPP:
		r = app_manager_error(APP_MANAGER_ERROR_NO_SUCH_APP, __FUNCTION__, NULL);
		break;
	case AUL_R_EINVAL:
		r = app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
		break;
	case AUL_R_ERROR:
		r = app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
		break;
	default:
		r = app_manager_error(APP_MANAGER_ERROR_REQUEST_FAILED, __FUNCTION__, NULL);
		break;
	}

	return r;
}

API int app_manager_get_shared_trusted_path(const char *app_id, char **path)
{
	int r;
	int retval = aul_get_app_shared_trusted_path_by_appid(app_id, path);

	switch (retval) {
	case AUL_R_OK:
		r = APP_MANAGER_ERROR_NONE;
		break;
	case AUL_R_ENOAPP:
		r = app_manager_error(APP_MANAGER_ERROR_NO_SUCH_APP, __FUNCTION__, NULL);
		break;
	case AUL_R_EINVAL:
		r = app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
		break;
	case AUL_R_ERROR:
		r = app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
		break;
	default:
		r = app_manager_error(APP_MANAGER_ERROR_REQUEST_FAILED, __FUNCTION__, NULL);
		break;
	}

	return r;
}

API int app_manager_get_external_shared_data_path(const char *app_id, char **path)
{
	int r;
	int retval = aul_get_app_external_shared_data_path_by_appid(app_id, path);

	switch (retval) {
	case AUL_R_OK:
		r = APP_MANAGER_ERROR_NONE;
		break;
	case AUL_R_ENOAPP:
		r = app_manager_error(APP_MANAGER_ERROR_NO_SUCH_APP, __FUNCTION__, NULL);
		break;
	case AUL_R_EINVAL:
		r = app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
		break;
	case AUL_R_ERROR:
		r = app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
		break;
	default:
		r = app_manager_error(APP_MANAGER_ERROR_REQUEST_FAILED, __FUNCTION__, NULL);
		break;
	}

	return r;
}
