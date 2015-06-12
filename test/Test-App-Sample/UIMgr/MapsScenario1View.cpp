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

#include "MapsScenario1View.h"
#include "MapsGeopointParamsView.h"
#include "MapsAddPlace.h"
#include "MapsRemovePlace.h"
#include "MapsUpdatePlace.h"
#include "WPageView.h"
#include "Evas.h"
#include "Ecore_X.h"
#include <efl_assist.h>
#include "WApp.h"
#include "WWindow.h"
#include "WNaviframe.h"
#include "MapsDebug.h"

#define PACKAGE "testgeo-sample"

geofence_manager_h MapsScenario1View::pGeofenceManager;
int MapsScenario1View::pFenceId[];
int MapsScenario1View::nFenceCnt = 0;
bool MapsScenario1View::bIsGeofenceManagerDestroyed = true;
bool bEventcb = false;

static const char* __EDJ = "/usr/apps/com.samsung.testgeo-sample/res/edje/homescreen.edj";

Evas_Object* scenario1Layout = NULL;
Evas_Object* createGeofenceBtn1 = NULL;
Evas_Object* destroyGeofenceBtn1 = NULL;
Evas_Object* createGeofenceParamsBtn1 = NULL;
Evas_Object* destroyGeofenceParamsBtn1 = NULL;
Evas_Object* addFenceBtn1 = NULL;
Evas_Object* addFenceBtn2 = NULL;
Evas_Object* removeFenceBtn1 = NULL;
Evas_Object* getstatusGeofenceBtn1 = NULL;
Evas_Object* removeFenceBtn2 = NULL;
Evas_Object* startGeofenceBtn1 = NULL;
Evas_Object* stopGeofenceBtn1 = NULL;
Evas_Object* fencePopup1 = NULL;
Evas_Object* fenceUpdateBtn1 = NULL;

void* __scenario1NaviframeObj = NULL;
int fenceID;

geofence_manager_error_e server_error;

MapsScenario1View::MapsScenario1View()
{
}

MapsScenario1View::~MapsScenario1View()
{
	WENTER();
	Evas_Object* resultEntry_svr = NULL;
	resultEntry_svr = elm_object_part_content_get(scenario1Layout, "geofence_result_svr");
	elm_object_text_set(resultEntry_svr,"--");
	
	Evas_Object* resultEntry_svr1 = NULL;
	resultEntry_svr1 = elm_object_part_content_get(scenario1Layout, "geofence_result_svr1");
	elm_object_text_set(resultEntry_svr1,"--");
	
	Evas_Object* resultEntry = NULL;
	resultEntry = elm_object_part_content_get(scenario1Layout, "geofence_result");
	elm_object_text_set(resultEntry,"--");
	
    int ret = 0;
	if(bIsGeofenceManagerDestroyed == false) {
		ret = geofence_manager_destroy(pGeofenceManager);

		if(ret == GEOFENCE_MANAGER_ERROR_NONE)
			WDEBUG("Geofence Manager Destroyed");
		else
			WDEBUG("Failed to destroy Geofence Manager");

		bIsGeofenceManagerDestroyed = true;
	}

	elm_exit();
}

void MapsScenario1View::geofence_state_changed_cb(int geofence_id, geofence_state_e state, void *user_data)
{
	WENTER();
	Evas_Object* resultEntry = NULL;
	resultEntry = elm_object_part_content_get(scenario1Layout, "geofence_result");
	elm_object_text_set(resultEntry,"--");

	char tmp[100];
	if(state == GEOFENCE_STATE_IN)
		snprintf(tmp, sizeof(tmp), "Fence Id : %d, (IN)", geofence_id);
	else if(state == GEOFENCE_STATE_OUT)
		snprintf(tmp, sizeof(tmp), "Fence Id : %d, (OUT)", geofence_id);
	else
		strcpy(tmp, "--");

	elm_object_text_set(resultEntry, tmp);
	evas_object_show(resultEntry);
}

