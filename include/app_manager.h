/*
 * Copyright (c) 2011 - 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file app_manager.h
 */

/**
 * @addtogroup CAPI_APPLICATION_MANAGER_MODULE
 * @{
 */

/**
 * @brief Enumerations for Application Manager Error .
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum {
	APP_MANAGER_ERROR_NONE = TIZEN_ERROR_NONE, /**< Successful */
	APP_MANAGER_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER, /**< Invalid parameter */
	APP_MANAGER_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY, /**< Out of memory */
	APP_MANAGER_ERROR_IO_ERROR = TIZEN_ERROR_IO_ERROR, /**< Internal I/O error */
	APP_MANAGER_ERROR_NO_SUCH_APP = TIZEN_ERROR_APPLICATION_MANAGER | 0x01, /**< No such application */
	APP_MANAGER_ERROR_DB_FAILED = TIZEN_ERROR_APPLICATION_MANAGER | 0x03, /**< Database error  */
	APP_MANAGER_ERROR_INVALID_PACKAGE = TIZEN_ERROR_APPLICATION_MANAGER | 0x04, /**< Invalid package name */
	APP_MANAGER_ERROR_APP_NO_RUNNING = TIZEN_ERROR_APPLICATION_MANAGER | 0x05, /**< App is not running */
	APP_MANAGER_ERROR_REQUEST_FAILED = TIZEN_ERROR_APPLICATION_MANAGER | 0x06, /**< Internal aul request error */
	APP_MANAGER_ERROR_PERMISSION_DENIED = TIZEN_ERROR_PERMISSION_DENIED, /**< Permission denied */
	APP_MANAGER_ERROR_NOT_SUPPORTED = TIZEN_ERROR_NOT_SUPPORTED /**< Not supported (Since 3.0) */
} app_manager_error_e;

/**
 * @brief Enumeration for application status type.
 * @since_tizen 3.0
 */
typedef enum {
	APP_MANAGER_APP_STATUS_TYPE_ALL = 0x00,     /**< All status */
	APP_MANAGER_APP_STATUS_TYPE_ENABLE = 0x01,  /**< Application enable status*/
	APP_MANAGER_APP_STATUS_TYPE_DISABLE = 0x02, /**< Application disable status */
} app_manager_app_enable_status_type_e;

/**
 * @brief Enumeration for application event type.
 * @since_tizen 3.0
 */
typedef enum {
	APP_MANAGER_EVENT_ENABLE_APP = 0,  /**< Application enable event */
	APP_MANAGER_EVENT_DISABLE_APP,     /**< Application disable event */
} app_event_type_e;

/**
 * @brief Enumeration for application event state.
 * @since_tizen 3.0
 */
typedef enum {
	APP_MANAGER_EVENT_STATE_STARTED = 0,  /**< Started event state */
	APP_MANAGER_EVENT_STATE_COMPLETED,    /**< Started completed state */
	APP_MANAGER_EVENT_STATE_FAIL,         /**< Started fail state */
} app_event_state_e;

/**
 * @brief  Called when an application is launched or terminated.
 * @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
 * @param[in]   app_context  The application context of the application launched or terminated
 * @param[in]   event        The application context event
 * @param[in]   user_data    The user data passed from the foreach function
 * @pre  This function is called when an application gets launched or terminated, after you register this callback using app_manager_set_app_context_event_cb().
 * @see app_manager_set_app_context_event_cb()
 * @see app_manager_unset_app_context_event_cb()
 */
typedef void (*app_manager_app_context_event_cb) (app_context_h app_context, app_context_event_e event, void *user_data);

/**
 * @brief  Called to get the application context once for each running application.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in]   app_context  The application context of each running application
 * @param[in]   user_data    The user data passed from the foreach function
 * @return      @c true to continue with the next iteration of the loop, \n
 *              otherwise @c false to break out of the loop
 * @pre app_manager_foreach_app_context() will invoke this callback.
 * @see app_manager_foreach_app_context()
 */
