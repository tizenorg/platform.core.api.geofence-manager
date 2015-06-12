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

#include "MapsApp.h"

#include <Ecore_X.h> // ui-gadget.h should have included this '#include' statement.
#include "ui-gadget.h"

#include "WWindow.h"
#include "WNaviframe.h"


#include "MapsScenario1View.h"
#include "MapsAppData.h"
#include "MapsDebug.h"
#define CHANGEABLE_COLOR_TABLE_PATH "/usr/apps/com.samsung.testgeo-lite/res/data/color_table.xml"


MapsApp::MapsApp()
{
}

MapsApp::~MapsApp()
{
}

void MapsApp::onPause()
{
	WENTER();
	ad->setIsPaused(true);
}

void MapsApp::onResume()
{
	WENTER();
	ad->setIsPaused(false);
}

void MapsApp::onRegionFormatChanged()
{
	WENTER();
	ad->setNeedToUpdate( true);
}

void MapsApp::onLanguageChanged()
{
	WENTER();
	ad->setNeedToUpdate(true);
}

bool MapsApp::onCreate()
{
	WENTER();
	attachWindow(new WWindow("testgeo-lite", ELM_WIN_BASIC));

	UG_INIT_EFL( getWindow()->getEvasObj(), UG_OPT_INDICATOR_ENABLE );

	getWindow()->attachBaseUiObject(new WNaviframe());

	mColorTable = NULL;
	mColorTable = ea_theme_color_table_new(CHANGEABLE_COLOR_TABLE_PATH);

	if (mColorTable)
		ea_theme_colors_set(mColorTable, EA_THEME_STYLE_LIGHT);
	else
		mColorTable = NULL;

	return true;
}

void MapsApp::onTerminate()
{
	if (mColorTable) {
		 ea_theme_colors_unset(mColorTable, EA_THEME_STYLE_DARK);
		 ea_theme_color_table_free(mColorTable);
	}
}

void MapsApp::onService(app_control_h service)
{
	WENTER();

	WNaviframe* frame = (WNaviframe*)getWindow()->getBaseUiObject();
	frame->push(new MapsScenario1View());
	//
	WApp::onService(service);

	WLEAVE();
}