void MapsScenario1View::geofence_event_cb(int place_id, int geofence_id, geofence_manager_error_e error, geofence_manage_e request, void *user_data)
{
	WENTER();
	Evas_Object* resultEntry_svr = NULL;
	resultEntry_svr = elm_object_part_content_get(scenario1Layout, "geofence_result_svr");
	elm_object_text_set(resultEntry_svr,"--");

	Evas_Object* resultEntry_svr1 = NULL;
	resultEntry_svr1 = elm_object_part_content_get(scenario1Layout, "geofence_result_svr1");
	elm_object_text_set(resultEntry_svr1,"--");

	if(bEventcb == false){
		Evas_Object* resultEntry = NULL;
		resultEntry = elm_object_part_content_get(scenario1Layout, "geofence_result");
		elm_object_text_set(resultEntry,"--");
	}

	char tmp[100];
	char tmp1[100];
	server_error = error;
	switch(error){
	case GEOFENCE_MANAGER_ERROR_NONE :
		if(request == GEOFENCE_MANAGE_FENCE_STARTED){
			MapsGeopointParamsView::addTrackingFenceId(fenceID);
			snprintf(tmp1, sizeof(tmp1), "Started geofence for fence \"%d\"", geofence_id);
			snprintf(tmp, sizeof(tmp), "Server Status : (Success)");
			fenceID=0;
		} else if(request == GEOFENCE_MANAGE_FENCE_STOPPED) {
			snprintf(tmp1, sizeof(tmp1), "Stopped geofence for fence \"%d\"", geofence_id);
			snprintf(tmp, sizeof(tmp), "Server Status : (Success)");
			fenceID=0;
		} else if(request == GEOFENCE_MANAGE_FENCE_REMOVED) {
			MapsGeopointParamsView::removeFenceId(fenceID);
			snprintf(tmp1, sizeof(tmp1), "Removed fence \"%d\"", geofence_id);
			snprintf(tmp, sizeof(tmp), "Server Status : (Success)");
			fenceID=0;
		} else if(request == GEOFENCE_MANAGE_FENCE_ADDED) {
			snprintf(tmp1, sizeof(tmp1), "Added fence \"%d\"", geofence_id);
			snprintf(tmp, sizeof(tmp), "Server Status : (Success)");
			fenceID=0;
		} else if(request == GEOFENCE_MANAGE_PLACE_ADDED) {
			snprintf(tmp1, sizeof(tmp1), "Added Place \"%d\"", place_id);
			snprintf(tmp, sizeof(tmp), "Server Status : (Success)");
			fenceID=0;
		} else if(request == GEOFENCE_MANAGE_PLACE_REMOVED) {
			snprintf(tmp1, sizeof(tmp1), "Removed Place \"%d\"", place_id);
			snprintf(tmp, sizeof(tmp), "Server Status : (Success)");
			fenceID=0;
		} else if(request == GEOFENCE_MANAGE_PLACE_UPDATED) {
			snprintf(tmp1, sizeof(tmp1), "Updated Place \"%d\"", place_id);
			snprintf(tmp, sizeof(tmp), "Server Status : (Success)");
			fenceID=0;
		} else {
			snprintf(tmp1, sizeof(tmp1), "--");	
			snprintf(tmp, sizeof(tmp), "Server Status : (Success)");
		}
		break;
	case GEOFENCE_MANAGER_ERROR_OUT_OF_MEMORY :
		snprintf(tmp, sizeof(tmp), "Server Status : (GEOFENCE_MANAGER_ERROR_OUT_OF_MEMORY)");
		break;
	case GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER :
		snprintf(tmp, sizeof(tmp), "Server Status : (GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER)");
		break;
	case GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED :
		snprintf(tmp, sizeof(tmp), "Server Status : (GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED)");
		break;
	case GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED :
		snprintf(tmp, sizeof(tmp), "Server Status : (GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED)");
		break;
	case GEOFENCE_MANAGER_ERROR_NOT_INITIALIZED :
		snprintf(tmp, sizeof(tmp), "Server Status : (GEOFENCE_MANAGER_ERROR_NOT_INITIALIZED)");
		break;
	case GEOFENCE_MANAGER_ERROR_INVALID_ID  :
		snprintf(tmp, sizeof(tmp), "Server Status : (GEOFENCE_MANAGER_ERROR_INALID_ID)");
		break;
	case GEOFENCE_MANAGER_ERROR_EXCEPTION  :
		snprintf(tmp, sizeof(tmp), "Server Status : (GEOFENCE_MANAGER_ERROR_EXCEPTION )");
		break;
	case GEOFENCE_MANAGER_ERROR_ALREADY_STARTED  :
		snprintf(tmp, sizeof(tmp), "Server Status : (GEOFENCE_MANAGER_ERROR_ALREADY_STARTED )");
		break;
	case GEOFENCE_MANAGER_ERROR_TOO_MANY_GEOFENCE  :
		snprintf(tmp, sizeof(tmp), "Server Status : (GEOFENCE_MANAGER_ERROR_TOO_MANY_GEOFENCE )");
		break;
	case GEOFENCE_MANAGER_ERROR_IPC  :
		snprintf(tmp, sizeof(tmp), "Server Status : (GEOFENCE_MANAGER_ERROR_IPC )");
		break;
	case GEOFENCE_MANAGER_ERROR_DATABASE  :
		snprintf(tmp, sizeof(tmp), "Server Status : (GEOFENCE_MANAGER_ERROR_DATABASE )");
		break;
	case GEOFENCE_MANAGER_ERROR_PLACE_ACCESS_DENIED  :
		snprintf(tmp, sizeof(tmp), "Server Status : (GEOFENCE_MANAGER_ERROR_PLACE_ACCESS_DENIED )");
		break;
	case GEOFENCE_MANAGER_ERROR_GEOFENCE_ACCESS_DENIED  :
		snprintf(tmp, sizeof(tmp), "Server Status : (GEOFENCE_MANAGER_ERROR_GEOFENCE_ACCESS_DENIED )");
		break;
	default :
		snprintf(tmp, sizeof(tmp), "Server Status %d : (some unknown error)",error );
		break;
	}

	elm_object_text_set(resultEntry_svr1, tmp1);
	evas_object_show(resultEntry_svr1);
	
	elm_object_text_set(resultEntry_svr, tmp);
	evas_object_show(resultEntry_svr);
	
	bEventcb = false;
}

void MapsScenario1View::getError(Evas_Object *obj, int ret)
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
		elm_object_text_set(obj,"geofence_manager_create FAILED with UNKNOWN ERROR");
		break;
	}
}

void MapsScenario1View::createGeofence(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
    Evas_Object* resultEntry_svr = NULL;
	resultEntry_svr = elm_object_part_content_get(scenario1Layout, "geofence_result_svr");
	elm_object_text_set(resultEntry_svr,"--");
	
	Evas_Object* resultEntry_svr1 = NULL;
	resultEntry_svr1 = elm_object_part_content_get(scenario1Layout, "geofence_result_svr1");
	elm_object_text_set(resultEntry_svr1,"--");
	
	Evas_Object* resultEntry = NULL;
	resultEntry = elm_object_part_content_get(scenario1Layout, "geofence_result");
	elm_object_text_set(resultEntry,"--");

	int ret = 0;
	ret = geofence_manager_create(&pGeofenceManager);
	if((ret == GEOFENCE_MANAGER_ERROR_NONE)) {
		bIsGeofenceManagerDestroyed = false;
		elm_object_text_set(resultEntry,"Geofence Manager Created");
		evas_object_show(resultEntry);		
		elm_object_disabled_set(createGeofenceBtn1, EINA_TRUE);
		elm_object_disabled_set(getstatusGeofenceBtn1, EINA_FALSE);
		elm_object_disabled_set(destroyGeofenceBtn1, EINA_FALSE);
		elm_object_disabled_set(addFenceBtn1, EINA_FALSE);
		elm_object_disabled_set(addFenceBtn2, EINA_FALSE);
		elm_object_disabled_set(removeFenceBtn1, EINA_FALSE);
		elm_object_disabled_set(removeFenceBtn2, EINA_FALSE);
		elm_object_disabled_set(startGeofenceBtn1, EINA_FALSE);
		elm_object_disabled_set(stopGeofenceBtn1, EINA_FALSE);
		elm_object_disabled_set(fenceUpdateBtn1, EINA_FALSE);

		ret = geofence_manager_set_geofence_event_cb(pGeofenceManager, geofence_event_cb, (void*)NULL); 
		ret = geofence_manager_set_geofence_state_changed_cb(pGeofenceManager, geofence_state_changed_cb, (void*)NULL);
	} else {
		bIsGeofenceManagerDestroyed = true;
		getError(resultEntry, ret);
		evas_object_show(resultEntry);
	}
}

