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

#ifndef __GEOFENCE_PARAM_H__
#define __GEOFENCE_PARAM_H__

#include "geofence-type-private.h"

G_BEGIN_DECLS

GType geofence_parameter_get_type (void);
#define GEOFENCE_PRAMETER (geofence_parameter_get_type ())

geofence_s* geofence_parameter_new (gint place_id, guint type, gdouble latitude, gdouble longitude, guint radius, const gchar* address, const gchar* bssid, const gchar* ssid);
geofence_s* geofence_parameter_copy (const geofence_s *params);
void geofence_parameter_free (geofence_s* params);

G_END_DECLS

#endif
