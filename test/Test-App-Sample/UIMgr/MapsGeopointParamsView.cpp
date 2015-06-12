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

#include "MapsGeopointParamsView.h"
#include "MapsRemovePlace.h"
#include "WPageView.h"
#include "Evas.h"
#include "Ecore_X.h"
#include <efl_assist.h>
#include "WApp.h"
#include "WWindow.h"
#include "WNaviframe.h"
#include "MapsDebug.h"

#define PACKAGE "testgeo-sample"

geofence_manager_h MapsGeopointParamsView::pGeofenceManager;
geofence_h MapsGeopointParamsView::pGeofenceParams;

Evas_Object* MapsGeopointParamsView::pLayout = NULL;
int MapsGeopointParamsView::pPlaceId[];
int MapsGeopointParamsView::nPlaceCnt = 0;

GList* MapsGeopointParamsView::pFenceList = NULL;
GList* MapsGeopointParamsView::pFenceListAll = NULL;
GList* MapsGeopointParamsView::pTrackingFenceList = NULL;

double MapsGeopointParamsView::dLat = 0.0;
double MapsGeopointParamsView::dLon = 0.0;
int MapsGeopointParamsView::nRadius = 0;
//int MapsGeopointParamsView::fcounter = 0;

Evas_Object* fencePopup_pid = NULL;

void* __geopointParamsNaviObj = NULL;
bool bFenceListRetrieval = false;
int gPlaceID = 0;

static const char* __EDJ = "/usr/apps/com.samsung.testgeo-sample/res/edje/homescreen.edj";

Evas_Object* geopointParamsLayout = NULL;

MapsGeopointParamsView::MapsGeopointParamsView(geofence_manager_h manager, Evas_Object* layout)
{
	pGeofenceManager = manager;
	pLayout = layout;
}

MapsGeopointParamsView::~MapsGeopointParamsView()
{
}

bool MapsGeopointParamsView::geofence_manager_fence_cb(int geofence_id, geofence_h params, int index, int total_fence_count, void *user_data)
{
	WENTER();
	//fcounter = total_fence_count;
	WDEBUG("inside geofence_manager_fence_cb:: index=%d total_fence_count=%d ",index,total_fence_count);

	if(total_fence_count == 0)
		bFenceListRetrieval = true;
	else if(index <= total_fence_count){
		if(pFenceListAll == NULL)
			pFenceListAll = g_list_append(pFenceListAll, (gpointer)geofence_id);
		else
			pFenceListAll = g_list_insert_before(pFenceListAll, NULL, (gpointer)geofence_id);

		if(index == total_fence_count)
			bFenceListRetrieval = true;
	}

	return true;
}

GList* MapsGeopointParamsView::getFenceList(geofence_manager_h manager)
{
	WENTER();
	bFenceListRetrieval = false;
	pFenceListAll = NULL;

    int ret = geofence_manager_foreach_geofence_list (manager, geofence_manager_fence_cb, (void*)NULL);

	if(ret == GEOFENCE_MANAGER_ERROR_NONE)
	{
		while(!bFenceListRetrieval) {}
		WDEBUG("inside if block getFenceList:: ret value=%d",ret);
		return pFenceListAll;
	}

	return NULL;
}

GList* MapsGeopointParamsView::getTrackingFenceList()
{
	return pTrackingFenceList;
}

void MapsGeopointParamsView::addTrackingFenceId(int fenceId)
{
	WENTER();

	if(pTrackingFenceList == NULL)
		pTrackingFenceList = g_list_append(pTrackingFenceList, (gpointer)fenceId);
	else
		pTrackingFenceList = g_list_insert_before(pTrackingFenceList, NULL, (gpointer)fenceId);
}

void
MapsGeopointParamsView::removeTrackingFenceId(int fenceId)
{
	if(pTrackingFenceList != NULL){
		pTrackingFenceList = g_list_remove(pTrackingFenceList, (gpointer)fenceId);
		WDEBUG("Fence ID :: %d removed from pTrackingFenceList", fenceId);
	}
}

