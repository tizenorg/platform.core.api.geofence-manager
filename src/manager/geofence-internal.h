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

#ifndef __GEOFENCE_INTERNAL_H__
#define __GEOFENCE_INTERNAL_H__

#include "geofence-ielement.h"

/**
 * @file geofence-internal.h
 * @brief This file contains the internal definitions and structures related to Geofence method.
 */

G_BEGIN_DECLS

#define GEOFENCE_TYPE_INTERNAL					(geofence_internal_get_type())
#define GEOFENCE_INTERNAL(obj)					(G_TYPE_CHECK_INSTANCE_CAST((obj), GEOFENCE_TYPE_INTERNAL, GeofenceInternal))
#define GEOFENCE_IS_INTERNAL(obj)				(G_TYPE_CHECK_INSTANCE_TYPE((obj), GEOFENCE_TYPE_INTERNAL))
#define GEOFENCE_INTERNAL_CLASS(klass)			(G_TYPE_CHECK_CLASS_CAST((klass), GEOFENCE_TYPE_INTERNAL, GeofenceInternalClass))
#define GEOFENCE_IS_INTERNAL_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE((klass), GEOFENCE_TYPE_INTERNAL))
#define GEOFENCE_INTERNAL_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS((obj), GEOFENCE_TYPE_INTERNAL, GeofenceInternalClass))

typedef struct _GeofenceInternal 	GeofenceInternal;
typedef struct _GeofenceInternalClass	GeofenceInternalClass;

struct _GeofenceInternal {
	GObject parent_instance;
};

struct _GeofenceInternalClass {
	GObjectClass parent_class;

	void (*zone_in)(guint geofence_id);
	void (*zone_out)(guint geofence_id);
	void (*geofence_proximity)(guint geofence_id, guint proximity_state, guint provider);
	void (*geofence_event)(guint place_id, guint geofence_id, guint error, guint state);
};

/*typedef void (*service_status_cb) (int status, void *user_data); */

GType geofence_internal_get_type(void);

G_END_DECLS

#endif
