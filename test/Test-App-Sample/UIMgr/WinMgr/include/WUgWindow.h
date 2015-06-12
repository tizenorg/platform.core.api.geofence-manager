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

#ifndef _WUG_WINDOW_H_
#define _WUG_WINDOW_H_

#include <Evas.h>
#include <ui-gadget.h>
#include "WDefine.h"

class WUiObject;

class WAPP_ASSIST_EXPORT WUgWindow
{
public:
	WUgWindow(ui_gadget_h ug, enum ug_mode mode);
	virtual const char* getClassName();
public:
	bool create();
	void destroy();

	Evas_Object* getEvasObj();
	Evas_Object* getBaseLayout();

	bool attachBaseUiObject( WUiObject* baseObj );
	WUiObject* getBaseUiObject();
protected:
	ui_gadget_h getUgHandle();
	enum ug_mode getUgMode();

	virtual Evas_Object* onCreateBaseLayout();
	virtual void onDestroy();
	virtual Evas_Object* onGetBaseLayout();
protected:
	virtual ~WUgWindow();
private:
	WUgWindow();
	WDECLARE_PRIVATE_IMPL(WUgWindow);
	WDISABLE_COPY_AND_ASSIGN(WUgWindow);
};


#endif /* _WUG_WINDOW_H_ */