typedef bool (*app_manager_app_context_cb) (app_context_h app_context, void *user_data);

/**
 * @brief  Called to get the application information once for each installed application.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in]   app_info   The application information of each installed application
 * @param[in]   user_data  The user data passed from the foreach function
 * @return      @c true to continue with the next iteration of the loop, \n
 *              otherwise @c false to break out of the loop
 * @pre app_manager_foreach_app_info() will invoke this callback.
 * @see app_manager_foreach_app_info()
 */
typedef bool (*app_manager_app_info_cb) (app_info_h app_info, void *user_data);

/**
 * @brief  Called when an application has enabled or disabled
 * @since_tizen 3.0
 * @param[in]   application   The application ID
 * @param[in]   event_type  Type of event which is occured
 * @param[in]   event_state  Status of event
 * @param[in]   user_data  User data given when register this callback
 * @pre app_manager_set_app_status_cb() will register this callback
 * @see app_manager_set_app_status_cb()
 * @see app_manager_unsetset_app_status_cb()
 */
typedef void (*app_manager_app_enable_cb) (const char *app_id, app_event_type_e event_type, app_event_state_e event_state, void *user_data);

/**
 * @brief The Application manager event handle.
 * @since_tizen 3.0
 */
typedef struct _app_manager_event_s *app_manager_event_h;

/**
 * @brief  Unregisters a application event callback function
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/packagemanager.info
 * @param[in]   manager   App manager handle given when registering event callback function
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_REQUEST_FAILED  Internal error
 * @retval  #APP_MANAGER_ERROR_PERMISSION_DENIED  Permission denied
 * @retval  #APP_MANAGER_ERROR_OUT_OF_MEMORY      Out of memory
 * @retval  #APP_MANAGER_ERROR_IO_ERROR  Internal I/O error
 * @see app_manager_set_app_enable_cb()
 */
int app_manager_unset_app_enable_cb(app_manager_event_h manager);

/**
 * @brief  Registers a callback function to be invoked when the application is enabled or disabled.
 * @details Application can be enabled or disabled independently of whether its package has disabled or not.
 *          Disabled application cannot be seen at homescreen and cannot be launched.
 *          This API let application listen enable or disable event of system and invoke callback function.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/packagemanager.info
 * @param[in]   status_type   Which event state want to listen
 * @param[in]   callback   The callback function to register
 * @param[in]   user_data  The user data to be passed to the callback function
 * @param[out]   manager   App manager handle  that is newly created on success
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_OUT_OF_MEMORY      Out of memory
 * @retval  #APP_MANAGER_ERROR_PERMISSION_DENIED  Permission denied
 * @retval  #APP_MANAGER_ERROR_REQUEST_FAILED  Internal error
 * @see app_manager_unset_app_enable_cb()
 * @see app_manager_app_enable_status_type_e
 */
int app_manager_set_app_enable_cb(int status_type, app_manager_app_enable_cb callback, void *user_data, app_manager_event_h *manager);

/**
 * @brief  Registers a callback function to be invoked when the applications get launched or terminated.
 * @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
 * @param[in]   callback   The callback function to register
 * @param[in]   user_data  The user data to be passed to the callback function
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_OUT_OF_MEMORY      Out of memory
 * @post It will invoke app_manager_app_context_event_cb() when the application is launched or terminated.
 * @see app_manager_unset_app_context_event_cb()
 * @see app_manager_app_context_event_cb()
 */
int app_manager_set_app_context_event_cb(app_manager_app_context_event_cb callback, void *user_data);

/**
 * @brief   Unregisters the callback function.
 * @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
 * @see app_manager_set_app_event_cb()
 * @see app_manager_app_context_event_cb()
 */
void app_manager_unset_app_context_event_cb(void);

