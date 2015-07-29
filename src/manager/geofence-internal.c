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

#include <stdio.h>
#include <stdlib.h>

#include "geofence-log.h"
#include "module-internal.h"
#include "geofence-internal.h"
#include "geofence-marshal.h"
#include "geofence-ielement.h"
#include "geofence-param.h"
#include "geofence-internal.h"
#include "geofence_module.h"


typedef struct _GeofenceInternalPrivate {
	GeofenceInternalMod		*mod;
	GMutex	 		mutex;
	geofence_s		*params;
} GeofenceInternalPrivate;

enum {
    PROP_0,
    PROP_GEOFENCE_PARAMS,
    PROP_MAX
};

static guint32 signals[LAST_SIGNAL] = { 0, };
static GParamSpec *properties[PROP_MAX] = {NULL, };
static int gIsCreated = 0;

#define GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), GEOFENCE_TYPE_INTERNAL, GeofenceInternalPrivate))

static void geofence_ielement_interface_init(GeofenceIElementInterface *iface);


G_DEFINE_TYPE_WITH_CODE(GeofenceInternal, geofence_internal, G_TYPE_OBJECT,
                        G_IMPLEMENT_INTERFACE(GEOFENCE_TYPE_IELEMENT,
                                              geofence_ielement_interface_init));

/*
static void geofence_parameter_set_property (GObject *object,
	guint property_id,
	const GValue *value,
	GParamSpec *pspec)
{
	GeofenceInternalPrivate* priv = GET_PRIVATE(object);
	g_return_if_fail (priv);
	g_return_if_fail (priv->mod);
	g_return_if_fail (priv->mod->handler);

	int ret = 0;
	switch (property_id){
		case PROP_GEOFENCE_PARAMS:
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}
*/

static void geofence_internal_dispose(GObject *gobject)
{
	GEOFENCE_LOGD("geofence_internal_dispose");

	GeofenceInternalPrivate *priv = GET_PRIVATE(gobject);
	g_return_if_fail(priv);

	g_mutex_clear(&priv->mutex);

	G_OBJECT_CLASS(geofence_internal_parent_class)->dispose(gobject);
}

static void geofence_internal_finalize(GObject *gobject)
{
	GEOFENCE_LOGD("geofence_internal_finalize");

	GeofenceInternalPrivate *priv = GET_PRIVATE(gobject);
	g_return_if_fail(priv);

	module_free(priv->mod, "geofence");
	priv->mod = NULL;

	if (priv->params) {
		geofence_parameter_free(priv->params);
		priv->params = NULL;
	}

	G_OBJECT_CLASS(geofence_internal_parent_class)->finalize(gobject);
}

