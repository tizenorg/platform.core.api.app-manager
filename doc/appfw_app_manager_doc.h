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


#ifndef __TIZEN_APPFW_APP_MANAGER_DOC_H__
#define __TIZEN_APPFW_APP_MANAGER_DOC_H__

/**
 * @defgroup CAPI_APPLICATION_MANAGER_MODULE Application Manager
 * @ingroup CAPI_APPLICATION_FRAMEWORK
 * @brief The @ref CAPI_APPLICATION_MANAGER_MODULE API provides functions to get information about installed or running applications and manage the applications.
 *
 * @section CAPI_APPLICATION_MANAGER_MODULE_HEADER Required Header
 *   \#include <app_manager.h>
 *
 * @section CAPI_APPLICATION_MANAGER_MODULE_OVERVIEW Overview
 * The @ref CAPI_APPLICATION_MANAGER_MODULE API provides information about applications. There are several different sorts of queries.
 * Two iterator functions step through a list of applications. One is used in running applications(#app_manager_foreach_app_context()), and
 * the other is used in available ("installed") but not necessarily running applications(#app_manager_foreach_app_info()).
 * Each will call a callback function, passing the package name of each application found.
 * A query function will respond whether the application represented by a particular package name is running.
 * Other query functions return static information about an application, such as a name, a type, an icon path, or a version.
 * The API provides functions to manage applications also. By using them, it is possible to resume(#app_manager_resume_app) applications.
 */

/**
 * @defgroup CAPI_APP_CONTEXT_MODULE Application Context
 * @ingroup CAPI_APPLICATION_MANAGER_MODULE
 * @brief The @ref CAPI_APP_CONTEXT_MODULE API contains functions for obtaining information about running applications.
 *
 * @section CAPI_APP_CONTEXT_MODULE_HEADER Required Header
 *   \#include <app_manager.h>
 *
 * @section CAPI_APP_CONTEXT_MODULE_OVERVIEW Overview
 * @remarks The @ref CAPI_APP_CONTEXT_MODULE API provides information about running applications.
 */

/**
 * @defgroup CAPI_APP_INFO_MODULE Application Information
 * @ingroup CAPI_APPLICATION_MANAGER_MODULE
 * @brief The @ref CAPI_APP_INFO_MODULE API contains functions for obtaining information about installed applications.
 *
 * @section CAPI_APP_INFO_MODULE_HEADER Required Header
 *   \#include <app_manager.h>
 *
 * @section CAPI_APP_INFO_MODULE_OVERVIEW Overview
 * @remarks The @ref CAPI_APP_INFO_MODULE API provides two types of functionality:
 * - Provides installed application's information such as name, type, icon path, and exec path.
 * - Supports filtering
 * <table>
 * <tr>
 * <th>Property for filtering</th>
 * <th>Type</th>
 * </tr>
 * <tr>
 * <td>PACKAGE_INFO_PROP_APP_ID</td>
 * <td>String</td>
 * </tr>
 * <tr>
 * <td>PACKAGE_INFO_PROP_APP_TYPE</td>
 * <td>String</td>
 * </tr>
 * <tr>
 * <td>PACKAGE_INFO_PROP_APP_CATEGORY</td>
 * <td>String</td>
 * </tr>
 * <tr>
 * <td>PACKAGE_INFO_PROP_APP_NODISPLAY</td>
 * <td>Boolean</td>
 * </tr>
 * <tr>
 * <td>PACKAGE_INFO_PROP_APP_TASKMANAGE</td>
 * <td>Boolean</td>
 * </tr>
 * </table>

 */

#endif /* __TIZEN_APPFW_APP_MANAGER_DOC_H__ */


