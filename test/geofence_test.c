/*
 * Copyright (c) 2012-2013 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <assert.h>
#include <geofence_manager.h>

#define PRT(format, args...) printf("%s:%d() "format, __FUNCTION__, __LINE__, ##args)
#define TC_PRT(format, args...) PRT(format"\n", ##args)

geofence_manager_h pGeofenceManager;
int fence_id;
int fence_id_fake;

GMainLoop *main_loop = NULL;

gboolean test_thread(GIOChannel *source, GIOCondition condition, gpointer data);

static const char *__test_convert_error_to_string(geofence_manager_error_e err_type)
{
	switch (err_type) {
	case GEOFENCE_MANAGER_ERROR_NONE:
		return "NONE";
	case GEOFENCE_MANAGER_ERROR_NOT_INITIALIZED:
		return "NOT INITIALIZED";
	case GEOFENCE_MANAGER_ERROR_INVALID_PARAMETER:
		return "ILLEGAL ARGUMENT";
	case GEOFENCE_MANAGER_ERROR_INVALID_ID:
		return "ID DOES NOT EXIST";
	case GEOFENCE_MANAGER_ERROR_EXCEPTION:
		return "EXCEPTION";
	case GEOFENCE_MANAGER_ERROR_ALREADY_STARTED:
		return "ALREADY STARTED";
	case GEOFENCE_MANAGER_ERROR_TOO_MANY_GEOFENCE:
		return "TOO MANY GEOFENCE";
	case GEOFENCE_MANAGER_ERROR_OUT_OF_MEMORY:
		return "Out of memory";
	case GEOFENCE_MANAGER_ERROR_PERMISSION_DENIED:
		return "permission denied";
	case GEOFENCE_MANAGER_ERROR_NOT_SUPPORTED:
		return "Not supported";
	}

	return "UNKNOWN";
}

bool __geofence_manager_fence_cb(int geofence_id, geofence_params_h params, int index, int fence_count, void *user_data)
{
	/* Request a state of the geofence */
	geofence_status_h status;
	const int error = geofence_manager_get_fence_status((geofence_manager_h)user_data, geofence_id, &status);
	if (error != GEOFENCE_MANAGER_ERROR_NONE) {
		TC_PRT("geofence_manager_get_fence_status for fence %d FAILED: %d\n", geofence_id, error);
	} else {
		geofence_state_e state;
		geofence_status_get_state(status, &state);

		int seconds;
		geofence_status_get_duration(status, &seconds);

		TC_PRT("\tfence_id: %d, state: %d, duration: %d sec\n", geofence_id, (int)state, seconds);
		switch (state) {
			case GEOFENCE_STATE_UNCERTAIN:
			TC_PRT("\t\tUncertain state of geofence\n");
			break;

			case GEOFENCE_STATE_IN:
			TC_PRT("\t\tGeofence In state\n");
			break;

			case GEOFENCE_STATE_OUT:
			TC_PRT("\t\tGeofence Out state\n");
			break;

			default:
			TC_PRT("\t\tUnknown state\n");
			break;
		}

		geofence_manager_destroy_fence_status(status);
	}

	return true;
}

static void __geofence_state_changed_cb(int geofence_id, geofence_state_e state, void *user_data)
{
	if (state == GEOFENCE_STATE_IN)
		TC_PRT("GEOFENCE TEST Fence Id : %d, (IN)\n", geofence_id);
	else if (state == GEOFENCE_STATE_OUT)
		TC_PRT("GEOFENCE TEST Fence Id : %d, (OUT)\n", geofence_id);
	else
		TC_PRT("GEOFENCE TEST Fence Id : %d, (UNKNOWN)\n", geofence_id);
}


