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

#include "WPageView.h"
#include "WDebugInternal.h"
#include "WDefineInternal.h"
#include "WView.h"
#include <list>

extern char* g_edjPath;

#define __CLASS_NAME	"WPageView"

class __WPageViewImpl
{
public:
	__WPageViewImpl(WPageView* host);
	~__WPageViewImpl();
public:
	WPageView* __b;
	//
	Evas_Object* __scroller;
	Evas_Object* __box;
	int __currentPage;
	int __pageWidth;
	int __pageHeight;
	void* __listPageBase;
	void* __listPage;
	//
	std::function<void (Evas_Object*)> __initScrollerHandler;

	static void __scrollCallback(void *data, Evas_Object *obj, void *event_info);
	static Eina_Bool __onTimer(void* data);
};

__WPageViewImpl::__WPageViewImpl(WPageView* host):
	__scroller(NULL),
	__box(NULL),
	__currentPage(0),
	__pageWidth(0),
	__pageHeight(0),
	__listPageBase(NULL),
	__listPage(NULL),
	__initScrollerHandler(NULL)
{
	__b = host;
	__listPage = new std::list<WView*>;
	__listPageBase = new std::list<Evas_Object*>;
}

__WPageViewImpl::~__WPageViewImpl()
{
	delete (std::list<WView*>*)__listPage;
	delete (std::list<Evas_Object*>*)__listPageBase;
}

void __WPageViewImpl::__scrollCallback(void *data, Evas_Object *obj, void *event_info)
{
	__WPageViewImpl* pageViewImpl = ((WPageView *)data)->__pv;
	int x;

	evas_object_geometry_get(pageViewImpl->__box, &x, NULL, NULL, NULL);

	if (x % pageViewImpl->__pageWidth == 0) {
		int page;
		elm_scroller_current_page_get(obj, &page, NULL);
		pageViewImpl->__currentPage = page;
	}
}

Eina_Bool __WPageViewImpl::__onTimer(void* data)
{
	__WPageViewImpl* pageViewImpl = ((WPageView *)data)->__pv;
	WPageView::layoutResizeCallback(data, NULL, pageViewImpl->__scroller, NULL);
	return ECORE_CALLBACK_CANCEL;
}


WPageView::WPageView()
{
	__pv = new __WPageViewImpl(this);
}

WPageView::~WPageView()
{
	delete __pv;
}

const char* WPageView::getClassName()
{
	return __CLASS_NAME;
}

bool WPageView::attachPage( WView* view)
{
	if( __pv->__scroller == NULL)
	{
		WASSERT("First, create page view!");
		return false;
	}

	if( onAttachPage( view) == false )
	{
		WERROR("Fail to attach the page! view=%s", view->getName() );
		return false;
	}


	std::list<WView*>* vlist = (std::list<WView*>*)__pv->__listPage;
	vlist->push_back( view );

	WNaviframe* frame = (WNaviframe*)evas_object_data_get( getEvasObj(), WKEY_FRAME_INSTANCE_OF_VIEW );
	if( frame )
		evas_object_data_set( view->getEvasObj(), WKEY_FRAME_INSTANCE_OF_VIEW, frame );
	else
	{
		WDEBUG("No frame of page view");
	}
	return true;
}

int WPageView::getPageCount()
{
	std::list<WView*>* vlist = (std::list<WView*>*)__pv->__listPage;
	return vlist->size();
}

WView* WPageView::getPage( int index)
{
	std::list<WView*>* vlist = (std::list<WView*>*)__pv->__listPage;
	int count = 0;
	for( auto it = vlist->begin(); it != vlist->end(); it++)
	{
		if( count == index)
			return (*it);
		count++;
	}
	return NULL;
}

int WPageView::getCurrentPageIndex()
{
	return __pv->__currentPage;
}

Evas_Object* WPageView::getScroller()
{
	return __pv->__scroller;
}

