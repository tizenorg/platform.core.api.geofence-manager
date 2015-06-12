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

#ifndef __GEOFENCE_LOG_H__
#define __GEOFENCE_LOG_H__

#include <dlog.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "CAPI_GEOFENCE_MANAGER"

/*
* Internal Macros
*/

#define GEOFENCE_LOGD(fmt,args...) LOGD(fmt, ##args)
#define GEOFENCE_LOGW(fmt,args...) LOGW(fmt, ##args)
#define GEOFENCE_LOGI(fmt,args...) LOGI(fmt, ##args)
#define GEOFENCE_LOGE(fmt,args...) LOGE(fmt, ##args)

#define GEOFENCE_CHECK_CONDITION(condition, error, msg)	\
		do { \
			if (condition) { \
			} else { \
				GEOFENCE_LOGE("%s(0x%08x)", msg, error); \
				return error; \
			} \
		} while (0)


#define GEOFENCE_PRINT_ERROR_CODE(error, msg)	\
		do { \
			GEOFENCE_LOGE("%s(0x%08x)", msg, error); \
			return error; \
		} while (0)


#ifdef __cplusplus
extern "C" {
#endif


#define GEOFENCE_NULL_ARG_CHECK(arg)	\
	GEOFENCE_CHECK_CONDITION((arg != NULL),GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER,"GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER") \

int __print_error_code(int code);


#ifdef __cplusplus
}
#endif

#endif //__GEOFENCE_LOG_H__