void MapsScenario1View::destroyGeofence(void *data, Evas_Object *obj, void *event_info)
{
    WENTER();
    Evas_Object* resultEntry_svr = NULL;
	resultEntry_svr = elm_object_part_content_get(scenario1Layout, "geofence_result_svr");
	elm_object_text_set(resultEntry_svr,"--");
	
	Evas_Object* resultEntry_svr1 = NULL;
	resultEntry_svr1 = elm_object_part_content_get(scenario1Layout, "geofence_result_svr1");
	elm_object_text_set(resultEntry_svr1,"--");

	
	Evas_Object* resultEntry = NULL;
	resultEntry = elm_object_part_content_get(scenario1Layout, "geofence_result");
	elm_object_text_set(resultEntry,"--");

	
	int ret;
	MapsGeopointParamsView::removeAllFenceList();
	ret = geofence_manager_destroy(pGeofenceManager);
	
	if((ret == GEOFENCE_MANAGER_ERROR_NONE)) {
		bIsGeofenceManagerDestroyed = true;
		elm_object_text_set(resultEntry,"Geofence Manager Destroyed");
		evas_object_show(resultEntry);		
		elm_object_disabled_set(createGeofenceBtn1, EINA_FALSE);
		elm_object_disabled_set(destroyGeofenceBtn1, EINA_TRUE);
		elm_object_disabled_set(getstatusGeofenceBtn1, EINA_TRUE);
		elm_object_disabled_set(addFenceBtn1, EINA_TRUE);
		elm_object_disabled_set(addFenceBtn2, EINA_TRUE);
		elm_object_disabled_set(removeFenceBtn1, EINA_TRUE);
		elm_object_disabled_set(removeFenceBtn2, EINA_TRUE);
		elm_object_disabled_set(startGeofenceBtn1, EINA_TRUE);
		elm_object_disabled_set(stopGeofenceBtn1, EINA_TRUE);
		elm_object_disabled_set(fenceUpdateBtn1, EINA_TRUE);
	} else {
		bIsGeofenceManagerDestroyed = false;
		getError(resultEntry, ret);
		evas_object_show(resultEntry);
	}
}

int MapsScenario1View::checkMatchingFenceId(int fenceId)
{
	WENTER();
	GList* pTrackingList = MapsGeopointParamsView::getTrackingFenceList();

	int tracking_id;
	int match_id = -1;
	GList* tmpList = g_list_first(pTrackingList);
	while(tmpList){
		tracking_id = (int)tmpList->data;
		tmpList = g_list_next(tmpList);

		if(tracking_id == fenceId) {
			match_id = fenceId;
			break;
		}
	}
	return match_id;
}
void MapsScenario1View::gl_startGeofence_radio_sel_cb(void *data, Evas_Object *obj, void *event_info)
{
    WENTER();
    Elm_Object_Item *it = (Elm_Object_Item*)event_info;
    int index = (int) data;
    Evas_Object *radio;
    elm_genlist_item_selected_set(it, EINA_FALSE);
    radio = elm_object_item_part_content_get(it, "elm.swallow.end");
    elm_radio_value_set(radio, index + 1);
    WDEBUG("Selected Index :: %d", index);
	WDEBUG("Selected Fence Id :: %d", pFenceId[index]);

	fenceID = pFenceId[index];

	evas_object_del(fencePopup1);
    fencePopup1 = NULL;

	Evas_Object* resultEntry = NULL;
	resultEntry = elm_object_part_content_get(scenario1Layout, "geofence_result");
	elm_object_text_set(resultEntry,"--");

	// Start Geofence
	//geofence_manager_error_e error1;
	char tmp[100];
	int ret = 0;
	ret = geofence_manager_start(pGeofenceManager, pFenceId[index]);

	//WDEBUG("[varun] after calling geofence_manager_start ::server_error= %d", server_error);

	if((ret == GEOFENCE_MANAGER_ERROR_NONE )) {
		//MapsGeopointParamsView::addTrackingFenceId(pFenceId[index]);
		snprintf(tmp, sizeof(tmp), "geofence_manager_start success .. ");
		elm_object_text_set(resultEntry, tmp);
		evas_object_show(resultEntry);
	} else {
		snprintf(tmp, sizeof(tmp), "Failed to start geofence for fence \"%d\"", pFenceId[index]);
		elm_object_text_set(resultEntry,tmp);
		evas_object_show(resultEntry);
	}
}

