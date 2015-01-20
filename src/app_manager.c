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

#include <aul.h>
#include <dlog.h>

#include <app_manager.h>
#include <app_manager_internal.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "CAPI_APPFW_APP_MANAGER"


static const char* app_manager_error_to_string(app_manager_error_e error)
{
	switch (error)
	{
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

	default:
		return "Unknown";
	}
}

int app_manager_error(app_manager_error_e error, const char* function, const char *description)
{
	if (description)
	{
		LOGE("[%s] %s(0x%08x) : %s", function, app_manager_error_to_string(error), error, description);
	}
	else
	{
		LOGE("[%s] %s(0x%08x)", function, app_manager_error_to_string(error), error);
	}

	return error;
}


int app_manager_set_app_context_event_cb(app_manager_app_context_event_cb callback, void *user_data)
{
	int retval = app_context_set_event_cb(callback, user_data);

	if (retval != APP_MANAGER_ERROR_NONE)
	{
		return app_manager_error(retval, __FUNCTION__, NULL);
	}
	else
	{
		return APP_MANAGER_ERROR_NONE;
	}
}

void app_manager_unset_app_context_event_cb(void)
{
	app_context_unset_event_cb();
}

int app_manager_foreach_app_context(app_manager_app_context_cb callback, void *user_data)
{
	int retval = app_context_foreach_app_context(callback, user_data);

	if (retval != APP_MANAGER_ERROR_NONE)
	{
		return app_manager_error(retval, __FUNCTION__, NULL);
	}
	else
	{
		return APP_MANAGER_ERROR_NONE;
	}
}

int app_manager_get_app_context(const char *app_id, app_context_h *app_context)
{
	int retval = app_context_get_app_context(app_id, app_context);

	if (retval != APP_MANAGER_ERROR_NONE)
	{
		return app_manager_error(retval, __FUNCTION__, NULL);
	}
	else
	{
		return APP_MANAGER_ERROR_NONE;
	}
}

int app_manager_resume_app(app_context_h app_context)
{
	char *app_id;
	int retval;

	if (app_context == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (app_context_get_app_id(app_context, &app_id) != APP_MANAGER_ERROR_NONE)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, "failed to get the application ID");
	}

	if (aul_app_is_running(app_id) == 0)
	{
		free(app_id);
		return app_manager_error(APP_MANAGER_ERROR_APP_NO_RUNNING, __FUNCTION__, NULL);
	}

	retval = aul_resume_app(app_id);

	free(app_id);
	if (retval == AUL_R_EINVAL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}
	else if (retval == AUL_R_EILLACC)
	{
		return app_manager_error(APP_MANAGER_ERROR_PERMISSION_DENIED, __FUNCTION__, NULL);
	}
	else if (retval < 0)
	{
		return app_manager_error(APP_MANAGER_ERROR_REQUEST_FAILED, __FUNCTION__, NULL);
	}

	return APP_MANAGER_ERROR_NONE;
}

int app_manager_foreach_app_info(app_manager_app_info_cb callback, void *user_data)
{
	int retval;

	retval = app_info_foreach_app_info(callback, user_data);

	if (retval != APP_MANAGER_ERROR_NONE)
	{
		return app_manager_error(retval, __FUNCTION__, NULL);
	}
	else
	{
		return APP_MANAGER_ERROR_NONE;
	}
}

int app_manager_get_app_info(const char *app_id, app_info_h *app_info)
{
	int retval;

	retval = app_info_create(app_id, app_info);

	if (retval != APP_MANAGER_ERROR_NONE)
	{
		return app_manager_error(retval, __FUNCTION__, NULL);
	}
	else
	{
		return APP_MANAGER_ERROR_NONE;
	}
}

