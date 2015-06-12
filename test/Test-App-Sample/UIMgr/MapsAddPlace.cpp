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

#include "MapsAddPlace.h"

#include "WPageView.h"
#include "Evas.h"
#include "Ecore_X.h"
#include <efl_assist.h>
#include "WApp.h"
#include "WWindow.h"
#include "WNaviframe.h"
#include "MapsDebug.h"

#define PACKAGE "testgeo-sample"

geofence_manager_h MapsAddPlace::pGeofenceManager;

geofence_h MapsAddPlace::pGeofenceParams;

Evas_Object* MapsAddPlace::pLayout = NULL;

GList* MapsAddPlace::pPlaceList = NULL;

void* __addPlaceNaviObj = NULL;

static const char* __EDJ = "/usr/apps/com.samsung.testgeo-sample/res/edje/homescreen.edj";

Evas_Object* addPlaceLayout = NULL;

MapsAddPlace::MapsAddPlace(geofence_manager_h manager, Evas_Object* layout)
{
	pGeofenceManager = manager;
	pLayout = layout;
}

MapsAddPlace::~MapsAddPlace()
{

}

void MapsAddPlace::removePlaceId(int placeId)
{
	if(pPlaceList != NULL)
		pPlaceList = g_list_remove(pPlaceList, (gpointer)placeId);
}

void MapsAddPlace::getError(Evas_Object *obj, int ret)
{
	switch(ret){
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

void MapsAddPlace::addPlace(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
    
	MapsAddPlace* self = (MapsAddPlace*)__addPlaceNaviObj;

	Evas_Object* resultEntry = NULL;
	resultEntry = elm_object_part_content_get(pLayout, "geofence_result");
	
	const char *Placename_Keyword = NULL;
	char result[1000] ; //tmp[50];
	strcpy(result, "");

	Evas_Object *fencename_searchbar_layout = elm_object_part_content_get(addPlaceLayout, "geofence_placename_part");
	Evas_Object* fencenameEntry = elm_object_part_content_get(fencename_searchbar_layout, "elm.swallow.content");

	Placename_Keyword = (char*)elm_entry_entry_get(fencenameEntry);
	WDEBUG("PlaceName :: %s", Placename_Keyword);

	int place_id = -1;

	if (Placename_Keyword ==NULL){
		WDEBUG("Input val error..!!!");
		elm_object_text_set(resultEntry,"Wrong input value..");
		evas_object_show(resultEntry);

		if (self->getNaviframe() != NULL)
			elm_naviframe_item_pop(self->getNaviframe()->getEvasObj());
		return;
	}

	int ret = 0;
    ret = geofence_manager_add_place(pGeofenceManager,Placename_Keyword, &place_id);

	int placeid = place_id;
	WDEBUG("added  placeid = %d",placeid);

	if (ret != GEOFENCE_MANAGER_ERROR_NONE){
		WDEBUG("geofence_manager_add_place FAILED");
		getError(resultEntry, ret);
		evas_object_show(resultEntry);

		if (self->getNaviframe() != NULL)
			elm_naviframe_item_pop(self->getNaviframe()->getEvasObj());
		return;
	} else {
		WDEBUG("geofence_manager_add_place SUCCESS");
		snprintf(result,sizeof(result), "Place Id : %d Created",placeid);
	}

	if (pPlaceList == NULL) {
		pPlaceList = g_list_append(pPlaceList, (gpointer)placeid);
	} else {
		pPlaceList = g_list_insert_before(pPlaceList, NULL, (gpointer)placeid);
	}

	elm_object_text_set(resultEntry,result);
	evas_object_show(resultEntry);

	if (self->getNaviframe() != NULL)
		elm_naviframe_item_pop(self->getNaviframe()->getEvasObj());
}

Evas_Object* MapsAddPlace::onCreate( Evas_Object* parent, void* viewParam )
{
	WENTER();
	setTitle("Add Place");

	__addPlaceNaviObj = this;

	Evas_Object* layout = elm_layout_add(parent);
	elm_layout_file_set(layout, __EDJ, "places_view");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(layout);

	Evas_Object* fencename_searchbar_layout = elm_layout_add(layout);
	elm_layout_theme_set(fencename_searchbar_layout, "layout", "searchbar", "default");
	Evas_Object* fencename_editfield = ea_editfield_add(layout, EA_EDITFIELD_SEARCHBAR);
	ea_entry_selection_back_event_allow_set(fencename_editfield, EINA_TRUE);
	evas_object_size_hint_weight_set(fencename_editfield, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(fencename_editfield, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_part_text_set(fencename_editfield, "elm.guide", "Enter Place Name");
	elm_object_part_content_set(fencename_searchbar_layout, "elm.swallow.content", fencename_editfield);
	elm_object_part_content_set(layout, "geofence_placename_part", fencename_searchbar_layout);

	Evas_Object* addwifiBtn = elm_button_add(layout);
	evas_object_size_hint_weight_set(addwifiBtn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "wifibtn_button", addwifiBtn);
	evas_object_smart_callback_add(addwifiBtn, "clicked", addPlace, (void*)NULL);
	elm_object_domain_translatable_text_set(addwifiBtn, PACKAGE, "Add Place");

	addPlaceLayout = layout;

	return layout;
}
