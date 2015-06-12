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

#ifndef _WUI_OBJECT_H_
#define _WUI_OBJECT_H_

#include <Evas.h>
#include "WDefine.h"

class WAPP_ASSIST_EXPORT WUiObject
{
public:
	//
	virtual bool create(Evas_Object* parent, void* param )=0;
	virtual void destroy()=0;
	virtual Evas_Object* getEvasObj()=0;

protected:
	virtual ~WUiObject(){};

	//
};


#endif /* _WUI_OBJECT_H_ */