Evas_Object* MapsScenario1View::gl_startGeofence_radio_content_get_cb(void *data, Evas_Object *obj, const char *part)
{
	//WENTER();
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
char* MapsScenario1View::gl_startGeofence_radio_text_get_cb(void *data, Evas_Object *obj, const char *part)
{
	if (!strcmp(part,"elm.text.main.left")){
		char buf[100];
		int index = (int) data;
		snprintf(buf, sizeof(buf), "Start Fence %d", pFenceId[index]);
		return strdup(buf);
	} else
		return NULL;
}

void MapsScenario1View::startGeofence(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	bEventcb = true;
	Evas_Object* resultEntry_svr = NULL;
	resultEntry_svr = elm_object_part_content_get(scenario1Layout, "geofence_result_svr");
	elm_object_text_set(resultEntry_svr,"--");
	
	Evas_Object* resultEntry_svr1 = NULL;
	resultEntry_svr1 = elm_object_part_content_get(scenario1Layout, "geofence_result_svr1");
	elm_object_text_set(resultEntry_svr1,"--");

	Evas_Object* resultEntry = NULL;
	resultEntry = elm_object_part_content_get(scenario1Layout, "geofence_result");
	elm_object_text_set(resultEntry,"--");

	// Get the fence List
	int fence_id;
	//int fenceid =1;
	GList* pFenceList = MapsGeopointParamsView::getFenceList(pGeofenceManager);
	nFenceCnt = 0;
	
	GList* tmpList = g_list_first(pFenceList);

	while(tmpList){
		fence_id = (int)tmpList->data;
        if(checkMatchingFenceId(fence_id) == -1){
			WDEBUG("NO Matching Tracking fence Id \"%d\"found", fence_id);
			pFenceId[nFenceCnt] = fence_id;
			nFenceCnt++;
		}
		tmpList = g_list_next(tmpList);
	}

	if(nFenceCnt == 0){
		WDEBUG("No fences to start");
		elm_object_text_set(resultEntry,"No fence found");
		evas_object_show(resultEntry);
		return;
	}

	// Start the fence popup
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	Evas_Object *popup;
	Evas_Object *genlist;
	Evas_Object *radio;
	Evas_Object *box;
	Evas_Object *win = scenario1Layout;
	int i;

	popup = elm_popup_add(win);
	ea_object_event_callback_add(popup, EA_CALLBACK_BACK, ea_popup_back_cb, NULL);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_smart_callback_add(popup, "block,clicked", popup_block_clicked_cb, (void*)win);

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
	itc->func.text_get = gl_startGeofence_radio_text_get_cb;
	itc->func.content_get = gl_startGeofence_radio_content_get_cb;
	itc->func.state_get = NULL;
	itc->func.del = NULL;

	for (i = 0; i < nFenceCnt; i++) {
		elm_genlist_item_append(genlist, itc, (void *) i, NULL, ELM_GENLIST_ITEM_NONE, gl_startGeofence_radio_sel_cb, (void *) i);
	}
	evas_object_show(genlist);

	elm_box_pack_end(box, genlist);
	evas_object_size_hint_min_set(box, -1, (ELM_SCALE_SIZE(288)));
	elm_object_content_set(popup, box);

	evas_object_show(popup);

	fencePopup1 = popup;
}

void MapsScenario1View::gl_stopGeofence_radio_sel_cb(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *it = (Elm_Object_Item*)event_info;
	int index = (int) data;
	Evas_Object *radio;
	elm_genlist_item_selected_set(it, EINA_FALSE);
	radio = elm_object_item_part_content_get(it, "elm.swallow.end");
	elm_radio_value_set(radio, index + 1);

	WDEBUG("Selected Index :: %d", index);
	WDEBUG("Selected Fence Id :: %d", pFenceId[index]);

	evas_object_del(fencePopup1);
	fencePopup1 = NULL;

	Evas_Object* resultEntry = NULL;
	resultEntry = elm_object_part_content_get(scenario1Layout, "geofence_result");
	elm_object_text_set(resultEntry,"--");

	// Stop Geofence
	char tmp[100];
	int ret = 0;
	ret = geofence_manager_stop(pGeofenceManager, pFenceId[index]);

	if((ret == GEOFENCE_MANAGER_ERROR_NONE)) {
		MapsGeopointParamsView::removeTrackingFenceId(pFenceId[index]);
		snprintf(tmp, sizeof(tmp), "Stopped geofence for fence \"%d\"", pFenceId[index]);
		elm_object_text_set(resultEntry, tmp);
		evas_object_show(resultEntry);
	} else {
		snprintf(tmp, sizeof(tmp), "Failed to stop geofence for fence \"%d\"", pFenceId[index]);
		elm_object_text_set(resultEntry,tmp);
		evas_object_show(resultEntry);
	}
}

Evas_Object* MapsScenario1View::gl_stopGeofence_radio_content_get_cb(void *data, Evas_Object *obj, const char *part)
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

char* MapsScenario1View::gl_stopGeofence_radio_text_get_cb(void *data, Evas_Object *obj, const char *part)
{
	if (!strcmp(part,"elm.text.main.left")){
		char buf[50];
		int index = (int) data;

		snprintf(buf, sizeof(buf), "Stop Fence %d", pFenceId[index]);
		return strdup(buf);
	} else
		return NULL;
}

void MapsScenario1View::stopGeofence(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	bEventcb = true;
	Evas_Object* resultEntry_svr = NULL;
	resultEntry_svr = elm_object_part_content_get(scenario1Layout, "geofence_result_svr");
	elm_object_text_set(resultEntry_svr,"--");
	
	Evas_Object* resultEntry_svr1 = NULL;
	resultEntry_svr1 = elm_object_part_content_get(scenario1Layout, "geofence_result_svr1");
	elm_object_text_set(resultEntry_svr1,"--");

	Evas_Object* resultEntry = NULL;
	resultEntry = elm_object_part_content_get(scenario1Layout, "geofence_result");
	elm_object_text_set(resultEntry,"--");

	// Get the fence List
	int fence_id;
	GList* pTrackingFenceList = MapsGeopointParamsView::getTrackingFenceList();
	nFenceCnt = 0;

	GList* tmpList = g_list_first(pTrackingFenceList);

	while(tmpList)
	{   WDEBUG(" scenario1::stopGeofence()::Inside While Loop");
		fence_id = (int)tmpList->data;
		pFenceId[nFenceCnt] = fence_id;
		nFenceCnt++;
		tmpList = g_list_next(tmpList);
	}
    WDEBUG("Total number of running fences are:: %d", nFenceCnt);
	if(nFenceCnt == 0) {
		WDEBUG("No fences to stop geofence");
		elm_object_text_set(resultEntry,"No fence found");
		evas_object_show(resultEntry);
		return;
	}

	// Stop geofence popup
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();

	if (itc != NULL){
	    Evas_Object *popup;
	    Evas_Object *genlist;
	    Evas_Object *radio;
	    Evas_Object *box;
	    Evas_Object *win = scenario1Layout;
	    int i;

	    popup = elm_popup_add(win);
	    ea_object_event_callback_add(popup, EA_CALLBACK_BACK, ea_popup_back_cb, NULL);
	    evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	    evas_object_smart_callback_add(popup, "block,clicked", popup_block_clicked_cb, (void*)win);

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
	    itc->func.text_get = gl_stopGeofence_radio_text_get_cb;
	    itc->func.content_get = gl_stopGeofence_radio_content_get_cb;
	    itc->func.state_get = NULL;
	    itc->func.del = NULL;

	    for (i = 0; i < nFenceCnt; i++) 
		{
			elm_genlist_item_append(genlist, itc, (void *) i, NULL, ELM_GENLIST_ITEM_NONE, gl_stopGeofence_radio_sel_cb, (void *) i);
	    }
	    evas_object_show(genlist);

	    elm_box_pack_end(box, genlist);
	    evas_object_size_hint_min_set(box, -1, (ELM_SCALE_SIZE(288)));
	    elm_object_content_set(popup, box);

		evas_object_show(popup);

		fencePopup1 = popup;
	}	
}

void MapsScenario1View::addFence(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	bEventcb = true;
	Evas_Object* resultEntry_svr = NULL;
	resultEntry_svr = elm_object_part_content_get(scenario1Layout, "geofence_result_svr");
	elm_object_text_set(resultEntry_svr,"--");
	
	Evas_Object* resultEntry_svr1 = NULL;
	resultEntry_svr1 = elm_object_part_content_get(scenario1Layout, "geofence_result_svr1");
	elm_object_text_set(resultEntry_svr1,"--");

	Evas_Object* resultEntry = NULL;
	resultEntry = elm_object_part_content_get(scenario1Layout, "geofence_result");
	elm_object_text_set(resultEntry,"--");

	MapsScenario1View* self = (MapsScenario1View*)__scenario1NaviframeObj;

	if (self->getNaviframe() != NULL)
		self->getNaviframe()->push(new MapsGeopointParamsView(pGeofenceManager, scenario1Layout));
}

void MapsScenario1View::addPlace(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	bEventcb = true;

	Evas_Object* resultEntry_svr = NULL;
	resultEntry_svr = elm_object_part_content_get(scenario1Layout, "geofence_result_svr");
	elm_object_text_set(resultEntry_svr,"--");
	
	Evas_Object* resultEntry_svr1 = NULL;
	resultEntry_svr1 = elm_object_part_content_get(scenario1Layout, "geofence_result_svr1");
	elm_object_text_set(resultEntry_svr1,"--");

	Evas_Object* resultEntry = NULL;
	resultEntry = elm_object_part_content_get(scenario1Layout, "geofence_result");
	elm_object_text_set(resultEntry,"--");

	MapsScenario1View* self = (MapsScenario1View*)__scenario1NaviframeObj;

	if (self->getNaviframe() != NULL)
		self->getNaviframe()->push(new MapsAddPlace(pGeofenceManager, scenario1Layout));
}


void MapsScenario1View::updatePlace(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	bEventcb = true;
	Evas_Object* resultEntry_svr = NULL;
	resultEntry_svr = elm_object_part_content_get(scenario1Layout, "geofence_result_svr");
	elm_object_text_set(resultEntry_svr,"--");

	Evas_Object* resultEntry_svr1 = NULL;
	resultEntry_svr1 = elm_object_part_content_get(scenario1Layout, "geofence_result_svr1");
	elm_object_text_set(resultEntry_svr1,"--");

	Evas_Object* resultEntry = NULL;
	resultEntry = elm_object_part_content_get(scenario1Layout, "geofence_result");
	elm_object_text_set(resultEntry,"--");

	MapsScenario1View* self = (MapsScenario1View*)__scenario1NaviframeObj;

	if (self->getNaviframe() != NULL)
		self->getNaviframe()->push(new MapsUpdatePlace(pGeofenceManager, scenario1Layout));
}

void MapsScenario1View::gl_fenceId_radio_sel_cb(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	Elm_Object_Item *it = (Elm_Object_Item*)event_info;
	int index = (int) data;
	Evas_Object *radio;
	elm_genlist_item_selected_set(it, EINA_FALSE);
	radio = elm_object_item_part_content_get(it, "elm.swallow.end");
	elm_radio_value_set(radio, index + 1);
	WDEBUG("Selected Index :: %d", index);
	WDEBUG("Selected Fence Id :: %d", pFenceId[index]);

	fenceID = pFenceId[index];

	evas_object_del(fencePopup1);
	fencePopup1 = NULL;

	int ret = 0;
	
	MapsGeopointParamsView::removeTrackingFenceId(pFenceId[index]);

	ret = geofence_manager_remove_fence(pGeofenceManager, pFenceId[index]);
	WDEBUG("[varun] after calling geofence_manager_remove_fence ::server_error= %d", server_error);

	Evas_Object* resultEntry = NULL;
	resultEntry = elm_object_part_content_get(scenario1Layout, "geofence_result");
	elm_object_text_set(resultEntry,"--");

	char tmp[50];
	if(ret == GEOFENCE_MANAGER_ERROR_NONE)
		snprintf(tmp, sizeof(tmp), "geofence_manager_remove_fence Success!!");
	else
		snprintf(tmp, sizeof(tmp), "Not Authorized to Remove Fence \"%d\"", pFenceId[index]);

	elm_object_text_set(resultEntry, tmp);
	evas_object_show(resultEntry);
 }

Evas_Object* MapsScenario1View::gl_fenceId_radio_content_get_cb(void *data, Evas_Object *obj, const char *part)
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

char* MapsScenario1View::gl_fenceId_radio_text_get_cb(void *data, Evas_Object *obj, const char *part)
{
	if (!strcmp(part,"elm.text.main.left")){
		char buf[100];
		int index = (int) data;

		snprintf(buf, sizeof(buf), "Remove Fence %d", pFenceId[index]);
		return strdup(buf);
	}
	return NULL;
}

void MapsScenario1View::popup_block_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	if(fencePopup1 != NULL) {
		evas_object_del(fencePopup1);
		fencePopup1 = NULL;
	}
}

