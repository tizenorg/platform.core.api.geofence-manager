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

#ifndef _MAPS_SCENARIO1_VIEW_H_
#define _MAPS_SCENARIO1_VIEW_H_

#include "WView.h"

#include "geofence_manager.h"


class MapsScenario1View: public WView
{
public:
	MapsScenario1View();
protected:
    ~MapsScenario1View();

private:	
    virtual Evas_Object* onCreate( Evas_Object* parent, void* viewParam );	

private:
	static void createGeofence(void *data, Evas_Object *obj, void *event_info);
	static void destroyGeofence(void *data, Evas_Object *obj, void *event_info);
	static void addFence(void *data, Evas_Object *obj, void *event_info);	
	static void addPlace(void *data, Evas_Object *obj, void *event_info);
	static void removeFence(void *data, Evas_Object *obj, void *event_info);
	static void getstatusGeofence(void *data, Evas_Object *obj, void *event_info);
	static void removePlace(void *data, Evas_Object *obj, void *event_info);
	static void updatePlace(void *data, Evas_Object *obj, void *event_info);
	static void startGeofence(void *data, Evas_Object *obj, void *event_info);	
	static void stopGeofence(void *data, Evas_Object *obj, void *event_info);	
	static int checkMatchingFenceId(int fenceId);	
	static void popup_block_clicked_cb(void *data, Evas_Object *obj, void *event_info);	
	static void gl_fenceId_radio_sel_cb(void *data, Evas_Object *obj, void *event_info);	
	static Evas_Object* gl_fenceId_radio_content_get_cb(void *data, Evas_Object *obj, const char *part);	
	static char* gl_fenceId_radio_text_get_cb(void *data, Evas_Object *obj, const char *part);	
	static void gl_getstatusfenceId_radio_sel_cb(void *data, Evas_Object *obj, void *event_info);	
	static Evas_Object* gl_getstatusfenceId_radio_content_get_cb(void *data, Evas_Object *obj, const char *part);	
	static char* gl_getstatusfenceId_radio_text_get_cb(void *data, Evas_Object *obj, const char *part);	
	static void gl_startGeofence_radio_sel_cb(void *data, Evas_Object *obj, void *event_info);	
	static Evas_Object* gl_startGeofence_radio_content_get_cb(void *data, Evas_Object *obj, const char *part);	
	static char* gl_startGeofence_radio_text_get_cb(void *data, Evas_Object *obj, const char *part);	
	static void gl_stopGeofence_radio_sel_cb(void *data, Evas_Object *obj, void *event_info);	
	static Evas_Object* gl_stopGeofence_radio_content_get_cb(void *data, Evas_Object *obj, const char *part);	
	static char* gl_stopGeofence_radio_text_get_cb(void *data, Evas_Object *obj, const char *part);	
	static void geofence_state_changed_cb(int geofence_id, geofence_state_e state, void *user_data);
	static void geofence_event_cb(int place_id, int geofence_id, geofence_manager_error_e error, geofence_manage_e request, void *user_data);
	static void getError(Evas_Object *obj, int ret);

private:	
	static geofence_manager_h pGeofenceManager; 
	static int pFenceId[1000];	
	static int nFenceCnt;	
	static bool bIsGeofenceManagerDestroyed;

};

#endif	// _MAPS_SCENARIO1_VIEW_H_
