cmake_minimum_required(VERSION 2.8)
project(pihm)

add_subdirectory( pihm )
add_subdirectory( qgis_plugin )
if( WIN32 )
	add_subdirectory( triangle )
endif()

#INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "modified PIHM & QGis plug-in")
SET(CPACK_PACKAGE_VENDOR "mlt")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.markdown")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "2")
SET(CPACK_PACKAGE_VERSION_MINOR "3")
SET(CPACK_PACKAGE_VERSION_PATCH "2")
IF( WIN32 )
  set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} )
  set( CPACK_NSIS_INSTALL_ROOT "C:" )
  set( CPACK_OUTPUT_FILE_NAME "pihm_mod_2.3.2-qgis_1.7-win32.exe" )
  set( CPACK_TEMPORARY_PACKAGE_FILE_NAME ${CPACK_TOPLEVEL_DIRECTORY}${CPACK_OUTPUT_FILE_NAME} )
  SET(CPACK_PACKAGE_INSTALL_DIRECTORY "OSGeo4W")
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
#  SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
#  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
  SET(CPACK_NSIS_HELP_LINK "http://www.pihm.psu.edu")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http://www.pihm.psu.edu")
#  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE()
	set( CPACK_GENERATOR "DEB" )
#  SET(CPACK_STRIP_FILES "bin/MyExecutable")
#  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF()
#SET(CPACK_PACKAGE_EXECUTABLES "MyExecutable" "My Executable")

#set(CPACK_SOURCE_GENERATOR ZIP)
set(CPACK_SOURCE_IGNORE_FILES
"~$"
"\\\\.bak$"
"\\\\.o$"
"^\\\\."
"DS_Store"
"PIHM_v1.5/"
"\\\\.svn/"
"/build/"
"/vs2008/"
"\\\\.dbf$"
"\\\\.shp$"
"\\\\.shx$"
"\\\\.pro$"
"\\\\.app/"
)

set( CPACK_DEBIAN_PACKAGE_MAINTAINER "Mikhail Titov <mlt@gmx.us>" )
SET( CPACK_DEBIAN_PACKAGE_DEPENDS "libsundials-cvode1, libsundials-nvecserial0, libqgis1.7.0, libqwt5-qt4, triangle-bin" )

INCLUDE(CPack)