int main(int argc, char **argv)
{
	main_loop = g_main_loop_new(NULL, FALSE);

	const char *bssid = argv[1];

	if (bssid) {
		geofence_params_h param;

		int rv = geofence_manager_create(&pGeofenceManager);

		if (rv == GEOFENCE_MANAGER_ERROR_NONE) {
			rv = geofence_manager_set_geofence_state_changed_cb(pGeofenceManager, __geofence_state_changed_cb, (void *)pGeofenceManager);
			if (rv == GEOFENCE_MANAGER_ERROR_NONE) {
				TC_PRT("state change callback set succeeded\n");
			}
		}
		TC_PRT("Bssid to be tracked is [%s]\n", bssid);
		rv = geofence_manager_create_wifi_params("wifi fence", bssid, &param);
		/* rv = geofence_manager_create_geopoint_params(12.9667, 77.5667, 200, &param); */
		TC_PRT("create wifi param... [ERROR - %s]\n", __test_convert_error_to_string(rv));
		rv = geofence_manager_add(pGeofenceManager, param, &fence_id);
		TC_PRT("Add geofence... [ERROR - %s]\n", __test_convert_error_to_string(rv));
		rv = geofence_manager_start(pGeofenceManager, fence_id);

		TC_PRT("Start geofence tracking... [ERROR - %s]\n", __test_convert_error_to_string(rv));
		geofence_manager_destroy_params(param);
	} else {
		TC_PRT("Try again with valid BSSID");
	}

	TC_PRT("ENTER  - Show options menu.......\n");
	GIOChannel *channel = g_io_channel_unix_new(0);
	g_io_add_watch(channel, (G_IO_IN|G_IO_ERR|G_IO_HUP|G_IO_NVAL), test_thread, NULL);
	g_main_loop_run(main_loop);

	return 0;
}

gboolean test_thread(GIOChannel *source, GIOCondition condition, gpointer data)
{
	int rv;
	char a[10];

	printf("Event received from stdin\n");

	rv = read(0, a, 10);

	if (rv <= 0 || a[0] == '0') {
		printf("Exiting the test application\n");
		geofence_manager_stop(pGeofenceManager, fence_id);
		geofence_manager_remove(pGeofenceManager, fence_id);
		geofence_manager_destroy(pGeofenceManager);

		exit(1);
	} if (a[0] == '1') {
		printf("Add fake wifi geofence\n");

		geofence_params_h param;
		rv = geofence_manager_create_wifi_params("wifi fence", "c0:ff:ee:c0:ff:ee", &param);
		TC_PRT("create wifi param... [ERROR - %s]\n", __test_convert_error_to_string(rv));
		rv = geofence_manager_add(pGeofenceManager, param, &fence_id_fake);
		TC_PRT("Add geofence... [ERROR - %s]\n", __test_convert_error_to_string(rv));
		rv = geofence_manager_start(pGeofenceManager, fence_id_fake);

		TC_PRT("Start geofence tracking... [ERROR - %s]\n", __test_convert_error_to_string(rv));
		geofence_manager_destroy_params(param);
	} if (a[0] == '2') {
		printf("Remove fake wifi geofence\n");

		geofence_manager_stop(pGeofenceManager, fence_id_fake);
		geofence_manager_remove(pGeofenceManager, fence_id_fake);
	} if (a[0] == '3') {
		printf("Request fence list and status\n");

		/* Request a list of registered geofences */
		const int error = geofence_manager_foreach_fence_list(
			(geofence_manager_h)pGeofenceManager, __geofence_manager_fence_cb, (void *)pGeofenceManager);
		if (error != GEOFENCE_MANAGER_ERROR_NONE) {
			TC_PRT("GEOFENCE TESTgeofence_manager_get_fence_list_for_each FAILED: %d\n", error);
		}
	} else {
		if (a[0] != '\n') {
			printf("Not a valid input\n");
		}
	}

	if (a[0] == '\n' || a[0] == '\r') {
		TC_PRT("Enter '0' to exit (No response is assumed to continue):  ");
		TC_PRT("Enter '1' to add fake fence:  ");
		TC_PRT("Enter '2' to remove fake fence:  ");
		TC_PRT("Enter '3' to request fence list and status:  ");
	}


	return TRUE;
}
