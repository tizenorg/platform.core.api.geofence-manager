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

#include "WUgWindow.h"
#include "WDebugInternal.h"
#include "WDefineInternal.h"
#include "WUiObject.h"

#include <Ecore.h>
#include <efl_assist.h>


#define __CLASS_NAME	"WUgWindow"

class __WUgWindowImpl
{
public:
	__WUgWindowImpl(ui_gadget_h ug, enum ug_mode mode);
	~__WUgWindowImpl();
public:
	//
	ui_gadget_h __ug;
	enum ug_mode __mode;
	Evas_Object* __baseLayout;
	WUiObject* __baseObj;
	//
	static void __delReqCb( void* data, Evas_Object* obj, void* event_info);
};

__WUgWindowImpl::__WUgWindowImpl(ui_gadget_h ug, enum ug_mode mode):
	__ug(ug),
	__mode(mode),
	__baseLayout(NULL),
	__baseObj(NULL)
{
}

__WUgWindowImpl::~__WUgWindowImpl()
{

}

void __WUgWindowImpl::__delReqCb( void* data, Evas_Object* obj, void* event_info)
{
	WHIT();

	WUgWindow* window = (WUgWindow*)data;
	window->onDestroy();

	delete window;
}


WUgWindow::WUgWindow(ui_gadget_h ug, enum ug_mode mode)
{
	__pv = new __WUgWindowImpl(ug, mode);
}

WUgWindow::~WUgWindow()
{
	WHIT();
	delete __pv;
}

const char* WUgWindow::getClassName()
{
	return __CLASS_NAME;
}

bool WUgWindow::create()
{
	if (__pv->__baseLayout){
		WDEBUG("Already created!");
		return true;
	}

	Evas_Object* base = onCreateBaseLayout();
	__pv->__baseLayout = base;

	if (base){
		evas_object_data_set( base, WKEY_UGWINDOW_INSTANCE, this );
		evas_object_smart_callback_add( base, "delete,request", __WUgWindowImpl::__delReqCb, this);
		return true;
	} else
		return false;
}

void WUgWindow::destroy()
{
	//
	if (__pv->__baseLayout) {
		evas_object_del(__pv->__baseLayout);
		// Do not leave any code here.
		// After executing upper statement "evas_object_del", this object will be deleted at evas object deletion callback!
	} else {
		onDestroy();
		delete this;
	}
}
Evas_Object* WUgWindow::getEvasObj()
{
	return (Evas_Object*)ug_get_window();
}

Evas_Object* WUgWindow::getBaseLayout()
{
	return onGetBaseLayout();
}

bool WUgWindow::attachBaseUiObject( WUiObject* baseObj )
{

	WASSERT( __pv->__baseObj == NULL);

	if (baseObj->create( __pv->__baseLayout, NULL) == false)
		return false;

	__pv->__baseObj = baseObj;

	return true;
}

WUiObject* WUgWindow::getBaseUiObject()
{
	return __pv->__baseObj;
}

ui_gadget_h WUgWindow::getUgHandle()
{
	return __pv->__ug;
}
enum ug_mode WUgWindow::getUgMode()
{
	return __pv->__mode;
}

Evas_Object* WUgWindow::onCreateBaseLayout()
{
	Evas_Object* win = (Evas_Object*)ug_get_window();
	if (win == NULL){
		WERROR("fail to get window");
		return NULL;
	}
	Evas_Object* parent = (Evas_Object*)ug_get_parent_layout(getUgHandle());
	if (parent == NULL){
		WERROR("fail to get parent layout");
		return NULL;
	}

	Evas_Object* base = elm_layout_add(parent);
	if (getUgMode() == UG_MODE_FULLVIEW){
		elm_layout_theme_set( base, "layout", "application", "default");
	} else
		elm_layout_theme_set( base, "layout", "application", "noindicator");

	evas_object_size_hint_weight_set( base, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	Evas_Object* bg = elm_bg_add(base);
	evas_object_size_hint_weight_set( bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set( base, "elm.swallow.bg", bg);

	return base;
}
void WUgWindow::onDestroy()
{

}

Evas_Object* WUgWindow::onGetBaseLayout()
{
	return __pv->__baseLayout;
}

