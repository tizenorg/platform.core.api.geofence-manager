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

#include "WUg.h"
#include <ui-gadget-module.h>
#include <memory.h>
#include "WDebugInternal.h"
#include "WUgWindow.h"

class __WUgImpl
{
public:
	__WUgImpl():__window(NULL), __ug(NULL) {}
	~__WUgImpl(){}
public:
	WUgWindow* __window;
	ui_gadget_h __ug;
	//
	static void* __onCreate(ui_gadget_h ug, enum ug_mode mode, app_control_h service, void* priv);
	static void __onStart(ui_gadget_h ug, app_control_h service, void* priv);
	static void __onPause(ui_gadget_h ug, app_control_h service, void* priv);
	static void __onResume(ui_gadget_h ug, app_control_h service, void* priv);
	static void __onDestroy(ui_gadget_h ug, app_control_h service, void* priv);
	static void __onMessage(ui_gadget_h ug, app_control_h msg, app_control_h service, void* priv);
	static void __onEvent(ui_gadget_h ug, enum ug_event event, app_control_h service, void* priv);
	static void __onKeyEvent(ui_gadget_h ug, enum ug_key_event event, app_control_h service, void* priv);
	static void __onDestroying(ui_gadget_h ug, app_control_h service, void* priv);
};

void* __WUgImpl::__onCreate(ui_gadget_h ug, enum ug_mode mode, app_control_h service, void* priv)
{
	WHIT();
	WUg* cug = (WUg*)priv;
	cug->__pv->__ug = ug;
	return cug->onCreate( ug, mode, service );

}


void __WUgImpl::__onStart(ui_gadget_h ug, app_control_h service, void* priv)
{
	WHIT();
	WUg* cug = (WUg*)priv;
	cug->onStart( ug, service );

}
void __WUgImpl::__onPause(ui_gadget_h ug, app_control_h service, void* priv)
{
	WHIT();
	WUg* cug = (WUg*)priv;
	cug->onPause( ug, service );
}

void __WUgImpl::__onResume(ui_gadget_h ug, app_control_h service, void* priv)
{
	WHIT();
	WUg* cug = (WUg*)priv;
	cug->onResume( ug, service );
}

void __WUgImpl::__onDestroy(ui_gadget_h ug, app_control_h service, void* priv)
{
	WHIT();
	WUg* cug = (WUg*)priv;
	cug->onDestroy( ug, service );

	if( cug->__pv->__window )
	{
		cug->__pv->__window->destroy();
		cug->__pv->__window = NULL;
	}
	delete cug;
}

void __WUgImpl::__onMessage(ui_gadget_h ug, app_control_h msg, app_control_h service, void* priv)
{
	WHIT();
	WUg* cug = (WUg*)priv;
	cug->onMessage( ug, msg, service );
}

void __WUgImpl::__onEvent(ui_gadget_h ug, enum ug_event event, app_control_h service, void* priv)
{
	WHIT();
	WUg* cug = (WUg*)priv;
	cug->onEvent( ug, event, service );
}

void __WUgImpl::__onKeyEvent(ui_gadget_h ug, enum ug_key_event event, app_control_h service, void* priv)
{
	WHIT();
	WUg* cug = (WUg*)priv;
	if(!ug)
		return;

	cug->onKeyEvent( ug, event, service );

	if( event == UG_KEY_EVENT_END )
	{
		ug_destroy_me(ug);
	}

}

void __WUgImpl::__onDestroying(ui_gadget_h ug, app_control_h service, void* priv)
{
	WHIT();
	WUg* cug = (WUg*)priv;
	cug->onDestroying( ug, service );
}

WUg::WUg()
{
	WHIT();
	__pv = new __WUgImpl();
}

WUg::~WUg()
{
	WHIT();
	delete __pv;
}

int WUg::start( struct ug_module_ops* ops, enum ug_option option, const char* data )
{
	ops->create = __WUgImpl::__onCreate;
	ops->start = __WUgImpl::__onStart;
	ops->pause = __WUgImpl::__onPause;
	ops->resume = __WUgImpl::__onResume;
	ops->destroy = __WUgImpl::__onDestroy;
	ops->message = __WUgImpl::__onMessage;
	ops->event = __WUgImpl::__onEvent;
	ops->key_event = __WUgImpl::__onKeyEvent;
	ops->destroying = __WUgImpl::__onDestroying;
	ops->priv =this;
	ops->opt = option;

	return 0;
}

ui_gadget_h WUg::getUgHandle()
{
	return __pv->__ug;
}

WUgWindow* WUg::getWindow()
{
	return __pv->__window;
}

bool WUg::attachWindow(WUgWindow* win)
{
	WASSERT( __pv->__window == NULL);
	__pv->__window = win;
	return win->create();
}

Evas_Object* WUg::onCreate(ui_gadget_h ug, enum ug_mode mode, app_control_h service)
{
	return NULL;
}
void WUg::onStart(ui_gadget_h ug, app_control_h service)
{

}
void WUg::onPause(ui_gadget_h ug, app_control_h service)
{

}
void WUg::onResume(ui_gadget_h ug, app_control_h service)
{

}
void WUg::onDestroy(ui_gadget_h ug, app_control_h service)
{

}
void WUg::onMessage(ui_gadget_h ug, app_control_h msg, app_control_h service)
{

}
void WUg::onEvent(ui_gadget_h ug, enum ug_event event, app_control_h service)
{

}
void WUg::onKeyEvent(ui_gadget_h ug, enum ug_key_event event, app_control_h service)
{

}
void WUg::onDestroying(ui_gadget_h ug, app_control_h service)
{

}