void MapsScenario1View::removePlace(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	bEventcb = true;
	Evas_Object* resultEntry_svr = NULL;
	resultEntry_svr = elm_object_part_content_get(scenario1Layout, "geofence_result_svr");
	elm_object_text_set(resultEntry_svr,"--");
	
	Evas_Object* resultEntry_svr1 = NULL;
	resultEntry_svr1 = elm_object_part_content_get(scenario1Layout, "geofence_result_svr1");
	elm_object_text_set(resultEntry_svr1,"--");

	Evas_Object* resultEntry = NULL;
	resultEntry = elm_object_part_content_get(scenario1Layout, "geofence_result");
	elm_object_text_set(resultEntry,"--");

	MapsScenario1View* self = (MapsScenario1View*)__scenario1NaviframeObj;
	if (self->getNaviframe() != NULL)
		self->getNaviframe()->push(new MapsRemovePlace(pGeofenceManager, scenario1Layout));
}

void MapsScenario1View::removeFence(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	bEventcb = true;
	Evas_Object* resultEntry = NULL;
	resultEntry = elm_object_part_content_get(scenario1Layout, "geofence_result");
	elm_object_text_set(resultEntry,"--");
	
	Evas_Object* resultEntry_svr = NULL;
	resultEntry_svr = elm_object_part_content_get(scenario1Layout, "geofence_result_svr");
	elm_object_text_set(resultEntry_svr,"--");
	
	Evas_Object* resultEntry_svr1 = NULL;
	resultEntry_svr1 = elm_object_part_content_get(scenario1Layout, "geofence_result_svr1");
	elm_object_text_set(resultEntry_svr1,"--");

	// Get the fence List
	int fence_id;
	//int fenceid=1;
	GList* pFenceList = MapsGeopointParamsView::getFenceList(pGeofenceManager);
	nFenceCnt = 0;
	GList* tmpList = g_list_first(pFenceList);

	while(tmpList){
		fence_id = (int)tmpList->data;
		pFenceId[nFenceCnt] = fence_id;

		nFenceCnt++;

		tmpList = g_list_next(tmpList);
	}

	if(nFenceCnt == 0) {
		WDEBUG("No fences to remove");
		elm_object_text_set(resultEntry,"No fence found to remove");
		evas_object_show(resultEntry);
		return;
	}

	// Remove the fence
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	Evas_Object *popup;
	Evas_Object *genlist;
	Evas_Object *radio;
	Evas_Object *box;
	Evas_Object *win = scenario1Layout;
	int i;

	popup = elm_popup_add(win);
	ea_object_event_callback_add(popup, EA_CALLBACK_BACK, ea_popup_back_cb, NULL);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_smart_callback_add(popup, "block,clicked", popup_block_clicked_cb, (void*)win);

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
	itc->func.text_get = gl_fenceId_radio_text_get_cb;
	itc->func.content_get = gl_fenceId_radio_content_get_cb;
	itc->func.state_get = NULL;
	itc->func.del = NULL;

	for (i = 0; i < nFenceCnt; i++) {
		elm_genlist_item_append(genlist, itc, (void *) i, NULL, ELM_GENLIST_ITEM_NONE, gl_fenceId_radio_sel_cb, (void *) i);
	}
	evas_object_show(genlist);

	elm_box_pack_end(box, genlist);
	evas_object_size_hint_min_set(box, -1, (ELM_SCALE_SIZE(288)));
	elm_object_content_set(popup, box);

	evas_object_show(popup);

	fencePopup1 = popup;
}