void MapsGeopointParamsView::removeFenceId(int fenceId)
{
	if(pFenceList != NULL)
		pFenceList = g_list_remove(pFenceList, (gpointer)fenceId);

	if(pTrackingFenceList != NULL)
		pTrackingFenceList = g_list_remove(pTrackingFenceList, (gpointer)fenceId);
}

void MapsGeopointParamsView::removeAllFenceList()
{
	if(pFenceList != NULL) {
		g_list_free(pFenceList);
		pFenceList = NULL;
	}

	if(pTrackingFenceList != NULL) {
		g_list_free(pTrackingFenceList);
		pTrackingFenceList = NULL;
	}
}

void MapsGeopointParamsView::popScreen(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();

	MapsGeopointParamsView* self = (MapsGeopointParamsView*)__geopointParamsNaviObj;

	if (self->getNaviframe() != NULL)
		elm_naviframe_item_pop(self->getNaviframe()->getEvasObj());
}

void MapsGeopointParamsView::getError(Evas_Object *obj, int ret)
{
	switch(ret) {
	case -1:
		elm_object_text_set(obj,"geofence_manager_create_geopoint_params FAILED with GEOFENCE_MANAGER_ERROR_NOT_INITIALIZED");
		break;
	case -2:
		elm_object_text_set(obj,"geofence_manager_create_geopoint_params FAILED with GEOFENCE_MANAGER_ERROR_ILLEGAL_ARGUMENT");
		break;
	case -3:
		elm_object_text_set(obj,"geofence_manager_create_geopoint_params FAILED with GEOFENCE_MANAGER_ERROR_INVALID_ID");
		break;
	case -4:
		elm_object_text_set(obj,"geofence_manager_create_geopoint_params FAILED with GEOFENCE_MANAGER_ERROR_EXCEPTION");
		break;
	case -5:
		elm_object_text_set(obj,"geofence_manager_create_geopoint_params FAILED with GEOFENCE_MANAGER_ERROR_ALREADY_STARTED");
		break;
	case -6:
		elm_object_text_set(obj,"geofence_manager_create_geopoint_params FAILED with GEOFENCE_MANAGER_ERROR_TOO_MANY_GEOFENCE");
		break;
	default:
		elm_object_text_set(obj,"geofence_manager_create_geopoint_params FAILED with UNKNOWN ERROR");
		break;
	}
}

void MapsGeopointParamsView::gl_placeId_radio_sel_cb(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	Evas_Object* resultEntry_svr = NULL;
	resultEntry_svr = elm_object_part_content_get(geopointParamsLayout, "geofence_result_svr");
	elm_object_text_set(resultEntry_svr,"--");

	Evas_Object* resultEntry = NULL;
	resultEntry = elm_object_part_content_get(geopointParamsLayout, "geofence_result");
	elm_object_text_set(resultEntry,"--");

	gPlaceID = 0;
	Elm_Object_Item *it = (Elm_Object_Item*)event_info;
	int index = (int) data;
	Evas_Object *radio;
	elm_genlist_item_selected_set(it, EINA_FALSE);
	radio = elm_object_item_part_content_get(it, "elm.swallow.end");
	elm_radio_value_set(radio, index + 1);

	WDEBUG("Selected Index :: %d", index);
	WDEBUG("Selected place Id :: %d", pPlaceId[index]);
	gPlaceID = pPlaceId[index];
	evas_object_del(fencePopup_pid);
	fencePopup_pid = NULL;

	Evas_Object* resultEntry_placeId = NULL;
	resultEntry_placeId = elm_object_part_content_get(geopointParamsLayout, "geofence_result_placeid");
	elm_object_text_set(resultEntry_placeId,"--");

	char tmp[100];
	snprintf(tmp, sizeof(tmp), "selected Place ID is : \"%d\"", pPlaceId[index]);
	elm_object_text_set(resultEntry_placeId, tmp);
	evas_object_show(resultEntry_placeId);
 }

