
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(fw_name "capi-geofence-manager")

PROJECT(${fw_name})

SET(CMAKE_INSTALL_PREFIX /usr)
SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(LIBDIR ${LIBDIR})

SET(INC_DIR
    include
    src
    src/manager
    src/module
    )
INCLUDE_DIRECTORIES(${INC_DIR})

SET(dependents "dlog vconf glib-2.0 gmodule-2.0 capi-appfw-app-manager capi-appfw-package-manager pkgmgr-info privacy-manager-client")
SET(pc_dependents "capi-base-common")

IF(ENABLE_GEOFENCE)
	ADD_DEFINITIONS(-DENABLE_GEOFENCE)
ENDIF(ENABLE_GEOFENCE)

INCLUDE(FindPkgConfig)
pkg_check_modules(${fw_name} REQUIRED ${dependents})
FOREACH(flag ${${fw_name}_CFLAGS})
    SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fvisibility=hidden -fPIC -Wall -Werror -fprofile-arcs -ftest-coverage")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

IF("${ARCH}" STREQUAL "arm")
    ADD_DEFINITIONS("-DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
ADD_DEFINITIONS("-DEXPORT_API=__attribute__((visibility(\"default\")))")
ADD_DEFINITIONS("-DTIZEN_DEBUG")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -Wl,--rpath=${LIBDIR}")

# Source Lists
SET(API_SRCS
    src/geofence_manager.c
)

SET(MODULE_SRCS
    src/module/module-internal.c
)

aux_source_directory(src/manager MANAGER_SRCS)

ADD_LIBRARY(${fw_name} SHARED ${API_SRCS} ${MANAGER_SRCS} ${MODULE_SRCS})

IF (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/manager/geofence-marshal.c)
    MESSAGE ("")
    MESSAGE ("<<< Generating geofence-marshal.c >>>")

    EXECUTE_PROCESS(
        COMMAND glib-genmarshal --header --prefix=geofence ${CMAKE_CURRENT_SOURCE_DIR}/src/manager/geofence-marshal.list
        OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/manager/geofence-marshal.h
    )

    EXECUTE_PROCESS(
        COMMAND glib-genmarshal --body --prefix=geofence ${CMAKE_CURRENT_SOURCE_DIR}/src/manager/geofence-marshal.list
        OUTPUT_VARIABLE marshal_var
    )

    EXECUTE_PROCESS(
        COMMAND echo "#include \"geofence-marshal.h\" ${marshal_var}"
        OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/manager/geofence-marshal.c
    )

ENDIF (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/manager/geofence-marshal.c)

SET_TARGET_PROPERTIES(${fw_name}
     PROPERTIES
     VERSION ${FULLVER}
     SOVERSION ${MAJORVER}
     CLEAN_DIRECT_OUTPUT 1
)

TARGET_LINK_LIBRARIES(${fw_name} ${${fw_name}_LDFLAGS})

INSTALL(TARGETS ${fw_name} DESTINATION ${LIBDIR})
INSTALL(DIRECTORY include/ DESTINATION include/geofence
        PATTERN "*.h")

SET(PC_NAME ${fw_name})
SET(PC_REQUIRED ${pc_dependents})
SET(PC_LDFLAGS -l${fw_name})
SET(PC_CFLAGS -I\${includedir}/geofence)

CONFIGURE_FILE(
    ${fw_name}.pc.in
    ${CMAKE_CURRENT_SOURCE_DIR}/${fw_name}.pc
    @ONLY
)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${fw_name}.pc DESTINATION ${LIBDIR}/pkgconfig)

# Test Suite
#IF("${BINTYPE}" STREQUAL "eng")
#	ADD_SUBDIRECTORY(test)
#ENDIF("${BINTYPE}" STREQUAL "eng")

IF(UNIX)

ADD_CUSTOM_TARGET (distclean @echo cleaning for source distribution)
ADD_CUSTOM_COMMAND(
        DEPENDS clean
        COMMENT "distribution clean"
        COMMAND find
        ARGS    .
        -not -name config.cmake -and \(
        -name tester.c -or
        -name Testing -or
        -name CMakeFiles -or
        -name cmake.depends -or
        -name cmake.check_depends -or
        -name CMakeCache.txt -or
        -name cmake.check_cache -or
        -name *.cmake -or
        -name Makefile -or
        -name core -or
        -name core.* -or
        -name gmon.out -or
        -name install_manifest.txt -or
        -name *.pc -or
        -name *~ \)
        | grep -v TC | xargs rm -rf
        TARGET  distclean
        VERBATIM
)

ENDIF(UNIX)

ADD_SUBDIRECTORY(test/Test-App-Sample)
