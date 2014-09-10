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

#include <app_context.h>
#include <app_info.h>


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
	APP_MANAGER_ERROR_IO_ERROR = TIZEN_ERROR_IO_ERROR, /**< Internal I/O error */
	APP_MANAGER_ERROR_NO_SUCH_APP = TIZEN_ERROR_APPLICATION_CLASS | 0x01, /**< No such application */

	APP_MANAGER_ERROR_DB_FAILED = TIZEN_ERROR_APPLICATION_CLASS | 0x03, /**< Database error  */
	APP_MANAGER_ERROR_INVALID_PACKAGE = TIZEN_ERROR_APPLICATION_CLASS | 0x04, /**< Invalid package name */
} app_manager_error_e;


/**
 * @brief Called when an application gets launched or termiated.
 * @param[in] app_context The application context of the application launched or termiated
 * @param[in] event The application context event
 * @param[in] user_data The user data passed from the foreach function
 * @pre This function is called when an application gets launched or terminated after you register this callback using app_manager_set_app_context_event_cb()
 * @see app_manager_set_app_context_event_cb()
 * @see app_manager_unset_app_context_event_cb()
 */
typedef void (*app_manager_app_context_event_cb) (app_context_h app_context, app_context_event_e event, void *user_data);


/**
 * @brief Called to get the application context once for each running application.
 * @param[in] app_context The application context of each running application
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break out of the loop.
 * @pre app_manager_foreach_app_context() will invoke this callback.
 * @see app_manager_foreach_app_context()
 */
typedef bool (*app_manager_app_context_cb) (app_context_h app_context, void *user_data);


/**
 * @internal
 * @brief Called when an application gets installed, terminated or updated.
 * @param[in] app_info The application information of the application installed, terminated or updated
 * @param[in] event The application information event
 * @param[in] user_data The user data passed from the foreach function
 * @pre This function is called when an application gets installed, uninstalled or updated after you register this callback using app_manager_set_app_info_event_cb()
 * @see app_manager_set_app_info_event_cb()
 * @see app_manager_unset_app_info_event_cb()
 */
typedef void (*app_manager_app_info_event_cb) (app_info_h app_info, app_info_event_e event, void *user_data);


/**
 * @internal
 * @brief Called to get the application information once for each installed application.
 * @param[in] app_info The application information of each installed application
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break out of the loop.
 * @pre app_manager_foreach_app_info() will invoke this callback.
 * @see app_manager_foreach_app_info()
 */
typedef bool (*app_manager_app_info_cb) (app_info_h app_info, void *user_data);


/**
 * @brief Registers a callback function to be invoked when the applications gets launched or termiated.
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return  0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE On Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_OUT_OF_MEMORY Out of memory
 * @post	It will invoke app_manager_app_context_event_cb() when the application is launched or termiated.
 * @see app_manager_unset_app_context_event_cb()
 * @see app_manager_app_context_event_cb()
 */
int app_manager_set_app_context_event_cb(app_manager_app_context_event_cb callback, void *user_data);


/**
 * @brief Unregisters the callback function.
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @see app_manager_set_app_event_cb()
 * @see app_manager_app_context_event_cb()
 */
void app_manager_unset_app_context_event_cb(void);


/**
 * @brief Retrieves all application contexts of running applications
 * @param [in] callback The callback function to invoke
 * @param [in] user_data The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @post	This function invokes app_manager_app_context_cb() repeatedly for each application context.
 * @see app_manager_app_context_cb()
 */
int app_manager_foreach_app_context(app_manager_app_context_cb callback, void *user_data);


/**
 * @brief Gets the application context for the given ID of the application
 * @remarks This function returns #APP_MANAGER_ERROR_NO_SUCH_APP if the application with the given application ID is not running \n
 * @a app_context must be released with app_context_destroy() by you.
 * @param [in] app_id The ID of the application
 * @param [out] app_context The application context of the given application ID
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_DB_FAILED Database error occurred
 * @retval #APP_MANAGER_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #APP_MANAGER_ERROR_NO_SUCH_APP No such application
 */
int app_manager_get_app_context(const char *app_id, app_context_h *app_context);


