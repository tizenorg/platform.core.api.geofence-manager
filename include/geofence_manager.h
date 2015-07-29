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

#ifndef __TIZEN_GEOFENCE_MANAGER_H__
#define __TIZEN_GEOFENCE_MANAGER_H__

#include <geofence_type.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @addtogroup CAPI_GEOFENCE_MANAGER_MODULE
 * @{
 */

/**
 * @brief Called when a device enters or exits the given geofence.
 * @since_tizen 2.4
 * @param[in] geofence_id  The specified geofence id
 * @param[in] state  The geofence state
 * @param[in] user_data  The user data passed from callback registration function
 * @pre geofence_manager_start() will invoke this callback if you register this callback using geofence_manager_set_geofence_state_changed_cb().
 * @see geofence_state_e
 * @see geofence_manager_start()
 * @see geofence_manager_set_geofence_state_changed_cb()
 */
typedef void(*geofence_state_changed_cb)(int geofence_id, geofence_state_e state, void *user_data);

/**
 * @brief Called when the some event occurs in geofence and place such as add, update, etc..
 * @details The events of public geofence is also received if there are public geofences.
 *
 * @since_tizen 2.4
 * @remarks The value of place_id or geofence_id is -1 when the place id or geofence id is not assigned.
 * @param[in] place_id The place id
 * @param[in] geofence_id The specified geofence id
 * @param[in] error The error code for the particular action
 * @param[in] manage The result code for the particular place and geofence management
 * @param[in] user_data The user data passed from callback registration function
 * @pre geofence_manager_start() will invoke this callback if you register this callback using geofence_manager_set_geofence_state_changed_cb()
 * @see geofence_manage_e
 * @see geofence_manager_start()
 * @see geofence_manager_set_geofence_state_changed_cb()
 */
typedef void(*geofence_event_cb)(int place_id, int geofence_id, geofence_manager_error_e error, geofence_manage_e manage, void *user_data);

/**
 * @brief Called when the fence list is requested.
 * @since_tizen 2.4
 * @param[in] geofence_id The geofence id
 * @param[in] fence The fence handle
 * @param[in] fence_index The index number of the fences in the list
 * @param[in] fence_cnt The total number of fences that exists for the requester
 * @param[in] user_data The user data passed from callback registration function
 * @return true to continue iterating, otherwise false
 * @pre geofence_manager_foreach_geofence_list() and geofence_manager_foreach_place_geofence_list() will invoke this callback if you specify this callback using geofence_manager_foreach_geofence_list().
 * @see geofence_manager_foreach_geofence_list()
 * @see geofence_manager_foreach_place_geofence_list()
 * @see geofence_manager_add()
 */
typedef bool(*geofence_manager_fence_cb)(int geofence_id, geofence_h fence, int fence_index, int fence_cnt, void *user_data);

/**
 * @brief Called when the place list is requested.
 * @since_tizen 2.4
 * @param[in] place_id The current place id
 * @param[in] place_name The current place name
 * @param[in] place_index The index number of the places in the list
 * @param[in] place_cnt The total number of places that exists for the requester
 * @param[in] user_data The user data passed from callback registration function
 * @return true to continue iterating, otherwise false
 * @pre geofence_manager_foreach_place_list() will invoke this callback if you specify this callback using geofence_manager_foreach_place_list().
 * @see geofence_manager_foreach_place_list()
 * @see geofence_manager_add_place()
 */
typedef bool(*geofence_manager_place_cb)(int place_id, const char *place_name, int place_index, int place_cnt, void *user_data);


/**
 * @brief Checks whether the geofence manager is available or not.
 * @since_tizen 2.4
 * @param[out] supported  @c true if geofence service is supported, otherwise @c false
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @see	geofence_manager_create()
 */
int geofence_manager_is_supported(bool *supported);

/**
 * @brief Creates a new geofence manager.
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @remarks The manager must be released using geofence_manager_destroy().
 * @param[out] manager  A geofence manager handle to be newly created on success
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_OUT_OF_MEMORY    Out of memory
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @retval #GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED The application does not have the privilege to call this function
 * @retval #GEOFENCE_MANAGER_ERROR_NOT_INITIALIZED Not initialized
 * @retval #GEOFENCE_MANAGER_ERROR_EXCEPTION	Exception occurred
 * @see geofence_manager_destroy()
 */
int geofence_manager_create(geofence_manager_h *manager);

/**
 * @brief Releases the geofence manager.
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @param[in] manager The geofence manager handle
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @retval #GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED The application does not have the privilege to call this function
 * @retval #GEOFENCE_MANAGER_ERROR_EXCEPTION    Exception occurred
 * @pre geofence_manager_stop() is called before.
 * @see geofence_manager_create()
 * @see geofence_manager_stop()
*/
int geofence_manager_destroy(geofence_manager_h manager);

/**
 * @brief Starts the geofencing service.
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 *
 * @remarks There is no limit on number of geofence managers for which this function called
 *
 * Calling this function invokes a location service event. When the location service is enabled, the service state change callback
 * (set using #geofence_manager_set_geofence_state_changed_cb()) notifies the user with geofence id as the 1st argument,
 * geofence zone state(#GEOFENCE_STATE_IN and #GEOFENCE_STATE_OUT) as the 2nd argument, and the service starts \n
 *
 * @param[in]   manager The geofence manager handle
 * @param[in]   geofence_id The specified geofence id
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @retval #GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED The application does not have the privilege to call this function
 * @retval #GEOFENCE_MANAGER_ERROR_EXCEPTION Exception occurred
 * @retval #GEOFENCE_MANAGER_ERROR_GEOFENCE_ACCESS_DENIED Access to specified geofence is denied
 * @see geofence_manager_stop()
 * @see geofence_manager_add_fence()
 * @see geofence_manager_set_geofence_state_changed_cb()
 */
int geofence_manager_start(geofence_manager_h manager, int geofence_id);

/**
 * @brief Stops the geofenceing service.
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @remarks 	This function initiates the process of stopping the service.
 *
 * You can stop and start the geofence manager as needed.
 *
 * @param[in]   manager The geofence manager handle
 * @param[in]   geofence_id The specified geofence id
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @retval #GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED The application does not have the privilege to call this function
 * @retval #GEOFENCE_MANAGER_ERROR_NOT_INITIALIZED Not initialized
 * @retval #GEOFENCE_MANAGER_ERROR_EXCEPTION Exception occurred
 * @retval #GEOFENCE_MANAGER_ERROR_GEOFENCE_ACCESS_DENIED Access to specified geofence is denied
 * @see   geofence_manager_start()
 */
int geofence_manager_stop(geofence_manager_h manager, int geofence_id);

/**
 * @brief Creates a new place for geofencing service.
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @param[in] manager The geofence manager handle
 * @param[in] place_name A place name to be created
 * @param[out] place_id The place id to be newly created on success
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @retval #GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED The application does not have the privilege to call this function
 * @retval #GEOFENCE_MANAGER_ERROR_NOT_INITIALIZED Not initialized
 * @retval #GEOFENCE_MANAGER_ERROR_EXCEPTION Exception occurred
 * @see   geofence_manager_update_place()
 * @see   geofence_manager_remove_place()
 */
int geofence_manager_add_place(geofence_manager_h manager, const char *place_name, int *place_id);

/**
 * @brief Updates the place name of a given place id.
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @param[in] manager The geofence manager handle
 * @param[in] place_id The specified place id
 * @param[in] place_name A new place name of the place id
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @retval #GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED The application does not have the privilege to call this function
 * @retval #GEOFENCE_MANAGER_ERROR_NOT_INITIALIZED Not initialized
 * @retval #GEOFENCE_MANAGER_ERROR_EXCEPTION Exception occurred
 * @retval #GEOFENCE_MANAGER_ERROR_PLACE_ACCESS_DENIED Access to specified place is denied
 * @see   geofence_manager_add_place()
 * @see   geofence_manager_remove_place()
 */
int geofence_manager_update_place(geofence_manager_h manager, int place_id, const char *place_name);

/**
 * @brief Removes the specific place for geofencing service.
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @remarks 	The place must have no geofences.
 * @param[in] manager The geofence manager handle
 * @param[in] place_id The specified place id
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @retval #GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED The application does not have the privilege to call this function
 * @retval #GEOFENCE_MANAGER_ERROR_NOT_INITIALIZED Not initialized
 * @retval #GEOFENCE_MANAGER_ERROR_EXCEPTION Exception occurred
 * @retval #GEOFENCE_MANAGER_ERROR_PLACE_ACCESS_DENIED Access to specified place is denied
 * @pre geofence_manager_remove_fence() in all geofence is called before.
 * @see   geofence_manager_add_place()
 * @see   geofence_manager_update_place()
 */
int geofence_manager_remove_place(geofence_manager_h manager, int place_id);

/**
 * @brief Adds a geofence for a given geofence manager.
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @param[in] manager The geofence manager handle
 * @param[in] fence The geofence handle
 * @param[out] geofence_id The geofence id handle to be newly created on success
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @retval #GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED The application does not have the privilege to call this function
 * @retval #GEOFENCE_MANAGER_ERROR_NOT_INITIALIZED Not initialized
 * @retval #GEOFENCE_MANAGER_ERROR_EXCEPTION Exception occurred
 * @see geofence_manager_remove_fence()
 * @see geofence_create_geopoint()
 * @see geofence_create_bluetooth()
 * @see geofence_create_wifi()
 * @see geofence_manager_start()
 */
int geofence_manager_add_fence(geofence_manager_h manager, const geofence_h fence, int *geofence_id);

/**
 * @brief Removes a geofence with a given geofence id
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @param[in] manager The geofence manager handle
 * @param[in] geofence_id The specified geofence id
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @retval #GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED The application does not have the privilege to call this function
 * @retval #GEOFENCE_MANAGER_ERROR_NOT_INITIALIZED Not initialized
 * @retval #GEOFENCE_MANAGER_ERROR_EXCEPTION Exception occurred
 * @retval #GEOFENCE_MANAGER_ERROR_GEOFENCE_ACCESS_DENIED Access to specified geofence is denied
 * @pre geofence_manager_stop() is called before.
 * @see geofence_manager_add_fence()
 */
int geofence_manager_remove_fence(geofence_manager_h manager, int geofence_id);

/**
 * @brief Registers a callback function to be invoked when a device enters or exits the specific geofence.
 * @since_tizen 2.4
 * @param[in] manager The geofence manager handle
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @post This function invokes geofence_state_changed_cb().
 * @see geofence_manager_unset_geofence_state_changed_cb()
 * @see geofence_state_changed_cb()
 */
int geofence_manager_set_geofence_state_changed_cb(geofence_manager_h manager, geofence_state_changed_cb callback, void *user_data);

/**
 * @brief Unregisters the callback function.
 * @since_tizen 2.4
 * @param[in] manager The geofence manager handle
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @see geofence_state_changed_cb()
 */
int geofence_manager_unset_geofence_state_changed_cb(geofence_manager_h manager);

/**
 * @brief Registers a callback function to be invoked when a response comes.
 * @since_tizen 2.4
 * @param[in] manager The geofence manager handle
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @post This function invokes geofence_event_cb().
 * @see geofence_manager_unset_geofence_event_cb()
 */
int geofence_manager_set_geofence_event_cb(geofence_manager_h manager, geofence_event_cb callback, void *user_data);

/**
 * @brief Unregisters the callback function.
 * @since_tizen 2.4
 * @param[in] manager The geofence manager handle
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @see geofence_event_cb()
 */
int geofence_manager_unset_geofence_event_cb(geofence_manager_h manager);

/**
 * @brief Retrieves a list of fences registered in the specified geofence manager.
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @param[in] manager The geofence manager handle
 * @param[in] callback The callback function to deliver each fence
 * @param[in] user_data The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @retval #GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED The application does not have the privilege to call this function
 * @retval #GEOFENCE_MANAGER_ERROR_DATABASE Database error
 * @post This function invokes geofence_manager_fence_cb().
 * @see geofence_manager_fence_cb()
 * @see geofence_manager_add()
 */
int geofence_manager_foreach_geofence_list(geofence_manager_h manager, geofence_manager_fence_cb callback, void *user_data);

/**
 * @brief Retrieves a list of fences registered in the specified place.
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @param[in] place_id The place id
 * @param[in] callback The callback function to deliver each fence of the specified place
 * @param[in] user_data The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @retval #GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED The application does not have the privilege to call this function
 * @retval #GEOFENCE_MANAGER_ERROR_DATABASE Database error
 * @retval #GEOFENCE_MANAGER_ERROR_PLACE_ACCESS_DENIED Access to specified place is denied
 * @post This function invokes geofence_manager_fence_cb().
 * @see geofence_manager_fence_at_place_cb()
 * @see geofence_manager_add_place()
 */
int geofence_manager_foreach_place_geofence_list(geofence_manager_h manager, int place_id, geofence_manager_fence_cb callback, void *user_data);

/**
 * @brief Retrieves a list of place registered in the specified geofence manager.
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @param[in] manager The geofence manager handle
 * @param[in] callback The callback function to deliver each place of the specified geofence manager
 * @param[in] user_data The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @retval #GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED The application does not have the privilege to call this function
 * @retval #GEOFENCE_MANAGER_ERROR_DATABASE Database error
 * @post This function invokes geofence_manager_place_cb().
 * @see geofence_manager_place_cb()
 * @see geofence_manager_add_place()
 */
int geofence_manager_foreach_place_list(geofence_manager_h manager, geofence_manager_place_cb callback, void *user_data);

/**
 * @brief Gets the name of place.
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @remarks The place_name must be released using free().
 * @param[in] manager The geofence manager handle
 * @param[in] place_id The place id
 * @param[out] place_name The name of the place
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @retval #GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED The application does not have the privilege to call this function
 * @retval #GEOFENCE_MANAGER_ERROR_INVALI_ID
 * @retval #GEOFENCE_MANAGER_ERROR_DATABASE Database error
 * @retval #GEOFENCE_MANAGER_ERROR_PLACE_ACCESS_DENIED Access to specified place is denied
 * @see geofence_manager_add_place()
 * @see geofence_manager_update_place()
 */
int geofence_manager_get_place_name(geofence_manager_h manager, int place_id, char **place_name);

/**
 * @}
 */


/**
 * @addtogroup CAPI_GEOFENCE_MODULE
 * @{
 */

/**
 * @brief Creates a geopoint type of new geofence.
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @remarks 	The fence must be released using geofence_destroy().
 * @param[in] place_id The current place id
 * @param[in] latitude Specifies the value of latitude of geofence [-90.0 ~ 90.0] (degrees)
 * @param[in] longitude Specifies the value of longitude of geofence [-180.0 ~ 180.0] (degrees)
 * @param[in] radius Specifies the value of radius of geofence [100 ~ 500](meter)
 * @param[in] address Specifies the value of address
 * @param[out] fence A geofence handle to be newly created on success
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @retval #GEOFENCE_MANAGER_ERROR_EXCEPTION Exception occurred
 * @retval #GEOFENCE_MANAGER_ERROR_PLACE_ACCESS_DENIED Access to specified place is denied
 * @see geofence_create_bluetooth()
 * @see geofence_create_wifi()
 * @see geofence_destroy()
 */
int geofence_create_geopoint(int place_id, double latitude, double longitude, int radius, const char *address, geofence_h *fence);

/**
 * @brief Creates a bluetooth type of new geofence.
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @remarks 	The fence must be released using geofence_destroy().
 * @param[in] place_id The current place id
 * @param[in] bssid Specifies the value of BSSID of BT MAC address
 * @param[in] ssid Specifies the value of SSID of BT Device
 * @param[out] fence A geofence handle to be newly created on success
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @retval #GEOFENCE_MANAGER_ERROR_EXCEPTION Exception occurred
 * @retval #GEOFENCE_MANAGER_ERROR_PLACE_ACCESS_DENIED Access to specified place is denied
 * @see geofence_create_geopoint()
 * @see geofence_create_wifi()
 * @see geofence_destroy()
 */
int geofence_create_bluetooth(int place_id, const char *bssid, const char *ssid, geofence_h *fence);

/**
 * @brief Creates a Wi-Fi type of new geofence.
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @remarks 	The fence must be released using geofence_destroy().
 * @param[in] place_id The current place id
 * @param[in] bssid Specifies the value of BSSID of Wi-Fi MAC address
 * @param[in] ssid Specifies the value of SSID of Wi-Fi Device
 * @param[out] fence A geofence handle to be newly created on success
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @retval #GEOFENCE_MANAGER_ERROR_EXCEPTION Exception occurred
 * @retval #GEOFENCE_MANAGER_ERROR_PLACE_ACCESS_DENIED Access to specified place is denied
 * @see geofence_create_geopoint()
 * @see geofence_create_bluetooth()
 * @see geofence_destroy()
 */
int geofence_create_wifi(int place_id, const char *bssid, const char *ssid, geofence_h *fence);

/**
 * @brief Releases the geofence.
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @param[in] fence The geofence handle
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @see geofence_create_geopoint()
 * @see geofence_create_bluetooth()
 * @see geofence_create_wifi()
 */
int geofence_destroy(geofence_h fence);

/**
 * @brief Gets the type of geofence.
 * @since_tizen 2.4
 * @param[in] fence The geofence handle
 * @param[out] type The type of geofence
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @see geofence_get_geopoint()
 * @see geofence_get_radius()
 * @see geofence_get_bssid()
 */
int geofence_get_type(geofence_h fence, geofence_type_e *type);

/**
 * @brief Gets the id of place.
 * @since_tizen 2.4
 * @param[in] fence The geofence handle
 * @param[out] place_id The id of the place
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @see geofence_create_geopoint()
 * @see geofence_create_bluetooth()
 * @see geofence_create_wifi()
 */
int geofence_get_place_id(geofence_h fence, int *place_id);

/**
 * @brief Gets the latitude of geofence.
 * @since_tizen 2.4
 * @remarks The geofence has latitude only when geofence_type_e is GEOFENCE_TYPE_GEOPOINT.
 * @param[in] fence The geofence handle
 * @param[out] latitude The latitude of geofence
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument.
 * @see geofence_get_longitude()
 * @see geofence_get_radius()
 * @see geofence_get_address()
 * @see geofence_get_type()
 * @see geofence_create_geopoint()
 */
int geofence_get_latitude(geofence_h fence, double *latitude);

/**
 * @brief Gets the longitude of geofence.
 * @since_tizen 2.4
 * @remarks The geofence has longitude only when geofence_type_e is GEOFENCE_TYPE_GEOPOINT.
 * @param[in] fence The geofence handle
 * @param[out] longitude The longitude of geofence
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @see geofence_get_latitude()
 * @see geofence_get_radius()
 * @see geofence_get_address()
 * @see geofence_get_type()
 * @see geofence_create_geopoint()
 */
int geofence_get_longitude(geofence_h fence, double *longitude);


/**
 * @brief Gets the radius of geofence.
 * @since_tizen 2.4
 * @remarks The geofence has radius only when geofence_type_e is GEOFENCE_TYPE_GEOPOINT.
 * @param[in] fence The geofence handle
 * @param[out] radius The radius of geofence
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @see geofence_get_latitude()
 * @see geofence_get_longitude()
 * @see geofence_get_address()
 * @see geofence_get_type()
 * @see geofence_create_geopoint()
 */
int geofence_get_radius(geofence_h fence, int *radius);

/**
 * @brief Gets the address of geofence
 * @since_tizen 2.4
 * @remarks The geofence has address only when geofence_type_e is GEOFENCE_TYPE_GEOPOINT. The address must be released using free().
 * @param[in] fence The geofence handle
 * @param[out] address Specifies the value of address
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @see geofence_get_latitude()
 * @see geofence_get_longitude()
 * @see geofence_get_radius()
 * @see geofence_get_type()
 * @see geofence_create_geopoint()
 */
int geofence_get_address(geofence_h fence, char **address);

/**
 * @brief Gets the bssid of geofence
 * @since_tizen 2.4
 * @remarks The geofence has bssid only when geofence_type_e is GEOFENCE_TYPE_BLUETOOTH or GEOFENCE_TYPE_WIFI. The bssid must be released using free().
 * @param[in] fence The geofence handle
 * @param[out] bssid The bssid of geofence
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @see geofence_get_type()
 * @see geofence_create_bluetooth()
 * @see geofence_create_wifi()
 * @see geofence_get_ssid()
 */
int geofence_get_bssid(geofence_h fence, char **bssid);

/**
 * @brief Gets the ssid of geofence
 * @since_tizen 2.4
 * @remarks The geofence has ssid only when geofence_type_e is GEOFENCE_TYPE_BLUETOOTH or GEOFENCE_TYPE_WIFI. The ssid must be released using free().
 * @param[in] fence The geofence handle
 * @param[out] ssid The ssid of geofence
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @see geofence_get_type()
 * @see geofence_create_bluetooth()
 * @see geofence_create_wifi()
 * @see geofence_get_bssid()
 */
int geofence_get_ssid(geofence_h fence, char **ssid);

/**
 * @brief Returns The geofence status of specified geofence.
 * @since_tizen 2.4
 * @remarks A status must be released using geofence_status_destroy()
 * @param[in] geofence_id The geofence id
 * @param[out] status The status handle of a specified geofence
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @retval #GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED The application does not have the privilege to call this function
 * @retval #GEOFENCE_MANAGER_ERROR_GEOFENCE_ACCESS_DENIED Access to specified geofence is denied
 * @see geofence_status_destroy()
 * @see geofence_manager_start()
 * @see geofence_manager_stop()
 */
int geofence_status_create(int geofence_id, geofence_status_h *status);

/**
 * @brief Releases the memory, used by the status data.
 * @since_tizen 2.4
 * @param[in] status The status handle of a specified geofence
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @see geofence_status_create()
 * @see geofence_status_get_state()
 * @see geofence_status_get_duration()
 */
int geofence_status_destroy(geofence_status_h status);

/**
 * @brief Gets the state of geofence.
 * @since_tizen 2.4
 * @param[in] status The geofence status handle
 * @param[out] state The state of geofence
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @see geofence_status_get_duration()
 */
int geofence_status_get_state(geofence_status_h status, geofence_state_e *state);

/**
 * @brief Gets the amount of seconds geofence is in the current state.
 * @since_tizen 2.4
 * @param[in] status The geofence status handle
 * @param[out] seconds The amount of seconds geofence is in the current state
 * @return 0 on success, otherwise a negative error value
 * @retval #GEOFENCE_MANAGER_ERROR_NONE Successful
 * @retval #GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER Illegal argument
 * @see geofence_status_get_state()
 */
int geofence_status_get_duration(geofence_status_h status, int *seconds);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_GEOFENCE_MANAGER_H__ */
