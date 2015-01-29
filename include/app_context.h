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


#ifndef __TIZEN_APPFW_APP_CONTEXT_H
#define __TIZEN_APPFW_APP_CONTEXT_H

#include <sys/types.h>
#include <tizen.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file  app_context.h
 */

/**
 * @addtogroup  CAPI_APP_CONTEXT_MODULE
 * @{
 */


/**
 * @brief  Application context handle.
 * @since_tizen  2.3
 */
typedef struct app_context_s *app_context_h;


/**
 * @brief  Enumeration for Application Context Event.
 * @since_tizen  2.3
 */
typedef enum
{
    APP_CONTEXT_EVENT_LAUNCHED, /**< The application is launched */
    APP_CONTEXT_EVENT_TERMINATED, /**< The application is terminated */
} app_context_event_e;


/**
 * @brief  Destroys the application context handle and releases all its resources.
 * @since_tizen 2.3
 * @param[in]   app_context  The application context handle
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @see app_manager_foreach_app_context()
 * @see app_manager_get_app_context()
 */
int app_context_destroy(app_context_h app_context);


/**
 * @brief    Gets the package with the given application context.
 * @since_tizen 2.3
 * @remarks  This function is @b deprecated. Use app_context_get_app_id() instead. \n
 *           You must release @a package using free().
 * @param[in]   app_context  The application context
 * @param[out]  package      The package of the given application context
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_DB_FAILED          Database error occurred
 * @retval  #APP_MANAGER_ERROR_OUT_OF_MEMORY      Out of memory
 */
int app_context_get_package(app_context_h app_context, char **package);


/**
 * @brief    Gets the application ID with the given application context.
 * @since_tizen 2.3
 * @remarks     You must release @a app_id using free().
 * @param[in]   app_context  The application context
 * @param[out]  app_id       The application ID of the given application context
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_DB_FAILED          Database error occurred
 * @retval  #APP_MANAGER_ERROR_OUT_OF_MEMORY      Out of memory
 */
int app_context_get_app_id(app_context_h app_context, char **app_id);


/**
 * @brief  Gets the process ID with the given application context.
 * @since_tizen 2.3
 * @param[in]   app_context  The application context
 * @param[out]  pid          The process ID of the given application context
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_DB_FAILED          Database error occurred
 * @retval  #APP_MANAGER_ERROR_OUT_OF_MEMORY      Out of memory
 */
int app_context_get_pid(app_context_h app_context, pid_t *pid);


/**
 * @brief  Checks whether the application with the given application context is terminated.
 * @since_tizen 2.3
 * @param[in]   app_context  The application context
 * @param[out]  terminated   @c true if the application is terminated, \n
 *                           otherwise @c false if the application is running
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_DB_FAILED          Database error occurred
 * @retval  #APP_MANAGER_ERROR_OUT_OF_MEMORY      Out of memory
 */
int app_context_is_terminated(app_context_h app_context, bool *terminated);


/**
 * @brief  Checks whether two application contexts are equal.
 * @since_tizen 2.3
 * @param[in]   lhs    The first application context to compare
 * @param[in]   rhs    The second application context to compare
 * @param[out]  equal  @c true if the application contexts are equal, \n
 *                     otherwise @c false if they are not equal
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_DB_FAILED          Database error occurred
 * @retval  #APP_MANAGER_ERROR_OUT_OF_MEMORY      Out of memory
 */
int app_context_is_equal(app_context_h lhs, app_context_h rhs, bool *equal);


/**
 * @brief  Clones the application context handle.
 * @since_tizen 2.3
 * @param[out]  clone        The newly created application context handle, if cloning is successful
 * @param[in]   app_context  The application context
 * @return      @c 0 on success,
 *              otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_DB_FAILED          Database error occurred
 * @retval  #APP_MANAGER_ERROR_OUT_OF_MEMORY      Out of memory
 */
int app_context_clone(app_context_h *clone, app_context_h app_context);


/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_APPFW_APP_CONTEXT_H */
