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

#include "geofence_type.h"
#include "geofence-log.h"

/*
* Internal Implementation
*/

int __print_error_code(int code)
{
	int ret = GEOFENCE_MANAGER_ERROR_NONE;
	char *msg = "GEOFENCE_MANAGER_ERROR_NONE";
	switch (code) {
	case GEOFENCE_MANAGER_ERROR_NONE:
		ret = GEOFENCE_MANAGER_ERROR_NONE;
		msg = "GEOFENCE_MANAGER_ERROR_NONE";
		break;
    	case GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED:
        	ret = GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED;
        	msg = "GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED";
        	break;
	case GEOFENCE_MANAGER_ERROR_NOT_INITIALIZED:
		ret = GEOFENCE_MANAGER_ERROR_NOT_INITIALIZED;
		msg = "GEOFENCE_MANAGER_ERROR_NOT_INITIALIZED";
		break;
	case GEOFENCE_MANAGER_ERROR_INVALID_ID:
		ret = GEOFENCE_MANAGER_ERROR_INVALID_ID;
		msg = "GEOFENCE_MANAGER_ERROR_INVALID_ID";
		break;
	case GEOFENCE_MANAGER_ERROR_EXCEPTION:
		ret = GEOFENCE_MANAGER_ERROR_EXCEPTION;
		msg = "GEOFENCE_MANAGER_ERROR_EXCEPTION";
		break;
	case GEOFENCE_MANAGER_ERROR_ALREADY_STARTED:
		ret = GEOFENCE_MANAGER_ERROR_ALREADY_STARTED;
		msg = "GEOFENCE_MANAGER_ERROR_ALREADY_STARTED";
		break;
	case GEOFENCE_MANAGER_ERROR_TOO_MANY_GEOFENCE:
		ret = GEOFENCE_MANAGER_ERROR_TOO_MANY_GEOFENCE;
		msg = "GEOFENCE_MANAGER_ERROR_TOO_MANY_GEOFENCE";
		break;
	case GEOFENCE_MANAGER_ERROR_IPC:
		ret = GEOFENCE_MANAGER_ERROR_IPC;
		msg = "GEOFENCE_MANAGER_ERROR_IPC";
		break;
	case GEOFENCE_MANAGER_ERROR_DATABASE:
		ret = GEOFENCE_MANAGER_ERROR_DATABASE;
		msg = "GEOFENCE_MANAGER_ERROR_DATABASE";
		break;
	case GEOFENCE_MANAGER_ERROR_PLACE_ACCESS_DENIED:
		ret = GEOFENCE_MANAGER_ERROR_PLACE_ACCESS_DENIED;
		msg = "GEOFENCE_MANAGER_ERROR_PLACE_ACCESS_DENIED";
		break;
	case GEOFENCE_MANAGER_ERROR_GEOFENCE_ACCESS_DENIED:
		ret = GEOFENCE_MANAGER_ERROR_GEOFENCE_ACCESS_DENIED;
		msg = "GEOFENCE_MANAGER_ERROR_GEOFENCE_ACCESS_DENIED";
		break;
	default:
		msg = "GEOFENCE_MANAGER_ERROR_NOT_INITIALIZED";
		ret = GEOFENCE_MANAGER_ERROR_NOT_INITIALIZED;
	}
	GEOFENCE_LOGE("ERROR : %s(0x%08x)", msg, ret);
	return ret;
}
