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

geofence_manager_h MapsRemovePlace::pGeofenceManager;
geofence_h MapsRemovePlace::pGeofenceParams;

Evas_Object* MapsRemovePlace::pLayout = NULL;

int MapsRemovePlace::premovePlaceId[];
int MapsRemovePlace::nremovePlaceCnt = 0;
//int MapsRemovePlace::pcounter = 0;

GList* MapsRemovePlace::pPlaceListAll = NULL;

Evas_Object* fencePopup_remove = NULL;

void* __removePlaceNaviObj = NULL;

static const char* __EDJ = "/usr/apps/com.samsung.testgeo-sample/res/edje/homescreen.edj";

Evas_Object* removePlaceLayout = NULL;

bool bPlaceListRetrieval = false;


MapsRemovePlace::MapsRemovePlace(geofence_manager_h manager, Evas_Object* layout)
{
	pGeofenceManager = manager;
	pLayout = layout;
}

MapsRemovePlace::~MapsRemovePlace()
{
}

bool MapsRemovePlace::geofence_manager_place_cb(int place_id, const char *place_name, int place_index, int place_cnt, void *user_data)
{
	WENTER();

	//pcounter++;
	if(place_name==NULL)
		WDEBUG("[varun]inside geofence_manager_place_cb:: place_name = NULL ");
	else
		WDEBUG("[varun] inside geofence_manager_place_cb:: place_name = %s ",place_name);

	WDEBUG("inside geofence_manager_place_cb:: place_index=%d place_cnt=%d ",place_index,place_cnt);

	if(place_cnt == 0)
		bPlaceListRetrieval = true;
	else if(place_index <= place_cnt){
		if(pPlaceListAll == NULL)
			pPlaceListAll = g_list_append(pPlaceListAll, (gpointer)place_id);
		else
			pPlaceListAll = g_list_insert_before(pPlaceListAll, NULL, (gpointer)place_id);

		if(place_index == place_cnt)
			bPlaceListRetrieval = true;
	}

	return true;
}

GList* MapsRemovePlace::getPlaceList(geofence_manager_h manager)
{
	WENTER();
	bPlaceListRetrieval = false;
	pPlaceListAll = NULL;
	//pcounter=0;
    int ret = geofence_manager_foreach_place_list(manager, geofence_manager_place_cb, (void*)NULL);

	if(ret == GEOFENCE_MANAGER_ERROR_NONE){
		while(!bPlaceListRetrieval) {}

		return pPlaceListAll;
	}

	return NULL;
}

void MapsRemovePlace::gl_RemoveplaceId_radio_sel_cb(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	MapsRemovePlace* self = (MapsRemovePlace*)__removePlaceNaviObj;
	char tmp[100];

	Evas_Object* resultEntry = NULL;
	resultEntry = elm_object_part_content_get(pLayout, "geofence_result");
	elm_object_text_set(resultEntry,"--");

	Elm_Object_Item *it = (Elm_Object_Item*)event_info;
	int index = (int) data;
	Evas_Object *radio;
	elm_genlist_item_selected_set(it, EINA_FALSE);
	radio = elm_object_item_part_content_get(it, "elm.swallow.end");
	elm_radio_value_set(radio, index + 1);

	WDEBUG("Selected Index :: %d", index);
	WDEBUG("Selected place Id :: %d", premovePlaceId[index]);
	evas_object_del(fencePopup_remove);
	fencePopup_remove = NULL;
	int ret = geofence_manager_remove_place(pGeofenceManager, premovePlaceId[index]);

	if(ret == GEOFENCE_MANAGER_ERROR_NONE)
	{
		//snprintf(tmp, sizeof(tmp), "Removed Place ID \"%d\"",  premovePlaceId[index]);
		snprintf(tmp,sizeof(tmp), "geofence_manager_remove_place Success");
		MapsAddPlace::removePlaceId(premovePlaceId[index]);
		elm_object_text_set(resultEntry, tmp);
		evas_object_show(resultEntry);

		if (self->getNaviframe() != NULL)
			elm_naviframe_item_pop(self->getNaviframe()->getEvasObj());
	} else {
		snprintf(tmp, sizeof(tmp), "Not Authorized to Remove Place ID \"%d\"",premovePlaceId[index]);
		elm_object_text_set(resultEntry, tmp);
		evas_object_show(resultEntry);

		if (self->getNaviframe() != NULL)
			elm_naviframe_item_pop(self->getNaviframe()->getEvasObj());
	}
}

