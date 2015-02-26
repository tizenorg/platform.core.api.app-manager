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
#include <pthread.h>

#include <glib.h>

#include <aul.h>
#include <dlog.h>

#include "app_context.h"
#include "app_manager.h"
#include "app_manager_internal.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "CAPI_APPFW_APP_MANAGER"

#define APPID_MAX 128

static int app_context_create(const char *app_id, pid_t pid, app_context_h *app_context);

static int app_context_get_app_context_by_pid(pid_t pid, app_context_h *app_context);

struct app_context_s {
	char *app_id;
	pid_t pid;
};

typedef struct _foreach_context_ {
	app_manager_app_context_cb callback;
	void *user_data;
	bool iteration;
} foreach_context_s;

typedef struct _retrieval_context_{
	const char *app_id;
	pid_t pid;
	bool matched;
} retrieval_context_s;

static int app_context_foreach_app_context_cb(const aul_app_info *aul_app_context, void *cb_data)
{
	foreach_context_s* foreach_context = cb_data;

	if (foreach_context == NULL)
	{
		app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
		return 0;
	}

	if (foreach_context->iteration == true)
	{
		app_context_h app_context = NULL;

		if (app_context_create(aul_app_context->appid, aul_app_context->pid, &app_context) == APP_MANAGER_ERROR_NONE)
		{
			foreach_context->iteration = foreach_context->callback(app_context, foreach_context->user_data);
			app_context_destroy(app_context);
		}
	}

	return 0;
}

int app_context_foreach_app_context(app_manager_app_context_cb callback, void *user_data)
{
	foreach_context_s foreach_context = {
		.callback = callback,
		.user_data = user_data,
		.iteration = true
	};

	if (callback == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (aul_app_get_running_app_info(app_context_foreach_app_context_cb, &foreach_context) != AUL_R_OK)
	{
		return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, NULL);
	}

	return APP_MANAGER_ERROR_NONE;
}


static int app_context_retrieve_app_context(const aul_app_info *aul_app_context, void *cb_data)
{
	retrieval_context_s *retrieval_context = cb_data;

	if (aul_app_context != NULL && retrieval_context != NULL && retrieval_context->matched == false)
	{
		if (!strcmp(aul_app_context->appid, retrieval_context->app_id))
		{
			retrieval_context->pid = aul_app_context->pid;
			retrieval_context->matched = true;
		}
	}

	return 0;
}

