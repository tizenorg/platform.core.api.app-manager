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

#include <ail.h>
#include <package-manager.h>
#include <dlog.h>

#include <app_info.h>
#include <app_manager.h>
#include <app_manager_private.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "TIZEN_N_APP_MANAGER"

static int app_info_create(const char *app_id, app_info_h *app_info);

struct app_info_s {
	char *app_id;
	ail_appinfo_h ail_app_info;
};

typedef struct _foreach_context_{
	app_manager_app_info_cb callback;
	void *user_data;
} foreach_context_s;


static ail_cb_ret_e app_info_foreach_app_info_cb(const ail_appinfo_h ail_app_info, void *cb_data)
{
	foreach_context_s *foreach_context = cb_data;
	app_info_h app_info = NULL;
	char *app_id;
	bool iteration_next = true;

	if (ail_app_info == NULL || foreach_context == NULL)
	{
		app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
		return AIL_CB_RET_CANCEL;
	}

	ail_appinfo_get_str(ail_app_info, AIL_PROP_PACKAGE_STR, &app_id);

	if (app_info_create(app_id, &app_info) == APP_MANAGER_ERROR_NONE)
	{
		iteration_next = foreach_context->callback(app_info, foreach_context->user_data);
		app_info_destroy(app_info);
	}

	if (iteration_next == true)
	{
		return AIL_CB_RET_CONTINUE;
	}
	else
	{
		return AIL_CB_RET_CANCEL;
	}
}

