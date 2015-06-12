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

#ifndef __TIZEN_GEOFENCE_SETTINGS_H__
#define __TIZEN_GEOFENCE_SETTINGS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 *@brief This API should be called when a fence needs to be enabled/disabled
 *@param[in] manager Geofence manager instance
 *@param[in] geofence_id ID of the fence which need to be enabled or disabled
 *@param[in] enable boolean value should be set to true for enable and set to false for diable
**/
int geofence_manager_enable_fence(geofence_manager_h manager, int geofence_id, bool enable);

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_GEOFENCE_SETTINGS_H__ */