/**
 * @brief  Retrieves all application contexts of running applications.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in]   callback   The callback function to invoke
 * @param[in]   user_data  The user data to be passed to the callback function
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @post   This function invokes app_manager_app_context_cb() for each application context.
 * @see app_manager_app_context_cb()
 */
int app_manager_foreach_app_context(app_manager_app_context_cb callback, void *user_data);


/**
 * @brief  Retrieves all application contexts of running applications including sub apps.
 * @details You can iterate all the running application contexts using this API.
 *          This API is similar to app_manager_foreach_app_context() except that this API iterates application contexts including sub apps in application groups.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/packagemanager.info
 * @param[in]   callback   The callback function to invoke
 * @param[in]   user_data  The user data to be passed to the callback function
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_PERMISSION_DENIED  Permission denied
 * @post   This function invokes app_manager_app_context_cb() for each application context.
 * @see app_manager_app_context_cb()
 */
int app_manager_foreach_running_app_context(app_manager_app_context_cb callback, void *user_data);


/**
 * @brief  Gets the application context for the given ID of the application.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks  This function returns #APP_MANAGER_ERROR_NO_SUCH_APP if the application with the given application ID is not running. \n
 *           You must release @a app_context using app_context_destroy().
 * @param[in]   app_id       The ID of the application
 * @param[out]  app_context  The application context of the given application ID
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_OUT_OF_MEMORY      Out of memory
 * @retval  #APP_MANAGER_ERROR_NO_SUCH_APP        No such application
 */
int app_manager_get_app_context(const char *app_id, app_context_h *app_context);

/**
 * @brief  Gets the ID of the application for the given process ID.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks  This function returns #APP_MANAGER_ERROR_NO_SUCH_APP if the application with the given process ID is not valid. \n
 *           You must release @a app_id using free().
 * @param[in]   pid     The process ID of the application
 * @param[out]  app_id  The ID of the application
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_OUT_OF_MEMORY      Out of memory
 */
int app_manager_get_app_id(pid_t pid, char **app_id);

/**
 * @brief  Checks whether the application with the given package name is running.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in]   app_id   The ID of the application
 * @param[out]  running  @c true if the application is running, \n
 *                       otherwise @c false if not running
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 */
int app_manager_is_running(const char *app_id, bool *running);

/**
 * @brief  Resumes the application.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/appmanager.launch
 * @param[in]   app_context  The application context
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_APP_NO_RUNNING     App is not running
 * @retval  #APP_MANAGER_ERROR_REQUEST_FAILED     Internal resume error
 * @retval  #APP_MANAGER_ERROR_PERMISSION_DENIED  Permission denied
 */
int app_manager_resume_app(app_context_h app_context);

/**
 * @brief  Terminates the back ground application.\n
 *         UI applications that are in paused state or some service applications could be required to terminate by this API.
 * @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/appmanager.kill.bgapp
 * @remarks This function returns after it just sends a request for terminating a background application.\n
 *          Platform will decide if the target application could be terminated or not according to the state of the target application.
 * @param[in]   app_context  The application context
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_REQUEST_FAILED  Failed to send terminate request
 * @retval #APP_MANAGER_ERROR_PERMISSION_DENIED Permission denied
 */
int app_manager_request_terminate_bg_app(app_context_h app_context);

/**
 * @brief  Retrieves all installed applications information.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in]   callback   The callback function to invoke
 * @param[in]   user_data  The user data to be passed to the callback function
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @post    This function invokes app_manager_app_info_cb() for each application information.
 * @see app_manager_app_info_cb()
 */
int app_manager_foreach_app_info(app_manager_app_info_cb callback, void *user_data);

/**
 * @brief  Gets the application information for the given application ID.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks     You must release @a app_info using app_info_destroy().
 * @param[in]   app_id    The ID of the application
 * @param[out]  app_info  The application information for the given application ID
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_OUT_OF_MEMORY      Out of memory
 * @retval  #APP_MANAGER_ERROR_NO_SUCH_APP        No such application
 */