void MapsScenario1View::gl_getstatusfenceId_radio_sel_cb(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	Elm_Object_Item *it = (Elm_Object_Item*)event_info;
	int index = (int) data;
	Evas_Object *radio;
	elm_genlist_item_selected_set(it, EINA_FALSE);
	radio = elm_object_item_part_content_get(it, "elm.swallow.end");
	elm_radio_value_set(radio, index + 1);

	WDEBUG("Selected Index :: %d", index);
	WDEBUG("Selected Fence Id :: %d", pFenceId[index]);

	evas_object_del(fencePopup1);
	fencePopup1 = NULL;

	Evas_Object* resultEntry = NULL;
	resultEntry = elm_object_part_content_get(scenario1Layout, "geofence_result");
	elm_object_text_set(resultEntry,"--");

	Evas_Object* resultEntry_svr = NULL;	
	resultEntry_svr = elm_object_part_content_get(scenario1Layout, "geofence_result_svr");	
	elm_object_text_set(resultEntry_svr,"--");

	geofence_status_h pStatus;
	geofence_state_e pState;
    int seconds;
	char tmp1[100];	
	char tmp2[100];	
	
	int ret = geofence_status_create(pFenceId[index], &pStatus);

	if(ret == GEOFENCE_MANAGER_ERROR_NONE)
		WDEBUG("geofence_status_create Success!!");
    else
		WDEBUG("geofence_status_create Fail!!");

	ret = geofence_status_get_state(pStatus, &pState);
     
	if(ret == GEOFENCE_MANAGER_ERROR_NONE){
		WDEBUG("geofence_status_get_state Success!!");

		if(pState == GEOFENCE_STATE_IN)		
			snprintf(tmp1, sizeof(tmp1), "Fence Id : %d, State : (IN)", pFenceId[index]);
		else if(pState == GEOFENCE_STATE_OUT)		
			snprintf(tmp1, sizeof(tmp1), "Fence Id : %d, State : (OUT)", pFenceId[index]);	
		else
			snprintf(tmp1, sizeof(tmp1), "Fence Id : %d, State : UNCERTAIN", pFenceId[index]);
	} else
		WDEBUG("geofence_status_get_state Fail!!");

	ret = geofence_status_get_duration(pStatus, &seconds);
	if(ret == GEOFENCE_MANAGER_ERROR_NONE){
		WDEBUG("geofence_status_get_duration Success!!");
		snprintf(tmp2, sizeof(tmp2), "Fence Id : %d, Duration : %d", pFenceId[index],seconds);
	} else {
		WDEBUG("geofence_status_get_duration Fail!!");
		strcpy(tmp2, "--");
	}

	ret = geofence_status_destroy(pStatus);
	if(ret == GEOFENCE_MANAGER_ERROR_NONE)
		WDEBUG("geofence_status_destroy Success!!");
    else
		WDEBUG("geofence_status_destroy Fail!!");

	elm_object_text_set(resultEntry, tmp1);
	evas_object_show(resultEntry);

	elm_object_text_set(resultEntry_svr, tmp2);
	evas_object_show(resultEntry_svr);
}

