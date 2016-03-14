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


#ifndef __TIZEN_APPFW_APP_EVENT_H
#define __TIZEN_APPFW_APP_EVENT_H

#include <sys/types.h>
#include <tizen.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file  app_event.h
 */

/**
 * @addtogroup  CAPI_APP_EVENT_MODULE
 * @{
 */


/**
 * @brief  Application event handle.
 * @since_tizen 3.0
 */
typedef struct app_event_s *app_event_h;

/**
 * @brief  Enumeration for application status type.
 * @since_tizen 3.0
 */
typedef enum {
	APP_EVENT_STATUS_TYPE_ALL = 0x00,		/**< All Status */
	APP_EVENT_STATUS_TYPE_ENABLE = 0x01,	/**< Application enable status */
	APP_EVENT_STATUS_TYPE_DISABLE = 0x02,	/**< Application disable status */
} app_event_status_type_e;

/**
 * @brief Enumeration for application event type.
 * @since_tizen 3.0
 */
typedef enum {
	APP_EVENT_ENABLE_APP = 0,	/**< Application enable event */
	APP_EVENT_DISABLE_APP,		/**< Application disable event. */
} app_event_type_e;

/**
 * @brief Enumeration for application event state.
 * @since_tizen 3.0
 */
typedef enum {
	APP_EVENT_STATE_STARTED = 0,	/**< Started event state */
	APP_EVENT_STATE_COMPLETED,		/**< Completed event state*/
	APP_EVENT_STATE_FAIL,			/**< Failed event state */
} app_event_state_e;

/**
 * @brief Called when the app is enabled or disabled.
 * @since_tizen 3.0
 *
 * @param[in] type        The type of the app to be enabled or disabled
 * @param[in] app     The name of the app to be enabled or disabled
 * @param[in] event_type  The event type of the app
 * @param[in] event_state The current event state of the app
 * @param[in] handle       The application event handle
 * @param[in] user_data   The user data passed from app_event_set_event_cb()
 *
 * @see app_event_set_event_cb()
 * @see app_event_unset_event_cb()
 */
typedef void (*app_event_cb) (
		const char *type,
		const char *app,
		app_event_type_e event_type,
		app_event_state_e event_state,
		app_event_h handle,
		void *user_data);

/**
 * @brief Creates a application event handle.
 * @since_tizen 3.0
 * @privlevel public
			 * @privilege %http://tizen.org/privilege/packagemanager.info
 * @remarks You must release @a manager using app_event_destroy().
 *
 * @param[out] handle The application event handle that is newly created on success
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #APP_MANAGER_ERROR_NONE              Successful
 * @retval #APP_MANAGER_ERROR_OUT_OF_MEMORY     Out of memory
 * @retval #APP_MANAGER_ERROR_PERMISSION_DENIED Permission denied
 * @see app_event_destroy()
 */
int app_event_create(app_event_h *handle);

/**
 * @brief Sets the event status of the application when the application is enabled or disabled.
 *        You can combine multiple status using OR operation which you want to listen.
 * @since_tizen 3.0
 *
 * @param [in] handle     The application event handle
 * @param [in] status_type The status of the application
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #APP_MANAGER_ERROR_NONE              Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_REQUEST_FAILED          Internal error
 *
 * @see app_event_status_type_e
 * @see app_event_set_event_cb()
 */
int app_event_set_event_status(app_event_h handle, int status_type);

/**
 * @brief Registers a callback function to be invoked when the application is enabled or disabled
 * @since_tizen 3.0
 * @param[in] handle    The application event handle
 * @param[in] callback   The callback function to be registered
 * @param[in] user_data  The user data to be passed to the callback function
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #APP_MANAGER_ERROR_NONE              Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_PERMISSION_DENIED Permission denied
 * @retval #APP_MANAGER_ERROR_REQUEST_FAILED          Internal error
 * @post app_event_cb() will be invoked.
 *
 * @see app_event_set_event_status()
 * @see app_event_cb()
 * @see app_event_unset_event_cb()
 */
int app_event_set_event_cb(app_event_h handle,
		app_event_cb callback,
		void *user_data);

/**
 * @brief Unregisters the callback function.
 * @since_tizen 3.0
 *
 * @param[in] handle The app event handle
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #APP_MANAGER_ERROR_NONE              Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #APP_MANAGER_ERROR_PERMISSION_DENIED Permission denied
 * @retval #APP_MANAGER_ERROR_REQUEST_FAILED          Internal error
 *
 * @see app_event_cb()
 * @see app_event_set_event_cb()
 */
int app_event_unset_event_cb(app_event_h handle);

/**
 * @brief Destroys the app event handle.
 * @since_tizen 3.0
 *
 * @param[in] handle The app event handle
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #APP_MANAGER_ERROR_NONE              Successful
 * @retval #APP_MANAGER_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see app_event_create()
 */
int app_event_destroy(app_event_h handle);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_APPFW_APP_EVENT_H */

