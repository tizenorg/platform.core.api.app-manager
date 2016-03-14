/*
 * Copyright (c) 2011 - 2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <aul.h>
#include <dlog.h>
#include <cynara-client.h>
#include <package-manager.h>

#include "app_manager.h"
#include "app_manager_internal.h"
#include "app_manager_event.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "CAPI_APPFW_APP_MANAGER"

#define SMACK_LABEL_LEN 255

static const char *app_manager_error_to_string(app_manager_error_e error)
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

int app_manager_error(app_manager_error_e error, const char *function, const char *description)
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

API int app_manager_event_create(app_manager_event_h *handle)
{
	pkgmgr_client *pc = NULL;
	app_manager_event *app_mgr_evt = NULL;

	/* TODO(jungh.yeon) : check privilege? */

	app_mgr_evt = (app_manager_event *)calloc(1, sizeof(app_manager_event));
	if (app_mgr_evt == NULL)
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY,
				__FUNCTION__, NULL);

	pc = pkgmgr_client_new(PC_LISTENING);
	if (pc == NULL)
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY,
				__FUNCTION__, NULL);

	app_mgr_evt->pc = pc;
	*handle = app_mgr_evt;
	return APP_MANAGER_ERROR_NONE;
}

API int app_manager_event_set_status(app_manager_event_h handle, int status_type)
{
	int ret = APP_MANAGER_ERROR_NONE;
	int pkgmgr_status_type = -1;

	if (handle == NULL || status_type < 0)
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER,
				__FUNCTION__, NULL);

	pkgmgr_status_type = convert_status_type(status_type);
	if (pkgmgr_status_type < 0)
		return app_manager_error(APP_MANAGER_ERROR_REQUEST_FAILED,
				__FUNCTION__, NULL);

	ret = pkgmgr_client_set_status_type(handle->pc, pkgmgr_status_type);
	if (ret != PKGMGR_R_OK) {
		LOGE("[%s] APP_MANAGER_ERROR_REQUEST_FAILED(0x%08x) : " \
				"Failed to set event status", __FUNCTION__,
				APP_MANAGER_ERROR_REQUEST_FAILED);
		return APP_MANAGER_ERROR_REQUEST_FAILED;
	}

	return APP_MANAGER_ERROR_NONE;
}

API int app_manager_event_set_cb(app_manager_event_h handle,
		app_manager_event_cb callback,
		void *user_data)
{
	int ret = APP_MANAGER_ERROR_NONE;

	if (handle == NULL || callback == NULL)
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER,
				__FUNCTION__, NULL);

	/* TODO(jungh.yeon) : check privilege? */

	handle->event_cb = callback;
	handle->user_data = user_data;

	ret = pkgmgr_client_listen_app_status(handle->pc,
			app_event_handler, handle);
	if (ret < PKGMGR_R_OK) {
		LOGE("[%s] APP_MANAGER_ERROR_REQUEST_FAILED(0x%08x) : " \
				"Failed to set event callback", __FUNCTION__,
				APP_MANAGER_ERROR_REQUEST_FAILED);
		return APP_MANAGER_ERROR_REQUEST_FAILED;
	}

	handle->req_id = ret;
	return APP_MANAGER_ERROR_NONE;
}

API int app_manager_event_unset_cb(app_manager_event_h handle)
{
	int ret = APP_MANAGER_ERROR_NONE;

	if (handle == NULL || handle->pc == NULL)
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER,
				__FUNCTION__, NULL);

	/* TODO(jungh.yeon) : check privilege? */

	ret = pkgmgr_client_remove_listen_status(handle->pc);
	if (ret != 0) {
		LOGE("[%s] APP_MANAGER_ERROR_REQUEST_FAILED(0x%08x) : " \
				"Failed to unset event callback", __FUNCTION__,
				APP_MANAGER_ERROR_REQUEST_FAILED);
		return APP_MANAGER_ERROR_REQUEST_FAILED;
	}

	return APP_MANAGER_ERROR_NONE;
}

API int app_manager_event_destroy(app_manager_event_h handle)
{
	int ret = APP_MANAGER_ERROR_NONE;

	if (handle == NULL)
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER,
				__FUNCTION__, NULL);

	ret = pkgmgr_client_free(handle->pc);
	if (ret != 0) {
		LOGE("[%s] APP_MANAGER_ERROR_REQUEST_FAILED(0x%08x) : " \
				"Failed to destroy handle", __FUNCTION__,
				APP_MANAGER_ERROR_REQUEST_FAILED);
		return APP_MANAGER_ERROR_REQUEST_FAILED;
	}
	remove_app_manager_event_info_list(handle->head);

	free(handle);
	return APP_MANAGER_ERROR_NONE;
}


