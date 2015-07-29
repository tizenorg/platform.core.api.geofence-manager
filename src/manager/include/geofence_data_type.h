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

#ifndef __GEOFENCE_DATA_TYPE_H__
#define __GEOFENCE_DATA_TYPE_H__

#include <geofence_type.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ADDRESS_LEN		64
#define WLAN_BSSID_LEN 	18
#define WLAN_SSID_LEN	18
#define PLACE_NAME_LEN	64

/**
 * @brief The geofence parameter structure
 */
typedef struct _geofence_s {
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
 * @brief The geofence place structure
 */
typedef struct _place_s {
	char place_name[PLACE_NAME_LEN];
} place_s;

#ifdef __cplusplus
}
#endif

#endif /* __GEOFENCE_DATA_TYPE_H__ */
