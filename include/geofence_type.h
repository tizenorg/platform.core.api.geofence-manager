/* Copyright 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __TIZEN_GEOFENCE_MANAGER_TYPES_H__
#define __TIZEN_GEOFENCE_MANAGER_TYPES_H__


#include <tizen_type.h>
#include <tizen_error.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup CAPI_GEOFENCE_MANAGER_MODULE
 * @since_tizen 2.4
 * @{
 */

/**
 * @brief Enumerations of error code for Geofence manager.
 * @since_tizen 2.4
 */
typedef enum {
	GEOFENCE_MANAGER_ERROR_NONE = TIZEN_ERROR_NONE,									      /**< Success */
	GEOFENCE_MANAGER_ERROR_OUT_OF_MEMORY           = TIZEN_ERROR_OUT_OF_MEMORY,           /**< Out of memory */
	GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER       = TIZEN_ERROR_INVALID_PARAMETER,       /**< Invalid parameter */
	GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED       = TIZEN_ERROR_PERMISSION_DENIED,       /**< Permission denied */
	GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED           = TIZEN_ERROR_NOT_SUPPORTED,           /**< Not supported */
	GEOFENCE_MANAGER_ERROR_NOT_INITIALIZED         = TIZEN_ERROR_GEOFENCE_MANAGER | 0x01, /**< Geofence Manager is not initialized */
	GEOFENCE_MANAGER_ERROR_INVALID_ID              = TIZEN_ERROR_GEOFENCE_MANAGER | 0x02, /**< Invalid geofence ID */
	GEOFENCE_MANAGER_ERROR_EXCEPTION               = TIZEN_ERROR_GEOFENCE_MANAGER | 0x03, /**< Exception occurs */
	GEOFENCE_MANAGER_ERROR_ALREADY_STARTED         = TIZEN_ERROR_GEOFENCE_MANAGER | 0x04, /**< Geofencing is already started */
	GEOFENCE_MANAGER_ERROR_TOO_MANY_GEOFENCE       = TIZEN_ERROR_GEOFENCE_MANAGER | 0x05, /**< Too many geofence */
	GEOFENCE_MANAGER_ERROR_IPC                     = TIZEN_ERROR_GEOFENCE_MANAGER | 0x06, /**< Error in GPS, Wi-Fi, or BT */
	GEOFENCE_MANAGER_ERROR_DATABASE                = TIZEN_ERROR_GEOFENCE_MANAGER | 0x07, /**< DB error in the server side */
	GEOFENCE_MANAGER_ERROR_PLACE_ACCESS_DENIED     = TIZEN_ERROR_GEOFENCE_MANAGER | 0x08, /**< Access to specified place is denied */
	GEOFENCE_MANAGER_ERROR_GEOFENCE_ACCESS_DENIED  = TIZEN_ERROR_GEOFENCE_MANAGER | 0x09, /**< Access to specified geofence is denied */
}
geofence_manager_error_e;

/**
 * @brief Enumerations for the state of geofence manager.
 * @since_tizen 2.4
 */
typedef enum {
    GEOFENCE_STATE_UNCERTAIN = 0,	/**< Uncertain state of geofence */
    GEOFENCE_STATE_IN,				/**< Geofence In state */
    GEOFENCE_STATE_OUT,				/**< Geofence Out state */
} geofence_state_e;


/**
 * @brief Enumeration for geofence type.
 * @since_tizen 2.4
 */
typedef enum {
    GEOFENCE_TYPE_GEOPOINT = 1,		/**< Geofence is specified by geospatial coordinate */
    GEOFENCE_TYPE_WIFI,				/**< Geofence is specified by Wi-Fi access point */
    GEOFENCE_TYPE_BT,				/**< Geofence is specified by Blutetooth device */
} geofence_type_e;

/**
  * @brief Enumerations for geofence management events.
  * @since_tizen 2.4
  */
typedef enum {
    GEOFENCE_MANAGE_FENCE_ADDED = 0x00,         /**< Geofence is added*/
    GEOFENCE_MANAGE_FENCE_REMOVED,              /**< Geofence is removed */
    GEOFENCE_MANAGE_FENCE_STARTED,              /**< Geofencing is started*/
    GEOFENCE_MANAGE_FENCE_STOPPED,              /**< Geofencing is stopped*/

    GEOFENCE_MANAGE_PLACE_ADDED = 0x10,         /**< Place is added*/
    GEOFENCE_MANAGE_PLACE_REMOVED,              /**< Place is removed*/
    GEOFENCE_MANAGE_PLACE_UPDATED,              /**< Place is updated*/

    GEOFENCE_MANAGE_SETTING_ENABLED = 0x20,     /**< Setting for geofencing is enabled */
    GEOFENCE_MANAGE_SETTING_DISABLED,           /**< Setting for geofencing is disabled */
} geofence_manage_e;

/**
 * @brief The geofence manager handle.
 * @since_tizen 2.4
 */
typedef struct geofence_manager_s *geofence_manager_h;

/**
 * @brief The geofence handle.
 * @since_tizen 2.4
 */
typedef struct geofence_s *geofence_h;

/**
 * @brief The geofence status handle.
 * @since_tizen 2.4
 */
typedef struct geofence_status_s *geofence_status_h;


/**
 * @}
 */


#ifdef __cplusplus
}
#endif


#endif /* __TIZEN_GEOFENCE_MANAGER_TYPES_H__ */