int app_context_get_app_context(const char *app_id, app_context_h *app_context)
{
	retrieval_context_s retrieval_context =  {
		.app_id = app_id,
		.pid = 0,
		.matched = false
	};

	if (app_id == NULL || app_context == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (aul_app_is_running(app_id) == 0)
	{
		return app_manager_error(APP_MANAGER_ERROR_NO_SUCH_APP, __FUNCTION__, NULL);
	}

	aul_app_get_running_app_info(app_context_retrieve_app_context, &retrieval_context);

	if (retrieval_context.matched == false)
	{
		return app_manager_error(APP_MANAGER_ERROR_NO_SUCH_APP, __FUNCTION__, NULL);
	}

	return app_context_create(retrieval_context.app_id, retrieval_context.pid, app_context);
}

static int app_context_get_app_context_by_pid(pid_t pid, app_context_h *app_context)
{
	int retval;
	char appid[APPID_MAX] = {0, };

	if (pid < 0 || app_context == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (aul_app_get_appid_bypid(pid, appid, sizeof(appid)) != AUL_R_OK)
	{
		return app_manager_error(APP_MANAGER_ERROR_NO_SUCH_APP, __FUNCTION__, NULL);
	}

	retval = app_context_get_app_context(appid, app_context);

	if (retval != APP_MANAGER_ERROR_NONE)
	{
		return app_manager_error(retval, __FUNCTION__, NULL);
	}

	return APP_MANAGER_ERROR_NONE;
}

static int app_context_create(const char *app_id, pid_t pid, app_context_h *app_context)
{
	app_context_h app_context_created;

	if (app_id == NULL || pid <= 0 || app_context == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	app_context_created = calloc(1, sizeof(struct app_context_s));

	if (app_context_created == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
	}

	app_context_created->app_id = strdup(app_id);

	if (app_context_created->app_id == NULL)
	{
		free(app_context_created);
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
	}

	app_context_created->pid = pid;

	*app_context = app_context_created;

	return APP_MANAGER_ERROR_NONE;
}

API int app_context_destroy(app_context_h app_context)
{
	if (app_context == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	free(app_context->app_id);
	free(app_context);

	return APP_MANAGER_ERROR_NONE;
}

API int app_context_get_package(app_context_h app_context, char **package)
{
	// TODO: this function must be deprecated
	return app_context_get_app_id(app_context, package);
}


API int app_context_get_app_id(app_context_h app_context, char **app_id)
{
	char *app_id_dup;

	if (app_context == NULL || app_id == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	app_id_dup = strdup(app_context->app_id);

	if (app_id_dup == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
	}

	*app_id = app_id_dup;

	return APP_MANAGER_ERROR_NONE;
}

API int app_context_get_pid(app_context_h app_context, pid_t *pid)
{
	if (app_context == NULL || pid == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	*pid = app_context->pid;

	return APP_MANAGER_ERROR_NONE;
}

API int app_context_is_terminated(app_context_h app_context, bool *terminated)
{
	if (app_context == NULL || terminated == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (aul_app_is_running(app_context->app_id) == 1)
	{
		*terminated = false;
	}
	else
	{
		char appid[APPID_MAX] = {0, };

		if (aul_app_get_appid_bypid(app_context->pid, appid, sizeof(appid)) == AUL_R_OK)
		{
			*terminated = false;
		}
		else
		{
			*terminated = true;
		}
	}

	return APP_MANAGER_ERROR_NONE;
}

API int app_context_is_equal(app_context_h lhs, app_context_h rhs, bool *equal)
{
	if (lhs == NULL || rhs == NULL || equal == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	if (!strcmp(lhs->app_id, rhs->app_id) && lhs->pid == rhs->pid)
	{
		*equal = true;
	}
	else
	{
		*equal = false;
	}

	return APP_MANAGER_ERROR_NONE;
}

API int app_context_clone(app_context_h *clone, app_context_h app_context)
{
	int retval;

	if (clone == NULL || app_context == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	retval = app_context_create(app_context->app_id, app_context->pid, clone);

	if (retval != APP_MANAGER_ERROR_NONE)
	{
		return app_manager_error(retval, __FUNCTION__, NULL);
	}

	return APP_MANAGER_ERROR_NONE;
}

typedef struct _event_cb_context_ {
	GHashTable *pid_table;
	app_manager_app_context_event_cb callback;
	void *user_data;
} event_cb_context_s;

static pthread_mutex_t event_cb_context_mutex = PTHREAD_MUTEX_INITIALIZER;
static event_cb_context_s *event_cb_context = NULL;

static void app_context_lock_event_cb_context()
{
	pthread_mutex_lock(&event_cb_context_mutex);
}

static void app_context_unlock_event_cb_context()
{
	pthread_mutex_unlock(&event_cb_context_mutex);
}

static bool app_context_load_all_app_context_cb_locked(app_context_h app_context, void *user_data)
{
	app_context_h app_context_cloned;

	if (app_context_clone(&app_context_cloned, app_context) == APP_MANAGER_ERROR_NONE)
	{
		SECURE_LOGI("[%s] app_id(%s), pid(%d)", __FUNCTION__, app_context->app_id, app_context->pid);

		if (event_cb_context != NULL && event_cb_context->pid_table != NULL)
		{
			g_hash_table_insert(event_cb_context->pid_table, GINT_TO_POINTER(&(app_context_cloned->pid)), app_context_cloned);
		}
		else
		{
			app_context_destroy(app_context_cloned);
			app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, "invalid callback context");
		}
	}

	return true;
}

static void app_context_pid_table_entry_destroyed_cb(void * data)
{
	app_context_h app_context = data;

	if (app_context != NULL)
	{
		char *app_id;
		int pid;
		app_context_get_app_id(app_context, &app_id);
		app_context_get_pid(app_context, &pid);
		SECURE_LOGI("[%s] app_id(%s), pid(%d)", __FUNCTION__, app_context->app_id, app_context->pid);
		free(app_id);

		app_context_destroy(app_context);
	}
}

static int app_context_launched_event_cb(pid_t pid, void *data)
{
	app_context_h app_context = NULL;

	app_context_lock_event_cb_context();

	if (app_context_get_app_context_by_pid(pid, &app_context) == APP_MANAGER_ERROR_NONE)
	{
		if (event_cb_context != NULL && event_cb_context->pid_table != NULL)
		{
			g_hash_table_insert(event_cb_context->pid_table, GINT_TO_POINTER(&(app_context->pid)), app_context);
			event_cb_context->callback(app_context, APP_CONTEXT_EVENT_LAUNCHED, event_cb_context->user_data);
		}
		else
		{
			app_context_destroy(app_context);
			app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, "invalid callback context");
		}
	}

	app_context_unlock_event_cb_context();

	return 0;
}

static int app_context_terminated_event_cb(pid_t pid, void *data)
{
	app_context_h app_context;
	int lookup_key = pid;

	app_context_lock_event_cb_context();

	if (event_cb_context != NULL && event_cb_context->pid_table != NULL)
	{
		app_context = g_hash_table_lookup(event_cb_context->pid_table, GINT_TO_POINTER(&lookup_key));

		if (app_context != NULL)
		{
			event_cb_context->callback(app_context, APP_CONTEXT_EVENT_TERMINATED, event_cb_context->user_data);
			g_hash_table_remove(event_cb_context->pid_table, GINT_TO_POINTER(&(app_context->pid)));
		}
	}
	else
	{
		app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, "invalid callback context");
	}

	app_context_unlock_event_cb_context();

	return 0;
}

int app_context_set_event_cb(app_manager_app_context_event_cb callback, void *user_data)
{
	if (callback == NULL)
	{
		return app_manager_error(APP_MANAGER_ERROR_INVALID_PARAMETER, __FUNCTION__, NULL);
	}

	app_context_lock_event_cb_context();

	if (event_cb_context == NULL)
	{
		event_cb_context = calloc(1, sizeof(event_cb_context_s));

		if (event_cb_context == NULL)
		{
			return app_manager_error(APP_MANAGER_ERROR_OUT_OF_MEMORY, __FUNCTION__, NULL);
		}

		event_cb_context->pid_table = g_hash_table_new_full(g_int_hash, g_int_equal, NULL, app_context_pid_table_entry_destroyed_cb);

		if (event_cb_context->pid_table == NULL)
		{
			return app_manager_error(APP_MANAGER_ERROR_IO_ERROR, __FUNCTION__, "failed to initialize pid-table");
		}

		app_context_foreach_app_context(app_context_load_all_app_context_cb_locked, NULL);

		aul_listen_app_dead_signal(app_context_terminated_event_cb, NULL);
		aul_listen_app_launch_signal(app_context_launched_event_cb, NULL);

	}

	event_cb_context->callback = callback;
	event_cb_context->user_data = user_data;

	app_context_unlock_event_cb_context();

	return APP_MANAGER_ERROR_NONE;
}

void app_context_unset_event_cb(void)
{
	app_context_lock_event_cb_context();

	if (event_cb_context != NULL)
	{
		//aul_listen_app_dead_signal(NULL, NULL);
		//aul_listen_app_launch_signal(NULL, NULL);

		g_hash_table_destroy(event_cb_context->pid_table);
		free(event_cb_context);
		event_cb_context = NULL;
	}

	app_context_unlock_event_cb_context();
}
