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

#ifndef __TIZEN_APPFW_APP_MANAGER_EXTENSION_H
#define __TIZEN_APPFW_APP_MANAGER_EXTENSION_H

#include <stdbool.h>

#include "app_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file app_manager_extension.h
 */

/**
 * @addtogroup CAPI_APPLICATION_MANAGER_MODULE
 * @{
 */

/**
 * @brief  Terminates the application.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
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
 * @brief Sets the display flag to enable/disable the splash screen.
 * @since_tizen 3.0
 * @privilege platform
 * @privilege %http://tizen.org/privilege/packagemanager.admin
 * @param[in]     app_id  The ID of the application
 * @param[in]     display The display flag to enable/disable the splash screen
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #APP_MANAGER_ERROR_NONE               Successful
 * @retval  #APP_MANAGER_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #APP_MANAGER_ERROR_OUT_OF_MEMORY      Out of memory
 * @retval  #APP_MANAGER_ERROR_PERMISSION_DENIED  Permission denied
 * @retval  #APP_MANAGER_ERROR_IO_ERROR           Internal I/O error
 */
int app_manager_set_splash_screen_display(const char *app_id, bool display);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_APPFW_APP_MANAGER_EXTENSION_H */

