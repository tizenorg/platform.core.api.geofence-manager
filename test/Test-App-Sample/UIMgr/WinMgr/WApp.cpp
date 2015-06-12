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

#include "WApp.h"
#include "WDebugInternal.h"

#include <memory.h>

#include "WWindow.h"

char* g_edjPath = NULL;

class __WAppImpl
{
public:
	__WAppImpl():__window(NULL) {}
	~__WAppImpl();
public:
	WWindow* __window;
	//
	static bool __onCreate( void* data);
	static void __onTerminate( void* data);
	static void __onPause( void* data);
	static void __onResume( void* data);
	static void __onService( app_control_h service, void* data);
	static void __onLowMemory(void* data);
	static void __onLowBattery(void* data);
	static void __onDeviceOrientation( app_device_orientation_e orientation, void* data);
	static void __onRegionFormatChanged( void* data);
	static void __onLanguageChanged( void* data);

};

__WAppImpl::~__WAppImpl()
{
	if( g_edjPath)
		free(g_edjPath);
	g_edjPath = NULL;
}

bool __WAppImpl::__onCreate( void* data)
{
	WHIT();
	bool ret = false;

	WApp* pApp = (WApp*)data;
	if (pApp != NULL)
		ret = pApp->onCreate();

	return ret;
}

void __WAppImpl::__onTerminate( void* data)
{
	WHIT();

	WApp* pApp = (WApp*)data;
	if (pApp != NULL) {
		pApp->onTerminate();

		if( pApp->__pv->__window )
			pApp->__pv->__window->destroy();

		pApp->__pv->__window = NULL;
	}
}

void __WAppImpl::__onPause( void* data)
{
	WHIT();

	WApp* pApp = (WApp*)data;
	if (pApp != NULL)
		pApp->onPause();
}

void __WAppImpl::__onResume( void* data)
{
	WHIT();

	WApp* pApp = (WApp*)data;
	if (pApp != NULL)
		pApp->onResume();
}

void __WAppImpl::__onService( app_control_h service, void* data)
{
	WHIT();

	WApp* pApp = (WApp*)data;
	if (pApp != NULL)
		pApp->onService(service);
}

void __WAppImpl::__onLowMemory(void* data)
{
	WHIT();

	WApp* pApp = (WApp*)data;
	if (pApp != NULL)
		pApp->onLowMemory();
}
void __WAppImpl::__onLowBattery(void* data)
{
	WHIT();

	WApp* pApp = (WApp*)data;
	if (pApp != NULL)
		pApp->onLowBattery();
}
void __WAppImpl::__onDeviceOrientation(app_device_orientation_e orientation, void* data)
{
	WHIT();

	WApp* pApp = (WApp*)data;
	if (pApp != NULL)
		pApp->onDeviceOrientation(orientation);
}


void __WAppImpl::__onRegionFormatChanged( void* data)
{
	WHIT();

	WApp* pApp = (WApp*)data;
	if (pApp != NULL)
		pApp->onRegionFormatChanged();
}

void __WAppImpl::__onLanguageChanged( void* data)
{
	WHIT();

	WApp* pApp = (WApp*)data;
	if (pApp != NULL)
		pApp->onLanguageChanged();
}


WApp::WApp()
{
	WHIT();
	__pv = new __WAppImpl();
}

WApp::~WApp()
{
	WHIT();
	delete __pv;
}

int WApp::start( int argc, char* argv[], const char* edjPath )
{
	return onStart( argc, argv, edjPath);
}

WWindow* WApp::getWindow()
{
	return __pv->__window;
}

bool WApp::attachWindow(WWindow* win)
{
	WASSERT( __pv->__window == NULL);
	__pv->__window = win;
	return win->create();
}

void WApp::onService(app_control_h service)
{
	Evas_Object* win = getWindow()->getEvasObj();
	if( win )
	{
		elm_win_activate( win);
		evas_object_show( win);
	}
}

int WApp::onStart( int argc, char* argv[], const char* edjPath)
{
    ui_app_lifecycle_callback_s cb = {0, };

    cb.create = __WAppImpl::__onCreate;
    cb.terminate = __WAppImpl::__onTerminate;
    cb.pause = __WAppImpl::__onPause;
    cb.resume = __WAppImpl::__onResume;
    cb.app_control = __WAppImpl::__onService;

	if(edjPath) {
		const char* edjName = "/testgeo-sample-efl.edj";
		g_edjPath = (char*)malloc( strlen(edjPath) + strlen(edjName)+ 1);
		sprintf( g_edjPath, "%s%s", edjPath, edjName);
	} else
		g_edjPath = strdup( "/usr/lib/testgeo-sample-efl/testgeo-sample-efl.edj");

	return ui_app_main( argc, argv, &cb, this);
}

