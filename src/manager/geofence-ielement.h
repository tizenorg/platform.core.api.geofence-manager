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

#ifndef __GEOFENCE_IELEMENT_H__
#define __GEOFENCE_IELEMENT_H__

#include <glib-object.h>
#include "geofence-type-private.h"

/**
 * @file geofence-ielement.h
 * @brief This file contains the internal definitions and structures related to geofence interface.
 */

G_BEGIN_DECLS

enum {
    ZONE_IN,
    ZONE_OUT,
    GEOFENCE_PROXIMITY,
    GEOFENCE_EVENT,
    LAST_SIGNAL
};

#define GEOFENCE_TYPE_IELEMENT                  (geofence_ielement_get_type ())
#define GEOFENCE_IELEMENT(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GEOFENCE_TYPE_IELEMENT, GeofenceIElement))
#define GEOFENCE_IS_IELEMENT(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEOFENCE_TYPE_IELEMENT))
#define GEOFENCE_IELEMENT_GET_INTERFACE(obj)    (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GEOFENCE_TYPE_IELEMENT, GeofenceIElementInterface))

typedef struct _GeofenceIElement          GeofenceIElement;
typedef struct _GeofenceIElementInterface GeofenceIElementInterface;

typedef int (*TYPE_GEOFENCE_CREATE)(GeofenceIElement *self);
typedef int (*TYPE_GEOFENCE_DESTROY)(GeofenceIElement *self);
typedef int (*TYPE_GEOFENCE_ENABLE)(GeofenceIElement *self, int geofence_id, bool enable);
typedef int (*TYPE_GEOFENCE_START)(GeofenceIElement *self, int geofence_id);
typedef int (*TYPE_GEOFENCE_STOP)(GeofenceIElement *self, int geofence_id);
typedef int (*TYPE_GEOFENCE_ADD)(GeofenceIElement *self, geofence_s *params, int *geofence_id);
typedef int (*TYPE_GEOFENCE_ADD_PLACE)(GeofenceIElement *self, const char *place_name, int *place_id);
typedef int (*TYPE_GEOFENCE_UPDATE_PLACE)(GeofenceIElement *self, const char *place_name, int place_id);
typedef int (*TYPE_GEOFENCE_REMOVE)(GeofenceIElement *self, int geofence_id);
typedef int (*TYPE_GEOFENCE_REMOVE_PLACE)(GeofenceIElement *self, int place_id);
typedef int (*TYPE_GEOFENCE_GET_PLACE_NAME)(GeofenceIElement *self, int place_id, char **place_name);
typedef int (*TYPE_GEOFENCE_GET_LIST)(GeofenceIElement *self, int place_id, int *fence_amount, int **fence_ids, geofence_s **params);
typedef int (*TYPE_GEOFENCE_GET_PLACE_LIST)(GeofenceIElement *self, int *place_amount, int **place_ids, place_s **params);

struct _GeofenceIElementInterface {
	GTypeInterface parent_iface;

	TYPE_GEOFENCE_CREATE		create;
	TYPE_GEOFENCE_DESTROY		destroy;
	TYPE_GEOFENCE_ENABLE		enable;
	TYPE_GEOFENCE_START		start;
	TYPE_GEOFENCE_STOP		stop;
	TYPE_GEOFENCE_ADD		add;
	TYPE_GEOFENCE_ADD_PLACE		add_place;
	TYPE_GEOFENCE_UPDATE_PLACE	update_place;
	TYPE_GEOFENCE_REMOVE		remove;
	TYPE_GEOFENCE_REMOVE_PLACE	remove_place;
	TYPE_GEOFENCE_GET_PLACE_NAME	get_place_name;
	TYPE_GEOFENCE_GET_LIST		get_list;
	TYPE_GEOFENCE_GET_PLACE_LIST	get_place_list;
};

GType geofence_ielement_get_type(void);

int geofence_ielement_create(GeofenceIElement *self);
int geofence_ielement_destroy(GeofenceIElement *self);
int geofence_ielement_enable_service(GeofenceIElement *self, int geofence_id, bool enable);
int geofence_ielement_start(GeofenceIElement *self, int geofence_id);
int geofence_ielement_stop(GeofenceIElement *self, int geofence_id);
int geofence_ielement_add(GeofenceIElement *self, geofence_s *params, int *geofence_id);
int geofence_ielement_add_place(GeofenceIElement *self, const char *place_name, int *place_id);
int geofence_ielement_update_place(GeofenceIElement *self, const char *place_name, int place_id);
int geofence_ielement_remove(GeofenceIElement *self, int geofence_id);
int geofence_ielement_remove_place(GeofenceIElement *self, int place_id);
int geofence_ielement_get_place_name(GeofenceIElement *self, int place_id, char **place_name);
int geofence_ielement_get_fence_list(GeofenceIElement *self, int place_id, int *fence_amount, int **fence_ids, geofence_s **params);
int geofence_ielement_get_place_list(GeofenceIElement *self, int *place_amount, int **place_ids, place_s **params);

G_END_DECLS

#endif