Evas_Object* MapsGeopointParamsView::gl_placeId_radio_content_get_cb(void *data, Evas_Object *obj, const char *part)
{
	int index = (int) data;

	if (!strcmp(part, "elm.icon.2")) {
		Evas_Object *content = elm_layout_add(obj);
		Evas_Object *radio;
		Evas_Object *radio_main = (Evas_Object*)evas_object_data_get(obj, "radio");
		elm_layout_theme_set(content, "layout", "list/C/type.2", "default");
		radio = elm_radio_add(content);
		elm_radio_group_add(radio, radio_main);
		elm_radio_state_value_set(radio, index + 1);

		if (index == 1)
			elm_radio_value_set(radio, 1);

		evas_object_size_hint_weight_set(radio, EVAS_HINT_EXPAND,EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(radio, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_propagate_events_set(radio, EINA_FALSE);
		elm_layout_content_set(content, "elm.swallow.content", radio);
		return content;
	}

	return NULL;
}

char* MapsGeopointParamsView::gl_placeId_radio_text_get_cb(void *data, Evas_Object *obj, const char *part)
{
	if (!strcmp(part,"elm.text.main.left")){
		char buf[100];
		int index = (int) data;

		snprintf(buf, sizeof(buf), "place ID %d", pPlaceId[index]);
		return strdup(buf);
	} else
		return NULL;
 }

void MapsGeopointParamsView::popup1_block_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	if(fencePopup_pid != NULL) {
		evas_object_del(fencePopup_pid);
		fencePopup_pid = NULL;
	}
}

void MapsGeopointParamsView::getPlaceID(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();

	int placeId;
	int nPlaceCnt =0;

    GList* pPlaceList = MapsRemovePlace::getPlaceList(pGeofenceManager);
	GList* tmpList = g_list_first(pPlaceList);

	while(tmpList) {
		placeId = (int)tmpList->data;
		pPlaceId[nPlaceCnt] = placeId;

		nPlaceCnt++;

		tmpList = g_list_next(tmpList);
	}

	if(nPlaceCnt == 0) {
		WDEBUG("No fences to remove");
		return;
	}

	/* get Place IDs */
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	Evas_Object *popup;
	Evas_Object *genlist;
	Evas_Object *radio;
	Evas_Object *box;
	Evas_Object *win = geopointParamsLayout;
	int i;

	popup = elm_popup_add(win);
	ea_object_event_callback_add(popup, EA_CALLBACK_BACK, ea_popup_back_cb, NULL);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_smart_callback_add(popup, "block,clicked", popup1_block_clicked_cb, (void*)win);

	box = elm_box_add(popup);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	genlist = elm_genlist_add(box);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);

	radio = elm_radio_add(genlist);
	elm_radio_state_value_set(radio, 0);
	elm_radio_value_set(radio, 0);
	evas_object_data_set(genlist, "radio", radio);

	itc->item_style = "1line";
	itc->func.text_get = gl_placeId_radio_text_get_cb;
	itc->func.content_get = gl_placeId_radio_content_get_cb;
	itc->func.state_get = NULL;
	itc->func.del = NULL;

	for (i = 0; i < nPlaceCnt; i++) {
		elm_genlist_item_append(genlist, itc, (void *) i, NULL, ELM_GENLIST_ITEM_NONE, gl_placeId_radio_sel_cb, (void *) i);
	}
	evas_object_show(genlist);

	elm_box_pack_end(box, genlist);
	evas_object_size_hint_min_set(box, -1, (ELM_SCALE_SIZE(288)));
	elm_object_content_set(popup, box);

	evas_object_show(popup);

	fencePopup_pid = popup;
}