static Evas_Object* __createPageBase(Evas_Object* parent)
{
	Evas_Object* page_base = elm_layout_add( parent);
	evas_object_size_hint_weight_set(page_base, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	Eina_Bool ret = elm_layout_file_set(page_base, g_edjPath, "page");

	if (ret == EINA_FALSE) {
		WERROR("Failed to set layout");
	}

	Evas_Object* rect = evas_object_rectangle_add(evas_object_evas_get(page_base));
	evas_object_color_set(rect, 10, 5, 5, 255);
	evas_object_size_hint_weight_set(rect, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(rect);

	elm_object_part_content_set(page_base, "sizer", rect);

	evas_object_show(page_base);
	return page_base;
}

bool WPageView::onAttachPage(WView* view)
{
	Evas_Object* pageBase = __createPageBase(__pv->__box);

	view->create(pageBase, NULL);
	elm_object_part_content_set(pageBase, "sw", view->getEvasObj());

	elm_box_pack_end(__pv->__box, pageBase);

	std::list<Evas_Object*>* vlist = (std::list<Evas_Object*>*)__pv->__listPageBase;
	vlist->push_back(pageBase);

	return true;
}



void WPageView::layoutResizeCallback(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	WHIT();

	WPageView* pageView = (WPageView *)data;
	__WPageViewImpl* pv = pageView->__pv;
	int w, h;

	evas_object_geometry_get(obj, NULL, NULL, &w, &h);

	pv->__pageWidth = w;
	pv->__pageHeight = h;

	WDEBUG("%d %d", w, h);

	std::list<Evas_Object*>* vlist = (std::list<Evas_Object*>*)pv->__listPageBase;
	if( vlist)
	for( auto it = vlist->begin(); it != vlist->end(); it++)
	{
		Evas_Object* page_base = (Evas_Object*)(*it);
		evas_object_size_hint_min_set(elm_object_part_content_get(page_base, "sizer"), w, h);
	}

	elm_scroller_page_size_set(pv->__scroller, w, h);
	elm_scroller_page_show(pv->__scroller, pv->__currentPage, 0);
}


void WPageView::setOnInitScroller( std::function<void (Evas_Object*)> handlerFunc )
{
	__pv->__initScrollerHandler = handlerFunc;
}

Evas_Object* WPageView::onCreate(Evas_Object* parent, void* viewParam)
{
	WHIT();

	Evas_Object* layout = onCreateLayout( parent );
	Evas_Object* scroller = NULL;
	if( layout ) {
		scroller = elm_scroller_add( layout);
		evas_object_event_callback_add(layout, EVAS_CALLBACK_RESIZE, layoutResizeCallback, this);
	} else {
		scroller = elm_scroller_add( parent);
		evas_object_event_callback_add(scroller, EVAS_CALLBACK_RESIZE, layoutResizeCallback, this);
	}

	evas_object_smart_callback_add(scroller, "scroll", __WPageViewImpl::__scrollCallback, this);

	elm_scroller_loop_set( scroller, EINA_FALSE, EINA_FALSE);
	evas_object_size_hint_weight_set( scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set( scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_scroller_page_relative_set( scroller, 1.0, 0.0);
	elm_scroller_policy_set( scroller, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_OFF);
	elm_scroller_page_scroll_limit_set( scroller, 1, 0);
	elm_object_scroll_lock_y_set( scroller, EINA_TRUE);

	if( __pv->__initScrollerHandler )
		__pv->__initScrollerHandler(scroller);
	else
		onInitScroller( scroller );

	if (layout)
		elm_object_part_content_set(layout, "scroller", scroller);

	Evas_Object* box = elm_box_add(scroller);

	__pv->__scroller = scroller;
	__pv->__box = box;

	elm_box_horizontal_set( box, EINA_TRUE);
	elm_object_content_set( scroller, box);
	evas_object_show( scroller);
	evas_object_show( box);

//	__layoutResizeCallback(this, NULL, scroller, NULL);
	ecore_timer_add(0.0, __WPageViewImpl::__onTimer, this);

//////////////////////////////////////////////////////////////////////////////////
//	Evas_Object* rect = evas_object_rectangle_add(evas_object_evas_get(box));
//	evas_object_color_set(rect, 255, 255, 0, 255);
//	evas_object_size_hint_min_set( rect, 500, 500);
//	evas_object_size_hint_weight_set(rect, 0, 0);
//	evas_object_size_hint_align_set(rect, 0, EVAS_HINT_FILL);
//
	if( layout )
		return layout;
	else
		return scroller;

}

void WPageView::onInitScroller( Evas_Object* scroller )
{
}

Evas_Object* WPageView::onCreateLayout( Evas_Object* parent)
{
	WHIT();
	return NULL;
}

