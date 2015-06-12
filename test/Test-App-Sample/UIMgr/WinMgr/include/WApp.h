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

#ifndef _WAPP_H_
#define _WAPP_H_

#include <app.h>
#include "WWindow.h"
#include "WDefine.h"


class WAPP_ASSIST_EXPORT WApp
{
public:
	WApp();
	virtual ~WApp();

public:
	int start( int argc, char* argv[], const char* edjPath = NULL );

protected:
	virtual bool onCreate(){return true;};
	virtual void onTerminate(){};
	virtual void onPause(){};
	virtual void onResume(){};
	virtual void onService(app_control_h service);
	virtual void onLowMemory(){};
	virtual void onLowBattery(){};
	virtual void onDeviceOrientation( app_device_orientation_e orientation ){};
	virtual void onRegionFormatChanged(){};
	virtual void onLanguageChanged(){};

	WWindow* getWindow();
	bool attachWindow(WWindow* win);
	virtual int onStart( int argc, char* argv[], const char* edjPath);

private:
	WDECLARE_PRIVATE_IMPL(WApp);
	WDISABLE_COPY_AND_ASSIGN(WApp);

};

#endif /* _WAPP_H_ */
