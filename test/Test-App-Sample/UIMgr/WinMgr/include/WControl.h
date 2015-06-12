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

#ifndef _WCONTROL_H_
#define _WCONTROL_H_

#include <Evas.h>
#include "WUiObject.h"

class WAPP_ASSIST_EXPORT WControl : public WUiObject
{
public:
	WControl();
	virtual const char* getClassName();
public:
	bool create(Evas_Object* parent, void* param );
	void destroy();

	Evas_Object* getEvasObj();

	void setName( const char* name);
	const char* getName();

	static WControl* getInstanceFromEvasObj(Evas_Object* obj);
protected:
	virtual Evas_Object* onCreate( Evas_Object* parent, void* param )=0;
	virtual void onDestroy()=0;

protected:
	virtual ~WControl();

private:
	WDECLARE_PRIVATE_IMPL(WControl);
	WDISABLE_COPY_AND_ASSIGN(WControl);

};


#endif /* _WCONTROL_H_ */