Evas_Object* MapsScenario1View::gl_getstatusfenceId_radio_content_get_cb(void *data, Evas_Object *obj, const char *part)
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

char* MapsScenario1View::gl_getstatusfenceId_radio_text_get_cb(void *data, Evas_Object *obj, const char *part)
{
	if (!strcmp(part,"elm.text.main.left")){
		char buf[100];
		int index = (int) data;
		snprintf(buf, sizeof(buf), "Get Status Fence %d", pFenceId[index]);
		return strdup(buf);
	} else
		return NULL;
}

void MapsScenario1View::getstatusGeofence(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	bEventcb = true;
	Evas_Object* resultEntry = NULL;
	resultEntry = elm_object_part_content_get(scenario1Layout, "geofence_result");
	elm_object_text_set(resultEntry,"--");

	Evas_Object* resultEntry_svr = NULL;
	resultEntry_svr = elm_object_part_content_get(scenario1Layout, "geofence_result_svr");
	elm_object_text_set(resultEntry_svr,"--");

	Evas_Object* resultEntry_svr1 = NULL;
	resultEntry_svr1 = elm_object_part_content_get(scenario1Layout, "geofence_result_svr1");
	elm_object_text_set(resultEntry_svr1,"--");

	int fence_id;
	//int fenceid=1;
	GList* pFenceList = MapsGeopointParamsView::getFenceList(pGeofenceManager);
	nFenceCnt = 0;
	GList* tmpList = g_list_first(pFenceList);

	while(tmpList){
		fence_id = (int)tmpList->data;
		pFenceId[nFenceCnt] = fence_id;

		nFenceCnt++;

		tmpList = g_list_next(tmpList);
	}

	if(nFenceCnt == 0) {
		WDEBUG("No fences to get status");
		elm_object_text_set(resultEntry,"No fences to get status");
		evas_object_show(resultEntry);
		return;
	}

	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	Evas_Object *popup;
	Evas_Object *genlist;
	Evas_Object *radio;
	Evas_Object *box;
	Evas_Object *win = scenario1Layout;
	int i;

	popup = elm_popup_add(win);
	ea_object_event_callback_add(popup, EA_CALLBACK_BACK, ea_popup_back_cb, NULL);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_smart_callback_add(popup, "block,clicked", popup_block_clicked_cb, (void*)win);

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
	itc->func.text_get = gl_getstatusfenceId_radio_text_get_cb;
	itc->func.content_get = gl_getstatusfenceId_radio_content_get_cb;
	itc->func.state_get = NULL;
	itc->func.del = NULL;

	for (i = 0; i < nFenceCnt; i++) {
		elm_genlist_item_append(genlist, itc, (void *) i, NULL, ELM_GENLIST_ITEM_NONE, gl_getstatusfenceId_radio_sel_cb, (void *) i);
	}
	evas_object_show(genlist);

	elm_box_pack_end(box, genlist);
	evas_object_size_hint_min_set(box, -1, (ELM_SCALE_SIZE(288)));
	elm_object_content_set(popup, box);

	evas_object_show(popup);

	fencePopup1 = popup;
}

