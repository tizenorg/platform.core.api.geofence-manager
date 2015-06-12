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


#ifndef __GEOFENCE_H__
#define __GEOFENCE_H__


#include <glib.h>
#include "geofence-type-private.h"

G_BEGIN_DECLS

/**
 * @file geofence.h
 * @brief This file contains the Geofence API and related structure and enumeration.
 */
/**
 * @defgroup GeofenceFW GeofenceFW
 * @brief This is a Geofence Framework for providing geofence services.
 * @addtogroup GeofenceFW
 * @{
 * @defgroup GeofenceAPI Geofence API
 * @brief This sub module provides the Geofence API.
 * @addtogroup GeofenceAPI
 * @{
 */

/**
 * @brief
 * Initialize geofence sub module.
 * @remarks None.
 * This API should be called before any other Geofence APIs.
 * @pre None.
 * @post None.
 * @return int
 * @retval 0                              Success
 *
 * @see None.
 */
int geofence_init (void);

/**
 * @brief
 * Create a new #GeofenceObject.
 * @remarks
 * Returned object is necessary for other APIs.
 * @pre
 * #geofence_init should be called before.
 * @post None.
 * @param [in]
 * method - Geofence method to be used.
 * @return a new #GeofenceObject
 * @retval NULL              if error occured
 * @see geofence_free
 */
GeofenceObject *geofence_new (void);

/**
 * @brief
 * Free memory of given #GeofenceObject.
 * @remarks None.
 * @pre
 * #geofence_init should be called before.
 * @post None.
 * @param [in]
 * obj - a #GeofenceObject created by #geofence_new.
 * @return int
 * @retval 0                              Success.
 *
 * @see geofence_new
 * @par Example
 * @code

 * @endcode
 */
int geofence_free (GeofenceObject *obj);

/**
 * @brief
 * Check wheither a method is available.
 * @remarks
 * @pre
 * #geofence_init should be called before.\n
 * @post None.
 * @param [in] method - a #GeofenceObject
 * @return int
 * @retval True		Supported
 *	   False 	Not supported
 * @par Example
 * @code
 * @endcode
 */
gboolean geofence_is_supported (void);

G_END_DECLS

#endif /* __GEOFENCE_H__ */

