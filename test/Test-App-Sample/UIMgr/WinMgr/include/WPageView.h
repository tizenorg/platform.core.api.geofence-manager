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

#ifndef _WPAGE_VIEW_H_
#define _WPAGE_VIEW_H_

#include "WView.h"
#include <Elementary.h>

class WAPP_ASSIST_EXPORT WPageView : public WView
{
public:
	WPageView();
	virtual const char* getClassName();

public:
	bool attachPage( WView* view );
	int getPageCount();
	WView* getPage( int index);
	int getCurrentPageIndex();

	Evas_Object* getScroller();

public:
	void setOnInitScroller( std::function<void (Evas_Object*)> handlerFunc );

protected:
	virtual Evas_Object* onCreate( Evas_Object* parent, void* viewParam );
	virtual Evas_Object* onCreateLayout( Evas_Object* parent);
	virtual void onInitScroller( Evas_Object* scroller );
	virtual bool onAttachPage( WView* view);

private:
	static void layoutResizeCallback(void *data, Evas *e, Evas_Object *obj, void *event_info);

protected:
	virtual ~WPageView();
private:
	WDECLARE_PRIVATE_IMPL(WPageView);
	WDISABLE_COPY_AND_ASSIGN(WPageView);
};

#endif /* _WPAGE_VIEW_H_ */
