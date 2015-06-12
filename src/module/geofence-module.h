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

#ifndef __GEOFENCE_MODULE_H__
#define __GEOFENCE_MODULE_H__

#include <gmodule.h>
#include "geofence-type-private.h"

G_BEGIN_DECLS

/**
 * @file geofence-module.h
 * @brief This file contains the structure and enumeration for geofence plug-in development.
 */

/**
 * @addtogroup LocationFW
 * @{
 * @defgroup LocationModules Location Modules
 * @brief  This sub module provides the definitions and structrues for 3rd party plugin modules.
 * @addtogroup LocationModules
 * @{
 */

/**
 * @brief This represents a geofence callback function for geofence plug-in.
 */
//typedef void (*GeofenceModGeofenceStatusCB) (gboolean enabled, gpointer userdata);

/**
 * @brief This represents a geofence callback function for geofence plug-in.
 */
typedef void (*GeofenceModCB) (int fence_id, int state, gpointer userdata);

typedef void (*GeofenceModEventCB) (int place_id, int fence_id, int error, int state, gpointer userdata);

/**
 * @brief This represents APIs declared in a Geofence plug-in for Geofence modules.
 */
typedef struct {
	int (*create)(void *handle, GeofenceModCB geofence_cb, GeofenceModEventCB geofence_event_cb, void *userdata);
	int (*destroy)(void *handle);
	int (*enable_service)(void *handle, int fence_id, bool enable);
	int (*add_geopoint)(void *handle, int place_id, double latitude, double longitude, int radius, const char *address, int *fence_id);
	int (*add_bssid)(void *handle, int place_id, const char *bssid, const char *ssid, geofence_type_e type, int *fence_id);
	int (*add_place)(void *handle, const char *place_name, int *place_id);
	int (*update_place)(void *handle, int place_id, const char *place_name);
	int (*remove_geofence)(void *handle, int fence_id);
	int (*remove_place)(void *handle, int place_id);
	int (*start_geofence)(void *handle, int fence_id);
	int (*stop_geofence)(void *handle, int fence_id);
	int (*get_place_name) (void *handle, int place_id, char **place_name);
	int (*get_list) (void *handle, int place_id, int *fence_amount, int **fence_ids, geofence_h *params);
	int (*get_place_list) (void *handle, int *place_amount, int **place_ids, place_s **params);
} GeofenceModOps;

/**
 * @brief This represents APIs declared in a HW Geofence plug-in for Geofence modules.
 */

/*
typedef struct {
	int (*add_fence)(void *handle, int fence_id, double latitude, double longitude, int radius, int last_state, int monitor_states, int notification_responsiveness, int unknown_timer);
	int (*delete_fence)(void *handle, int fence_id);
	int (*pause_fence)(void *handle, int fence_id);
	int (*resume_fence)(void *handle, int fence_id);
	int (*set_callback)(void *handle, LocModGeofenceStatusCB status_cb, LocModGeofenceCB geofence_cb, void *userdata);
	int (*unset_callback)(void *handle);
} LocModGpsGeofenceOps;
*/

/**
 * @brief This is used for exported APIs in a plug-in for a geofence framework.
 */
#define GEOFENCE_MODULE_API __attribute__((visibility("default"))) G_MODULE_EXPORT

/**
 * @} @}
 */
G_END_DECLS

#endif
