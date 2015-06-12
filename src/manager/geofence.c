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

#include <glib.h>
#include <stdio.h>
#include <pthread.h>

#include "geofence_type.h"
#include "geofence-internal.h"
#include "geofence-log.h"
#include "module-internal.h"
#include "geofence.h"

int geofence_init(void)
{
	GEOFENCE_LOGD("geofence_init");

#if !GLIB_CHECK_VERSION(2, 35, 0)
	g_type_init();
#endif

#if !GLIB_CHECK_VERSION(2, 31, 0)
	if (!g_thread_supported()) g_thread_init(NULL);
#endif
	if (FALSE == module_init())
		return GEOFENCE_MANAGER_ERROR_NOT_INITIALIZED;

	return GEOFENCE_MANAGER_ERROR_NONE;
}

GeofenceObject *geofence_new(void)
{
	GEOFENCE_LOGD("geofence_new");

	GeofenceObject *self = NULL;

	self = g_object_new(GEOFENCE_TYPE_INTERNAL, NULL);

	if (!self) GEOFENCE_LOGD("Fail to create geofence object. [%d]");
	return self;
}

int geofence_free(GeofenceObject *obj)
{
	GEOFENCE_LOGD("geofence_free");

	g_return_val_if_fail(obj, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_object_unref(obj);
	return GEOFENCE_MANAGER_ERROR_NONE;
}

gboolean geofence_is_supported(void)
{
	GEOFENCE_LOGD("geofence_is_supported");

	gboolean supported = FALSE;

	if (module_is_supported("geofence")) {
		supported = TRUE;
	}

	return supported;
}

