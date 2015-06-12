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

#ifndef _WNAVI_FRAME_H_
#define _WNAVI_FRAME_H_

#include <functional>
#include <Evas.h>
#include <Eina.h>
#include <Elementary.h>
#include "WUiObject.h"

// forward declaration
class WWindow;
class WView;

class WAPP_ASSIST_EXPORT WNaviframe : public WUiObject
{
public:
	WNaviframe();
	virtual const char* getClassName();

public:
	bool create( Evas_Object* parent, void* param );
	void destroy();
	Evas_Object* getEvasObj();

	void setName( const char* name);
	const char* getName();

	WWindow* getWindow();

	bool push( WView* view, const char* viewName = NULL, void* viewParam = NULL );
public:
	void setOnCreate( std::function<Evas_Object*(Evas_Object*, void*)> handlerFunc);
	void setOnLastItemPop( std::function<void(bool*)> handlerFunc);
protected:
	virtual Evas_Object* onCreate( Evas_Object* parent, void* param);
	virtual void onDestroy();
	virtual bool onPush( WView* view, void* viewParam);
	virtual void onLastItemPop( bool* popOut );

protected:
	virtual ~WNaviframe();
private:
	static Eina_Bool __popCb( void* data, Elm_Object_Item* item);
private:
	WDECLARE_PRIVATE_IMPL(WNaviframe);
	WDISABLE_COPY_AND_ASSIGN(WNaviframe);

};

#endif /* _WNAVI_FRAME_H_ */
