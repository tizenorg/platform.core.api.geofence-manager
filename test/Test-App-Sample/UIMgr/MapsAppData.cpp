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

#include "MapsAppData.h"
#include <app.h>

MapsAppData* MapsAppData::__instance = NULL;

MapsAppData::MapsAppData()
{
	__needToUpdate = false;
	__isPaused = false;
	__viewType = VIEW_TYPE_MAIN;
}

MapsAppData::~MapsAppData()
{
}

MapsAppData* MapsAppData::getInstance()
{
	if (__instance == NULL)
		__instance = new MapsAppData();
	return __instance;
}


void MapsAppData::destroy()
{
	if( __instance )
	{
		delete __instance;
		__instance = NULL;
	}
}