Evas_Object* MapsScenario1View::onCreate( Evas_Object* parent, void* viewParam )
{
	WENTER();
	setTitle("Test App Sample");

	fenceID =0;
	__scenario1NaviframeObj = this;
	MapsGeopointParamsView::removeAllFenceList();
	bIsGeofenceManagerDestroyed = true;
	Evas_Object* layout = elm_layout_add(parent);
	elm_layout_file_set(layout, __EDJ, "scenario_a_view");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(layout);

	const char *output_style = "DEFAULT='font=Arial:style=Bold font_size=35 color=#000000'";

	createGeofenceBtn1 = elm_button_add(layout);
	evas_object_size_hint_weight_set(createGeofenceBtn1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "create_geofence", createGeofenceBtn1);
	evas_object_smart_callback_add(createGeofenceBtn1, "clicked", createGeofence, (void*)NULL);
	evas_object_name_set(createGeofenceBtn1, "Create Geofence");
	elm_object_domain_translatable_text_set(createGeofenceBtn1, PACKAGE, "Create");

	getstatusGeofenceBtn1 = elm_button_add(layout);
	evas_object_size_hint_weight_set(getstatusGeofenceBtn1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "getstatus_geofence", getstatusGeofenceBtn1);
	evas_object_smart_callback_add(getstatusGeofenceBtn1, "clicked", getstatusGeofence, (void*)NULL);
	evas_object_name_set(getstatusGeofenceBtn1, "Fence Status");
	elm_object_domain_translatable_text_set(getstatusGeofenceBtn1, PACKAGE, "Fence Status");

	destroyGeofenceBtn1 = elm_button_add(layout);
	evas_object_size_hint_weight_set(destroyGeofenceBtn1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "destroy_geofence", destroyGeofenceBtn1);
	evas_object_smart_callback_add(destroyGeofenceBtn1, "clicked", destroyGeofence, (void*)NULL);
	evas_object_name_set(destroyGeofenceBtn1, "Destroy Geofence");
	elm_object_domain_translatable_text_set(destroyGeofenceBtn1, PACKAGE, "Destroy");
	startGeofenceBtn1 = elm_button_add(layout);
	evas_object_size_hint_weight_set(startGeofenceBtn1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "start_geofence", startGeofenceBtn1);
	evas_object_smart_callback_add(startGeofenceBtn1, "clicked", startGeofence, (void*)NULL);
	evas_object_name_set(startGeofenceBtn1, "Start Geofence");
	elm_object_domain_translatable_text_set(startGeofenceBtn1, PACKAGE, "Start");
	stopGeofenceBtn1 = elm_button_add(layout);
	evas_object_size_hint_weight_set(stopGeofenceBtn1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "stop_geofence", stopGeofenceBtn1);
	evas_object_smart_callback_add(stopGeofenceBtn1, "clicked", stopGeofence, (void*)NULL);
	evas_object_name_set(stopGeofenceBtn1, "Stop Geofence");
	elm_object_domain_translatable_text_set(stopGeofenceBtn1, PACKAGE, "Stop");

	addFenceBtn1 = elm_button_add(layout);
	evas_object_size_hint_weight_set(addFenceBtn1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "add_fence", addFenceBtn1);
	evas_object_smart_callback_add(addFenceBtn1, "clicked", addFence, (void*)NULL);
	evas_object_name_set(addFenceBtn1, "Add fence");
	elm_object_domain_translatable_text_set(addFenceBtn1, PACKAGE, "Add Fence");

	addFenceBtn2 = elm_button_add(layout);
	evas_object_size_hint_weight_set(addFenceBtn2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "add_place", addFenceBtn2);
	evas_object_smart_callback_add(addFenceBtn2, "clicked", addPlace, (void*)NULL);
	evas_object_name_set(addFenceBtn2, "Add Place");
	elm_object_domain_translatable_text_set(addFenceBtn2, PACKAGE, "Add place");
	removeFenceBtn1 = elm_button_add(layout);
	evas_object_size_hint_weight_set(removeFenceBtn1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "remove_fence", removeFenceBtn1);
	evas_object_smart_callback_add(removeFenceBtn1, "clicked", removeFence, (void*)NULL);
	evas_object_name_set(removeFenceBtn1, "Remove fence");
	elm_object_domain_translatable_text_set(removeFenceBtn1, PACKAGE, "Remove Fence");

	removeFenceBtn2 = elm_button_add(layout);
	evas_object_size_hint_weight_set(removeFenceBtn2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "remove_place", removeFenceBtn2);
	evas_object_smart_callback_add(removeFenceBtn2, "clicked", removePlace, (void*)NULL);
	evas_object_name_set(removeFenceBtn1, "Remove Place");
	elm_object_domain_translatable_text_set(removeFenceBtn2, PACKAGE, "Remove place");

	fenceUpdateBtn1 = elm_button_add(layout);
	evas_object_size_hint_weight_set(fenceUpdateBtn1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "update_place", fenceUpdateBtn1);
	evas_object_smart_callback_add(fenceUpdateBtn1, "clicked", updatePlace, (void*)NULL);
	evas_object_name_set(fenceUpdateBtn1, "Update Fence");
	elm_object_domain_translatable_text_set(fenceUpdateBtn1, PACKAGE, "Update place");

	Evas_Object* resultEntry = elm_entry_add(layout);
	ea_entry_selection_back_event_allow_set(resultEntry, EINA_TRUE);
	elm_entry_editable_set(resultEntry, EINA_FALSE);
	evas_object_size_hint_weight_set(resultEntry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(resultEntry, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_part_content_set(layout, "geofence_result", resultEntry);

	elm_entry_text_style_user_push(resultEntry, output_style);
	elm_entry_entry_insert(resultEntry,"--");
	evas_object_show(resultEntry);
   
	Evas_Object* resultEntry_svr = elm_entry_add(layout);
	ea_entry_selection_back_event_allow_set(resultEntry_svr, EINA_TRUE);
	elm_entry_editable_set(resultEntry_svr, EINA_FALSE);
	evas_object_size_hint_weight_set(resultEntry_svr, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(resultEntry_svr, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_part_content_set(layout, "geofence_result_svr", resultEntry_svr);

	elm_entry_text_style_user_push(resultEntry_svr, output_style);
	elm_entry_entry_insert(resultEntry_svr,"");
	evas_object_show(resultEntry_svr);

	Evas_Object* resultEntry_svr1 = elm_entry_add(layout);
	ea_entry_selection_back_event_allow_set(resultEntry_svr1, EINA_TRUE);
	elm_entry_editable_set(resultEntry_svr1, EINA_FALSE);
	evas_object_size_hint_weight_set(resultEntry_svr1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(resultEntry_svr1, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_part_content_set(layout, "geofence_result_svr1", resultEntry_svr1);
	elm_entry_text_style_user_push(resultEntry_svr1, output_style);
	elm_entry_entry_insert(resultEntry_svr1,"--");
	evas_object_show(resultEntry_svr1);
	
	elm_object_disabled_set(createGeofenceBtn1, EINA_FALSE);
	elm_object_disabled_set(getstatusGeofenceBtn1, EINA_TRUE);
	elm_object_disabled_set(destroyGeofenceBtn1, EINA_TRUE);
	elm_object_disabled_set(addFenceBtn1, EINA_TRUE);
	elm_object_disabled_set(addFenceBtn2, EINA_TRUE);
	elm_object_disabled_set(removeFenceBtn1, EINA_TRUE);
	elm_object_disabled_set(removeFenceBtn2, EINA_TRUE);
	elm_object_disabled_set(startGeofenceBtn1, EINA_TRUE);
	elm_object_disabled_set(stopGeofenceBtn1, EINA_TRUE);
	elm_object_disabled_set(fenceUpdateBtn1, EINA_TRUE);

	scenario1Layout = layout;

	return layout;
}

