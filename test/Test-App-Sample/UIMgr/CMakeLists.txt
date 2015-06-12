CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(testgeo-sample C CXX)

SET( SRCS 
	main.cpp
	MapsApp.cpp
	MapsAppData.cpp
	MapsScenario1View.cpp
	MapsGeopointParamsView.cpp
	MapsAddPlace.cpp
	MapsRemovePlace.cpp
	MapsUpdatePlace.cpp

	#WinMgr
	WinMgr/WApp.cpp
	WinMgr/WNaviframe.cpp
	WinMgr/WPageView.cpp
	WinMgr/WUg.cpp
	WinMgr/WUgWindow.cpp
	WinMgr/WView.cpp
	WinMgr/WWindow.cpp
	WinMgr/WControl.cpp
)

SET(PKGNAME, "com.samsung.testgeo-sample")

SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
SET(CMAKE_INSTALL_RPATH "${CMAKE_APP_LIB_DIR};/usr/apps/com.samsung.testgeo-sample/lib")

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF()
MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
INCLUDE_DIRECTORIES(
	WinMgr/include
	${CMAKE_SOURCE_DIR}/UIMgr
)

INCLUDE(FindPkgConfig)
pkg_check_modules(lib_pkgs REQUIRED
	appcore-efl
       	capi-appfw-application
       	ui-gadget-1
       	efl-assist
)

FOREACH(flag ${lib_pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC -Wall -fdata-sections -ffunction-sections -Wl,--gc-sections")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")
SET(CMAKE_C_FLAGS_RELEASE "-O2")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CFLAGS} -Wall -g -fpie -fPIC -std=c++0x ${GC_SECTIONS_FLAGS}")

IF("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	ADD_DEFINITIONS("-DCLD_DEBUG")
	ADD_DEFINITIONS("-DCLD_FILE_LOGGING")
ENDIF()

FIND_PROGRAM(UNAME NAMES uname)
EXEC_PROGRAM("${UNAME}" ARGS "-m" OUTPUT_VARIABLE "ARCH")
IF("${ARCH}" STREQUAL "arm")
	ADD_DEFINITIONS("-DTARGET")
	MESSAGE(STATUS "add -DTARGET")
ENDIF()


ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} capi-geofence-manager ${lib_pkgs_LDFLAGS} "-pie -lm" )
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION /usr/apps/com.samsung.testgeo-sample/bin)

ADD_SUBDIRECTORY(res)
