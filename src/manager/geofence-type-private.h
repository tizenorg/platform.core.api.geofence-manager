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

#ifndef __GEOFENCE_TYPE_PRIAVE_H__
#define __GEOFENCE_TYPE_PRIAVE_H__

#include <glib.h>
#include <glib-object.h>
#include "geofence_type.h"

/**
 * @brief This represents the state whether geofence state is IN or not(OUT).
 */

G_BEGIN_DECLS


#define _GEOFENCE_CALLBACK_MAX		100
#define WLAN_BSSID_LEN 	18
#define WLAN_SSID_LEN 18
#define PLACE_NAME_LEN  64
#define ADDRESS_LEN 64

/**
 * @brief Geofence object redefined by GObject.
 */
typedef GObject GeofenceObject;


/**
 * @brief Geofence data.
 */

typedef struct _GeofenceService 	GeofenceService;

/**
 * @brief The geofence signal types
 */
typedef enum {
	_GEOFENCE_SIGNAL_ZONE_IN,
	_GEOFENCE_SIGNAL_ZONE_OUT,
	_GEOFENCE_SIGNAL_EVENT,
	_GEOFENCE_SIGNAL_NUM
} _geofence_signal_e;

/**
 * @brief The geofence manager structure
 */
typedef struct _geofence_manager_s {
	GeofenceObject* object;
	const void* user_cb;
	const void* user_event_cb;
	void* user_data;
	gulong sig_id[_GEOFENCE_SIGNAL_NUM];
	GList* app_fence_list;
} geofence_manager_s;

/**
 * @brief The geofence place structure
 */
typedef struct _place_s{
	char place_name[PLACE_NAME_LEN];
} place_s;

/**
 * @brief The geofence parameter structure
 */
typedef struct _geofence_s{
	geofence_type_e type;
	double latitude;
	double longitude;
	int radius;
	char address[ADDRESS_LEN];
	char bssid[WLAN_BSSID_LEN];
	char ssid[WLAN_SSID_LEN];
    	int place_id;
} geofence_s;

/**
* @brief The geofence complete information structure
*/
typedef struct _geofence_info_s {
	geofence_s *info;
	geofence_state_e state;
	int last_updated_time;
} geofence_info_s;
/**
 * @brief The geofence status structure
 */
typedef struct _geofence_status_s {
	int fence_id;
	geofence_state_e state;
	int seconds;
} geofence_status_s;


G_END_DECLS

#endif /* __GEOFENCE_TYPE_PRIAVE_H__ */
