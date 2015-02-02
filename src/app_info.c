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

#include <pkgmgr-info.h>
#include <package-manager.h>
#include <dlog.h>

#include "app_info.h"
#include "app_manager.h"
#include "app_manager_internal.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "TIZEN_N_APP_MANAGER"

struct app_info_s {
	char *app_id;
	pkgmgrinfo_appinfo_h pkg_app_info;
};

struct app_info_filter_s {
	pkgmgrinfo_appinfo_filter_h pkg_app_info_filter;
};

struct app_info_metadata_filter_s {
	pkgmgrinfo_appinfo_metadata_filter_h pkg_app_info_metadata_filter;
};

typedef struct _foreach_context_{
	app_manager_app_info_cb callback;
	void *user_data;
} foreach_context_s;

typedef struct _foreach_metada_context_{
	app_info_metadata_cb callback;
	void *user_data;
} foreach_metadata_context_s;

static int app_info_convert_str_property(const char *property, char **converted_property)
{
	if (property == NULL)
		return -1;

	if (strcmp(property, PACKAGE_INFO_PROP_APP_ID)==0)
		*converted_property = PMINFO_APPINFO_PROP_APP_ID;

	else if (strcmp(property, PACKAGE_INFO_PROP_APP_TYPE)==0)
		*converted_property = PMINFO_APPINFO_PROP_APP_TYPE;

	else if (strcmp(property, PACKAGE_INFO_PROP_APP_CATEGORY)==0)
		*converted_property = PMINFO_APPINFO_PROP_APP_CATEGORY;

	else
		return -1;

	return 0;
}

static int app_info_convert_bool_property(const char *property, char **converted_property)
{
	if (property == NULL)
		return -1;

	if (strcmp(property, PACKAGE_INFO_PROP_APP_NODISPLAY)==0)
		*converted_property = PMINFO_APPINFO_PROP_APP_NODISPLAY;

	else if (strcmp(property, PACKAGE_INFO_PROP_APP_TASKMANAGE)==0)
		*converted_property = PMINFO_APPINFO_PROP_APP_TASKMANAGE;

	else
		return -1;

	return 0;
}

static int app_info_foreach_app_filter_cb(pkgmgrinfo_appinfo_h handle, void *user_data)
{
	int retval = 0;
	char *appid = NULL;
	app_info_h info = NULL;
	info = calloc(1, sizeof(struct app_info_s));
	if (info == NULL) {
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
	}

	foreach_context_s *foreach_context = user_data;
	if (handle == NULL || foreach_context == NULL)
	{
		free(info);
		return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}

	retval = pkgmgrinfo_appinfo_get_appid(handle, &appid);
	if (retval < 0)
	{
		free(info);
		return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}

	info->app_id = strdup(appid);
	info->pkg_app_info = handle;

	foreach_context->callback(info, foreach_context->user_data);

	return APP_MANAGER_ERROR_NONE;
}

static int app_info_foreach_app_metadata_cb(const char *metadata_key, const char *metadata_value, void *user_data)
{
	foreach_metadata_context_s *foreach_context = user_data;

	if (metadata_value == NULL || foreach_context == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}

	foreach_context->callback(metadata_key, metadata_value, foreach_context->user_data);

	return APP_MANAGER_ERROR_NONE;
}