int app_manager_get_app_info(const char *app_id, app_info_h *app_info);

/**
 * @deprecated Deprecated since 3.0.
 * @brief  Gets the absolute path to the shared data directory of the application specified
 *         with an application ID.
 * @details     An application can only read the files of other application's shared data directory.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks     The specified @a path should be released. @n
 *		shared/data directory is not supported since Tizen 3.0.
 *		You MUST NOT use this API when you develop new application.
 *		Actually, we strongly recommend to stop using shared/data path for all your previous applications.
 *		Files in shared/data directory can be read by all other applications.
 *		You cannot control what applications can read the files in shared/data directory.
 *		If you want to share files with other applications, consider passing path via @ref CAPI_APP_CONTROL_MODULE API.
 *		The @ref CAPI_APP_CONTROL_MODULE API supports giving permission to other applications by passing path via app_control. @n
 *		shared/data directory is only available for applications with api-version lower than 3.0 from Tizen 3.0 platform.
 *		The applications with api-version from 3.0 cannot access other applications' shared/data directory.
 *		For example, a Tizen 2.4 application can access another Tizen 2.4 application's shared/data directory as it did in Tizen 2.4 platform.
 *		However, a Tizen 3.0 application cannot access another application's shared/data directory even the another application is Tizen 2.4 application.
 *		Note that Tizen 3.0 platform only supports share/data directory among applications with api-version lower than 3.0 for minimum backward compatibility.
 *
 * @param[in]      app_id  The ID of the application
 * @param[in,out]  path    The absolute path to the shared data directory of the application
 *                         specified with the @a app_id
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_NO_SUCH_APP        No such application
 * @retval  #APP_MANAGER_ERROR_OUT_OF_MEMORY      Out of memory
 * @retval  #APP_MANAGER_ERROR_NOT_SUPPORTED	  Not supported
 */
int app_manager_get_shared_data_path(const char *app_id, char **path);

/**
 * @brief  Gets the absolute path to the shared resource directory of the application specified
 *         with an application ID.
 * @details     An application can only read the files of other application's shared resource directory.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks     The specified @a path should be released.
 *
 * @param[in]      app_id  The ID of the application
 * @param[in,out]  path    The absolute path to the shared resource directory of the application
 *                         specified with the @a app_id
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_NO_SUCH_APP        No such application
 * @retval  #APP_MANAGER_ERROR_OUT_OF_MEMORY      Out of memory
 */
int app_manager_get_shared_resource_path(const char *app_id, char **path);

/**
 * @brief  Gets the absolute path to the shared trusted directory of the application specified
 *         with an application ID.
 * @details     An application signed with the same certificate can read and write the files of
 *              other application's shared trusted directory.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks     The specified @a path should be released.
 *
 * @param[in]      app_id  The ID of the application
 * @param[in,out]  path    The absolute path to the shared trusted directory of the application
 *                         specified with the @a app_id
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_NO_SUCH_APP        No such application
 * @retval  #APP_MANAGER_ERROR_OUT_OF_MEMORY      Out of memory
 */
int app_manager_get_shared_trusted_path(const char *app_id, char **path);

/**
 * @deprecated Deprecated since @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif.
 * @brief  Gets the absolute path to the shared data directory of the application specified
 *         with an application ID.
 * @details     An application can only read the files of other application's shared data directory.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks     The specified @a path should be released.
 * @remarks     To access the path returned by this function may not work as intended in certain devices due to some implementation issues.
 *
 * @param[in]      app_id  The ID of the application
 * @param[in,out]  path    The absolute path to the shared data directory of the application
 *                         specified with the @a app_id
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_NO_SUCH_APP        No such application
 * @retval  #APP_MANAGER_ERROR_OUT_OF_MEMORY      Out of memory
 */
int app_manager_get_external_shared_data_path(const char *app_id, char **path);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_APPFW_APP_MANAGER_H */