/**
 * @brief Gets the name of the application package for the given process ID of the application
 * @remark This function is @b deprecated. Use app_manager_get_app_id() instead.
 * @remarks This function returns #APP_MANAGER_ERROR_NO_SUCH_APP if the application with the given process ID is not valid \n
 * @a package must be released with free() by you.
 * @param [in] pid The process ID of the given application
 * @param [out] package The package name of the application
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_DB_FAILED Database error occurred
 * @retval #APP_MANAGER_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #APP_MANAGER_ERROR_NO_SUCH_APP No such application
 */
int app_manager_get_package(pid_t pid, char **package);


/**
 * @brief Gets the ID of the application for the given process ID
 * @remarks This function returns #APP_MANAGER_ERROR_NO_SUCH_APP if the application with the given process ID is not valid \n
 * @a app_id must be released with free() by you.
 * @param [in] pid The process ID of the application
 * @param [out] app_id The ID of the application
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_DB_FAILED Database error occurred
 * @retval #APP_MANAGER_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #APP_MANAGER_ERROR_NO_SUCH_APP No such application
 */
int app_manager_get_app_id(pid_t pid, char **app_id);


/**
 * @brief Checks whether the application with the given package name is running.
 * @param [in] app_id The ID of the application
 * @param [out] running @c true if the application is running, \n @c false if not running.
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 */
int app_manager_is_running(const char *app_id, bool *running);


/**
 * @brief Resume the application
 * @param [in] app_context The application context
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 */
int app_manager_resume_app(app_context_h app_context);


/**
 * @brief Terminate the application
 * @param [in] app_context The application context
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 */
int app_manager_terminate_app(app_context_h app_context);


/**
 * @internal
 * @brief Registers a callback function to be invoked when the applications gets installed, uninstalled or updated.
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return  0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE On Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_OUT_OF_MEMORY Out of memory
 * @post	It will invoke app_manager_app_info_event_cb() when the application gets installed, uninstalled or updated.
 * @see app_manager_unset_app_info_event_cb()
 * @see app_manager_app_info_event_cb()
 */
int app_manager_set_app_info_event_cb(app_manager_app_info_event_cb callback, void *user_data);


/**
 * @internal
 * @brief Unregisters the callback function.
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @see app_manager_set_app_info_event_cb()
 * @see app_manager_app_info_event_cb()
 */
void app_manager_unset_app_info_event_cb(void);


/**
 * @internal
 * @brief Retrieves all application information of installed applications
 * @param [in] callback The callback function to invoke
 * @param [in] user_data The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @post	This function invokes app_manager_app_info_cb() repeatedly for each application information.
 * @see app_manager_app_info_cb()
 */
int app_manager_foreach_app_info(app_manager_app_info_cb callback, void *user_data);


/**
 * @internal
 * @brief Gets the application information for the given application ID
 * @remarks @a app_info must be released with app_info_destroy() by you.
 * @param [in] app_id The ID of the application
 * @param [out] app_info The application information for the given application ID
 * @return 0 on success, otherwise a negative error value.
 * @retval #APP_MANAGER_ERROR_NONE Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_DB_FAILED Database error occurred
 * @retval #APP_MANAGER_ERROR_OUT_OF_MEMORY Out of memory
 */
int app_manager_get_app_info(const char *app_id, app_info_h *app_info);



#define __START_OF_DEPRECATED_API
typedef bool (*app_manager_ui_app_info_cb) (ui_app_info_h ui_app_info, void *user_data);
typedef bool (*app_manager_service_app_info_cb) (service_app_info_h service_app_info, void *user_data);
int app_manager_foreach_ui_app_info(app_manager_ui_app_info_cb callback, void *user_data);
int app_manager_get_ui_app_info(const char *app_id, ui_app_info_h *ui_app_info);
int app_manager_foreach_service_app_info(app_manager_service_app_info_cb callback, void *user_data);
int app_manager_get_service_app_info(const char *app_id, service_app_info_h *service_app_info);
#define __END_OF_DEPRECATED_API


/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_APPFW_APP_MANAGER_H */

