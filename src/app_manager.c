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
#include <app_manager_private.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "TIZEN_N_APP_MANAGER"


static const char* app_manager_error_to_string(app_manager_error_e error)
{
	switch (error)
	{
	case APP_MANAGER_ERROR_NONE:
		return "NONE";

	case APP_MANAGER_ERROR_INVALID_PARAMETER:
		return "INVALID_PARAMETER";

	case APP_MANAGER_ERROR_OUT_OF_MEMORY:
		return "OUT_OF_MEMORY";

	case APP_MANAGER_ERROR_IO_ERROR:
		return "IO_ERROR";

	case APP_MANAGER_ERROR_NO_SUCH_APP:
		return "NO_SUCH_APP";

	case APP_MANAGER_ERROR_DB_FAILED:
		return "DB_FAILED";

	case APP_MANAGER_ERROR_INVALID_PACKAGE:
		return "INVALID_PACKAGE";

	default :
		return "UNKNOWN";
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
	int retval;

	retval = app_context_set_event_cb(callback, user_data);

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
	int retval;

	retval = app_context_foreach_app_context(callback, user_data);

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
	int retval;

	retval = app_context_get_app_context(app_id, app_context);

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

	if (app_context == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (app_context_get_app_id(app_context, &app_id) != APP_MANAGER_ERROR_NONE)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, "failed to get the application ID");
	}

	aul_resume_app(app_id);

	return APP_MANAGER_ERROR_NONE;
}

int app_manager_set_app_info_event_cb(app_manager_app_info_event_cb callback, void *user_data)
{
	int retval;

	retval = app_info_set_event_cb(callback, user_data);

	if (retval != APP_MANAGER_ERROR_NONE)
	{
		return app_manager_error(retval, __FUNCTION__, NULL);
	}
	else
	{
		return APP_MANAGER_ERROR_NONE;
	}
}

void app_manager_unset_app_info_event_cb(void)
{
	app_info_unset_event_cb();
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

	retval = app_info_get_app_info(app_id, app_info);

	if (retval != APP_MANAGER_ERROR_NONE)
	{
		return app_manager_error(retval, __FUNCTION__, NULL);
	}
	else
	{
		return APP_MANAGER_ERROR_NONE;
	}
}

int app_manager_get_package(pid_t pid, char **package)
{
	// TODO: this function must be deprecated
	return app_manager_get_app_id(pid, package);
}

int app_manager_get_app_id(pid_t pid, char **app_id)
{
	char buffer[256] = {0, };
	char *app_id_dup = NULL;

	if (app_id == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (aul_app_get_pkgname_bypid(pid, buffer, sizeof(buffer)) != AUL_R_OK)
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
	pid_t pid = 0;

	if (app_context == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (app_context_get_pid(app_context, &pid) != APP_MANAGER_ERROR_NONE) {
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, "failed to get the process ID");
	}

	aul_terminate_pid(pid);

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
