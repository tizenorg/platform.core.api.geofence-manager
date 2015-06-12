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

#ifndef __GEOFENCE_DATA_H__
#define __GEOFENCE_DATA_H__

#include "module-internal.h"
#include "geofence-type-private.h"

G_BEGIN_DECLS

struct _GeofenceService {
	GeofenceInternalMod *mod;
	gboolean enabled;

	GList *geocoordinate_list;
	GList *specificAP_list;
	GList *currentlocation_list;

	void *service_cb;
	void *state_cb;
	void *user_data;
};

G_END_DECLS

#endif /* __GEOFENCE_DATA_H__ */

