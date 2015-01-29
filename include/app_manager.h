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
 * @file app_manager.h
 */

/**
 * @addtogroup CAPI_APPLICATION_MANAGER_MODULE
 * @{
 */

/**
 * @brief Enumerations for Application Manager Error .
 * @since_tizen 2.3
 */
typedef enum
{
    APP_MANAGER_ERROR_NONE = TIZEN_ERROR_NONE, /**< Successful */
    APP_MANAGER_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER, /**< Invalid parameter */
    APP_MANAGER_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY, /**< Out of memory */
    APP_MANAGER_ERROR_IO_ERROR = TIZEN_ERROR_IO_ERROR, /**< Internal I/O error */
    APP_MANAGER_ERROR_NO_SUCH_APP = TIZEN_ERROR_APPLICATION_MANAGER | 0x01, /**< No such application */

    APP_MANAGER_ERROR_DB_FAILED = TIZEN_ERROR_APPLICATION_MANAGER | 0x03, /**< Database error  */
    APP_MANAGER_ERROR_INVALID_PACKAGE = TIZEN_ERROR_APPLICATION_MANAGER | 0x04, /**< Invalid package name */
    APP_MANAGER_ERROR_APP_NO_RUNNING = TIZEN_ERROR_APPLICATION_MANAGER | 0x05, /**< App is not running */
    APP_MANAGER_ERROR_REQUEST_FAILED = TIZEN_ERROR_APPLICATION_MANAGER | 0x06, /**< Internal aul request error */
    APP_MANAGER_ERROR_PERMISSION_DENIED = TIZEN_ERROR_PERMISSION_DENIED /**< Permission denied */
} app_manager_error_e;

/**
 * @internal
 * @brief  Called when an application is launched or terminated.
 * @since_tizen 2.3
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
 * @since_tizen 2.3
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
 * @since_tizen 2.3
 * @param[in]   app_info   The application information of each installed application
 * @param[in]   user_data  The user data passed from the foreach function
 * @return      @c true to continue with the next iteration of the loop, \n
 *              otherwise @c false to break out of the loop
 * @pre app_manager_foreach_app_info() will invoke this callback.
 * @see app_manager_foreach_app_info()
 */
typedef bool (*app_manager_app_info_cb) (app_info_h app_info, void *user_data);

/**
 * @internal
 * @brief  Registers a callback function to be invoked when the applications get launched or terminated.
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/packagemanager.info
 * @param[in]   callback   The callback function to register
 * @param[in]   user_data  The user data to be passed to the callback function
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_OUT_OF_MEMORY      Out of memory
 * @retval #APP_MANAGER_ERROR_PERMISSION_DENIED Permission denied
 * @post It will invoke app_manager_app_context_event_cb() when the application is launched or terminated.
 * @see app_manager_unset_app_context_event_cb()
 * @see app_manager_app_context_event_cb()
 */
int app_manager_set_app_context_event_cb(app_manager_app_context_event_cb callback, void *user_data);

/**
 * @internal
 * @brief   Unregisters the callback function.
 * @since_tizen 2.3
 * @see app_manager_set_app_event_cb()
 * @see app_manager_app_context_event_cb()
 */
void app_manager_unset_app_context_event_cb(void);

/**
 * @brief  Retrieves all application contexts of running applications.
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/packagemanager.info
 * @param[in]   callback   The callback function to invoke
 * @param[in]   user_data  The user data to be passed to the callback function
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #APP_MANAGER_ERROR_PERMISSION_DENIED Permission denied
 * @post   This function invokes app_manager_app_context_cb() for each application context.
 * @see app_manager_app_context_cb()
 */
int app_manager_foreach_app_context(app_manager_app_context_cb callback, void *user_data);

/**
 * @brief  Gets the application context for the given ID of the application.
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/packagemanager.info
 * @remarks  This function returns #APP_MANAGER_ERROR_NO_SUCH_APP if the application with the given application ID is not running. \n
 *           You must release @a app_context using app_context_destroy().
 * @param[in]   app_id       The ID of the application
 * @param[out]  app_context  The application context of the given application ID
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_DB_FAILED          Database error occurred
 * @retval  #APP_MANAGER_ERROR_OUT_OF_MEMORY      Out of memory
 * @retval  #APP_MANAGER_ERROR_NO_SUCH_APP        No such application
 * @retval #APP_MANAGER_ERROR_PERMISSION_DENIED Permission denied
 */
