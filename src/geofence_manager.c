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


#ifndef GEOFENCE_DUMMY_API

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <system_info.h>

#include "geofence-log.h"
#include "geofence.h"
#include "geofence-ielement.h"
#include "geofence-param.h"
#include "geofence_manager.h"

static GHashTable *fence_map = NULL;
static GList *tracking_list = NULL;
static geofence_manager_s *handle = NULL;
static int gIsMgrCreated = 0;

bool __is_geofence_feature_enabled()
{
	const char *geofence_feature = "http://tizen.org/feature/location.geofence";
	bool geofence_enabled = false;

	system_info_get_platform_bool(geofence_feature, &geofence_enabled);

	return geofence_enabled;
}

static void __cb_zone_in(GObject *self, guint geofence_id, gpointer userdata)
{
	GEOFENCE_LOGD("__cb_zone_in : geofence_id = %d", geofence_id);

	geofence_manager_s *handle = (geofence_manager_s *) userdata;
	GList *fence_list = g_list_first(tracking_list);

	while (fence_list) {
		if (GPOINTER_TO_UINT(fence_list->data) == geofence_id) {
			geofence_info_s *info = (geofence_info_s *)g_hash_table_lookup(fence_map, GUINT_TO_POINTER(geofence_id));
			if (info != NULL) {
				GEOFENCE_LOGD("Info exists");
				info->info = NULL;
				info->state = GEOFENCE_STATE_IN;
				info->last_updated_time = (g_get_real_time() / 1000000); /* microsecs->millisecs->secs */
				g_hash_table_replace(fence_map, GUINT_TO_POINTER(geofence_id), (gpointer)info);
			}
			if (handle->user_cb) {
				((geofence_state_changed_cb) handle->user_cb)(geofence_id, GEOFENCE_STATE_IN, handle->user_data);
				break;
			}
		}
		fence_list = g_list_next(fence_list);
	}
}

static void __cb_zone_out(GObject *self, guint geofence_id, gpointer userdata)
{
	GEOFENCE_LOGD("__cb_zone_out : geofence_id = %d", geofence_id);

	geofence_manager_s *handle = (geofence_manager_s *) userdata;
	GList *fence_list = g_list_first(tracking_list);

	while (fence_list) {
		if (GPOINTER_TO_UINT(fence_list->data) == geofence_id) {
			geofence_info_s *info = (geofence_info_s *)g_hash_table_lookup(fence_map, GUINT_TO_POINTER(geofence_id));
			if (info != NULL) {
				GEOFENCE_LOGD("Info exists");
				info->info = NULL;
				info->state = GEOFENCE_STATE_OUT;
				info->last_updated_time = (g_get_real_time() / 1000000); /* microsecs->millisecs->secs */
				g_hash_table_replace(fence_map, GUINT_TO_POINTER(geofence_id), (gpointer)info);
			}
			if (handle->user_cb) {
				((geofence_state_changed_cb) handle->user_cb)(geofence_id, GEOFENCE_STATE_OUT, handle->user_data);
				break;
			}
		}
		fence_list = g_list_next(fence_list);
	}
}

void __handle_action(int place_id, int geofence_id, geofence_manager_error_e error, geofence_manage_e action)
{
	if (error != GEOFENCE_MANAGER_ERROR_NONE) {
		GEOFENCE_LOGD("__handle_action: error=%d", error);
		return;
	}

	switch (action) {
	case GEOFENCE_MANAGE_FENCE_STARTED: {
			geofence_info_s *fence_info = (geofence_info_s *)malloc(sizeof(geofence_info_s));
			if (fence_info == NULL)
				break;
			fence_info->info = NULL;
			fence_info->state = GEOFENCE_STATE_UNCERTAIN;
			fence_info->last_updated_time = (g_get_real_time() / 1000000);
		g_hash_table_insert(fence_map, GINT_TO_POINTER(geofence_id), (gpointer)fence_info);
			/* Inserting the fence in the tracking list */
		tracking_list = g_list_append(tracking_list, GINT_TO_POINTER(geofence_id));
		}
		break;
	case GEOFENCE_MANAGE_FENCE_STOPPED: {
		geofence_info_s *info = (geofence_info_s *)g_hash_table_lookup(fence_map, GINT_TO_POINTER(geofence_id));
			if (info != NULL) {
			g_hash_table_remove(fence_map, GINT_TO_POINTER(geofence_id));
				free(info);
			}
			/* Removing the fence in the tracking list */
		tracking_list = g_list_remove(tracking_list, GINT_TO_POINTER(geofence_id));
		}
		break;
	default:
		break;
	}
}

