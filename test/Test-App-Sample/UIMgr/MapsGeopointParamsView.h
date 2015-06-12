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

#ifndef _MAPS_GEOPOINT_PARAMS_VIEW_H_
#define _MAPS_GEOPOINT_PARAMS_VIEW_H_

#include "WView.h"
#include <glib.h>

#include "geofence_manager.h"

class MapsGeopointParamsView: public WView
{
public:
	MapsGeopointParamsView(geofence_manager_h manager, Evas_Object* layout);
protected:
        ~MapsGeopointParamsView();

private:	
        virtual Evas_Object* onCreate( Evas_Object* parent, void* viewParam );

private:
	static void addFence(void *data, Evas_Object *obj, void *event_info);
	static void getPlaceID(void *data, Evas_Object *obj, void *event_info);
	static void popup1_block_clicked_cb(void *data, Evas_Object *obj, void *event_info);	
	
	 static void gl_placeId_radio_sel_cb(void *data, Evas_Object *obj, void *event_info);	
	 static Evas_Object* gl_placeId_radio_content_get_cb(void *data, Evas_Object *obj, const char *part);	
	 static char* gl_placeId_radio_text_get_cb(void *data, Evas_Object *obj, const char *part);	
	
	
	
	static void popScreen(void *data, Evas_Object *obj, void *event_info);

	static void getError(Evas_Object *obj, int ret);

public:
	
	static GList* getFenceList(geofence_manager_h manager);

	static bool geofence_manager_fence_cb(int geofence_id, geofence_h params, int index, int total_fence_count, void *user_data);
	static GList* getTrackingFenceList();

	static void removeFenceId(int fenceId);

	static void addTrackingFenceId(int fenceId);
	static void removeTrackingFenceId(int fenceId);

	static void removeAllFenceList();


private:
	static geofence_manager_h pGeofenceManager;
	static geofence_h pGeofenceParams;
	
    static int pPlaceId[1000];
	static int nPlaceCnt;
	static Evas_Object* pLayout;

	static int* pFenceId;
	static int fenceCount;

	static double dLat;
	static double dLon;
	static int nRadius;
	

	static GList* pFenceList;
	static GList* pTrackingFenceList;

	public:
	static GList* pFenceListAll;
	//static int fcounter; 


};

#endif	// _MAPS_GEOPOINT_PARAMS_VIEW_H_
