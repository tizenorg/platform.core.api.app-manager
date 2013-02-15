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

 
#ifndef __TIZEN_APPFW_APP_INFO_H
#define __TIZEN_APPFW_APP_INFO_H

#include <tizen.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Application information handle.
 */
typedef struct app_info_s *app_info_h;


/**
 * @brief Enumerations of event type for the application information event
 */
typedef enum
{
	APP_INFO_EVENT_INSTALLED, /**< The application is newly installed */
	APP_INFO_EVENT_UNINSTALLED, /**< The application is uninstalled */
	APP_INFO_EVENT_UPDATED, /**< The application is updated */
} app_info_event_e;


/**
 * @brief Destroys the application information handle and releases all its resources.
 * @param [in] app_info The application information handle
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @see app_manager_foreach_app_info()
 * @see app_manager_get_app_info()
 */
int app_info_destroy(app_info_h app_info);


/**
 * @brief Gets the application ID with the given application context.
 * @remarks @a app_id must be released with free() by you.
 * @param [in] app_info The application information
 * @param [out] app_id The application ID of the given application context
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_DB_FAILED Database error occurred
 * @retval #APP_MANAGER_ERROR_OUT_OF_MEMORY Out of memory
 */
int app_info_get_app_id(app_info_h app_info, char **app_id);


/**
 * @brief Gets the name of the application
 * @remarks @a name must be released with free() by you.
 * @param [in] app_info The application information
 * @param [out] name The label of the application
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_DB_FAILED Database error occurred
 * @retval #APP_MANAGER_ERROR_OUT_OF_MEMORY Out of memory
 */
int app_info_get_label(app_info_h app_info, char **label);


/**
 * @brief Gets the absolute path to the icon image
 * @remarks @a path must be released with free() by you.
 * @param [in] app_info The application information
 * @param [out] path The absolute path to the icon
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_DB_FAILED Database error occurred
 * @retval #APP_MANAGER_ERROR_OUT_OF_MEMORY Out of memory
 */
int app_info_get_icon(app_info_h app_info, char **path);


/**
 * @brief Checks whether two application information are equal.
 * @param [in] lhs	The first application information to compare
 * @param [in] rhs	The second application information to compare
 * @param [out] equal true if the application information are equal, otherwise false
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_DB_FAILED Database error occurred
 * @retval #APP_MANAGER_ERROR_OUT_OF_MEMORY Out of memory
 */
int app_info_is_equal(app_info_h lhs, app_info_h rhs, bool *equal);


/**
 * @brief Clones the application information handle.
 * @param [out] clone If successful, a newly created application information handle will be returned.
 * @param [in] app_info The application information
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_DB_FAILED Database error occurred
 * @retval #APP_MANAGER_ERROR_OUT_OF_MEMORY Out of memory
 */
int app_info_clone(app_info_h *clone, app_info_h app_info);


/**
 * @brief Gets the absolute path to the icon image
 * @remarks @a path must be released with free() by you.
 * @param [in] app_info The application information
 * @param [out] package The package name
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_DB_FAILED Database error occurred
 * @retval #APP_MANAGER_ERROR_OUT_OF_MEMORY Out of memory
 */
int app_info_get_package(app_info_h app_info, char **package);


/**
 * @brief Gets the absolute path to the icon image
 * @remarks @a path must be released with free() by you.
 * @param [in] app_info The application information
 * @param [out] type The package type
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_DB_FAILED Database error occurred
 * @retval #APP_MANAGER_ERROR_OUT_OF_MEMORY Out of memory
 */
int app_info_get_type(app_info_h app_info, char **type);


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


#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_APPFW_APP_INFO_H */
