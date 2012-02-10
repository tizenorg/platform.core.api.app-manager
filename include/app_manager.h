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

 
#ifndef __TIZEN_APPFW_APP_MANAGER_H
#define __TIZEN_APPFW_APP_MANAGER_H

#include <tizen.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup CAPI_APPLICATION_MANAGER_MODULE
 * @{
 */

/**
 * @brief Enumerations of error code for Application Manager.
 */
typedef enum
{
	APP_MANAGER_ERROR_NONE = TIZEN_ERROR_NONE,	/**< Successful */
	APP_MANAGER_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER, /**< Invalid parameter */
	APP_MANAGER_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY, /**< Out of memory */
	APP_MANAGER_ERROR_DB_FAILED = TIZEN_ERROR_APPLICATION_CLASS | 0x03,	/**< Database error  */
	APP_MANAGER_ERROR_INVALID_PACKAGE = TIZEN_ERROR_APPLICATION_CLASS | 0x04, /**< Can not find the package */
} app_manager_error_e;

/**
 * @brief Enumerations of event type for the list of the installed applications.
 */
typedef enum
{
	APP_MANAGER_EVENT_INSTALLED = 40,   	/**< Install event */
	APP_MANAGER_EVENT_UNINSTALLED = 41,      	/**< Uninstall event */
	APP_MANAGER_EVENT_UPDATED = 42,   	/**< Update event */
} app_manger_event_type_e;

/**
 * @brief Called to get the package name once for each running application.
 * @param[in] package The package name of each running application
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break out of the loop.
 * @pre	app_manager_foreach_app_running() will invoke this callback.
 * @see app_manager_foreach_app_running()
 */
typedef bool (*app_manager_app_running_cb) (const char *package, void *user_data);

/**
 * @brief Called to get the package name once for each installed application.
 *
 * @param[in] package The package name of each installed application
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break out of the loop.
 * @pre	app_manager_foreach_app_installed() will invoke this callback.
 * @see app_manager_foreach_app_installed()
 */
typedef bool (*app_manager_app_installed_cb) (const char *package, void *user_data);

/**
 * @brief  Called when the list of the installed applications changes.
 *
 * @param[in] event_type The type of applist modification event
 * @param[in] package The package name of application installed or uninstalled
 * @param[in] user_data The user data passed from the callback registration function
 * @pre	An application registers this callback using app_manager_set_app_list_changed_cb() 
 *	to detect change of list of the installed applications.
 * @see app_manager_set_app_list_changed_cb()
 * @see app_manager_unset_app_list_changed_cb()
 */
typedef void(*app_manager_app_list_changed_cb) (app_manger_event_type_e event_type, const char *package, void *user_data);

/**
 * @brief Retrieves the package names of all running applications 
 * by invoking the callback once for each running application to get their package names.
 *
 * @param [in] callback The callback function to invoke
 * @param [in] user_data The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @post	This function invokes app_manager_app_running_cb() repeatedly for each running application.
 * @see	app_manager_app_running_cb()
 */
int app_manager_foreach_app_running(app_manager_app_running_cb callback, void *user_data);

/**
 * @brief Retrieves the package names of all installed applications 
 * by invoking the callback once for each installed application to get their package names.
 *
 * @param [in] callback The callback function to invoke
 * @param [in] user_data The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_OUT_OF_MEMORY Out of memory
 * @post	This function invokes app_manager_app_installed_cb() repeatedly for each installed application.
 * @see	app_manager_app_installed_cb()
 */
int app_manager_foreach_app_installed(app_manager_app_installed_cb callback, void *user_data);

/**
 * @brief Checks whether the application with the given package name is running.
 *
 *
 * @param [in] package The package name of the application
 * @param [out] is_running @c true if the application is running, \n @c false if not running.
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 */
int app_manager_is_running(const char *package, bool *is_running);

/**
 * @brief Gets the application name with the given package name.
 *
 * @remarks @a name must be released with free() by you.
 * @param [in] package	The package name of the application
 * @param [out] name	The application name
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_INVALID_PACKAGE Package name is invalid, Can not find the package
 * @retval #APP_MANAGER_ERROR_DB_FAILED Database error occurred
 * @retval #APP_MANAGER_ERROR_OUT_OF_MEMORY Out of memory
 */
int app_manager_get_app_name(const char *package, char **name);

/**
 * @brief Gets the application icon path with the given package name.
 *
 * @remarks @a icon_path must be released with free() by you.
 * @param [in] package	The package name of the application
 * @param [out] icon_path The icon path to represent the application
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_INVALID_PACKAGE Package name is invalid, Can not find the package
 * @retval #APP_MANAGER_ERROR_DB_FAILED Database error occurred
 * @retval #APP_MANAGER_ERROR_OUT_OF_MEMORY Out of memory
 */
int app_manager_get_app_icon_path(const char *package, char **icon_path);

/**
 * @brief Gets the application version with the given package name.
 *
 * @remarks @a version must be released with free() by you.
 * @param [in] package	The package name of the application
 * @param [out] version The version of the application
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_INVALID_PACKAGE Package name is invalid, Can not find the package
 * @retval #APP_MANAGER_ERROR_DB_FAILED Database error occurred
 * @retval #APP_MANAGER_ERROR_OUT_OF_MEMORY Out of memory
 */
int app_manager_get_app_version(const char *package, char **version);

/**
 * @brief   Registers a callback function to be invoked when the list of the installed applications changes.
 *
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return  0 on success, otherwise a negative error value.
 * @retval  #APP_MANAGER_ERROR_NONE On Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @post	It will invoke app_manager_app_list_changed_cb() when the list of installed application changes.
 * @see app_manager_unset_app_list_changed_cb()
 * @see app_manager_app_list_changed_cb()
 */
int app_manager_set_app_list_changed_cb(app_manager_app_list_changed_cb callback, void *user_data);

/**
 * @brief   Unregisters the callback function.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_DB_FAILED Database error occurred
 * @see app_manager_set_app_list_changed_cb()
 * @see app_manager_app_list_changed_cb()
 */
int app_manager_unset_app_list_changed_cb(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_APPFW_APP_MANAGER_H */
