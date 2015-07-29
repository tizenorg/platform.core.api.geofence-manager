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
#include "geofence-ielement.h"

static void _glib_log(const gchar *log_domain, GLogLevelFlags log_level,
                      const gchar *msg, gpointer user_data)
{

	GEOFENCE_LOGD("GLIB[%d] : %s", log_level, msg);
}


GType geofence_ielement_get_type(void)
{
	g_log_set_default_handler(_glib_log, NULL);

	static GType iface_type = 0;

	if (!iface_type) {
		static const GTypeInfo info = {
			sizeof(GeofenceIElementInterface),	/* class_size */
			NULL, 				/* base_init */
			NULL 				/* base_finalize */
		};

		iface_type = g_type_register_static(G_TYPE_INTERFACE, 	/* parent-type */
		                                    "GeofenceIElement", /* type name */
		                                    &info, 				/* info */
		                                    0);					/* flags */
	}

	return iface_type;
}

int geofence_ielement_create(GeofenceIElement *self)
{
	g_return_val_if_fail(GEOFENCE_IS_IELEMENT(self), GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self), GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self)->create, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	return GEOFENCE_IELEMENT_GET_INTERFACE(self)->create(self);
}

int geofence_ielement_destroy(GeofenceIElement *self)
{
	g_return_val_if_fail(GEOFENCE_IS_IELEMENT(self), GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self), GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self)->destroy, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	return GEOFENCE_IELEMENT_GET_INTERFACE(self)->destroy(self);
}

int geofence_ielement_enable_service(GeofenceIElement *self, int geofence_id, bool enable)
{
	g_return_val_if_fail(GEOFENCE_IS_IELEMENT(self), GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(geofence_id, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self), GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self)->enable, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	return GEOFENCE_IELEMENT_GET_INTERFACE(self)->enable(self, geofence_id, enable);
}

int geofence_ielement_start(GeofenceIElement *self, int geofence_id)
{
	g_return_val_if_fail(GEOFENCE_IS_IELEMENT(self), GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(geofence_id, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self), GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self)->start, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	return GEOFENCE_IELEMENT_GET_INTERFACE(self)->start(self, geofence_id);
}

int geofence_ielement_stop(GeofenceIElement *self, int geofence_id)
{
	g_return_val_if_fail(GEOFENCE_IS_IELEMENT(self), GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(geofence_id, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self), GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self)->stop, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	return GEOFENCE_IELEMENT_GET_INTERFACE(self)->stop(self, geofence_id);
}

int geofence_ielement_add(GeofenceIElement *self, geofence_s *params, int *geofence_id)
{
	g_return_val_if_fail(GEOFENCE_IS_IELEMENT(self), GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(params, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(geofence_id, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self), GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self)->add, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	return GEOFENCE_IELEMENT_GET_INTERFACE(self)->add(self, params, geofence_id);
}

int geofence_ielement_add_place(GeofenceIElement *self, const char *place_name, int *place_id)
{
	g_return_val_if_fail(GEOFENCE_IS_IELEMENT(self), GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(place_name, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(place_id, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self), GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self)->add_place, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	return GEOFENCE_IELEMENT_GET_INTERFACE(self)->add_place(self, place_name, place_id);
}

int geofence_ielement_update_place(GeofenceIElement *self, const char *place_name, int place_id)
{
	g_return_val_if_fail(GEOFENCE_IS_IELEMENT(self), GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(place_name, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(place_id, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self), GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self)->update_place, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	return GEOFENCE_IELEMENT_GET_INTERFACE(self)->update_place(self, place_name, place_id);
}

int geofence_ielement_remove(GeofenceIElement *self, int geofence_id)
{
	g_return_val_if_fail(GEOFENCE_IS_IELEMENT(self), GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(geofence_id, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self), GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self)->remove, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	return GEOFENCE_IELEMENT_GET_INTERFACE(self)->remove(self, geofence_id);
}

int geofence_ielement_remove_place(GeofenceIElement *self, int place_id)
{
	g_return_val_if_fail(GEOFENCE_IS_IELEMENT(self), GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(place_id, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self), GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self)->remove_place, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	return GEOFENCE_IELEMENT_GET_INTERFACE(self)->remove_place(self, place_id);
}

int geofence_ielement_get_place_name(GeofenceIElement *self, int place_id, char **place_name)
{
	g_return_val_if_fail(GEOFENCE_IS_IELEMENT(self), GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(place_id, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(place_name, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self), GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self)->get_place_name, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	return GEOFENCE_IELEMENT_GET_INTERFACE(self)->get_place_name(self, place_id, place_name);
}

int geofence_ielement_get_fence_list(GeofenceIElement *self, int place_id, int *fence_amount, int **fence_ids, geofence_s **params)
{
	g_return_val_if_fail(GEOFENCE_IS_IELEMENT(self), GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(fence_amount, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(fence_ids, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(params, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self), GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self)->get_list, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	return GEOFENCE_IELEMENT_GET_INTERFACE(self)->get_list(self, place_id, fence_amount, fence_ids, params);
}

int geofence_ielement_get_place_list(GeofenceIElement *self, int *place_amount, int **place_ids, place_s **params)
{
	g_return_val_if_fail(GEOFENCE_IS_IELEMENT(self), GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(place_amount, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(place_ids, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(params, GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self), GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(GEOFENCE_IELEMENT_GET_INTERFACE(self)->get_place_list, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	return GEOFENCE_IELEMENT_GET_INTERFACE(self)->get_place_list(self, place_amount, place_ids, params);
}

