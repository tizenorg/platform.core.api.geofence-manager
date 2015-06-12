SET(EDJDIR "/usr/apps/com.samsung.testgeo-sample/res/edje")
SET(IMGDIR "/usr/apps/com.samsung.testgeo-sample/res/images")
SET(DATADIR "/usr/apps/com.samsung.testgeo-sample/res/data")

ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_BINARY_DIR}/homescreen.edj
	COMMAND edje_cc -no-save -id ${CMAKE_CURRENT_SOURCE_DIR}/images
	${CMAKE_CURRENT_SOURCE_DIR}/homescreen.edc
	${CMAKE_BINARY_DIR}/homescreen.edj
	oDEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/homescreen.edc
)

ADD_CUSTOM_TARGET(testgeo_homescreen_edj_build DEPENDS ${CMAKE_BINARY_DIR}/homescreen.edj)

ADD_DEPENDENCIES(${PROJECT_NAME} testgeo_homescreen_edj_build)

ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_BINARY_DIR}/testgeo-sample-efl.edj
	COMMAND edje_cc 
	${CMAKE_CURRENT_SOURCE_DIR}/layouts.edc
	${CMAKE_BINARY_DIR}/testgeo-sample-efl.edj
	oDEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/testgeo-sample-efl.edc
)

# install edj
ADD_CUSTOM_TARGET(testgeo_sample_efl_edj_build DEPENDS ${CMAKE_BINARY_DIR}/testgeo-sample-efl.edj)
ADD_DEPENDENCIES(${PROJECT_NAME} testgeo_sample_efl_edj_build)

INSTALL(FILES ${CMAKE_BINARY_DIR}/testgeo-sample-efl.edj DESTINATION ${EDJDIR})
INSTALL(FILES ${CMAKE_BINARY_DIR}/homescreen.edj DESTINATION ${EDJDIR})
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/images/ DESTINATION ${IMGDIR} FILES_MATCHING PATTERN "*.png")
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/images/ DESTINATION ${IMGDIR} FILES_MATCHING PATTERN "*.jpg")
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/images/ DESTINATION ${IMGDIR} FILES_MATCHING PATTERN "*.jpeg")
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/ DESTINATION ${DATADIR} FILES_MATCHING PATTERN "*") 
