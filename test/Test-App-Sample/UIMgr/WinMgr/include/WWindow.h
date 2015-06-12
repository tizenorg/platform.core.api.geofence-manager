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

#ifndef _WWINDOW_H_
#define _WWINDOW_H_

#include <functional>
#include <Evas.h>
#include <Elementary.h>
#include "WDefine.h"

class WUiObject;

class WAPP_ASSIST_EXPORT WWindow
{
public:
	WWindow(const char* name, Elm_Win_Type winType);
	virtual const char* getClassName();
public:
	bool create();
	void destroy();

	Evas_Object* getEvasObj();
	Evas_Object* getBaseLayout();

	bool attachBaseUiObject( WUiObject* baseObj, void* creationParam = NULL );
	WUiObject* getBaseUiObject();
	WUiObject* detachBaseUiObject();
public:
	void setOnCreateWin( std::function<Evas_Object* (void)> handlerFunc );
	void setOnCreateBaseLayout( std::function<Evas_Object* (Evas_Object*)> handlerFunc );
	void setOnDestroy( std::function<void (void)> handlerFunc );

protected:
	const char* getName();
	Elm_Win_Type getWinType();

protected:
	virtual Evas_Object* onCreateWin();
	virtual Evas_Object* onCreateBaseLayout(Evas_Object* win);
	virtual void onDestroy();

protected:
	virtual ~WWindow();
private:
	WWindow();
	WDECLARE_PRIVATE_IMPL(WWindow);
	WDISABLE_COPY_AND_ASSIGN(WWindow);
};

#endif /* _WWINDOW_H_ */
