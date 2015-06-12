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

#ifndef _WVIEW_H_
#define _WVIEW_H_

#include <functional>
#include <Evas.h>
#include <Elementary.h>
#include "WUiObject.h"
#include "WNaviframe.h"


class WAPP_ASSIST_EXPORT WView : public WUiObject
{
public:
	WView();
	WView( const char* name );
	virtual const char* getClassName();

public:
	bool create( Evas_Object* parent, void* viewParam );
	void destroy();

	Evas_Object* getEvasObj();
	WNaviframe* getNaviframe();

	void setName( const char* name );
	const char* getName();
	//
	void setTitle( const char* title );
	const char* getTitle();

	static WView* getInstanceFromEvasObj(Evas_Object* obj);
	//
public:
	void setOnCreate( std::function<Evas_Object* (Evas_Object*, void*)> handlerFunc );
	void setOnDestroy( std::function<void (void)> handlerFunc );
	void setOnPushed( std::function<void (Elm_Object_Item*)> handlerFunc );

protected:
	virtual void onPushed(Elm_Object_Item* naviItem);
	virtual Elm_Object_Item* onDoPush( Evas_Object* naviFrame);
	virtual bool onPop();
//	virtual void onBecomeTop();
//	virtual void onExpose();
	//
	virtual Evas_Object* onCreate( Evas_Object* parent, void* viewParam );
	virtual void onDestroy();

protected:
	virtual ~WView();


private:
	bool __callPushedHandlerFunc(Elm_Object_Item* naviItem);
	void __destroy();

private:
	WDECLARE_PRIVATE_IMPL(WView);
	WDISABLE_COPY_AND_ASSIGN(WView);
	friend class WNaviframe;
};



#endif /* _WVIEW_H_ */
