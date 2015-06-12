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

#include <glib.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <app_manager.h>
#include <package_manager.h>
#include <pkgmgr-info.h>
#include <privacy_checker_client.h>

#include "geofence-log.h"
#include "geofence-type-private.h"

#define CAPP    1
#define WEBAPP  2
#define CPPAPP  3
#define LOCATION_PRIVILEGE	"http://tizen.org/privilege/location"

int
geofence_get_app_type(void)
{
	int ret = 0;
	pid_t pid = 0;
	char *app_id = NULL;
	app_info_h app_info;
	char *type = NULL;

	pid = getpid();
	ret = app_manager_get_app_id(pid, &app_id);
	if (ret != APP_MANAGER_ERROR_NONE) {
		GEOFENCE_LOGE("Fail to get app_id. Err[%d]", ret);
		return 0;
	}

	ret = app_info_create(app_id, &app_info);
	if (ret != APP_MANAGER_ERROR_NONE) {
		GEOFENCE_LOGE("Fail to get app_id. Err[%d]", ret);
		free(app_id);
		return 0;
	}

	ret = app_info_get_type(app_info, &type);
	if (ret != APP_MANAGER_ERROR_NONE) {
		GEOFENCE_LOGE("Fail to get type. Err[%d]", ret);
		free(app_id);
		app_info_destroy(app_info);
		return 0;
	}

	if (strcmp(type, "c++app") == 0)
		ret = CPPAPP;
	else if (strcmp(type, "webapp") == 0)
		ret = WEBAPP;
	else
		ret = CAPP;

	free(type);
	free(app_id);
	app_info_destroy(app_info);

	return ret;
}

void geofence_privacy_initialize()
{
	int ret = 0;
	pid_t pid = 0;
	char *app_id = NULL;
	char *package_id = NULL;
	pkgmgrinfo_appinfo_h pkgmgrinfo_appinfo;

	pid = getpid();
	ret = app_manager_get_app_id(pid, &app_id);
	if (ret != APP_MANAGER_ERROR_NONE) {
		GEOFENCE_LOGE("Fail to get app_id. Err[%d]", ret);
		return;
	}

	ret = pkgmgrinfo_appinfo_get_appinfo(app_id, &pkgmgrinfo_appinfo);
    if (ret != PACKAGE_MANAGER_ERROR_NONE) {
        GEOFENCE_LOGE("Fail to get appinfo for [%s]. Err[%d]", app_id, ret);
        free(app_id);
        return;
    }
    ret = pkgmgrinfo_appinfo_get_pkgname(pkgmgrinfo_appinfo, &package_id);
	if (ret != PACKAGE_MANAGER_ERROR_NONE) {
		GEOFENCE_LOGE("Fail to get package_id for [%s]. Err[%d]", app_id, ret);
		free(app_id);
		return;
	}

	ret = privacy_checker_initialize(package_id);
	if (ret != PRIV_MGR_ERROR_SUCCESS) {
		GEOFENCE_LOGE("Fail to initialize privacy checker. err[%d]", ret);
		free(package_id);
		free(app_id);
		return;
	}

	GEOFENCE_LOGD("Success to initialize privacy checker");

	free(app_id);
	free(package_id);
}

void geofence_privacy_finalize(void)
{
	int ret = 0;
	ret = privacy_checker_finalize();
	if (ret != PRIV_MGR_ERROR_SUCCESS) {
		GEOFENCE_LOGE("Fail to finalize privacy_cehecker. Err[%d]", ret);
		return;
	}

	GEOFENCE_LOGD("Success to finalize privacy checker");
}

int geofence_get_privacy(void)
{
	int ret = 0;
	pid_t pid = 0;
	char *app_id = NULL;
	char *package_id = NULL;

	pkgmgrinfo_appinfo_h pkgmgrinfo_appinfo;

	pid = getpid();
	ret = app_manager_get_app_id(pid, &app_id);
	if (ret != APP_MANAGER_ERROR_NONE) {
		GEOFENCE_LOGE("Fail to get app_id. Err[%d]", ret);
		return GEOFENCE_MANAGER_ERROR_NONE;
	}

	ret = pkgmgrinfo_appinfo_get_appinfo(app_id, &pkgmgrinfo_appinfo);
    if (ret != PACKAGE_MANAGER_ERROR_NONE) {
        GEOFENCE_LOGE("Fail to get appinfo for [%s]. Err[%d]", app_id, ret);
        free(app_id);
        return GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED;
    }
    ret = pkgmgrinfo_appinfo_get_pkgname(pkgmgrinfo_appinfo, &package_id);
	if (ret != PACKAGE_MANAGER_ERROR_NONE) {
		GEOFENCE_LOGE("Fail to get package_id for [%s]. Err[%d]", app_id, ret);
		free(app_id);
		return GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED;
	}
	ret = privacy_checker_check_package_by_privilege(package_id, LOCATION_PRIVILEGE);
	if (ret != PRIV_MGR_ERROR_SUCCESS) {
		GEOFENCE_LOGE("Fail to get privilege for [%s]. Err[%d]", package_id, ret);

		free(app_id);
		return GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED;
	}
	
	free(package_id);
	free(app_id);

	return GEOFENCE_MANAGER_ERROR_NONE;
}

