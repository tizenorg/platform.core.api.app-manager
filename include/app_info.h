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
 * @param [out] name The name of the application
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_DB_FAILED Database error occurred
 * @retval #APP_MANAGER_ERROR_OUT_OF_MEMORY Out of memory
 */
int app_info_get_name(app_info_h app_info, char **name);


/**
 * @brief Gets the version of the application
 * @remarks @a version must be released with free() by you.
 * @param [in] app_info The application information
 * @param [out] version The version of the application
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_DB_FAILED Database error occurred
 * @retval #APP_MANAGER_ERROR_OUT_OF_MEMORY Out of memory
 */
int app_info_get_version(app_info_h app_info, char **version);


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


#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_APPFW_APP_INFO_H */