static void __cb_fence_evt(GObject *self, guint place_id, guint geofence_id, guint error, guint action, gpointer userdata)
{
	GEOFENCE_LOGD("place_id: %d, geofence_id: %d, error: %d, action: %d", place_id, geofence_id, error, action);
	geofence_manager_s *handle = (geofence_manager_s *) userdata;

	/* This will take care of actions in the manager side for successful execution on the server side */
	__handle_action(place_id, geofence_id, error, action);

	if (handle->user_event_cb)
		((geofence_event_cb) handle->user_event_cb)(place_id, geofence_id, error, action, handle->user_data);
}

static void __cb_fence_proximity(GObject *self, guint geofence_id, geofence_proximity_state_e state, geofence_proximity_provider_e provider, gpointer userdata)
{
	GEOFENCE_LOGD("geofence_id: %d, proximity_state: %d, proximity_provider: %d", geofence_id, state, provider);
	geofence_manager_s *handle = (geofence_manager_s *) userdata;
	GList *fence_list = g_list_first(tracking_list);

	while (fence_list) {
		if (GPOINTER_TO_INT(fence_list->data) == geofence_id) {
			if (handle->user_proximity_cb) {
				((geofence_proximity_state_changed_cb) handle->user_proximity_cb)(geofence_id, state, provider, handle->user_data);
				break;
			}
		}
		fence_list = g_list_next(fence_list);
	}
}

static int __set_callback(geofence_manager_h manager, void *callback, void *user_data)
{
	GEOFENCE_LOGD("__set_callback");
	GEOFENCE_NULL_ARG_CHECK(manager);
	GEOFENCE_NULL_ARG_CHECK(callback);

	geofence_manager_s *handle = (geofence_manager_s *) manager;
	handle->user_cb = callback;
	handle->user_data = user_data;

	return GEOFENCE_MANAGER_ERROR_NONE;
}

static int __unset_callback(geofence_manager_h manager)
{
	GEOFENCE_LOGD("__unset_callback");
	GEOFENCE_NULL_ARG_CHECK(manager);

	geofence_manager_s *handle = (geofence_manager_s *) manager;
	handle->user_cb = NULL;
	handle->user_data = NULL;

	return GEOFENCE_MANAGER_ERROR_NONE;
}

static int __set_event_callback(geofence_manager_h manager, void *callback, void *user_data)
{
	GEOFENCE_LOGD("__set_event_callback");
	GEOFENCE_NULL_ARG_CHECK(manager);
	GEOFENCE_NULL_ARG_CHECK(callback);

	geofence_manager_s *handle = (geofence_manager_s *) manager;
	handle->user_event_cb = callback;
	handle->user_data = user_data;

	return GEOFENCE_MANAGER_ERROR_NONE;
}

static int __unset_event_callback(geofence_manager_h manager)
{
	GEOFENCE_LOGD("__unset_event_callback");
	GEOFENCE_NULL_ARG_CHECK(manager);

	geofence_manager_s *handle = (geofence_manager_s *) manager;
	handle->user_event_cb = NULL;
	handle->user_data = NULL;

	return GEOFENCE_MANAGER_ERROR_NONE;
}

static int __set_proximity_callback(geofence_manager_h manager, void *callback, void *user_data)
{
	GEOFENCE_LOGD("__set_proximity_callback");
	GEOFENCE_NULL_ARG_CHECK(manager);
	GEOFENCE_NULL_ARG_CHECK(callback);

	geofence_manager_s *handle = (geofence_manager_s *) manager;
	handle->user_proximity_cb = callback;
	handle->user_data = user_data;

	return GEOFENCE_MANAGER_ERROR_NONE;
}