static int app_info_foreach_app_info_cb(pkgmgrinfo_appinfo_h handle, void *cb_data)
{
	foreach_context_s *foreach_context = cb_data;
	app_info_h app_info = NULL;
	char *appid = NULL;
	int ret = 0;
	bool iteration_next = true;

	if (handle == NULL || foreach_context == NULL)
	{
		app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
		return PMINFO_R_ERROR;
	}

	ret = pkgmgrinfo_appinfo_get_appid(handle, &appid);
	if (ret != PMINFO_R_OK) {
		app_manager_error(APP_MANAGER_ERROR_NO_SUCH_APP, __FUNCTION__, NULL);
		return PMINFO_R_ERROR;
	}

	if (app_info_create(appid, &app_info) == APP_MANAGER_ERROR_NONE)
	{
		iteration_next = foreach_context->callback(app_info, foreach_context->user_data);
		app_info_destroy(app_info);
	}

	if (iteration_next == true)
	{
		return PMINFO_R_OK;
	}
	else
	{
		return PMINFO_R_ERROR;
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

	pkgmgrinfo_appinfo_get_usr_installed_list(app_info_foreach_app_info_cb, getuid(), &foreach_context);

	return APP_MANAGER_ERROR_NONE;
}

API int app_info_create(const char *app_id, app_info_h *app_info)
{
	pkgmgrinfo_pkginfo_h pkginfo = NULL;
	pkgmgrinfo_appinfo_h appinfo = NULL;
	app_info_h info = NULL;
	int retval = 0;
	char *main_appid = NULL;

	if (app_id == NULL || app_info == NULL) {
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}
	info = calloc(1, sizeof(struct app_info_s));
	if (info == NULL) {
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
	}

	retval = pkgmgrinfo_pkginfo_get_usr_pkginfo(app_id, getuid(), &pkginfo);
	if (retval < 0) {
		if (pkgmgrinfo_appinfo_get_usr_appinfo(app_id, getuid(), &appinfo)) {
			free(info);
			return app_manager_error(APP_MANAGER_ERROR_NO_SUCH_APP, __FUNCTION__, NULL);
		}

		info->app_id = strdup(app_id);
		info->pkg_app_info = appinfo;
		*app_info = info;
		return APP_MANAGER_ERROR_NONE;
	}

	retval = pkgmgrinfo_pkginfo_get_mainappid(pkginfo, &main_appid);
	if (retval < 0) {
		app_manager_error(APP_MANAGER_ERROR_NO_SUCH_APP, __FUNCTION__, NULL);
	}

	if (pkgmgrinfo_appinfo_get_usr_appinfo(main_appid, getuid(), &appinfo)) {
		free(info);
		return app_manager_error(APP_MANAGER_ERROR_NO_SUCH_APP, __FUNCTION__, NULL);
	}

	info->app_id = strdup(main_appid);
	info->pkg_app_info = appinfo;
	*app_info = info;

	pkgmgrinfo_pkginfo_destroy_pkginfo(pkginfo);

	return APP_MANAGER_ERROR_NONE;
}

API int app_info_destroy(app_info_h app_info)
{
	if (app_info == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}
	if (app_info->app_id) {
		free(app_info->app_id);
		app_info->app_id = NULL;
	}
	pkgmgrinfo_appinfo_destroy_appinfo(app_info->pkg_app_info);
	free(app_info);
	return APP_MANAGER_ERROR_NONE;
}

API int app_info_get_app_id(app_info_h app_info, char **app_id)
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

API int app_info_get_exec(app_info_h app_info, char **exec)
{
	char *val;
	char *app_exec_dup;

	if (app_info == NULL || exec == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	pkgmgrinfo_appinfo_get_exec(app_info->pkg_app_info, &val);
	if (val == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	app_exec_dup = strdup(val);
	if (app_exec_dup == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
	}

	*exec = app_exec_dup;

	return APP_MANAGER_ERROR_NONE;
}

API int app_info_get_label(app_info_h app_info, char **label)
{
	char *val;
	char *app_label_dup;

	if (app_info == NULL || label == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	pkgmgrinfo_appinfo_get_label(app_info->pkg_app_info, &val);
	if (val == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	app_label_dup = strdup(val);
	if (app_label_dup == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
	}

	*label = app_label_dup;

	return APP_MANAGER_ERROR_NONE;
}

API int app_info_get_localed_label(const char *app_id, const char *locale, char **label)
{
	char *val;
	char *app_label_dup;

	if (app_id == NULL || locale == NULL || label == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	pkgmgrinfo_appinfo_get_localed_label(app_id, locale, &val);
	if (val == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	app_label_dup = strdup(val);
	if (app_label_dup == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
	}

	*label = app_label_dup;
	free(val);

	return APP_MANAGER_ERROR_NONE;
}

API int app_info_get_icon(app_info_h app_info, char **path)
{
	char *val;
	char *app_icon_dup;

	if (app_info == NULL || path == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	pkgmgrinfo_appinfo_get_icon(app_info->pkg_app_info, &val);
	if (val == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	app_icon_dup = strdup(val);
	if (app_icon_dup == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
	}

	*path = app_icon_dup;

	return APP_MANAGER_ERROR_NONE;
}

API int app_info_get_package(app_info_h app_info, char **package)
{
	char *val;
	char *app_package_dup;

	if (app_info == NULL || package == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	pkgmgrinfo_appinfo_get_pkgname(app_info->pkg_app_info, &val);

	app_package_dup = strdup(val);
	if (app_package_dup == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
	}

	*package = app_package_dup;

	return APP_MANAGER_ERROR_NONE;
}

API int app_info_get_type(app_info_h app_info, char **type)
{
	char *val;
	char *app_type_dup;

	if (app_info == NULL || type == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	pkgmgrinfo_appinfo_get_apptype(app_info->pkg_app_info, &val);

	app_type_dup = strdup(val);
	if (app_type_dup == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
	}

	*type = app_type_dup;

	return APP_MANAGER_ERROR_NONE;
}

API int app_info_foreach_metadata(app_info_h app_info, app_info_metadata_cb callback, void *user_data)
{
	int retval = 0;

	if (app_info == NULL || callback == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	foreach_metadata_context_s foreach_context = {
		.callback = callback,
		.user_data = user_data,
	};

	retval = pkgmgrinfo_appinfo_foreach_metadata(app_info->pkg_app_info, app_info_foreach_app_metadata_cb, &foreach_context);
	if (retval < 0)
	{
		return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}

	return APP_MANAGER_ERROR_NONE;
}

API int app_info_is_nodisplay(app_info_h app_info, bool *nodisplay)
{
	bool val;

	if (app_info == NULL || nodisplay == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (pkgmgrinfo_appinfo_is_nodisplay(app_info->pkg_app_info, &val) < 0)
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);

	*nodisplay = val;
	return APP_MANAGER_ERROR_NONE;
}

API int app_info_is_enabled(app_info_h app_info, bool *enabled)
{
	bool val;

	if (app_info == NULL || enabled == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (pkgmgrinfo_appinfo_is_enabled(app_info->pkg_app_info, &val) < 0)
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);

	*enabled = val;
	return APP_MANAGER_ERROR_NONE;

}

API int app_info_is_equal(app_info_h lhs, app_info_h rhs, bool *equal)
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

API int app_info_is_onboot(app_info_h app_info, bool *onboot)
{
	bool val;

	if (app_info == NULL || onboot == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (pkgmgrinfo_appinfo_is_onboot(app_info->pkg_app_info, &val) < 0)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	*onboot = val;
	return APP_MANAGER_ERROR_NONE;
}

API int app_info_is_preload(app_info_h app_info, bool *preload)
{
	bool val;

	if (app_info == NULL || preload == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (pkgmgrinfo_appinfo_is_preload(app_info->pkg_app_info, &val) < 0)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	*preload = val;
	return APP_MANAGER_ERROR_NONE;
}

API int app_info_clone(app_info_h *clone, app_info_h app_info)
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

API int app_info_filter_create(app_info_filter_h *handle)
{
	int retval = 0;
	app_info_filter_h filter_created = NULL;
	pkgmgrinfo_appinfo_filter_h filter_h = NULL;

	if (handle == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	retval = pkgmgrinfo_appinfo_filter_create(&filter_h);
	if (retval < 0) {
		return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}

	filter_created = calloc(1, sizeof(struct app_info_filter_s));
	if (filter_created == NULL)
	{
		free(filter_h);
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
	}

	filter_created->pkg_app_info_filter = filter_h;

	*handle = filter_created;

	return APP_MANAGER_ERROR_NONE;
}

API int app_info_filter_destroy(app_info_filter_h handle)
{
	int retval = 0;

	if (handle == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	retval = pkgmgrinfo_appinfo_filter_destroy(handle->pkg_app_info_filter);
	if (retval < 0)
	{
		return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}

	free(handle);
	return APP_MANAGER_ERROR_NONE;
}

API int app_info_filter_add_bool(app_info_filter_h handle, const char *property, const bool value)
{
	int retval = 0;
	char *converted_property = NULL;

	if ((handle == NULL) || (property == NULL))
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	retval = app_info_convert_bool_property(property, &converted_property);
	if (retval < 0)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	retval = pkgmgrinfo_appinfo_filter_add_bool(handle->pkg_app_info_filter, converted_property, value);
	if (retval < 0)
	{
		return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}

	return APP_MANAGER_ERROR_NONE;
}

API int app_info_filter_add_string(app_info_filter_h handle, const char *property, const char *value)
{
	int retval = 0;
	char *converted_property = NULL;

	if ((handle == NULL) || (property == NULL))
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	retval = app_info_convert_str_property(property, &converted_property);
	if (retval < 0)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	retval = pkgmgrinfo_appinfo_filter_add_string(handle->pkg_app_info_filter, converted_property, value);
	if (retval < 0)
	{
		return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}

	return APP_MANAGER_ERROR_NONE;
}

API int app_info_filter_count_appinfo(app_info_filter_h handle, int *count)
{
	int retval = 0;

	if ((handle == NULL) || (count == NULL))
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	retval = pkgmgrinfo_appinfo_filter_count(handle->pkg_app_info_filter, count);
	if (retval < 0)
	{
		return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}

	return APP_MANAGER_ERROR_NONE;
}

API int app_info_filter_foreach_appinfo(app_info_filter_h handle, app_info_filter_cb callback, void * user_data)
{
	int retval = 0;

	foreach_context_s foreach_context = {
		.callback = callback,
		.user_data = user_data,
	};

	if ((handle == NULL) || (callback == NULL))
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	retval = pkgmgrinfo_appinfo_filter_foreach_appinfo(handle->pkg_app_info_filter, app_info_foreach_app_filter_cb, &foreach_context);
	if (retval < 0)
	{
		return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}

	return APP_MANAGER_ERROR_NONE;
}

API int app_info_metadata_filter_create(app_info_metadata_filter_h *handle)
{
	int retval = 0;
	app_info_metadata_filter_h filter_created = NULL;
	pkgmgrinfo_appinfo_metadata_filter_h filter_h = NULL;

	if (handle == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	filter_created = calloc(1, sizeof(struct app_info_metadata_filter_s));
	if (filter_created == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
	}

	retval = pkgmgrinfo_appinfo_metadata_filter_create(&filter_h);
	if (retval < 0) {
		free(filter_created);
		return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}

	filter_created->pkg_app_info_metadata_filter = filter_h;

	*handle = filter_created;

	return APP_MANAGER_ERROR_NONE;
}

API int app_info_metadata_filter_destroy(app_info_metadata_filter_h handle)
{
	int retval = 0;

	if (handle == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	retval = pkgmgrinfo_appinfo_metadata_filter_destroy(handle->pkg_app_info_metadata_filter);
	if (retval < 0)
	{
		return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}

	free(handle);
	return APP_MANAGER_ERROR_NONE;
}

API int app_info_metadata_filter_add(app_info_metadata_filter_h handle, const char *key, const char *value)
{
	int retval = 0;

	if ((handle == NULL) || (key == NULL))
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	retval = pkgmgrinfo_appinfo_metadata_filter_add(handle->pkg_app_info_metadata_filter, key, value);
	if (retval < 0)
	{
		return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}

	return APP_MANAGER_ERROR_NONE;
}

API int app_info_metadata_filter_foreach(app_info_metadata_filter_h handle, app_info_filter_cb callback, void *user_data)
{
	int retval = 0;

	foreach_context_s foreach_context = {
		.callback = callback,
		.user_data = user_data,
	};

	if (handle == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (callback == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	retval = pkgmgrinfo_appinfo_metadata_filter_foreach(handle->pkg_app_info_metadata_filter, app_info_foreach_app_filter_cb, &foreach_context);
	if (retval < 0)
	{
		return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}

	return APP_MANAGER_ERROR_NONE;
}