void MapsGeopointParamsView::addFence(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	int index = (int) data;
	MapsGeopointParamsView* self = (MapsGeopointParamsView*)__geopointParamsNaviObj;

	Evas_Object* resultEntry = NULL;
	resultEntry = elm_object_part_content_get(pLayout, "geofence_result");
	elm_object_text_set(resultEntry,"--");

	const char *latKeyword = NULL, *lonKeyword = NULL, *radiusKeyword = NULL;
	const char *bssidKeyword = NULL;
	const char *add_ssidKeyword = NULL;

	char result[1000], tmp1[1000];
	strcpy(result, "");

	Evas_Object *add_ssid_searchbar_layout = elm_object_part_content_get(geopointParamsLayout, "places_address_part");
	Evas_Object* add_ssidEntry = elm_object_part_content_get(add_ssid_searchbar_layout, "elm.swallow.content");

	add_ssidKeyword = (char*)elm_entry_entry_get(add_ssidEntry);
	WDEBUG("Address/SSID :: %s", add_ssidKeyword);

	Evas_Object *lat_searchbar_layout = elm_object_part_content_get(geopointParamsLayout, "places_latitude_part");
	Evas_Object* latEntry = elm_object_part_content_get(lat_searchbar_layout, "elm.swallow.content");

	latKeyword = (char*)elm_entry_entry_get(latEntry);
	WDEBUG("Latitude :: %s", latKeyword);

	Evas_Object *lon_searchbar_layout = elm_object_part_content_get(geopointParamsLayout, "places_longitude_part");
	Evas_Object* lonEntry = elm_object_part_content_get(lon_searchbar_layout, "elm.swallow.content");

	lonKeyword = (char*)elm_entry_entry_get(lonEntry);
	WDEBUG("Longitude :: %s", lonKeyword);

	Evas_Object *radius_searchbar_layout = elm_object_part_content_get(geopointParamsLayout, "places_distance_part");
	Evas_Object* radiusEntry = elm_object_part_content_get(radius_searchbar_layout, "elm.swallow.content");

	radiusKeyword = (char*)elm_entry_entry_get(radiusEntry);
	WDEBUG("Radius :: %s", radiusKeyword);

	Evas_Object *bssid_searchbar_layout = elm_object_part_content_get(geopointParamsLayout, "places_bssid_part");
	Evas_Object* bssidEntry = elm_object_part_content_get(bssid_searchbar_layout, "elm.swallow.content");

	bssidKeyword = (char*)elm_entry_entry_get(bssidEntry);
	WDEBUG(" Bssid :: %s", bssidKeyword);

	int place_id=gPlaceID;
	WDEBUG("selected place_id from list is = %d",place_id);
	double lat = 0.0, lon = 0.0;
	int radius = 0;

	if (latKeyword != NULL)
		lat = atof(latKeyword);
	if (lonKeyword != NULL)
		lon = atof(lonKeyword);
	radius = atoi(radiusKeyword);


	if(place_id == 0 || lat == 0.0 || lon == 0.0 || radius == 0 || bssidKeyword == NULL || add_ssidKeyword == NULL) {
		WDEBUG("Input val error..!!!");
		elm_object_text_set(resultEntry,"Wrong input value. Fence Id not created");
		evas_object_show(resultEntry);

		if (self->getNaviframe() != NULL)
			elm_naviframe_item_pop(self->getNaviframe()->getEvasObj());
		return;
	}

	int ret = 0;

	if(index == 0)
		ret = geofence_create_geopoint(place_id, lat, lon, radius, add_ssidKeyword, &pGeofenceParams);

	if(index == 1)
		ret = geofence_create_wifi(place_id, bssidKeyword, add_ssidKeyword, &pGeofenceParams);

	if(index == 2)
		ret = geofence_create_bluetooth(place_id, bssidKeyword, add_ssidKeyword, &pGeofenceParams);

	if(ret != GEOFENCE_MANAGER_ERROR_NONE) {
		WDEBUG("geofence_manager_create_geopoint_params FAILED");
		getError(resultEntry, ret);
		evas_object_show(resultEntry);

		if (self->getNaviframe() != NULL)
			elm_naviframe_item_pop(self->getNaviframe()->getEvasObj());
		return;
	} else
		WDEBUG("geofence_manager_create_params SUCCESS");

	int fenceId = -1;

	ret = geofence_manager_add_fence(pGeofenceManager, pGeofenceParams, &fenceId);

	//reset the place-id
	gPlaceID = 0;
	if(ret != GEOFENCE_MANAGER_ERROR_NONE) {
		WDEBUG("geofence_manager_add FAILED");
		getError(resultEntry, ret);
		evas_object_show(resultEntry);

		if (self->getNaviframe() != NULL)
			elm_naviframe_item_pop(self->getNaviframe()->getEvasObj());
		return;
	} else {
		WDEBUG("[VARUN]geofence_manager_add SUCCESS");
		snprintf(tmp1, sizeof(tmp1), "Fence Id: %d created", fenceId);
		strcat(result, tmp1);
	}

	ret = geofence_destroy(pGeofenceParams);
	if(ret != GEOFENCE_MANAGER_ERROR_NONE) {
		WDEBUG("geofence_destroy FAILED");
		getError(resultEntry, ret);
		evas_object_show(resultEntry);

		if (self->getNaviframe() != NULL)
			elm_naviframe_item_pop(self->getNaviframe()->getEvasObj());
		return;
	} else {
		WDEBUG("geofence_destroy SUCCESS");
	}

	if(pFenceList == NULL)
		pFenceList = g_list_append(pFenceList, (gpointer)fenceId);
	else
		pFenceList = g_list_insert_before(pFenceList, NULL, (gpointer)fenceId);

	WDEBUG("[vaun]before showing resultEntry");
	elm_object_text_set(resultEntry,result);
	evas_object_show(resultEntry);

	if (self->getNaviframe() != NULL)
		elm_naviframe_item_pop(self->getNaviframe()->getEvasObj());
}

