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

#include <Ecore.h>
#include <efl_assist.h>

#include "WWindow.h"
#include "WUiObject.h"
#include "WDebugInternal.h"
#include "WDefineInternal.h"


#define __CLASS_NAME	"WWindow"

class __WWindowImpl
{
public:
	__WWindowImpl(const char* inName, Elm_Win_Type inWinType);
	~__WWindowImpl();
public:
	//
	Evas_Object* __win;
	Evas_Object* __baseLayout;
	WUiObject* __baseObj;
	char* __name;
	Elm_Win_Type __winType;
	//
	std::function<Evas_Object* (void)> __createWinHandler;
	std::function<Evas_Object* (Evas_Object*)> __createBaseLayoutHandler;
	std::function<void (void)> __destroyHandler;


	static void __delReqCb( void* data, Evas_Object* obj, void* event_info);
};

__WWindowImpl::__WWindowImpl(const char* name, Elm_Win_Type winType):
	__win(NULL),
	__baseLayout(NULL),
	__baseObj(NULL),
	__name(NULL),
	__winType(winType),
	__createWinHandler(NULL),
	__createBaseLayoutHandler(NULL),
	__destroyHandler(NULL)
{
	WASSERT( name );
	__name = (char*)malloc( strlen(name)+1);

	if (__name != NULL)
		strcpy( __name, name);
}

__WWindowImpl::~__WWindowImpl()
{
	free(__name);
}


void __WWindowImpl::__delReqCb( void* data, Evas_Object* obj, void* event_info)
{
	WHIT();

	WWindow* window = (WWindow*)data;

	if( window->__pv->__destroyHandler )
		window->__pv->__destroyHandler();
	else
		window->onDestroy();

	delete window;

	elm_exit();


}


WWindow::WWindow(const char* name, Elm_Win_Type winType)
{
	__pv = new __WWindowImpl(name, winType);
}

WWindow::~WWindow()
{
	WHIT();
	delete __pv;
}

const char* WWindow::getClassName()
{
	return __CLASS_NAME;
}

bool WWindow::create()
{
	if(__pv->__win)
	{
		WDEBUG("Already created!");
		return true;
	}

	Evas_Object* win = NULL;
	if( __pv->__createWinHandler )
		win = __pv->__createWinHandler();
	else
		win = onCreateWin();
	__pv->__win = win;
	if( win )
	{
		if(__pv->__createBaseLayoutHandler)
			__pv->__baseLayout = __pv->__createBaseLayoutHandler(win);
		else
			__pv->__baseLayout = onCreateBaseLayout(win);
		evas_object_data_set( win, WKEY_WINDOW_INSTANCE, this );
		// When this callback function is called???????
		evas_object_smart_callback_add( win, "delete,request", __WWindowImpl::__delReqCb, this);

		return true;
	}
	else
	{
		return false;
	}
}

void WWindow::destroy()
{
	if( __pv->__win )
	{
		evas_object_del(__pv->__win);
		// Do not leave any code here.
		// After executing upper statement "evas_object_del", this object will be deleted at evas object deletion callback!

	}
	else
	{
		if( __pv->__destroyHandler)
			__pv->__destroyHandler();
		else
			onDestroy();
		delete this;
	}
}

Evas_Object* WWindow::getEvasObj()
{
	return __pv->__win;
}
Evas_Object* WWindow::getBaseLayout()
{
	return __pv->__baseLayout;
}

bool WWindow::attachBaseUiObject( WUiObject* baseObj, void* creationParam )
{

	WASSERT( __pv->__baseObj == NULL);

	if( baseObj->create( __pv->__baseLayout, creationParam) == false )
	{
		return false;
	}

	__pv->__baseObj = baseObj;

	return true;
}

WUiObject* WWindow::getBaseUiObject()
{
	return __pv->__baseObj;
}

WUiObject* WWindow::detachBaseUiObject()
{
	WUiObject* obj = __pv->__baseObj;
	__pv->__baseObj = NULL;
	return obj;
}

void WWindow::setOnCreateWin( std::function<Evas_Object* (void)> handlerFunc )
{
	__pv->__createWinHandler = handlerFunc;
}
void WWindow::setOnCreateBaseLayout( std::function<Evas_Object* (Evas_Object*)> handlerFunc )
{
	__pv->__createBaseLayoutHandler = handlerFunc;

}
void WWindow::setOnDestroy( std::function<void (void)> handlerFunc )
{
	__pv->__destroyHandler = handlerFunc;
}

const char* WWindow::getName()
{
	return __pv->__name;
}
Elm_Win_Type WWindow::getWinType()
{
	return __pv->__winType;
}



Evas_Object* WWindow::onCreateWin()
{

	Evas_Object* win = NULL;
//	win = (Evas_Object*)app_get_preinitialized_window(__pv->__name);
//	if( win == NULL)
		win = elm_win_add( NULL, __pv->__name, __pv->__winType);

	if( win == NULL)
	{
		WERROR("Fail to create win!");
		return NULL;
	}

	elm_win_title_set( win, __pv->__name);
	elm_win_autodel_set( win, EINA_TRUE);
	if( elm_win_wm_rotation_supported_get(win))
	{
		int rotation[4] = {0,90,180,270};
		elm_win_wm_rotation_available_rotations_set( win, (const int*)(&rotation), 4);

	}
	elm_win_indicator_mode_set( win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(win, ELM_WIN_INDICATOR_OPAQUE);


	return win;
}

Evas_Object* WWindow::onCreateBaseLayout( Evas_Object* win)
{
	Evas_Object* bg = elm_bg_add( win );
	evas_object_size_hint_weight_set( bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add( win, bg);
	evas_object_show(bg);

	Evas_Object* cf = elm_conformant_add( win); // If window has no conformant object, ug creation is failed.
	evas_object_size_hint_weight_set( cf, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add( win, cf);
	evas_object_show(cf);
//
	Evas_Object* cf_bg = elm_bg_add( cf);
	elm_object_style_set( cf_bg, "indicator/headerbg");
	elm_object_part_content_set( cf, "elm.swallow.indicator_bg", cf_bg);
	evas_object_show(cf_bg);


	Evas_Object* layout = elm_layout_add( cf);
	elm_layout_theme_set( layout, "layout", "application", "default");
	evas_object_size_hint_weight_set( layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_content_set( cf, layout);

	evas_object_show(layout);

	return layout;
}

void WWindow::onDestroy()
{

}



