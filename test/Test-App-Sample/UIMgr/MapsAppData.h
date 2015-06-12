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

#ifndef _MAPS_APP_DATA_H_
#define _MAPS_APP_DATA_H_

#include "WDefine.h"

#define ad MapsAppData::getInstance()


class MapsAppData
{
public:
	static MapsAppData* getInstance();
	static void destroy();

public:
	typedef enum {
		VIEW_TYPE_UNKNOWN = 0,
		VIEW_TYPE_MAIN,
		VIEW_TYPE_ALARM,
	} ViewType;

	void setNeedToUpdate( bool value ){ __needToUpdate = value; }
	void setIsPaused( bool value ){ __isPaused = value; }

private:
	MapsAppData();
	~MapsAppData();
	WDISABLE_COPY_AND_ASSIGN(MapsAppData);
private:
	static MapsAppData* __instance;
private:
	bool __needToUpdate;
	bool __isPaused;
	ViewType __viewType;
};

#endif // _MAPS_APP_DATA_H_