int app_manager_get_app_context(const char *app_id, app_context_h *app_context);

/**
 * @brief  Gets the ID of the application for the given process ID.
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/packagemanager.info
 * @remarks  This function returns #APP_MANAGER_ERROR_NO_SUCH_APP if the application with the given process ID is not valid. \n
 *           You must release @a app_id using free().
 * @param[in]   pid     The process ID of the application
 * @param[out]  app_id  The ID of the application
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_DB_FAILED          Database error occurred
 * @retval  #APP_MANAGER_ERROR_OUT_OF_MEMORY      Out of memory
 * @retval  #APP_MANAGER_ERROR_NO_SUCH_APP        No such application
 * @retval #APP_MANAGER_ERROR_PERMISSION_DENIED Permission denied
 */
int app_manager_get_app_id(pid_t pid, char **app_id);

/**
 * @brief  Checks whether the application with the given package name is running.
 * @since_tizen 2.3
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
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/appmanager.launch
 * @param[in]   app_context  The application context
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_APP_NO_RUNNING  App is not running
 * @retval  #APP_MANAGER_ERROR_REQUEST_FAILED  Internal resume error
 * @retval #APP_MANAGER_ERROR_PERMISSION_DENIED Permission denied
 */
int app_manager_resume_app(app_context_h app_context);

/**
 * @internal
 * @brief  Opens the application.
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/appmanager.launch
 * @param[in]   app_id  The ID of the application
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_NO_SUCH_APP  No such application
 * @retval  #APP_MANAGER_ERROR_REQUEST_FAILED  Internal open error
 * @retval #APP_MANAGER_ERROR_PERMISSION_DENIED Permission denied
 */
int app_manager_open_app(const char *app_id);

/**
 * @internal
 * @brief  Terminates the application.
 * @since_tizen 2.3
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/appmanager.kill
 * @param[in]   app_context  The application context
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_REQUEST_FAILED  Internal terminate error
 * @retval #APP_MANAGER_ERROR_PERMISSION_DENIED Permission denied
 */
int app_manager_terminate_app(app_context_h app_context);

/**
 * @brief  Retrieves all installed applications information.
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/packagemanager.info
 * @param[in]   callback   The callback function to invoke
 * @param[in]   user_data  The user data to be passed to the callback function
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #APP_MANAGER_ERROR_PERMISSION_DENIED Permission denied
 * @post    This function invokes app_manager_app_info_cb() for each application information.
 * @see app_manager_app_info_cb()
 */
int app_manager_foreach_app_info(app_manager_app_info_cb callback, void *user_data);

/**
 * @brief  Gets the application information for the given application ID.
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/packagemanager.info
 * @remarks     You must release @a app_info using app_info_destroy().
 * @param[in]   app_id    The ID of the application
 * @param[out]  app_info  The application information for the given application ID
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_DB_FAILED          Database error occurred
 * @retval  #APP_MANAGER_ERROR_OUT_OF_MEMORY      Out of memory
 * @retval #APP_MANAGER_ERROR_PERMISSION_DENIED Permission denied
 */
int app_manager_get_app_info(const char *app_id, app_info_h *app_info);

/**
 * @brief  Gets the absolute path to the shared data directory of the application specified
 *         with an application ID.
 * @details     An application can only read the files of other application's shared data directory.
 * @since_tizen 2.3
 * @remarks     The specified @a path should be released.
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
int app_manager_get_shared_data_path(const char *app_id, char **path);

/**
 * @brief  Gets the absolute path to the shared resource directory of the application specified
 *         with an application ID.
 * @details     An application can only read the files of other application's shared resource directory.
 * @since_tizen 2.3
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
 * @since_tizen 2.3
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
 * @brief  Gets the absolute path to the shared data directory of the application specified
 *         with an application ID.
 * @details     An application can only read the files of other application's shared data directory.
 * @since_tizen 2.3
 * @remarks     The specified @a path should be released.
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