int app_info_foreach_app_info(app_manager_app_info_cb callback, void *user_data)
{
	foreach_context_s foreach_context = {
		.callback = callback,
		.user_data = user_data,
	};

	if (callback == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	ail_filter_list_appinfo_foreach(NULL, app_info_foreach_app_info_cb, &foreach_context);

	return APP_MANAGER_ERROR_NONE;
}

int app_info_get_app_info(const char *app_id, app_info_h *app_info)
{
	return app_info_create(app_id, app_info);
}

static int app_info_create(const char *app_id, app_info_h *app_info)
{
	app_info_h app_info_created;
	ail_appinfo_h ail_app_info;

	if (app_id == NULL || app_info == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (ail_package_get_appinfo(app_id, &ail_app_info) != AIL_ERROR_OK)
	{
		return app_manager_error(APP_MANAGER_ERROR_NO_SUCH_APP, __FUNCTION__, NULL);
	}

	app_info_created = calloc(1, sizeof(struct app_info_s));

	if (app_info_created == NULL)
	{
		ail_package_destroy_appinfo(ail_app_info);
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
	}

	app_info_created->app_id = strdup(app_id);

	if (app_info_created->app_id == NULL)
	{
		ail_package_destroy_appinfo(ail_app_info);
		free(app_info_created);
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
	}

	app_info_created->ail_app_info = ail_app_info;

	*app_info = app_info_created;

	return APP_MANAGER_ERROR_NONE;
}

int app_info_destroy(app_info_h app_info)
{
	if (app_info == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	free(app_info->app_id);

	ail_package_destroy_appinfo(app_info->ail_app_info);

	free(app_info);	

	return APP_MANAGER_ERROR_NONE;
}

int app_info_get_app_id(app_info_h app_info, char **app_id)
{
	char *app_id_dup;

	if (app_info == NULL || app_id == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	app_id_dup = strdup(app_info->app_id);

	if (app_id_dup == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
	}

	*app_id = app_id_dup;

	return APP_MANAGER_ERROR_NONE;
}


int app_info_get_name(app_info_h app_info, char **name)
{
	char *ail_value = NULL;
	char *name_dup;

	if (app_info == NULL || name == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (ail_appinfo_get_str(app_info->ail_app_info, AIL_PROP_NAME_STR, &ail_value) != AIL_ERROR_OK)
	{
		return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}

	name_dup = strdup(ail_value);

	if (name_dup == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
	}

	*name = name_dup;

	return APP_MANAGER_ERROR_NONE;
}


int app_info_get_version(app_info_h app_info, char **version)
{
	char *ail_value = NULL;
	char *version_dup;

	if (app_info == NULL || version == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (ail_appinfo_get_str(app_info->ail_app_info, AIL_PROP_VERSION_STR, &ail_value) != AIL_ERROR_OK)
	{
		return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}

	version_dup = strdup(ail_value);

	if (version_dup == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
	}

	*version = version_dup;

	return APP_MANAGER_ERROR_NONE;
}


int app_info_get_icon(app_info_h app_info, char **path)
{
	char *ail_value = NULL;
	char *path_dup;

	if (app_info == NULL || path == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (ail_appinfo_get_str(app_info->ail_app_info, AIL_PROP_ICON_STR, &ail_value) != AIL_ERROR_OK)
	{
		return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}

	path_dup = strdup(ail_value);

	if (path_dup == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
	}

	*path = path_dup;

	return APP_MANAGER_ERROR_NONE;
}


int app_info_is_equal(app_info_h lhs, app_info_h rhs, bool *equal)
{
	if (lhs == NULL || rhs == NULL || equal == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (!strcmp(lhs->app_id, rhs->app_id))
	{
		*equal = true;
	}
	else
	{
		*equal = false;
	}

	return APP_MANAGER_ERROR_NONE;
}


int app_info_clone(app_info_h *clone, app_info_h app_info)
{
	int retval;

	if (clone == NULL || app_info == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	retval = app_info_create(app_info->app_id, clone);

	if (retval != APP_MANAGER_ERROR_NONE)
	{
		return app_manager_error(retval, __FUNCTION__, NULL);
	}

	return APP_MANAGER_ERROR_NONE;
}

static pkgmgr_client *package_event_listener = NULL;
static app_manager_app_info_event_cb app_info_event_cb = NULL;
static void *app_info_event_cb_data = NULL;

static app_info_event_e app_info_get_app_info_event(const char *value)
{
	if (!strcasecmp(value, "install"))
	{
		return APP_INFO_EVENT_INSTALLED;
	}
	else if (!strcasecmp(value, "uninstall"))
	{
		return APP_INFO_EVENT_UNINSTALLED;	
	}
	else if (!strcasecmp(value, "update"))
	{
		return APP_INFO_EVENT_UPDATED;		
	}
	else
	{
		return APP_MANAGER_ERROR_INVALID_PARAMETER;
	}
}

static int app_info_package_event_listener_cb(
	int id, const char *type, const char *package, const char *key, const char *val, const void *msg, void *data)
{
	static int event_id = -1;
	static app_info_event_e event_type = -1;

	if (!strcasecmp(key, "start"))
	{
		event_id = id;
		event_type = app_info_get_app_info_event(val);
	}
	else if (!strcasecmp(key, "end") && !strcasecmp(val, "ok") && id == event_id)
	{
		if (app_info_event_cb != NULL && event_type >= 0)
		{
			app_info_h app_info;
			
			if (app_info_create(package, &app_info) == APP_MANAGER_ERROR_NONE)
			{
				app_info_event_cb(app_info, event_type, app_info_event_cb_data);
				app_info_destroy(app_info);
			}
		}

		event_id = -1;
		event_type = -1;
	}

	return APP_MANAGER_ERROR_NONE;
}

int app_info_set_event_cb(app_manager_app_info_event_cb callback, void *user_data)
{
	if (callback == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (app_info_event_cb == NULL)
	{
		package_event_listener = pkgmgr_client_new(PC_LISTENING);
		
		if (package_event_listener == NULL)
		{
			return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
		}

		pkgmgr_client_listen_status(package_event_listener, app_info_package_event_listener_cb, NULL);
	}

	app_info_event_cb = callback;
	app_info_event_cb_data = user_data;

	return APP_MANAGER_ERROR_NONE;
}


void app_info_unset_event_cb(void)
{
	if (app_info_event_cb != NULL)
	{
		pkgmgr_client_free(package_event_listener);
		package_event_listener = NULL;
	}

	app_info_event_cb = NULL;
	app_info_event_cb_data = NULL;
}

