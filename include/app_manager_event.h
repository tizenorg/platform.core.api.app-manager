/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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


#ifndef __TIZEN_APPFW_APP_MANAGER_EVENT_H
#define __TIZEN_APPFW_APP_MANAGER_EVENT_H

#include <sys/types.h>
#include <tizen.h>

#include "app_manager.h"

#include <package-manager.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _app_manager_event_info {
	int req_id;
	app_manager_event_type_e event_type;
	app_manager_event_state_e event_state;
	struct _app_manager_event_info *next;
} app_manager_event_info;

typedef struct _app_manager_event_s {
	int req_id;
	pkgmgr_client *pc;
	app_manager_event_cb event_cb;
	void *user_data;
	app_manager_event_info *head;
} app_manager_event;

int app_event_handler(uid_t target_uid, int req_id,
				const char *pkg_type, const char *pkgid, const char *appid,
				const char *key, const char *val, const void *pmsg, void *data);

int convert_status_type(int status_type);

void remove_app_manager_event_info_list(app_manager_event_info *head);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_APPFW_APP_MANAGER_EVENT_H */

