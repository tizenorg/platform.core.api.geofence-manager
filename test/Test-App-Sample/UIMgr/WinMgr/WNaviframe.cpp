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

#include <memory.h>
#include "WNaviframe.h"
#include "WDebugInternal.h"
#include "WWindow.h"
#include "WDefineInternal.h"

#include <efl_assist.h>

#include "WView.h"

#define __CLASS_NAME	"WNaviframe"

class __WNaviframeImpl
{
public:
	__WNaviframeImpl();
	~__WNaviframeImpl();
public:
	//
	char* __name;
	Evas_Object* __evasObj;
	//
	std::function<Evas_Object*(Evas_Object*, void*)> __createHandler;
	std::function<void(bool*)> __lastItemPopHandler;

	static void __objDelCb( void* data, Evas* evas, Evas_Object* obj, void* event_info);
	static void __transitionFinishedCb(void* data, Evas_Object* obj, void* event_info);
};

__WNaviframeImpl::__WNaviframeImpl():
		__name(NULL),
		__evasObj(NULL),
		__createHandler(NULL),
		__lastItemPopHandler(NULL)
{
}
__WNaviframeImpl::~__WNaviframeImpl()
{
	if( __name)
		free(__name);
}


void __WNaviframeImpl::__objDelCb( void* data, Evas* evas, Evas_Object* obj, void* event_info)
{
	WHIT();

	WNaviframe* frame = (WNaviframe*)data;
	frame->onDestroy();
	//
	delete frame;
}


void __WNaviframeImpl::__transitionFinishedCb(void* data, Evas_Object* obj, void* event_info)
{
//	WNaviframe* nf = (WNaviframe*)data;
//	nf->getTopView()->onExpose();
}


WNaviframe::WNaviframe()
{
	__pv = new __WNaviframeImpl();
}

WNaviframe::~WNaviframe()
{
	WHIT();
	delete __pv;
}

const char* WNaviframe::getClassName()
{
	return __CLASS_NAME;
}

bool WNaviframe::create( Evas_Object* parent, void* param )
{
//	mWindow = win;
	if (__pv->__evasObj){
		WDEBUG("Already created!");
		return true;
	}
	__pv->__evasObj = NULL;
	if (__pv->__createHandler)
		__pv->__evasObj = __pv->__createHandler(parent, param);
	else
		__pv->__evasObj = onCreate(parent, param);
	//
	if (__pv->__evasObj ==  NULL){
		WERROR("No frame object created!");
		return false;
	}
	evas_object_data_set(__pv->__evasObj, WKEY_FRAME_INSTANCE, this);
	evas_object_event_callback_add(__pv->__evasObj, EVAS_CALLBACK_DEL, __WNaviframeImpl::__objDelCb, this);

	return true;
}

void WNaviframe::destroy()
{
	WHIT();
	if (__pv->__evasObj){
		evas_object_del(__pv->__evasObj);
		// Do not leave any code here.
		// After executing upper statement "evas_object_del", this object will be deleted at evas object deletion callback!
	} else {
		onDestroy();
		delete this;
	}
}

bool WNaviframe::push( WView* view, const char* viewName, void* viewParam )
{
	if( viewName )
		setName( viewName );
	return onPush( view, viewParam );
	//

}

Evas_Object* WNaviframe::getEvasObj()
{
	return __pv->__evasObj;
}

void WNaviframe::setName(const char* name)
{
	if (__pv->__name){
		free(__pv->__name);
		__pv->__name = NULL;
	}
	if (name){
		__pv->__name = (char*)malloc( strlen(name)+1);
		strcpy( __pv->__name, name);
	}
}

const char* WNaviframe::getName()
{
	return __pv->__name;
}

WWindow* WNaviframe::getWindow()
{
	if( __pv->__evasObj == NULL)
		return NULL;

	Evas_Object* win = evas_object_top_get( evas_object_evas_get(__pv->__evasObj));

	WWindow* window = (WWindow*)evas_object_data_get( win, WKEY_WINDOW_INSTANCE );

	return window;
}


//WView* WNaviframe::getTopView()
//{
//	//todo
//	return NULL;
//}

void WNaviframe::setOnCreate( std::function<Evas_Object*(Evas_Object*, void*)> handlerFunc)
{
	__pv->__createHandler = handlerFunc;
}


void WNaviframe::setOnLastItemPop( std::function<void(bool*)> handlerFunc)
{
	__pv->__lastItemPopHandler = handlerFunc;
}



Evas_Object* WNaviframe::onCreate(Evas_Object* parent, void* param)
{
	Evas_Object* obj = elm_naviframe_add( parent);
	elm_object_part_content_set(parent, "elm.swallow.content", obj);
	ea_object_event_callback_add(obj, EA_CALLBACK_BACK, ea_naviframe_back_cb, NULL);
	ea_object_event_callback_add(obj, EA_CALLBACK_MORE, ea_naviframe_more_cb, NULL);
	evas_object_smart_callback_add(obj, "transition,finished", __WNaviframeImpl::__transitionFinishedCb, this);

	evas_object_show(obj);

	return obj;
}

void WNaviframe::onDestroy()
{

}

void WNaviframe::onLastItemPop( bool* popOut )
{
	WHIT();

	*popOut = false;

}

bool WNaviframe::onPush( WView* view, void* viewParam )
{
	WASSERT( getEvasObj() != NULL );
	view->create( getEvasObj(), viewParam );
	//
	Elm_Object_Item* item = view->onDoPush( getEvasObj() );
	if(item == NULL)
		item = elm_naviframe_item_push( getEvasObj(), view->getTitle(), NULL, NULL, view->getEvasObj(), NULL);
	evas_object_data_set( view->getEvasObj(), WKEY_FRAME_INSTANCE_OF_VIEW, this );
	//
	if( view->__callPushedHandlerFunc( item ) == false)
		view->onPushed( item );

	elm_naviframe_item_pop_cb_set( item, __popCb, view);

	return true;
}

Eina_Bool WNaviframe::__popCb( void* data, Elm_Object_Item* item)
{
	WView* view = (WView*)data;

	WNaviframe* nf = view->getNaviframe();
	if (nf == NULL)
		return EINA_FALSE;

	if (item == elm_naviframe_bottom_item_get( nf->getEvasObj())){
		bool ret = view->onPop();
		if (ret == false)
			return EINA_FALSE;

		bool popOut = true;

		if (nf->__pv->__lastItemPopHandler)
			nf->__pv->__lastItemPopHandler(&popOut);
		else
			nf->onLastItemPop(&popOut);
		if (popOut == true){
			if (nf->getWindow() != NULL)
				elm_win_lower( nf->getWindow()->getEvasObj() );
			elm_exit();
			return EINA_TRUE;
		} else {
			if (nf->getWindow() != NULL)
				elm_win_lower( nf->getWindow()->getEvasObj() );
			return EINA_FALSE;
		}
	} else {
		bool ret = view->onPop();
		if (ret)
			return EINA_TRUE;
		else
			return EINA_FALSE;
	}
}
