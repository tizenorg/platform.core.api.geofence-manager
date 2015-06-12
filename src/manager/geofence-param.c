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

#include "geofence-log.h"
#include "geofence-param.h"


GType geofence_parameter_get_type(void)
{
	static volatile gsize type_volatile = 0;
	if (g_once_init_enter(&type_volatile)) {
		GType type = g_boxed_type_register_static(g_intern_static_string("geofence_s"), (GBoxedCopyFunc) geofence_parameter_copy, (GBoxedFreeFunc) geofence_parameter_free);
		g_once_init_leave(&type_volatile, type);
	}
	return type_volatile;
}

geofence_s *geofence_parameter_new(gint place_id, guint type, gdouble latitude, gdouble longitude, guint radius, const gchar *address, const gchar *bssid, const gchar *ssid)
{
    GEOFENCE_LOGD("geofence_parameter_new");

	if (place_id < 0) return NULL;
	if (type == GEOFENCE_TYPE_GEOPOINT && (latitude < -90 || latitude > 90)) return NULL;
	if (type == GEOFENCE_TYPE_GEOPOINT && (longitude < -180 || longitude > 180)) return NULL;
	if (type == GEOFENCE_TYPE_GEOPOINT && (radius < 100 || radius > 500)) return NULL;
	if (type == GEOFENCE_TYPE_GEOPOINT && address == NULL) return NULL;
	if (type != GEOFENCE_TYPE_GEOPOINT && (bssid == NULL) && (ssid == NULL)) return NULL;

	geofence_s *param = g_slice_new0(geofence_s);
	g_return_val_if_fail(param, NULL);

	param->place_id = place_id;
	param->type = type;
	param->latitude = latitude;
	param->longitude = longitude;
	param->radius = radius;
	if (type != GEOFENCE_TYPE_GEOPOINT) {
		g_strlcpy(param->bssid, bssid, WLAN_BSSID_LEN);
		g_strlcpy(param->ssid, ssid, WLAN_BSSID_LEN);
	} else
		g_strlcpy(param->address, address, ADDRESS_LEN);

	GEOFENCE_LOGD("geofence_parameter_new : type[%d], place_id[%d], latitude[%f], longitude[%f], radius[%d]", param->type, param->place_id, param->latitude, param->longitude, param->radius);

	return param;
}

geofence_s *geofence_parameter_copy(const geofence_s *params)
{
	g_return_val_if_fail(params, NULL);

	geofence_s *new_param = NULL;

	new_param = geofence_parameter_new(params->place_id, params->type, params->latitude, params->longitude, params->radius, params->address, params->bssid, params->ssid);
	if (new_param == NULL) {
		GEOFENCE_LOGD("fail to create new param");
		return NULL;
	}

	if (params->type != GEOFENCE_TYPE_GEOPOINT) {
		GEOFENCE_LOGD("bssid : %s", new_param->bssid);
		GEOFENCE_LOGD("ssid : %s", new_param->ssid);
	} else {
		GEOFENCE_LOGD("Address: %s", new_param->address);
	}

	return new_param;
}

void geofence_parameter_free(geofence_s *params)
{
	g_return_if_fail(params);
	g_slice_free(geofence_s, params);
}