int app_manager_get_app_id(pid_t pid, char **app_id)
{
	char buffer[256] = {0, };
	char *app_id_dup = NULL;

	if (app_id == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (aul_app_get_appid_bypid(pid, buffer, sizeof(buffer)) != AUL_R_OK)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, "Invalid process ID");
	}

	app_id_dup = strdup(buffer);

	if (app_id_dup == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
	}

	*app_id = app_id_dup;

	return APP_MANAGER_ERROR_NONE;

}

int app_manager_terminate_app(app_context_h app_context)
{
	int retval;
	pid_t pid = 0;

	if (app_context == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (app_context_get_pid(app_context, &pid) != APP_MANAGER_ERROR_NONE) {
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, "failed to get the process ID");
	}

	retval = aul_terminate_pid(pid);

	if (retval == AUL_R_EINVAL)
	{
		LOGE("[%s] APP_MANAGER_ERROR_INVALID_PARAMETER(0x%08x) : Invalid param", __FUNCTION__, APP_MANAGER_ERROR_INVALID_PARAMETER);
		return APP_MANAGER_ERROR_INVALID_PARAMETER;
	}
	else if (retval == AUL_R_EILLACC)
	{
		LOGE("[%s] APP_MANAGER_ERROR_PERMISSION_DENIED(0x%08x) : Permission denied", __FUNCTION__, APP_MANAGER_ERROR_PERMISSION_DENIED);
		return APP_MANAGER_ERROR_PERMISSION_DENIED;
	}
	else if (retval < 0)
	{
		return APP_MANAGER_ERROR_REQUEST_FAILED;
	}

	return APP_MANAGER_ERROR_NONE;
}

int app_manager_is_running(const char *app_id, bool *running)
{
	if (app_id == NULL)
	{
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : invalid package", __FUNCTION__, APP_MANAGER_ERROR_INVALID_PARAMETER);
		return APP_MANAGER_ERROR_INVALID_PARAMETER;
	}

	if (running == NULL)
	{
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : invalid output param", __FUNCTION__, APP_MANAGER_ERROR_INVALID_PARAMETER);
		return APP_MANAGER_ERROR_INVALID_PARAMETER;
	}

	*running = aul_app_is_running(app_id);

	return APP_MANAGER_ERROR_NONE;
}

int app_manager_open_app(const char *app_id)
{
	int retval;
	retval = aul_open_app(app_id);

	if (retval == AUL_R_ERROR)
	{
		LOGE("[%s] APP_MANAGER_ERROR_NO_SUCH_APP(0x%08x) : No such application", __FUNCTION__, APP_MANAGER_ERROR_NO_SUCH_APP);
		return APP_MANAGER_ERROR_NO_SUCH_APP;
	}
	else if (retval == AUL_R_EINVAL)
	{
		LOGE("[%s] APP_MANAGER_ERROR_INVALID_PARAMETER(0x%08x) : Invalid param", __FUNCTION__, APP_MANAGER_ERROR_INVALID_PARAMETER);
		return APP_MANAGER_ERROR_INVALID_PARAMETER;
	}
	else if (retval == AUL_R_EILLACC)
	{
		LOGE("[%s] APP_MANAGER_ERROR_PERMISSION_DENIED(0x%08x) : Permission denied", __FUNCTION__, APP_MANAGER_ERROR_PERMISSION_DENIED);
		return APP_MANAGER_ERROR_PERMISSION_DENIED;
	}
	else if (retval < 0)
	{
		return APP_MANAGER_ERROR_REQUEST_FAILED;
	}

	return APP_MANAGER_ERROR_NONE;
}

int app_manager_get_shared_data_path(const char *app_id, char **path)
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
	default:
		r = app_manager_error(APP_MANAGER_ERROR_REQUEST_FAILED, __FUNCTION__, NULL);
		break;
	}

	return r;
}

int app_manager_get_shared_resource_path(const char *app_id, char **path)
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

int app_manager_get_shared_trusted_path(const char *app_id, char **path)
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

int app_manager_get_external_shared_data_path(const char *app_id, char **path)
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