Evas_Object* MapsRemovePlace::gl_RemoveplaceId_radio_content_get_cb(void *data, Evas_Object *obj, const char *part)
{
	int index = (int) data;

	if (!strcmp(part, "elm.icon.2")){
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

char* MapsRemovePlace::gl_RemoveplaceId_radio_text_get_cb(void *data, Evas_Object *obj, const char *part)
{
	if (!strcmp(part,"elm.text.main.left")){
		char buf[100];
		int index = (int) data;

		snprintf(buf, sizeof(buf), "place ID %d", premovePlaceId[index]);
		return strdup(buf);
	} else
		return NULL;
}

void MapsRemovePlace::popup2_block_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	if(fencePopup_remove != NULL) {
		evas_object_del(fencePopup_remove);
		fencePopup_remove = NULL;
	}
}

void MapsRemovePlace::getRemovePlaceID(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	int placeId=1;

	GList* pPlaceList = getPlaceList(pGeofenceManager);

	nremovePlaceCnt = 0;

	GList* tmpList = g_list_first(pPlaceList);

	while(tmpList){
		placeId = (int)tmpList->data;
		premovePlaceId[nremovePlaceCnt] = placeId;

		nremovePlaceCnt++;

		tmpList = g_list_next(tmpList);
	}

	if(nremovePlaceCnt == 0){
		WDEBUG("No Place to remove");
		return;
	}

	/* get Place IDs */
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	Evas_Object *popup;
	Evas_Object *genlist;
	Evas_Object *radio;
	Evas_Object *box;
	Evas_Object *win = removePlaceLayout;
	int i;

	popup = elm_popup_add(win);
	ea_object_event_callback_add(popup, EA_CALLBACK_BACK, ea_popup_back_cb, NULL);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_smart_callback_add(popup, "block,clicked", popup2_block_clicked_cb, (void*)win);

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
	itc->func.text_get = gl_RemoveplaceId_radio_text_get_cb;
	itc->func.content_get = gl_RemoveplaceId_radio_content_get_cb;
	itc->func.state_get = NULL;
	itc->func.del = NULL;

	for (i = 0; i < nremovePlaceCnt; i++) {
		elm_genlist_item_append(genlist, itc, (void *) i, NULL, ELM_GENLIST_ITEM_NONE, gl_RemoveplaceId_radio_sel_cb, (void *) i);
	}
	evas_object_show(genlist);

	elm_box_pack_end(box, genlist);
	evas_object_size_hint_min_set(box, -1, (ELM_SCALE_SIZE(288)));
	elm_object_content_set(popup, box);

	evas_object_show(popup);

	fencePopup_remove = popup;
}

Evas_Object* MapsRemovePlace::onCreate( Evas_Object* parent, void* viewParam )
{
	WENTER();
	setTitle("Remove Place");

	__removePlaceNaviObj = this;

	Evas_Object* layout = elm_layout_add(parent);
	elm_layout_file_set(layout, __EDJ, "places_view");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(layout);

	const char *output_style = "DEFAULT='font=Arial:style=Bold font_size=35 color=#000000'";

	//  Remove PlaceID BTN
	Evas_Object* addgetlistBtn = elm_button_add(layout);
	evas_object_size_hint_weight_set(addgetlistBtn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "removeplaceID_button", addgetlistBtn);
	evas_object_smart_callback_add(addgetlistBtn, "clicked", getRemovePlaceID, (void*)NULL);
	elm_object_domain_translatable_text_set(addgetlistBtn, PACKAGE, "Select Place ID");


    Evas_Object* resultEntry_placeId = elm_entry_add(layout);
    ea_entry_selection_back_event_allow_set(resultEntry_placeId, EINA_TRUE);
    elm_entry_editable_set(resultEntry_placeId, EINA_FALSE);
    evas_object_size_hint_weight_set(resultEntry_placeId, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(resultEntry_placeId, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_part_content_set(layout, "geofence_result_placeid", resultEntry_placeId);

    elm_entry_text_style_user_push(resultEntry_placeId, output_style);
	elm_entry_entry_insert(resultEntry_placeId,"Please select place ID to Remove !!");
    evas_object_show(resultEntry_placeId);

	removePlaceLayout = layout;

	return layout;
}
