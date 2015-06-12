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

#include "WView.h"
#include "WDebugInternal.h"
#include "WDefineInternal.h"

#include <Elementary.h>
#include <memory.h>

extern char* g_edjPath;

#define __CLASS_NAME	"WView"


class __WViewImpl
{
public:
	__WViewImpl();
	~__WViewImpl();
public:
	//
	Evas_Object* __obj;
	char* __name;
	char* __title;
	//
	std::function<Evas_Object* (Evas_Object*, void*)> __createHandler;
	std::function<void (void)> __destroyHandler;
	std::function<void (Elm_Object_Item*)> __pushedHandler;

	static void __objDelCb( void* data, Evas* evas, Evas_Object* obj, void* event_info);

};
__WViewImpl::__WViewImpl()
{
	__obj = NULL;
	__name = NULL;
	__title = NULL;
	__createHandler = NULL;
	__destroyHandler = NULL;
	__pushedHandler = NULL;
}

__WViewImpl::~__WViewImpl()
{
	if(__name)
		free(__name);
}

void __WViewImpl::__objDelCb( void* data, Evas* evas, Evas_Object* obj, void* event_info)
{
	WHIT();

	WView* view = (WView*)data;

	view->__destroy();
}

WView::WView()
{
	__pv = new __WViewImpl();
}

WView::WView( const char* name )
{
	__pv = new __WViewImpl();
	setName( name);
}

WView::~WView()
{
	WHIT();
	if( __pv->__name) WDEBUG( "name=%s", __pv->__name );

	delete __pv;
}

const char* WView::getClassName()
{
	return __CLASS_NAME;
}


bool WView::create( Evas_Object* parent, void* viewParam )
{
	WHIT();

	if( __pv->__obj )
	{
		WDEBUG("Already created!");
//		WASSERT( elm_object_parent_widget_get(__obj) == parent );
		return true;
	}
	if( __pv->__createHandler )
		__pv->__obj = __pv->__createHandler( parent, viewParam );
	else
		__pv->__obj = onCreate( parent, viewParam );

	if( __pv->__obj == NULL)
		return false;


	evas_object_data_set( __pv->__obj, WKEY_VIEW_INSTANCE, this );
	evas_object_event_callback_add( __pv->__obj, EVAS_CALLBACK_DEL, __WViewImpl::__objDelCb, this);
	return true;
}

void WView::destroy()
{
	if(__pv->__obj)
	{
		evas_object_del(__pv->__obj);
		// Do not leave any code here.
		// After executing upper statement "evas_object_del", this object will be deleted at evas object deletion callback!
	}
	else
	{
		__destroy();
	}
}

Evas_Object* WView::getEvasObj()
{
	return __pv->__obj;
}

void WView::setName(const char* name)
{
	if( __pv->__name)
	{
		free( __pv->__name);
		__pv->__name = NULL;
	}
	if( name )
	{
		__pv->__name = (char*)malloc( strlen(name)+1);
		strcpy( __pv->__name, name);
	}
}

const char* WView::getName()
{
	return __pv->__name;
}

void WView::setTitle( const char* title )
{
	if( __pv->__title)
	{
		free( __pv->__title);
		__pv->__title = NULL;
	}
	if( title )
	{
		__pv->__title = (char*)malloc( strlen(title)+1);
		strcpy( __pv->__title, title);
	}
}
const char* WView::getTitle()
{
	return __pv->__title;
}

WView* WView::getInstanceFromEvasObj(Evas_Object* obj)
{
	return (WView*)evas_object_data_get(obj, WKEY_VIEW_INSTANCE);
}

void WView::setOnCreate( std::function<Evas_Object* (Evas_Object*, void*)> handlerFunc )
{
	__pv->__createHandler = handlerFunc;
}

void WView::setOnPushed( std::function<void (Elm_Object_Item*)> handlerFunc )
{
	__pv->__pushedHandler = handlerFunc;
}

void WView::setOnDestroy( std::function<void (void)> handlerFunc )
{
	__pv->__destroyHandler = handlerFunc;
}




bool WView::onPop()
{
	return true;
}

//void WView::onBecomeTop()
//{
//}
//
//void WView::onExpose()
//{
//}

WNaviframe* WView::getNaviframe()
{
	WNaviframe* frame = (WNaviframe*)evas_object_data_get( getEvasObj(), WKEY_FRAME_INSTANCE_OF_VIEW );

	if( frame == NULL)
	{
		WDEBUG("Frame does not exist!");
	}

	return frame;
}

void WView::onPushed(Elm_Object_Item* naviItem)
{

}

Elm_Object_Item* WView::onDoPush( Evas_Object* naviFrame)
{
	return NULL;
}


Evas_Object* WView::onCreate( Evas_Object* parent, void* viewParam )
{
	Evas_Object* layout = elm_layout_add(parent);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	if( g_edjPath)
	{
		elm_layout_file_set(layout, g_edjPath, "default-view");
		Evas_Object* rect = evas_object_rectangle_add(evas_object_evas_get(layout));
		evas_object_color_set(rect, rand() % 256, rand() % 256, rand() % 256, 255);
		evas_object_size_hint_weight_set(rect, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(rect, EVAS_HINT_FILL, EVAS_HINT_FILL);

		elm_object_part_content_set(layout, "rect", rect);
		if( getName() != NULL )
			elm_object_part_text_set(layout, "name", getName());
	}
	else
	{
		WDEBUG( "edj file is not set!");
	}
	evas_object_show(layout);

	return layout;

//	Evas_Object* rect = evas_object_rectangle_add(evas_object_evas_get(parent));
//	evas_object_color_set(rect, rand() % 256, rand() % 256, rand() % 256, 255);
//	evas_object_size_hint_weight_set(rect, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
//	evas_object_size_hint_align_set(rect, EVAS_HINT_FILL, EVAS_HINT_FILL);
//	evas_object_show(rect);
//	return rect;
}

void WView::onDestroy()
{

}

bool WView::__callPushedHandlerFunc(Elm_Object_Item* naviItem)
{
	if( __pv->__pushedHandler )
	{
		__pv->__pushedHandler(naviItem);
		return true;
	}
	return false;
}

void WView::__destroy()
{
	if( __pv->__destroyHandler)
		__pv->__destroyHandler();
	else
		onDestroy();
	delete this;
}
