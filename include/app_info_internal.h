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


#ifndef __TIZEN_APPFW_APP_INFO_INTERNAL_H
#define __TIZEN_APPFW_APP_INFO_INTERNAL_H

#include "app_info.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @addtogroup CAPI_APP_INFO_MODULE
* @{
*/

#define __START_OF_DEPRECATED_API
int app_info_get_name(app_info_h app_info, char **name);

typedef struct ui_app_info_s *ui_app_info_h;
typedef struct service_app_info_s *service_app_info_h;
int ui_app_info_destroy(ui_app_info_h ui_app_info);
int ui_app_info_get_app_id(ui_app_info_h ui_app_info, char **app_id);
int ui_app_info_get_label(ui_app_info_h ui_app_info, char **label);
int ui_app_info_get_icon(ui_app_info_h ui_app_info, char **iconpath);
int ui_app_info_is_equal(ui_app_info_h lhs, ui_app_info_h rhs, bool *equal);
int ui_app_info_clone(ui_app_info_h *clone, ui_app_info_h ui_app_info);
int ui_app_info_get_package(ui_app_info_h ui_app_info, char **package);
int ui_app_info_get_type(ui_app_info_h ui_app_info, char **type);
//int ui_app_info_get_category(ui_app_info_h ui_app_info, char **category);
int service_app_info_destroy(service_app_info_h service_app_info);
int service_app_info_get_app_id(service_app_info_h service_app_info, char **app_id);
int service_app_info_get_label(service_app_info_h service_app_info, char **label);
int service_app_info_get_icon(service_app_info_h service_app_info, char **iconpath);
int service_app_info_is_equal(service_app_info_h lhs, service_app_info_h rhs, bool *equal);
int service_app_info_clone(service_app_info_h *clone, service_app_info_h service_app_info);
int service_app_info_get_package(service_app_info_h service_app_info, char **package);
int service_app_info_get_type(service_app_info_h service_app_info, char **type);
int service_app_info_is_onboot(service_app_info_h service_app_info, bool *onboot);
int service_app_info_is_autorestart(service_app_info_h service_app_info, bool *autorestart);
#define __END_OF_DEPRECATED_API

/**
* @}
*/

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_APPFW_APP_INFO_INTERNAL_H */
