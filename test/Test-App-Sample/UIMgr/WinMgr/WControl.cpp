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

#include "WControl.h"
#include "WDebugInternal.h"
#include "WDefineInternal.h"

#include <Elementary.h>
#include <memory.h>

#define __CLASS_NAME	"WControl"


class __WControlImpl
{
public:
	__WControlImpl();
	~__WControlImpl();
public:
	//
	Evas_Object* __obj;
	char* __name;
	//
	static void __objDelCb( void* data, Evas* evas, Evas_Object* obj, void* event_info);

};
__WControlImpl::__WControlImpl()
{
	__obj = NULL;
	__name = NULL;
}

__WControlImpl::~__WControlImpl()
{
	if(__name)
		free(__name);
}

void __WControlImpl::__objDelCb( void* data, Evas* evas, Evas_Object* obj, void* event_info)
{
	WControl* ctrl = (WControl*)data;
	ctrl->onDestroy();

	delete ctrl;
}
WControl::WControl()
{
	__pv = new __WControlImpl();
}

WControl::~WControl() {
	WHIT();
	delete __pv;
}
const char* WControl::getClassName()
{
	return __CLASS_NAME;
}

bool WControl::create( Evas_Object* parent, void* param )
{
	WHIT();

	if (__pv->__obj){
		WDEBUG("Already created!");
		return true;
	}
	__pv->__obj = onCreate( parent, param );

	if( __pv->__obj == NULL)
		return false;

	evas_object_data_set( __pv->__obj, WKEY_CONTROL_INSTANCE, this );
	evas_object_event_callback_add( __pv->__obj, EVAS_CALLBACK_DEL, __WControlImpl::__objDelCb, this);
	return true;
}

void WControl::destroy()
{
	if (__pv->__obj){
		evas_object_del(__pv->__obj);
		// Do not leave any code here.
		// After executing upper statement "evas_object_del", this object will be deleted at evas object deletion callback!
	} else {
		onDestroy();
		delete this;
	}
}
Evas_Object* WControl::getEvasObj()
{
	return __pv->__obj;
}

void WControl::setName(const char* name)
{
	if(__pv->__name){
		free( __pv->__name);
		__pv->__name = NULL;
	}
	if(name) {
		__pv->__name = (char*)malloc( strlen(name)+1);
		strcpy( __pv->__name, name);
	}
}

const char* WControl::getName()
{
	return __pv->__name;
}

WControl* WControl::getInstanceFromEvasObj(Evas_Object* obj)
{
	return (WControl*)evas_object_data_get(obj, WKEY_CONTROL_INSTANCE);
}

