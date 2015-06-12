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

#include "MapsUpdatePlace.h"
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

geofence_manager_h MapsUpdatePlace::pGeofenceManager;
geofence_h MapsUpdatePlace::pGeofenceParams;

Evas_Object* MapsUpdatePlace::pLayout = NULL;

int MapsUpdatePlace::pupdatePlaceId[];
int MapsUpdatePlace::nupdatePlaceCnt = 0;

Evas_Object* fencePopup_update = NULL;

int gupdatePlaceID = 0;

void* __updateNaviObj = NULL;

static const char* __EDJ = "/usr/apps/com.samsung.testgeo-sample/res/edje/homescreen.edj";

Evas_Object* updatePlaceLayout = NULL;

MapsUpdatePlace::MapsUpdatePlace(geofence_manager_h manager, Evas_Object* layout)
{
	pGeofenceManager = manager;
	pLayout = layout;
}

MapsUpdatePlace::~MapsUpdatePlace()
{
}

void MapsUpdatePlace::gl_UpdateplaceId_radio_sel_cb(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	gupdatePlaceID = 0;
	char* placeName = NULL;
	Elm_Object_Item *it = (Elm_Object_Item*)event_info;
	int index = (int) data;
	Evas_Object *radio;
	elm_genlist_item_selected_set(it, EINA_FALSE);
	radio = elm_object_item_part_content_get(it, "elm.swallow.end");
	elm_radio_value_set(radio, index + 1);

	WDEBUG("Selected Index :: %d", index);
	WDEBUG("Selected place Id :: %d", pupdatePlaceId[index]);
	gupdatePlaceID = pupdatePlaceId[index];
	evas_object_del(fencePopup_update);
	fencePopup_update = NULL;

	Evas_Object* resultEntry_placeId = NULL;
	resultEntry_placeId = elm_object_part_content_get(updatePlaceLayout, "geofence_result_placeid");
	elm_object_text_set(resultEntry_placeId,"--");

	Evas_Object* fencename_editfield =NULL;
	fencename_editfield = elm_object_part_content_get(updatePlaceLayout, "geofence_placename_part");
	Evas_Object* edit = elm_object_part_content_get(fencename_editfield, "elm.swallow.content");
	elm_object_part_text_set(fencename_editfield, "elm.guide", "Enter Place Name");

	int ret = geofence_manager_get_place_name(pGeofenceManager, gupdatePlaceID, &placeName);

	char tmp[100];
	snprintf(tmp, sizeof(tmp), "selected PlaceID: \"%d\"            PlaceName: \"%s\"", pupdatePlaceId[index], placeName);
	elm_object_text_set(resultEntry_placeId, tmp);
	evas_object_show(resultEntry_placeId);

	if(ret == GEOFENCE_MANAGER_ERROR_NONE){ 
		WDEBUG("geofence_manager_get_place_name function success!!");
		elm_entry_entry_set(edit, placeName);
		evas_object_show(edit);
	} else
		WDEBUG("geofence_manager_get_place_name function failed !!");

	if(placeName != NULL)
		free(placeName);
 }

Evas_Object* MapsUpdatePlace::gl_UpdateplaceId_radio_content_get_cb(void *data, Evas_Object *obj, const char *part)
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

char* MapsUpdatePlace::gl_UpdateplaceId_radio_text_get_cb(void *data, Evas_Object *obj, const char *part)
{
	if (!strcmp(part,"elm.text.main.left")){
		char buf[100];
		int index = (int) data;

		snprintf(buf, sizeof(buf), "place ID %d", pupdatePlaceId[index]);
		return strdup(buf);
	} else
		return NULL;
}

void MapsUpdatePlace::popup3_block_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	if(fencePopup_update != NULL){
		evas_object_del(fencePopup_update);
		fencePopup_update = NULL;
	}
}

void MapsUpdatePlace::getupdatePlaceID(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	int placeId;
	nupdatePlaceCnt = 0;

	GList* pPlaceList = MapsRemovePlace::getPlaceList(pGeofenceManager);

	GList* tmpList = g_list_first(pPlaceList);

	while(tmpList){
		placeId = (int)tmpList->data;
		pupdatePlaceId[nupdatePlaceCnt] = placeId ;

		nupdatePlaceCnt++;

		tmpList = g_list_next(tmpList);
	}

	if(nupdatePlaceCnt == 0) {
		WDEBUG("No Place to Update");
		return;
	}

	// get Place IDs
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	Evas_Object *popup;
	Evas_Object *genlist;
	Evas_Object *radio;
	Evas_Object *box;
	Evas_Object *win = updatePlaceLayout;
	int i;

	popup = elm_popup_add(win);
	ea_object_event_callback_add(popup, EA_CALLBACK_BACK, ea_popup_back_cb, NULL);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_smart_callback_add(popup, "block,clicked", popup3_block_clicked_cb, (void*)win);

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
	itc->func.text_get = gl_UpdateplaceId_radio_text_get_cb;
	itc->func.content_get = gl_UpdateplaceId_radio_content_get_cb;
	itc->func.state_get = NULL;
	itc->func.del = NULL;

	for (i = 0; i < nupdatePlaceCnt; i++) {
		elm_genlist_item_append(genlist, itc, (void *) i, NULL, ELM_GENLIST_ITEM_NONE, gl_UpdateplaceId_radio_sel_cb, (void *) i);
	}
	evas_object_show(genlist);

	elm_box_pack_end(box, genlist);
	evas_object_size_hint_min_set(box, -1, (ELM_SCALE_SIZE(288)));
	elm_object_content_set(popup, box);

	evas_object_show(popup);

	fencePopup_update = popup;
}