static void geofence_internal_get_property(GObject *object,
                                           guint property_id,
                                           GValue *value,
                                           GParamSpec *pspec)
{
	GEOFENCE_LOGD("geofence_internal_get_property");
	GeofenceInternalPrivate *priv = GET_PRIVATE(object);
	g_return_if_fail(priv);
	g_return_if_fail(priv->mod);
	g_return_if_fail(priv->mod->handler);

	switch (property_id) {
		case PROP_GEOFENCE_PARAMS:
			g_value_set_boxed(value, priv->params);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

void geofence_signaling(GeofenceObject *obj,
                        guint32 signals[LAST_SIGNAL],
                        guint geofence_id,
                        guint state)
{
	GEOFENCE_LOGD("geofence_signaling");

	g_return_if_fail(obj);
	g_return_if_fail(signals);
	g_return_if_fail(state);

	if (state == GEOFENCE_STATE_IN) {
		GEOFENCE_LOGD("Signal emit: ZONE IN");
		g_signal_emit(obj, signals[ZONE_IN], 0, geofence_id, NULL);
	} else if (state == GEOFENCE_STATE_OUT) {
		GEOFENCE_LOGD("Signal emit : ZONE_OUT");
		g_signal_emit(obj, signals[ZONE_OUT], 0, geofence_id, NULL);
	}
}

void geofence_event_signaling(GeofenceObject *obj,
                              guint32 signals[LAST_SIGNAL],
                              guint place_id,
                              guint fence_id,
                              guint error,
                              guint state)
{
	GEOFENCE_LOGD("geofence_event_signaling");
	g_return_if_fail(obj);
	g_return_if_fail(signals);

	/* Signal has to be emitted here */
	g_signal_emit(obj, signals[GEOFENCE_EVENT], 0, place_id, fence_id, error, state, NULL);
}

static void _state_cb(int geofence_id, int state, gpointer userdata)
{
	GEOFENCE_LOGD("_state_cb");
	g_return_if_fail(userdata);
	GeofenceInternalPrivate *priv = GET_PRIVATE(userdata);
	g_return_if_fail(priv);
	GEOFENCE_LOGD("geofence id [%d] state [%d]", geofence_id, state);
	geofence_signaling(userdata, signals, geofence_id, state);
}

static void _geofence_event_cb(int place_id, int geofence_id, int error, int state, gpointer userdata)
{
	GEOFENCE_LOGD("_geofence_event_cb");
	g_return_if_fail(userdata);
	GeofenceInternalPrivate *priv = GET_PRIVATE(userdata);
	g_return_if_fail(priv);
	geofence_event_signaling(userdata, signals, place_id, geofence_id, error, state);
}

static int geofence_internal_create(GeofenceInternal *self)
{
	GEOFENCE_LOGD("geofence_internal_create");
	GeofenceInternalPrivate *priv = GET_PRIVATE(self);
	g_return_val_if_fail(priv, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->handler, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->ops.create, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	if (gIsCreated == 0) {
		ret = priv->mod->ops.create(priv->mod->handler, _state_cb, _geofence_event_cb, self);
		if (ret != GEOFENCE_MANAGER_ERROR_NONE) {
			GEOFENCE_LOGE("Fail to create. Error[%d]", ret);
		}
		gIsCreated = 1;
	}

	return ret;
}

static int geofence_internal_destroy(GeofenceInternal *self)
{
	GEOFENCE_LOGD("geofence_internal_destroy");
	GeofenceInternalPrivate *priv = GET_PRIVATE(self);
	g_return_val_if_fail(priv, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->handler, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->ops.destroy, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	ret = priv->mod->ops.destroy(priv->mod->handler);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE) {
		GEOFENCE_LOGE("Fail to destroy. Error[%d]", ret);
	}

	if (gIsCreated == 1) {
		gIsCreated = 0;
	}

	return ret;
}

static int geofence_internal_enable_service(GeofenceInternal *self, int geofence_id, bool enable)
{
	GEOFENCE_LOGD("geofence_internal_enable_service");
	GeofenceInternalPrivate *priv = GET_PRIVATE(self);
	g_return_val_if_fail(priv, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->handler, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->ops.enable_service, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	int ret = GEOFENCE_MANAGER_ERROR_NONE;
	ret = priv->mod->ops.enable_service(priv->mod->handler, geofence_id, enable);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE) {
		GEOFENCE_LOGE("Fail to enable the geofence. Error[%d]", ret);
	}
	return ret;
}

static int geofence_internal_start(GeofenceInternal *self, int geofence_id)
{
	GEOFENCE_LOGD("geofence_internal_start");
	GeofenceInternalPrivate *priv = GET_PRIVATE(self);
	g_return_val_if_fail(priv, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->handler, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->ops.start_geofence, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	ret = priv->mod->ops.start_geofence(priv->mod->handler, geofence_id);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE) {
		GEOFENCE_LOGE("Fail to start geofence. Error[%d]", ret);
	}
	return ret;
}

static int geofence_internal_stop(GeofenceInternal *self, int geofence_id)
{
	GEOFENCE_LOGD("geofence_internal_stop");
	GeofenceInternalPrivate *priv = GET_PRIVATE(self);
	g_return_val_if_fail(priv, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->handler, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->ops.stop_geofence, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	ret = priv->mod->ops.stop_geofence(priv->mod->handler, geofence_id);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE) {
		GEOFENCE_LOGE("Failed to stop. Error[%d]", ret);
	}
	return ret;
}

static int geofence_internal_add(GeofenceInternal *self, geofence_s *params, int *geofence_id)
{
	GEOFENCE_LOGD("geofence_internal_add");
	GeofenceInternalPrivate *priv = GET_PRIVATE(self);

	g_return_val_if_fail(priv, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->handler, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	g_return_val_if_fail(priv->mod->ops.add_geopoint, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->ops.add_bssid, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	if (priv->params) geofence_parameter_free(priv->params);
	priv->params = geofence_parameter_copy(params);

	g_return_val_if_fail(priv->params, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	if (priv->params->type == GEOFENCE_TYPE_GEOPOINT) {
		ret = priv->mod->ops.add_geopoint(priv->mod->handler, priv->params->place_id, priv->params->latitude, priv->params->longitude, priv->params->radius, priv->params->address, geofence_id);
		if (ret != GEOFENCE_MANAGER_ERROR_NONE) {
			GEOFENCE_LOGE("Fail to add geofence. Error[%d]", ret);
		} else {
			GEOFENCE_LOGE("Success to add geofence. Error[%d]", ret);
		}
	} else {
		ret = priv->mod->ops.add_bssid(priv->mod->handler, priv->params->place_id, priv->params->bssid, priv->params->ssid, priv->params->type, geofence_id);
		if (ret != GEOFENCE_MANAGER_ERROR_NONE) {
			GEOFENCE_LOGE("Fail to add geofence. Error[%d]", ret);
		} else {
			GEOFENCE_LOGE("Success to add geofence. Error[%d]", ret);
		}
	}

	return ret;
}

static int geofence_internal_remove(GeofenceInternal *self, int geofence_id)
{
	GEOFENCE_LOGD("geofence_internal_remove");
	GeofenceInternalPrivate *priv = GET_PRIVATE(self);
	g_return_val_if_fail(priv, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->handler, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->ops.remove_geofence, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	ret = priv->mod->ops.remove_geofence(priv->mod->handler, geofence_id);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE) {
		GEOFENCE_LOGE("Failed to remove geofence. Error[%d]", ret);
	}
	return ret;
}

static int geofence_internal_get_list(GeofenceInternal *self, int place_id, int *fence_amount, int **fence_ids, geofence_s **params)
{
	GEOFENCE_LOGD("geofence_internal_get_list");
	GeofenceInternalPrivate *priv = GET_PRIVATE(self);
	g_return_val_if_fail(priv, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->handler, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->ops.get_geofences, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	ret = priv->mod->ops.get_geofences(priv->mod->handler, place_id, fence_amount, fence_ids, params);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE) {
		GEOFENCE_LOGE("Failed to get list. Error[%d]", ret);
	}
	return ret;
}

static int geofence_internal_add_place(GeofenceInternal *self, const char *place_name, int *place_id)
{
	GEOFENCE_LOGD("geofence_internal_add_place");
	GeofenceInternalPrivate *priv = GET_PRIVATE(self);
	g_return_val_if_fail(priv, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->handler, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->ops.add_place, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	ret = priv->mod->ops.add_place(priv->mod->handler, place_name, place_id);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE) {
		GEOFENCE_LOGE("Failed to add place. Error[%d]", ret);
	} else {
		GEOFENCE_LOGE("Success to add place. Error[%d]", ret);
	}

	return ret;
}

static int geofence_internal_update_place(GeofenceInternal *self, const char *place_name, int place_id)
{
	GEOFENCE_LOGD("geofence_internal_update_place");

	GeofenceInternalPrivate *priv = GET_PRIVATE(self);

	g_return_val_if_fail(priv, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->handler, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->ops.update_place, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	ret = priv->mod->ops.update_place(priv->mod->handler, place_id, place_name);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE) {
		GEOFENCE_LOGE("Fail to update place. Error[%d]", ret);
	} else {
		GEOFENCE_LOGE("Success to update place. Error[%d]", ret);
	}

	return ret;
}

static int geofence_internal_remove_place(GeofenceInternal *self, int place_id)
{

	GEOFENCE_LOGD("geofence_internal_remove_place");
	GeofenceInternalPrivate *priv = GET_PRIVATE(self);
	g_return_val_if_fail(priv, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->handler, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->ops.remove_place, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	ret = priv->mod->ops.remove_place(priv->mod->handler, place_id);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE) {
		GEOFENCE_LOGE("Failed to remove place. Error[%d]", ret);
	} else {
		GEOFENCE_LOGE("Success to remove place. Error[%d]", ret);
	}
	return ret;
}

static int geofence_internal_get_place_name(GeofenceInternal *self, int place_id, char **place_name)
{
	GEOFENCE_LOGD("geofence_internal_get_place_name");
	GeofenceInternalPrivate *priv = GET_PRIVATE(self);
	g_return_val_if_fail(priv, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->handler, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->ops.get_place_name, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	ret = priv->mod->ops.get_place_name(priv->mod->handler, place_id, place_name);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE) {
		GEOFENCE_LOGE("Failed to get the place name. Error[%d]", ret);
	} else {
		GEOFENCE_LOGE("Success to get the place name. Error[%d]", ret);
	}
	return ret;
}

static int geofence_internal_get_place_list(GeofenceInternal *self, int *place_amount, int **place_ids, place_s **params)
{

	GEOFENCE_LOGD("geofence_internal_get_place_list");
	GeofenceInternalPrivate *priv = GET_PRIVATE(self);
	g_return_val_if_fail(priv, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->handler, GEOFENCE_MANAGER_ERROR_EXCEPTION);
	g_return_val_if_fail(priv->mod->ops.get_places, GEOFENCE_MANAGER_ERROR_EXCEPTION);

	int ret = GEOFENCE_MANAGER_ERROR_NONE;

	ret = priv->mod->ops.get_places(priv->mod->handler, place_amount, place_ids, params);
	if (ret != GEOFENCE_MANAGER_ERROR_NONE) {
		GEOFENCE_LOGE("Failed to get place list. Error[%d]", ret);
	}
	return ret;
}

static void geofence_ielement_interface_init(GeofenceIElementInterface *iface)
{
	iface->create = (TYPE_GEOFENCE_CREATE) geofence_internal_create;
	iface->destroy = (TYPE_GEOFENCE_DESTROY) geofence_internal_destroy;
	iface->enable = (TYPE_GEOFENCE_ENABLE) geofence_internal_enable_service;
	iface->start = (TYPE_GEOFENCE_START) geofence_internal_start;
	iface->stop = (TYPE_GEOFENCE_STOP) geofence_internal_stop;
	iface->add = (TYPE_GEOFENCE_ADD) geofence_internal_add;
	iface->remove = (TYPE_GEOFENCE_REMOVE) geofence_internal_remove;
	iface->get_list = (TYPE_GEOFENCE_GET_LIST) geofence_internal_get_list;

	iface->add_place = (TYPE_GEOFENCE_ADD_PLACE) geofence_internal_add_place;
	iface->update_place = (TYPE_GEOFENCE_UPDATE_PLACE) geofence_internal_update_place;
	iface->remove_place = (TYPE_GEOFENCE_REMOVE_PLACE) geofence_internal_remove_place;
	iface->get_place_name = (TYPE_GEOFENCE_GET_PLACE_NAME) geofence_internal_get_place_name;
	iface->get_place_list = (TYPE_GEOFENCE_GET_PLACE_LIST) geofence_internal_get_place_list;
}

static void geofence_internal_init(GeofenceInternal *self)
{
	GEOFENCE_LOGD("geofence_internal_init");

	GeofenceInternalPrivate *priv = GET_PRIVATE(self);
	g_return_if_fail(priv);

	priv->mod = (GeofenceInternalMod *)module_new("geofence");
	if (!priv->mod) GEOFENCE_LOGW("geofence module loading failed");

	g_mutex_init(&priv->mutex);
}

static void geofence_internal_class_init(GeofenceInternalClass *klass)
{
	GEOFENCE_LOGD("geofence_internal_class_init");
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	/*	gobject_class->set_property = geofence_parameter_set_property; */
	gobject_class->get_property = geofence_internal_get_property;

	gobject_class->dispose = geofence_internal_dispose;
	gobject_class->finalize = geofence_internal_finalize;

	g_type_class_add_private(klass, sizeof(GeofenceInternalPrivate));

	signals[ZONE_IN] = g_signal_new("zone-in",
	                                G_TYPE_FROM_CLASS(klass),
	                                G_SIGNAL_RUN_FIRST |
	                                G_SIGNAL_NO_RECURSE,
	                                G_STRUCT_OFFSET(GeofenceInternalClass, zone_in),
	                                NULL, NULL,
	                                geofence_VOID__UINT,
	                                G_TYPE_NONE, 1,
	                                G_TYPE_UINT);

	signals[ZONE_OUT] = g_signal_new("zone-out",
	                                 G_TYPE_FROM_CLASS(klass),
	                                 G_SIGNAL_RUN_FIRST |
	                                 G_SIGNAL_NO_RECURSE,
	                                 G_STRUCT_OFFSET(GeofenceInternalClass, zone_out),
	                                 NULL, NULL,
	                                 geofence_VOID__UINT,
	                                 G_TYPE_NONE, 1,
	                                 G_TYPE_UINT);

	signals[GEOFENCE_EVENT] = g_signal_new("geofence-event",
	                                       G_TYPE_FROM_CLASS(klass),
	                                       G_SIGNAL_RUN_FIRST |
	                                       G_SIGNAL_NO_RECURSE,
	                                       G_STRUCT_OFFSET(GeofenceInternalClass, geofence_event),
	                                       NULL, NULL,
	                                       g_cclosure_marshal_generic,
	                                       G_TYPE_NONE, 4,
	                                       G_TYPE_UINT,
	                                       G_TYPE_UINT,
	                                       G_TYPE_UINT,
	                                       G_TYPE_UINT);

	properties[PROP_GEOFENCE_PARAMS] = g_param_spec_boxed("geofence-parameter",
	                                                      "geofence parameter prop",
	                                                      "geofence parameter data",
	                                                      GEOFENCE_PRAMETER,
	                                                      G_PARAM_READABLE);

	g_object_class_install_properties(gobject_class,
	                                  PROP_MAX,
	                                  properties);

}