static int __unset_proximity_callback(geofence_manager_h manager)
{
	GEOFENCE_LOGD("__unset_proximity_callback");
	GEOFENCE_NULL_ARG_CHECK(manager);

	geofence_manager_s *handle = (geofence_manager_s *) manager;
	handle->user_proximity_cb = NULL;
	handle->user_data = NULL;

	return GEOFENCE_MANAGER_ERROR_NONE;
}

static bool __is_fence_started(int fence_id)
{
	GEOFENCE_LOGD("__is_fence_started");
	GList *fence_list = g_list_first(tracking_list);

	while (fence_list) {
		if (GPOINTER_TO_INT(fence_list->data) == fence_id)
			return true;
		fence_list = g_list_next(fence_list);
	}
	return false;
}

/*
* Geofence Manager
*/

/*
* Public Implementation
*/

EXPORT_API int geofence_manager_is_supported(bool *supported)
{
	GEOFENCE_LOGD("geofence_manager_is_supported");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(supported);

	*supported = geofence_is_supported();

	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_manager_create(geofence_manager_h *manager)
{
	GEOFENCE_LOGD("geofence_manager_create");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(manager);

	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	ret = geofence_init();
	if (ret != GEOFENCE_MANAGER_ERROR_NONE)
		return __print_error_code(GEOFENCE_MANAGER_ERROR_NOT_INITIALIZED);

	if (gIsMgrCreated == 0) {
		handle = (geofence_manager_s *) malloc(sizeof(geofence_manager_s));
		if (handle == NULL)
			return __print_error_code(GEOFENCE_MANAGER_ERROR_OUT_OF_MEMORY);

		memset(handle, 0, sizeof(geofence_manager_s));

		handle->object = geofence_new();
		if (handle->object == NULL) {
			free(handle);
			return __print_error_code(GEOFENCE_MANAGER_ERROR_NOT_INITIALIZED);
		}
		if (!handle->sig_id[_GEOFENCE_SIGNAL_ZONE_IN])
			handle->sig_id[_GEOFENCE_SIGNAL_ZONE_IN] = g_signal_connect(handle->object, "zone-in", G_CALLBACK(__cb_zone_in), handle);
		if (!handle->sig_id[_GEOFENCE_SIGNAL_ZONE_OUT])
			handle->sig_id[_GEOFENCE_SIGNAL_ZONE_OUT] = g_signal_connect(handle->object, "zone-out", G_CALLBACK(__cb_zone_out), handle);
		if (!handle->sig_id[_GEOFENCE_SIGNAL_EVENT])
			handle->sig_id[_GEOFENCE_SIGNAL_EVENT] = g_signal_connect(handle->object, "geofence-event", G_CALLBACK(__cb_fence_evt), handle);
		if (!handle->sig_id[_GEOFENCE_SIGNAL_PROXIMITY])
			handle->sig_id[_GEOFENCE_SIGNAL_PROXIMITY] = g_signal_connect(handle->object, "geofence-proximity", G_CALLBACK(__cb_fence_proximity), handle);

		ret = geofence_ielement_create(GEOFENCE_IELEMENT(handle->object));
		if (ret != GEOFENCE_MANAGER_ERROR_NONE)
			return __print_error_code(GEOFENCE_MANAGER_ERROR_EXCEPTION);
		/* Initialize the hashmaps to store the fence and place info */
		fence_map = g_hash_table_new(g_direct_hash, g_direct_equal);
		if (fence_map == NULL)
			GEOFENCE_LOGD("Fence Map is NULL");
		gIsMgrCreated = 1; /* Setting the create flag */
	} else {
		GEOFENCE_LOGE("Geofence Manager is already created. Returning the same handle");
	}

	*manager = (geofence_manager_h) handle;

	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_manager_destroy(geofence_manager_h manager)
{
	GEOFENCE_LOGD("geofence_manager_destroy");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(manager);
	geofence_manager_s *handle = (geofence_manager_s *) manager;
	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	geofence_manager_unset_geofence_event_cb(manager);
	geofence_manager_unset_geofence_state_changed_cb(manager);

	GList *fence_list = g_list_first(tracking_list);

	/* Stopping the already started fences by this application */
	while (fence_list) {
		if (GEOFENCE_MANAGER_ERROR_NONE != geofence_manager_stop(manager, GPOINTER_TO_INT(fence_list->data))) {
			GEOFENCE_LOGD("Stopping the geofence_list failed. Unable to destroy the manager.");
			return __print_error_code(GEOFENCE_MANAGER_ERROR_EXCEPTION);
		}
		fence_list = g_list_next(fence_list);
	}

	if (handle->sig_id[_GEOFENCE_SIGNAL_ZONE_IN]) {
		g_signal_handler_disconnect(handle->object, handle->sig_id[_GEOFENCE_SIGNAL_ZONE_IN]);
		handle->sig_id[_GEOFENCE_SIGNAL_ZONE_IN] = 0;
	}
	if (handle->sig_id[_GEOFENCE_SIGNAL_ZONE_OUT]) {
		g_signal_handler_disconnect(handle->object, handle->sig_id[_GEOFENCE_SIGNAL_ZONE_OUT]);
		handle->sig_id[_GEOFENCE_SIGNAL_ZONE_OUT] = 0;
	}
	if (handle->sig_id[_GEOFENCE_SIGNAL_EVENT]) {
		g_signal_handler_disconnect(handle->object, handle->sig_id[_GEOFENCE_SIGNAL_EVENT]);
		handle->sig_id[_GEOFENCE_SIGNAL_EVENT] = 0;
	}
	if (handle->sig_id[_GEOFENCE_SIGNAL_PROXIMITY]) {
		g_signal_handler_disconnect(handle->object, handle->sig_id[_GEOFENCE_SIGNAL_PROXIMITY]);
		handle->sig_id[_GEOFENCE_SIGNAL_PROXIMITY] = 0;
	}

	ret = geofence_ielement_destroy(GEOFENCE_IELEMENT(handle->object));
	if (ret != GEOFENCE_MANAGER_ERROR_NONE)
		return __print_error_code(GEOFENCE_MANAGER_ERROR_EXCEPTION);

	/* destroy the hashtables */
	g_hash_table_destroy(fence_map);
	fence_map = NULL;
	g_list_free(tracking_list);
	geofence_free(handle->object);
	free(handle);
	tracking_list = NULL;
	handle = NULL;
	gIsMgrCreated = 0; /* Resetting the create flag */
	return GEOFENCE_MANAGER_ERROR_NONE;
}

/*EXPORT_API int geofence_manager_enable_fence(geofence_manager_h manager, int geofence_id, bool enable)
{
	GEOFENCE_LOGD("geofence_manager_enable_service");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(manager);
	GEOFENCE_CHECK_CONDITION(geofence_id > 0, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER, "GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER");
	geofence_manager_s *handle = (geofence_manager_s *) manager;
	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	ret = geofence_ielement_enable_service(GEOFENCE_IELEMENT(handle->object), geofence_id, enable);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE)
		return __print_error_code(ret);

	return GEOFENCE_MANAGER_ERROR_NONE;
}*/

EXPORT_API int geofence_manager_start(geofence_manager_h manager, int geofence_id)
{
	GEOFENCE_LOGD("geofence_manager_start : Geofence ID[%d]", geofence_id);

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(manager);
	GEOFENCE_CHECK_CONDITION(geofence_id > 0, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER, "GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER");

	geofence_manager_s *handle = (geofence_manager_s *) manager;
	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	if (__is_fence_started(geofence_id) == true)
		return __print_error_code(GEOFENCE_MANAGER_ERROR_ALREADY_STARTED);
	ret = geofence_ielement_start(GEOFENCE_IELEMENT(handle->object), geofence_id);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE)
		return __print_error_code(ret);

	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_manager_stop(geofence_manager_h manager, int geofence_id)
{
	GEOFENCE_LOGD("geofence_manager_stop");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(manager);
	GEOFENCE_CHECK_CONDITION(geofence_id > 0, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER, "GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER");

	geofence_manager_s *handle = (geofence_manager_s *) manager;
	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	if (__is_fence_started(geofence_id) == false) {
		GEOFENCE_LOGE("Fence is not currently running.");
		return __print_error_code(GEOFENCE_MANAGER_ERROR_EXCEPTION);
	}
	ret = geofence_ielement_stop(GEOFENCE_IELEMENT(handle->object), geofence_id);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE)
		return __print_error_code(ret);

	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_manager_add_fence(geofence_manager_h manager, geofence_h params, int *geofence_id)
{
	GEOFENCE_LOGD("ENTER >>>");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(manager);
	GEOFENCE_NULL_ARG_CHECK(params);
	GEOFENCE_NULL_ARG_CHECK(geofence_id);

	geofence_manager_s *handle = (geofence_manager_s *) manager;
	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	ret = geofence_ielement_add(GEOFENCE_IELEMENT(handle->object), (geofence_s *) params, geofence_id);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE)
		return __print_error_code(ret);

	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_manager_add_place(geofence_manager_h manager, const char *place_name, int *place_id)
{
	GEOFENCE_LOGD("ENTER >>>");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(manager);
	GEOFENCE_NULL_ARG_CHECK(place_id);
	GEOFENCE_NULL_ARG_CHECK(place_name);

	geofence_manager_s *handle = (geofence_manager_s *) manager;
	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	ret = geofence_ielement_add_place(GEOFENCE_IELEMENT(handle->object), place_name, place_id);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE)
		return __print_error_code(ret);

	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_manager_update_place(geofence_manager_h manager, int place_id, const char *place_name)
{
	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(manager);
	GEOFENCE_NULL_ARG_CHECK(place_name);
	GEOFENCE_CHECK_CONDITION(place_id > 0, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER, "GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER");

	GEOFENCE_LOGD("place_id[%d]", place_id);

	geofence_manager_s *handle = (geofence_manager_s *) manager;
	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	ret = geofence_ielement_update_place(GEOFENCE_IELEMENT(handle->object), place_name, place_id);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE)
		return __print_error_code(ret);

	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_manager_remove_fence(geofence_manager_h manager, int geofence_id)
{
	GEOFENCE_LOGD("geofence_manager_remove_geofence");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(manager);
	GEOFENCE_CHECK_CONDITION(geofence_id > 0, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER, "GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER");

	geofence_manager_s *handle = (geofence_manager_s *) manager;
	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	ret = geofence_ielement_remove(GEOFENCE_IELEMENT(handle->object), geofence_id);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE)
		return __print_error_code(ret);

	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_manager_remove_place(geofence_manager_h manager, int place_id)
{
	GEOFENCE_LOGD("geofence_manager_remove_place");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(manager);
	GEOFENCE_CHECK_CONDITION(place_id > 0, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER, "GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER");

	geofence_manager_s *handle = (geofence_manager_s *) manager;
	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	ret = geofence_ielement_remove_place(GEOFENCE_IELEMENT(handle->object), place_id);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE)
		return __print_error_code(ret);

	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_manager_get_place_name(geofence_manager_h manager, int place_id, char **place_name)
{
	GEOFENCE_LOGD("geofence_manager_remove_place");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(manager);
	GEOFENCE_NULL_ARG_CHECK(place_name);
	GEOFENCE_CHECK_CONDITION(place_id > 0, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER, "GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER");

	geofence_manager_s *handle = (geofence_manager_s *) manager;
	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	ret = geofence_ielement_get_place_name(GEOFENCE_IELEMENT(handle->object), place_id, place_name);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE)
		return __print_error_code(ret);

	return GEOFENCE_MANAGER_ERROR_NONE;
}


EXPORT_API int geofence_manager_set_geofence_state_changed_cb(geofence_manager_h manager, geofence_state_changed_cb callback, void *user_data)
{
	GEOFENCE_LOGD("geofence_manager_set_geofence_state_changed_cb");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(manager);

	return __set_callback(manager, callback, user_data);
}

EXPORT_API int geofence_manager_set_geofence_proximity_state_changed_cb(geofence_manager_h manager, geofence_proximity_state_changed_cb callback, void *user_data)
{
	GEOFENCE_LOGD("geofence_manager_set_geofence_proximity_state_changed_cb");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(manager);

	return __set_proximity_callback(manager, callback, user_data);
}

EXPORT_API int geofence_manager_set_geofence_event_cb(geofence_manager_h manager, geofence_event_cb callback, void *user_data)
{
	GEOFENCE_LOGD("geofence_manager_set_geofence_event_cb");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(manager);

	return __set_event_callback(manager, callback, user_data);
}

EXPORT_API int geofence_manager_unset_geofence_state_changed_cb(geofence_manager_h manager)
{
	GEOFENCE_LOGD("geofence_manager_unset_geofence_state_changed_cb");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(manager);

	return __unset_callback(manager);
}

EXPORT_API int geofence_manager_unset_geofence_proximity_state_changed_cb(geofence_manager_h manager)
{
	GEOFENCE_LOGD("geofence_manager_unset_geofence_proximity_state_changed_cb");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(manager);

	return __unset_proximity_callback(manager);
}

EXPORT_API int geofence_manager_unset_geofence_event_cb(geofence_manager_h manager)
{
	GEOFENCE_LOGD("geofence_manager_unset_geofence_event_cb");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(manager);

	return __unset_event_callback(manager);
}

EXPORT_API int geofence_manager_foreach_geofence_list(geofence_manager_h manager, geofence_manager_fence_cb callback, void *user_data)
{
	GEOFENCE_LOGD(">>> Start");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(manager);
	GEOFENCE_NULL_ARG_CHECK(callback);

	geofence_manager_s *handle = (geofence_manager_s *) manager;
	int ret = GEOFENCE_MANAGER_ERROR_NONE;
	int fence_amount = 0;
	int *fence_ids = NULL;
	geofence_s *params = NULL;

	ret = geofence_ielement_get_fence_list(GEOFENCE_IELEMENT(handle->object), -1, &fence_amount, &fence_ids, &params);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE)
		return __print_error_code(ret);

	if (fence_amount == 0) {
		if (callback)
			callback(-1, NULL, 0, 0, user_data);
		return GEOFENCE_MANAGER_ERROR_NONE;
	}
	int i = 0;
	for (i = 0; i < fence_amount; i++) {
		if (!callback(fence_ids[i], (geofence_h)geofence_parameter_copy((const geofence_s *)(params + i)), (i + 1), fence_amount, user_data))
			break;
		GEOFENCE_LOGD("Printing the details:");
		GEOFENCE_LOGD("Fence id: %d, lat: %lf, lon: %lf, rad: %d, bssid: %s", fence_ids[i], params[i].latitude, params[i].longitude, params[i].radius, params[i].bssid);
	}

	int iterations = fence_amount;
	while (iterations-- > 0)
		geofence_parameter_free((geofence_s *)(params++));

	g_slice_free1(sizeof(int)*fence_amount, fence_ids);

	GEOFENCE_LOGD("<<< End");

	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_manager_foreach_place_geofence_list(geofence_manager_h manager, int place_id, geofence_manager_fence_cb callback, void *user_data)
{
	GEOFENCE_LOGD(">>> Start");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(manager);
	GEOFENCE_NULL_ARG_CHECK(callback);
	GEOFENCE_CHECK_CONDITION(place_id > 0, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER, "GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER");

	geofence_manager_s *handle = (geofence_manager_s *) manager;
	int ret = GEOFENCE_MANAGER_ERROR_NONE;
	int fence_amount = 0;
	int *fence_ids = NULL;
	geofence_s *params = NULL;

	GEOFENCE_LOGD("place_id: %d", place_id);
	ret = geofence_ielement_get_fence_list(GEOFENCE_IELEMENT(handle->object), place_id, &fence_amount, &fence_ids, &params);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE)
		return __print_error_code(ret);

	if (fence_amount == 0) {
		callback(0, NULL, 0, 0, user_data);
		return GEOFENCE_MANAGER_ERROR_NONE;
	}
	int i = 0;
	for (i = 0; i < fence_amount; i++)	{
		callback(fence_ids[i], (geofence_h)geofence_parameter_copy((const geofence_s *)(params + i)), (i + 1), fence_amount, user_data);
		GEOFENCE_LOGD("Fence id: %d, lat: %lf, lon: %lf, rad: %d, address: %s, bssid: %s, ssid: %s", fence_ids[i], params[i].latitude, params[i].longitude, params[i].radius, params[i].address, params[i].bssid, params[i].ssid);
	}
	int iterations = fence_amount;
	while (iterations-- > 0)
		geofence_parameter_free((geofence_s *)(params++));

	g_slice_free1(sizeof(int)*fence_amount, fence_ids);

	GEOFENCE_LOGD("<<< End");
	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_manager_foreach_place_list(geofence_manager_h manager, geofence_manager_place_cb callback, void *user_data)
{
	GEOFENCE_LOGD(">>> Start");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(manager);
	GEOFENCE_NULL_ARG_CHECK(callback);

	geofence_manager_s *handle = (geofence_manager_s *) manager;
	int ret = GEOFENCE_MANAGER_ERROR_NONE;
	int place_amount = 0;
	int *place_ids = NULL;
	place_s *params = NULL;

	ret = geofence_ielement_get_place_list(GEOFENCE_IELEMENT(handle->object), &place_amount, &place_ids, &params);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE)
		return __print_error_code(ret);

	if (place_amount == 0) {
		callback(0, NULL, 0, 0, user_data);
		return GEOFENCE_MANAGER_ERROR_NONE;
	}

	int i = 0;
	for (i = 0; i < place_amount; i++)
		callback(place_ids[i], ((const place_s *)(params + i))->place_name, (i + 1), place_amount, user_data);

	int iterations = place_amount;
	while (iterations-- > 0)
		g_slice_free(place_s, (params++));

	g_slice_free1(sizeof(int)*place_amount, place_ids);

	GEOFENCE_LOGD("<<< End");
	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_create_geopoint(int place_id, double latitude, double longitude, int radius, const char *address, geofence_h *fence)
{
	GEOFENCE_LOGD("geofence_manager_create_geopoint");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(fence);
	GEOFENCE_NULL_ARG_CHECK(address);
	GEOFENCE_CHECK_CONDITION(place_id > 0, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER, "GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER");
	GEOFENCE_CHECK_CONDITION(latitude >= -90 && latitude <= 90, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER, "GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER");
	GEOFENCE_CHECK_CONDITION(longitude >= -180 && longitude <= 180, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER, "GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER");
	GEOFENCE_CHECK_CONDITION(radius >= 100 && radius <= 500, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER, "GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER");

	*fence = (geofence_h)geofence_parameter_new(place_id, GEOFENCE_TYPE_GEOPOINT, latitude, longitude, radius, address, NULL, NULL);
	return (*fence) ? GEOFENCE_MANAGER_ERROR_NONE : __print_error_code(GEOFENCE_MANAGER_ERROR_EXCEPTION);
}

EXPORT_API int geofence_create_bluetooth(int place_id, const char *bssid, const char *ssid, geofence_h *fence)
{
	GEOFENCE_LOGD("geofence_create_bluetooth");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(fence);
	GEOFENCE_NULL_ARG_CHECK(bssid);
	GEOFENCE_NULL_ARG_CHECK(ssid);
	GEOFENCE_CHECK_CONDITION(place_id > 0, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER, "GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER");

	*fence = (geofence_h)geofence_parameter_new(place_id, GEOFENCE_TYPE_BT, .0, .0, 0, NULL, bssid, ssid);
	return (*fence) ? GEOFENCE_MANAGER_ERROR_NONE : __print_error_code(GEOFENCE_MANAGER_ERROR_EXCEPTION);
}

EXPORT_API int geofence_create_wifi(int place_id, const char *bssid, const char *ssid, geofence_h *fence)
{
	GEOFENCE_LOGD("geofence_create_wifi");

	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(fence);
	GEOFENCE_NULL_ARG_CHECK(bssid);
	GEOFENCE_NULL_ARG_CHECK(ssid);
	GEOFENCE_CHECK_CONDITION(place_id > 0, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER, "GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER");

	*fence = (geofence_h)geofence_parameter_new(place_id, GEOFENCE_TYPE_WIFI, .0, .0, 0, NULL, bssid, ssid);
	return (*fence) ? GEOFENCE_MANAGER_ERROR_NONE : __print_error_code(GEOFENCE_MANAGER_ERROR_EXCEPTION);
}

EXPORT_API int geofence_destroy(geofence_h fence)
{
	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(fence);

	geofence_parameter_free((geofence_s *)fence);
	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_get_type(geofence_h fence, geofence_type_e *type)
{
	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(fence);
	GEOFENCE_NULL_ARG_CHECK(type);

	geofence_s *p = (geofence_s *)fence;
	*type = p->type;
	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_get_place_id(geofence_h fence, int *place_id)
{
	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(fence);
	GEOFENCE_NULL_ARG_CHECK(place_id);

	geofence_s *p = (geofence_s *)fence;
	*place_id = p->place_id;
	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_get_latitude(geofence_h fence, double *latitude)
{
	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(fence);
	GEOFENCE_NULL_ARG_CHECK(latitude);

	geofence_s *p = (geofence_s *)fence;
	*latitude = p->latitude;
	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_get_longitude(geofence_h fence, double *longitude)
{
	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(fence);
	GEOFENCE_NULL_ARG_CHECK(longitude);

	geofence_s *p = (geofence_s *)fence;
	*longitude = p->longitude;
	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_get_radius(geofence_h fence, int *radius)
{
	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(fence);
	GEOFENCE_NULL_ARG_CHECK(radius);

	geofence_s *p = (geofence_s *)fence;
	*radius = p->radius;
	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_get_address(geofence_h fence, char **address)
{
	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(fence);
	GEOFENCE_NULL_ARG_CHECK(address);

	geofence_s *p = (geofence_s *)fence;
	*address = strdup(p->address);
	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_get_bssid(geofence_h fence, char **bssid)
{
	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(fence);
	GEOFENCE_NULL_ARG_CHECK(bssid);

	geofence_s *p = (geofence_s *)fence;
	*bssid = strdup(p->bssid);
	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_get_ssid(geofence_h fence, char **ssid)
{
	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(fence);
	GEOFENCE_NULL_ARG_CHECK(ssid);

	geofence_s *p = (geofence_s *)fence;
	*ssid = strdup(p->ssid);
	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_status_create(int geofence_id, geofence_status_h *status)
{
	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(status);
	GEOFENCE_CHECK_CONDITION(geofence_id > 0, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER, "GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER");
	GEOFENCE_LOGD("Allocating memory for new status");

	geofence_status_s *new_status = g_slice_new0(geofence_status_s);
	GEOFENCE_CHECK_CONDITION(new_status != NULL, GEOFENCE_MANAGER_ERROR_OUT_OF_MEMORY, "GEOFENCE_MANAGER_ERROR_OUT_OF_MEMORY");

	new_status->fence_id = geofence_id;
	new_status->state = GEOFENCE_STATE_UNCERTAIN;
	new_status->seconds = 0;
	*status = (geofence_status_h)new_status;
	GEOFENCE_LOGD("Status created for fence: %d", geofence_id);
	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_status_destroy(geofence_status_h status)
{
	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(status);

	g_slice_free(geofence_status_s, ((geofence_status_s *)status));
	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_status_get_state(geofence_status_h status, geofence_state_e *state)
{
	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(status);
	GEOFENCE_NULL_ARG_CHECK(state);

	geofence_status_s *p = (geofence_status_s *)status;
	GEOFENCE_LOGD("Looking up for state in hashmap");
	geofence_info_s *info = (geofence_info_s *)g_hash_table_lookup(fence_map, GINT_TO_POINTER(p->fence_id));
	if (info != NULL)
		*state = info->state;
	else
		*state = GEOFENCE_STATE_UNCERTAIN;
	GEOFENCE_LOGD("state value being assigned: %d", *state);
	return GEOFENCE_MANAGER_ERROR_NONE;
}

EXPORT_API int geofence_status_get_duration(geofence_status_h status, int *seconds)
{
	if (__is_geofence_feature_enabled() == false)
		return GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED;

	GEOFENCE_NULL_ARG_CHECK(status);
	GEOFENCE_NULL_ARG_CHECK(seconds);

	geofence_status_s *p = (geofence_status_s *)status;
	geofence_info_s *info = (geofence_info_s *)g_hash_table_lookup(fence_map, GINT_TO_POINTER(p->fence_id));
	if (info != NULL) {
		gint64 time_in_secs = (g_get_real_time() / 1000000);
		GEOFENCE_LOGD("Current time: %d, last updated: %d", time_in_secs, info->last_updated_time);
		*seconds = (time_in_secs - info->last_updated_time); /* difference of time */
	} else
		*seconds = 0;
	GEOFENCE_LOGD("Duration value assigned: %d", *seconds);
	return GEOFENCE_MANAGER_ERROR_NONE;
}

#endif /* GEOFENCE_DUMMY_API */