void MapsUpdatePlace::updatePlace(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();

	MapsUpdatePlace* self = (MapsUpdatePlace*)__updateNaviObj;

	Evas_Object* resultEntry = NULL;
	resultEntry = elm_object_part_content_get(pLayout, "geofence_result");

	const char *placename_Keyword = NULL;
	char result[1000]; // tmp[50];
	strcpy(result, "");
	Evas_Object *fencename_searchbar_layout = elm_object_part_content_get(updatePlaceLayout, "geofence_placename_part");
	Evas_Object* fencenameEntry = elm_object_part_content_get(fencename_searchbar_layout, "elm.swallow.content");
	placename_Keyword = (char*)elm_entry_entry_get(fencenameEntry);
	WDEBUG("FenceName :: %s", placename_Keyword);

	int place_id = gupdatePlaceID;
	if(placename_Keyword ==NULL || place_id == 0 ) {
		WDEBUG("Input val error..!!!");
		elm_object_text_set(resultEntry,"Wrong input value. Place is not Updated");
		evas_object_show(resultEntry);

		if (self->getNaviframe() != NULL)
			elm_naviframe_item_pop(self->getNaviframe()->getEvasObj());
		return;
	}
	int ret = 0;
	ret = geofence_manager_update_place(pGeofenceManager, place_id, placename_Keyword);

	// reset placeid
	gupdatePlaceID=0;

	if(ret != GEOFENCE_MANAGER_ERROR_NONE) {
		WDEBUG("geofence_manager_update_place FAILED");
		evas_object_show(resultEntry);

		if (self->getNaviframe() != NULL)
			elm_naviframe_item_pop(self->getNaviframe()->getEvasObj());
		return;
	} else {
		WDEBUG("geofence_manager_update_place SUCCESS");
		elm_object_text_set(resultEntry,"geofence_manager_update_place success");
		evas_object_show(resultEntry);

		if (self->getNaviframe() != NULL)
			elm_naviframe_item_pop(self->getNaviframe()->getEvasObj());
	}
}

Evas_Object* MapsUpdatePlace::onCreate( Evas_Object* parent, void* viewParam )
{
	WENTER();
	setTitle("Update Place");

	__updateNaviObj = this;

	gupdatePlaceID=0;

	// reset placeid
	gupdatePlaceID=0;

	Evas_Object* layout = elm_layout_add(parent);
	elm_layout_file_set(layout, __EDJ, "places_view");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(layout);

	const char *output_style = "DEFAULT='font=Arial:style=Bold font_size=35 color=#000000'";


	// getPlaceID BTN
	Evas_Object* addgetlistBtn = elm_button_add(layout);
	evas_object_size_hint_weight_set(addgetlistBtn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "updateplaceID_button", addgetlistBtn);
	evas_object_smart_callback_add(addgetlistBtn, "clicked", getupdatePlaceID, (void*)NULL);
	elm_object_domain_translatable_text_set(addgetlistBtn, PACKAGE, "Select PlaceID");

	// PlaceName editfield
	Evas_Object* fencename_searchbar_layout = elm_layout_add(layout);
	elm_layout_theme_set(fencename_searchbar_layout, "layout", "searchbar", "default");

	Evas_Object* fencename_editfield = ea_editfield_add(layout, EA_EDITFIELD_SEARCHBAR);
	ea_entry_selection_back_event_allow_set(fencename_editfield, EINA_TRUE);
	evas_object_size_hint_weight_set(fencename_editfield, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(fencename_editfield, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_part_text_set(fencename_editfield, "elm.guide", "Enter Place Name");
	elm_object_part_content_set(fencename_searchbar_layout, "elm.swallow.content", fencename_editfield);
	elm_object_part_content_set(layout, "geofence_placename_part", fencename_searchbar_layout);

	// Update button
	Evas_Object* addwifiBtn = elm_button_add(layout);
	evas_object_size_hint_weight_set(addwifiBtn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "updateplace_button", addwifiBtn);
	evas_object_smart_callback_add(addwifiBtn, "clicked", updatePlace, (void*)NULL);
	elm_object_domain_translatable_text_set(addwifiBtn, PACKAGE, "Update Place");

	Evas_Object* resultEntry_placeId = elm_entry_add(layout);
	ea_entry_selection_back_event_allow_set(resultEntry_placeId, EINA_TRUE);
	elm_entry_editable_set(resultEntry_placeId, EINA_FALSE);
	evas_object_size_hint_weight_set(resultEntry_placeId, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(resultEntry_placeId, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_object_part_content_set(layout, "geofence_result_placeid", resultEntry_placeId);
	elm_entry_text_style_user_push(resultEntry_placeId, output_style);
	elm_entry_entry_insert(resultEntry_placeId,"Please select place ID First ..");
	evas_object_show(resultEntry_placeId);

	updatePlaceLayout = layout;

	return layout;
}
