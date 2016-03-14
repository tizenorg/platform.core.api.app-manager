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


#include <stdlib.h>

#include <dlog.h>
#include <package-manager.h>

#include "app_event.h"
#include "app_manager.h"
#include "app_manager_internal.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "CAPI_APPFW_APP_MANAGER"

typedef struct _app_event_info {
	int req_id;
	app_event_type_e event_type;
	app_event_state_e event_state;
	struct _app_event_info *next;
} app_event_info;

typedef struct app_event_s {
	int req_id;
	pkgmgr_client *pc;
	app_event_cb event_cb;
	void *user_data;
	app_event_info *head;
} app_event;

static void __remove_app_event_info_list(app_event_info *head)
{
	if (head == NULL)
		return;

	app_event_info *current = head;

	if (current->next != NULL)
		__remove_app_event_info_list(current->next);

	free(current);
	return;
}

static int __remove_app_event_info(app_event_info *head, int req_id)
{
	app_event_info *prev;
	app_event_info *current;

	current = prev = head;
	if (current == NULL)
		return APP_MANAGER_ERROR_INVALID_PARAMETER;

	while (current) {
		if (current->req_id == req_id) {
			prev->next = current->next;
			free(current);
			current = NULL;
			return APP_MANAGER_ERROR_NONE;
		}
		prev = current;
		current = current->next;
	}

	return APP_MANAGER_ERROR_NONE;
}

static int __find_app_event_info(app_event_info **head,
		int req_id, app_event_type_e *event_type)
{
	app_event_info *tmp;

	tmp = *head;

	if (tmp == NULL) {
		LOGE("head is null");
		return APP_MANAGER_ERROR_INVALID_PARAMETER;
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

static int __add_app_event_info(app_event_info **head,
		int req_id, app_event_type_e event_type)
{
	app_event_info *event_info;
	app_event_info *current;
	app_event_info *prev;

	event_info = (app_event_info *)calloc(1, sizeof(app_event_info));
	if (event_info == NULL)
		return APP_MANAGER_ERROR_OUT_OF_MEMORY;

	event_info->req_id = req_id;
	event_info->event_type = event_type;
	event_info->next = NULL;

	if (*head == NULL) {
		*head = event_info;
	} else {
		current = prev = *head;
		while (current) {
			prev = current;
			current = current->next;
		}
		prev->next = event_info;
	}

	return APP_MANAGER_ERROR_NONE;
}

static int __get_app_event_type(const char *key, app_event_type_e *event_type)
{
	if (key == NULL)
		return APP_MANAGER_ERROR_INVALID_PARAMETER;

	if (strcasecmp(key, "disable_app") == 0 ||
			strcasecmp(key, "disable_global_app_for_uid") == 0)
		*event_type = APP_EVENT_DISABLE_APP;
	else if (strcasecmp(key, "enable_app") == 0 ||
			strcasecmp(key, "enable_global_app_for_uid") == 0)
		*event_type = APP_EVENT_ENABLE_APP;
	else
		return APP_MANAGER_ERROR_INVALID_PARAMETER;
	return APP_MANAGER_ERROR_NONE;
}

static int _app_event_handler(uid_t target_uid, int req_id,
				const char *pkg_type, const char *pkgid, const char *appid,
				const char *key, const char *val, const void *pmsg, void *data)
{
	app_event *app_evt = (app_event *)data;
	app_event_type_e event_type = -1;
	int ret = -1;

	LOGI("app_event_handler called");

	if (app_evt == NULL || app_evt->event_cb == NULL)
		return APP_MANAGER_ERROR_INVALID_PARAMETER;

	if (strcasecmp(key, "start") == 0) {
		ret = __get_app_event_type(val, &event_type);
		if (ret != APP_MANAGER_ERROR_NONE)
			return APP_MANAGER_ERROR_INVALID_PARAMETER;

		ret = __add_app_event_info(&(app_evt->head), req_id, event_type);
		if (ret != APP_MANAGER_ERROR_NONE)
			return APP_MANAGER_ERROR_REQUEST_FAILED;

		app_evt->event_cb(pkg_type, appid, event_type,
				APP_EVENT_STATE_STARTED, app_evt, app_evt->user_data);
	} else if (strcasecmp(key, "end") == 0) {
		if (__find_app_event_info(&(app_evt->head), req_id, &event_type)
				!= APP_MANAGER_ERROR_NONE)
			return APP_MANAGER_ERROR_REQUEST_FAILED;

		if (strcasecmp(val, "ok") == 0)
			app_evt->event_cb(pkg_type, appid, event_type,
					APP_EVENT_STATE_COMPLETED, app_evt, app_evt->user_data);
		else if (strcasecmp(val, "fail") == 0)
			app_evt->event_cb(pkg_type, appid, event_type,
					APP_EVENT_STATE_FAIL, app_evt, app_evt->user_data);

		ret = __remove_app_event_info(app_evt->head, req_id);
		if (ret != APP_MANAGER_ERROR_NONE) {
			LOGE("failed to remove app event info");
			return APP_MANAGER_ERROR_REQUEST_FAILED;
		}

	} else {
		return APP_MANAGER_ERROR_INVALID_PARAMETER;
	}

	return APP_MANAGER_ERROR_NONE;
}

static int _convert_status_type(int status_type)
{
	int result = 0;

	if (status_type == 0)
		return PKGMGR_CLIENT_STATUS_ALL;

	if ((status_type & APP_EVENT_STATUS_TYPE_ENABLE)
			== APP_EVENT_STATUS_TYPE_ENABLE)
		result += PKGMGR_CLIENT_STATUS_ENABLE_APP;

	if ((status_type & APP_EVENT_STATUS_TYPE_DISABLE)
			== APP_EVENT_STATUS_TYPE_DISABLE)
		result += PKGMGR_CLIENT_STATUS_DISABLE_APP;

	return result;
}

API int app_event_create(app_event_h *handle)
{
	pkgmgr_client *pc = NULL;
	app_event *app_evt = NULL;

	/* TODO(jungh.yeon) : check privilege? */

	app_evt = (app_event *)calloc(1, sizeof(app_event));
	if (app_evt == NULL)
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY,
				__FUNCTION__, NULL);

	pc = pkgmgr_client_new(PC_LISTENING);
	if (pc == NULL)
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY,
				__FUNCTION__, NULL);

	app_evt->pc = pc;
	*handle = app_evt;
	return APP_MANAGER_ERROR_NONE;
}

API int app_event_set_event_status(app_event_h handle, int status_type)
{
	int ret = APP_MANAGER_ERROR_NONE;
	int pkgmgr_status_type = -1;

	if (handle == NULL || status_type < 0)
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER,
				__FUNCTION__, NULL);

	pkgmgr_status_type = _convert_status_type(status_type);
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

API int app_event_set_event_cb(app_event_h handle,
		app_event_cb callback,
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
			_app_event_handler, handle);
	if (ret < PKGMGR_R_OK) {
		LOGE("[%s] APP_MANAGER_ERROR_REQUEST_FAILED(0x%08x) : " \
				"Failed to set event callback", __FUNCTION__,
				APP_MANAGER_ERROR_REQUEST_FAILED);
		return APP_MANAGER_ERROR_REQUEST_FAILED;
	}

	handle->req_id = ret;
	return APP_MANAGER_ERROR_NONE;
}

API int app_event_unset_event_cb(app_event_h handle)
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
	}

	return APP_MANAGER_ERROR_NONE;
}

API int app_event_destroy(app_event_h handle)
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
	}
	__remove_app_event_info_list(handle->head);

	free(handle);
	return APP_MANAGER_ERROR_NONE;
}
