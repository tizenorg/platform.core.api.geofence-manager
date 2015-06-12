CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
#PROJECT(testgeo-lite-efl C CXX)

FIND_PROGRAM(UNAME NAMES uname)
EXEC_PROGRAM("${UNAME}" ARGS "-m" OUTPUT_VARIABLE "ARCH")
IF("${ARCH}" STREQUAL "armv7l")
	ADD_DEFINITIONS("-DTARGET")
	SET(CMAKE_C_FLAGS_RELEASE "-mabi=aapcs-linux -O2")
ENDIF("${ARCH}" STREQUAL "armv7l")


# Add your submodule directory name
#ADD_SUBDIRECTORY(WinMgr)
ADD_SUBDIRECTORY(UIMgr)

# install configuration file
CONFIGURE_FILE(com.samsung.testgeo-sample.xml ${CMAKE_BINARY_DIR}/${PKGNAME}.xml)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/com.samsung.testgeo-sample.png DESTINATION /usr/apps/icons/default/small)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/com.samsung.testgeo-sample.xml DESTINATION /usr/share/packages)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/com.samsung.testgeo-sample.rule DESTINATION /etc/smack/accesses2.d)
#INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.Flora DESTINATION /usr/share/license RENAME com.samsung.testgeo-sample)