Evas_Object* MapsGeopointParamsView::onCreate( Evas_Object* parent, void* viewParam )
{
	WENTER();
	setTitle("Add Fence");

	__geopointParamsNaviObj = this;

	/* reset placeid */
	gPlaceID=0;
	Evas_Object* layout = elm_layout_add(parent);
	elm_layout_file_set(layout, __EDJ, "places_view");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(layout);

	const char *output_style = "DEFAULT='font=Arial:style=Bold font_size=35 color=#000000'";

	/* getPlaceID BTN */
	Evas_Object* addgetlistBtn = elm_button_add(layout);
	evas_object_size_hint_weight_set(addgetlistBtn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "getplaceID_button", addgetlistBtn);
	evas_object_smart_callback_add(addgetlistBtn, "clicked", getPlaceID, (void*)NULL);
	elm_object_domain_translatable_text_set(addgetlistBtn, PACKAGE, "Select PlaceID");

	/* Address/SSID editfield */
	/* Latitude editfield */
	Evas_Object* address_searchbar_layout = elm_layout_add(layout);
	elm_layout_theme_set(address_searchbar_layout, "layout", "searchbar", "default");
	Evas_Object* address_editfield = ea_editfield_add(layout, EA_EDITFIELD_SEARCHBAR);
	ea_entry_selection_back_event_allow_set(address_editfield, EINA_TRUE);
	evas_object_size_hint_weight_set(address_editfield, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(address_editfield, EVAS_HINT_FILL, EVAS_HINT_FILL);
	/* elm_object_part_text_set(address_editfield, "elm.guide", "Enter Address / SSID"); */
	/* elm_object_part_text_set(lat_editfield, "elm.guide", "Latitude"); */
	elm_object_text_set(address_editfield, "Dubai MR");
	elm_object_part_content_set(address_searchbar_layout, "elm.swallow.content", address_editfield);
	elm_object_part_content_set(layout, "places_address_part", address_searchbar_layout);

	/* Latitude editfield */
	Evas_Object* lat_searchbar_layout = elm_layout_add(layout);
	elm_layout_theme_set(lat_searchbar_layout, "layout", "searchbar", "default");
	Evas_Object* lat_editfield = ea_editfield_add(layout, EA_EDITFIELD_SEARCHBAR);
	ea_entry_selection_back_event_allow_set(lat_editfield, EINA_TRUE);
	evas_object_size_hint_weight_set(lat_editfield, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(lat_editfield, EVAS_HINT_FILL, EVAS_HINT_FILL);
	/* elm_object_part_text_set(lat_editfield, "elm.guide", "Latitude");*/
	elm_object_text_set(lat_editfield, "12.98025");
	elm_object_part_content_set(lat_searchbar_layout, "elm.swallow.content", lat_editfield);
	elm_object_part_content_set(layout, "places_latitude_part", lat_searchbar_layout);

	/* Longitude editifield */
	Evas_Object* lon_searchbar_layout = elm_layout_add(layout);
	elm_layout_theme_set(lon_searchbar_layout, "layout", "searchbar", "default");
	Evas_Object* lon_editfield = ea_editfield_add(layout, EA_EDITFIELD_SEARCHBAR);
	ea_entry_selection_back_event_allow_set(lon_editfield, EINA_TRUE);
	evas_object_size_hint_weight_set(lon_editfield, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(lon_editfield, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_text_set(lon_editfield, "77.69773");
	elm_object_part_content_set(lon_searchbar_layout, "elm.swallow.content", lon_editfield);
	elm_object_part_content_set(layout, "places_longitude_part", lon_searchbar_layout);

	/* Distance editfield */
	Evas_Object* dist_searchbar_layout = elm_layout_add(layout);
	elm_layout_theme_set(dist_searchbar_layout, "layout", "searchbar", "default");

	Evas_Object* dist_editfield = ea_editfield_add(layout, EA_EDITFIELD_SEARCHBAR);
	ea_entry_selection_back_event_allow_set(dist_editfield, EINA_TRUE);
	evas_object_size_hint_weight_set(dist_editfield, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(dist_editfield, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_text_set(dist_editfield, "200.0");
	elm_object_part_content_set(dist_searchbar_layout, "elm.swallow.content", dist_editfield);
	elm_object_part_content_set(layout, "places_distance_part", dist_searchbar_layout);

	/* BSSID editfield */
	Evas_Object* bssid_searchbar_layout = elm_layout_add(layout);
	elm_layout_theme_set(bssid_searchbar_layout, "layout", "searchbar", "default");
	Evas_Object* bssid_editfield = ea_editfield_add(layout, EA_EDITFIELD_SEARCHBAR);
	ea_entry_selection_back_event_allow_set(bssid_editfield, EINA_TRUE);
	evas_object_size_hint_weight_set(bssid_editfield, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(bssid_editfield, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_text_set(bssid_editfield, "88:30:8a:6c:28:15");
	elm_object_part_content_set(bssid_searchbar_layout, "elm.swallow.content", bssid_editfield);
	elm_object_part_content_set(layout, "places_bssid_part", bssid_searchbar_layout);

	/* gps button */
	int index = 0;
	Evas_Object* addgpsBtn = elm_button_add(layout);
	evas_object_size_hint_weight_set(addgpsBtn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "gpsbtn_button", addgpsBtn);
	evas_object_smart_callback_add(addgpsBtn, "clicked", addFence, (void*)(index));
	elm_object_domain_translatable_text_set(addgpsBtn, PACKAGE, "GPS");
	index++;

	/* wifi button */
	Evas_Object* addwifiBtn = elm_button_add(layout);
	evas_object_size_hint_weight_set(addwifiBtn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "wifibtn_button", addwifiBtn);
	evas_object_smart_callback_add(addwifiBtn, "clicked", addFence, (void*)(index));
	elm_object_domain_translatable_text_set(addwifiBtn, PACKAGE, "Wi-Fi");
	index++;

	/* BT button */
	Evas_Object* addbtBtn = elm_button_add(layout);
	evas_object_size_hint_weight_set(addbtBtn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "btbtn_button", addbtBtn);
	evas_object_smart_callback_add(addbtBtn, "clicked", addFence, (void*)(index));
	elm_object_domain_translatable_text_set(addbtBtn, PACKAGE, "BT");

	Evas_Object* resultEntry_placeId = elm_entry_add(layout);
	ea_entry_selection_back_event_allow_set(resultEntry_placeId, EINA_TRUE);
	elm_entry_editable_set(resultEntry_placeId, EINA_FALSE);
	evas_object_size_hint_weight_set(resultEntry_placeId, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(resultEntry_placeId, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_part_content_set(layout, "geofence_result_placeid", resultEntry_placeId);
	elm_entry_text_style_user_push(resultEntry_placeId, output_style);
	elm_entry_entry_insert(resultEntry_placeId,"Please select Place ID First!!");
	evas_object_show(resultEntry_placeId);

	geopointParamsLayout = layout;

	return layout;
}
