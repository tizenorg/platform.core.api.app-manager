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

#ifndef __TIZEN_APPFW_APP_MANAGER_INTERNAL_H
#define __TIZEN_APPFW_APP_MANAGER_INTERNAL_H

#include "app_manager.h"

#ifndef API
#define API __attribute__ ((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file app_manager_internal.h
 */

/**
 * @addtogroup CAPI_APPLICATION_MANAGER_MODULE
 * @{
 */

int app_manager_error(app_manager_error_e error, const char* function, const char *description);

int app_context_foreach_app_context(app_manager_app_context_cb callback, void *user_data);

int app_context_foreach_all_app_context(app_manager_app_context_cb callback, void *user_data);

int app_context_get_app_context(const char *app_id, app_context_h *app_context);

int app_context_set_event_cb(app_manager_app_context_event_cb callback, void *user_data);

void app_context_unset_event_cb(void);

int app_info_foreach_app_info(app_manager_app_info_cb callback, void *user_data);

int app_info_get_app_info(const char *app_id, app_info_h *app_info);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_APPFW_APP_MANAGER_INTERNAL_H */
