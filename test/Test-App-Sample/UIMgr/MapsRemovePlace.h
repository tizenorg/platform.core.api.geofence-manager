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

#ifndef _MAPS_REMOVE_PLACE_H_
#define _MAPS_REMOVE_PLACE_H_

#include "WView.h"
#include <glib.h>

#include "geofence_manager.h"

class MapsRemovePlace: public WView
{
public:
	MapsRemovePlace(geofence_manager_h manager, Evas_Object* layout);
protected:
	~MapsRemovePlace();

private:	
	virtual Evas_Object* onCreate( Evas_Object* parent, void* viewParam );

private:
	static void getRemovePlaceID(void *data, Evas_Object *obj, void *event_info);
	static void popup2_block_clicked_cb(void *data, Evas_Object *obj, void *event_info);	
	static void gl_RemoveplaceId_radio_sel_cb(void *data, Evas_Object *obj, void *event_info);	
	static Evas_Object* gl_RemoveplaceId_radio_content_get_cb(void *data, Evas_Object *obj, const char *part);	
	static char* gl_RemoveplaceId_radio_text_get_cb(void *data, Evas_Object *obj, const char *part);	

public:
	static GList* getPlaceList(geofence_manager_h manager);
	static bool geofence_manager_place_cb(int place_id, const char *place_name, int place_index, int place_cnt, void *user_data);

private:
	static geofence_manager_h pGeofenceManager;
	static geofence_h pGeofenceParams;
	static int premovePlaceId[1000];
	static int nremovePlaceCnt;
	static Evas_Object* pLayout;

	static GList* pFenceList;

	public:
	static GList* pPlaceListAll;
	//static int pcounter; 	
};

#endif	// _MAPS_REMOVE_PLACE_H_
