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

#ifndef _WUG_H_
#define _WUG_H_

#include <ui-gadget.h>
#include <Evas.h>
#include "WDefine.h"

class WUgWindow;

class WAPP_ASSIST_EXPORT WUg
{
public:
	WUg();
	virtual ~WUg();

public:
	int start(struct ug_module_ops* ops, enum ug_option option, const char* data);
	ui_gadget_h getUgHandle();

protected:
	virtual Evas_Object* onCreate(ui_gadget_h ug, enum ug_mode mode, app_control_h service);
	virtual void onStart(ui_gadget_h ug, app_control_h service);
	virtual void onPause(ui_gadget_h ug, app_control_h service);
	virtual void onResume(ui_gadget_h ug, app_control_h service);
	virtual void onDestroy(ui_gadget_h ug, app_control_h service);
	virtual void onMessage(ui_gadget_h ug, app_control_h msg, app_control_h service);
	virtual void onEvent(ui_gadget_h ug, enum ug_event event, app_control_h service);
	virtual void onKeyEvent(ui_gadget_h ug, enum ug_key_event event, app_control_h service);
	virtual void onDestroying(ui_gadget_h ug, app_control_h service);

	WUgWindow* getWindow();
	bool attachWindow(WUgWindow* win);

private:
	WDECLARE_PRIVATE_IMPL(WUg);
	WDISABLE_COPY_AND_ASSIGN(WUg);

};


#endif /* _WUG_H_ */
