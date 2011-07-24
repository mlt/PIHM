cmake_minimum_required(VERSION 2.8)

add_definitions( -DSUNDIALS_240=1 )

if( WIN32 )
# Get OSGeo4W with dev packages for QGis, Qt & Qwt. Get VC++ 2008 Express Edition & CMake
# Start command line for VC++ 2008 Express
# > cd to source code
# > mkdir build
# > cd build
# > "\Program Files\CMake 2.8\bin\cmake.exe" ..
# > nmake
# > nmake install
# Alternatively (especially for debug)
# > mkdir vs2008
# > cd vs2008
# > "\Program Files\CMake 2.8\bin\cmake.exe" -G "Visual Studio 9 2008" ..
  set( OSGEO_DIR "C:\\OSGeo4W" )
	set( ENV{GDAL_ROOT} ${OSGEO_DIR} )
	set( ENV{QTDIR} ${OSGEO_DIR} )
	set( SUNDIALS_DIR "C:\\Progra~1\\sundials" )
#	set( CMAKE_INSTALL_PREFIX "${OSGEO_DIR}" CACHE PATH "" FORCE )
	#	set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "apps\\qgis\\plugins" )
	#	set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin" )
else() # Should work for Ubuntu 11.04 with sudo. QGis doesn't work with symlinked plugin :( Give a try to QGIS_PLUGINPATH
# > sudo apt-get install libqgis-dev libqwt5-qt4-dev libsundials-serial-dev triangle-bin liblapack-dev libshp-dev
	set( CMAKE_INSTALL_PREFIX "/usr/lib/qgis" CACHE PATH "" FORCE )
	#	set( SUNDIALS_LINUX_EXTRA gslcblas )
	#	find_package(LAPACK REQUIRED) # broken and requires fortran compiler
	#	message(${LAPACK_LIBRARIES})
	set ( SUNDIALS_LINUX_EXTRA lapack )
endif()

# set( CMAKE_BUILD_TYPE Release FORCE)

project(pihm)

########################################################
# Files

set( pihm_SRCS
	#PIHM_v2.0/calib.h
	#PIHM_v2.0/calib.cpp
	#PIHM_v2.0/f.h
	PIHM_v2.0/update.cpp
	PIHM_v2.0/f.cpp
	PIHM_v2.0/pihm.h
	PIHM_v2.0/pihm.cpp
	#PIHM_v2.0/print_functions.h
	PIHM_v2.0/print.cpp
	#PIHM_v2.0/initialize.h
	PIHM_v2.0/initialize.cpp
	#PIHM_v2.0/read_alloc.h
	PIHM_v2.0/read_alloc.cpp
	#PIHM_v2.0/et_is.h
	PIHM_v2.0/is_sm_et.cpp
)

SET (pihmgis_SRCS
	pihmgis.cpp
	Project/DefineProject/defineproject.cpp
	RasterProcessing/RunAllRaster/runallraster.cpp
	pihmLIBS/helpDialog/helpdialog.cpp
	pihmRasterLIBS/bin2ascii.h
	pihmRasterLIBS/bin2ascii.cpp
	pihmRasterLIBS/avexec32.h
	pihmRasterLIBS/avcalls_null.cpp
	pihmRasterLIBS/gioapi.h
	pihmRasterLIBS/gridio.h
	pihmRasterLIBS/gridio_null.cpp
	pihmRasterLIBS/lsm.h
	pihmRasterLIBS/lsmcom.cpp
	pihmRasterLIBS/callocate.cpp
	pihmRasterLIBS/gridio.cpp
	pihmRasterLIBS/gridio.h
	pihmRasterLIBS/flood.h
	pihmRasterLIBS/flood.cpp
	pihmRasterLIBS/setdir.h
	pihmRasterLIBS/setdir.cpp
	pihmRasterLIBS/aread8.h
	pihmRasterLIBS/aread8.cpp
	pihmRasterLIBS/streamDefinition.h
	pihmRasterLIBS/streamDefinition.cpp
	pihmRasterLIBS/streamSegmentation.h
	pihmRasterLIBS/streamSegmentation.cpp
	pihmRasterLIBS/streamSegmentationShp.h
	pihmRasterLIBS/streamSegmentationShp.cpp
	pihmRasterLIBS/catProcessing.h
	pihmRasterLIBS/catProcessing.cpp
	pihmRasterLIBS/catPoly.h
	pihmRasterLIBS/catPoly.cpp
	RasterProcessing/FillPits/fillpits.cpp	
	RasterProcessing/FlowGrid/flowgrid.cpp
	RasterProcessing/StreamGrid/streamgrid.cpp
	RasterProcessing/LinkGrid/linkgrid.cpp
	RasterProcessing/StreamPolyLine/streampolyline.cpp
	RasterProcessing/CatchmentGrid/catchmentgrid.cpp
	RasterProcessing/CatchmentPolygon/catchmentpolygon.cpp
	pihmLIBS/polygonToPolyline.h
	pihmLIBS/simplifyPolySHP.h
	pihmLIBS/splitLineAtVertices.h
	pihmLIBS/mergeFeatures.h
	VectorProcessing/Dissolve/dissolve.cpp
	VectorProcessing/PolygonToPolyLine/polygontopolylinedialog.cpp
	VectorProcessing/SimplifyLine/simplifylinedialog.cpp
	VectorProcessing/SplitLine/splitlinedialog.cpp
	VectorProcessing/MergeFeatures/mergefeaturesdialog.cpp
	pihmLIBS/generatePolyFile.cpp
	pihmLIBS/createTinShapeFile.cpp
	DomainDecomposition/RunAllDomain/runnalldomain.cpp
	DomainDecomposition/GenerateShapeTopology/generateshapetopology.cpp
	DomainDecomposition/RunTriangle/runtriangle.cpp
	DomainDecomposition/CreateTINs/createtins.cpp
	pihmLIBS/pickGridValue.h
	pihmLIBS/pickGridValue.cpp
	pihmLIBS/addFID.h
	pihmLIBS/addFID.cpp
	pihmLIBS/addToFromNode.h
	pihmLIBS/addToFromNode.cpp
	pihmLIBS/addSOrder.h
	pihmLIBS/addSOrder.cpp
	pihmLIBS/calDownSegment.h
	pihmLIBS/calDownSegment.cpp
	pihmLIBS/Point.h
	pihmLIBS/Point.cpp
	pihmLIBS/extractRiver4mTIN.h
	pihmLIBS/extractRiver4mTIN.cpp
	DataModelLoader/MeshFile/mshfile.cpp
	DataModelLoader/AttFile/attfile.cpp
	DataModelLoader/RivFile/rivfile.cpp
	DataModelLoader/ParaFile/parafile.cpp
	DataModelLoader/SoilFile/soilfile.cpp
	DataModelLoader/GeolFile/geolfile.cpp
	DataModelLoader/LCFile/lcfile.cpp
	DataModelLoader/CalibFile/calibfile.cpp
	DataModelLoader/InitFile/initfile.cpp
	DataModelLoader/IbcFile/ibcfile.cpp
	RunPIHM/runpihm.cpp
	pihmLIBS/generateShape.h
	pihmLIBS/generateShape.cpp
	Analysis/TimeSeries/plotTS.cpp
	Analysis/TimeSeries/timeseries.cpp
	Analysis/SpatialPlot/spatialplot.cpp
	Help/About/about.cpp
)

SET (pihmgis_UIS
	Project/DefineProject/defineproject.ui
	RasterProcessing/RunAllRaster/runallraster.ui
	pihmLIBS/helpDialog/helpdialog.ui
	RasterProcessing/FillPits/fillpits.ui
	RasterProcessing/FlowGrid/flowgrid.ui
	RasterProcessing/StreamGrid/streamgrid.ui
	RasterProcessing/LinkGrid/linkgrid.ui
	RasterProcessing/StreamPolyLine/streampolyline.ui
	RasterProcessing/CatchmentGrid/catchmentgrid.ui
	RasterProcessing/CatchmentPolygon/catchmentpolygon.ui
	VectorProcessing/Dissolve/dissolve.ui
	VectorProcessing/PolygonToPolyLine/polygontopolylinedialog.ui
	VectorProcessing/SimplifyLine/simplifylinedialog.ui
	VectorProcessing/SplitLine/splitlinedialog.ui
	VectorProcessing/MergeFeatures/mergefeaturesdialog.ui
	DomainDecomposition/RunAllDomain/runnalldomain.ui
	DomainDecomposition/GenerateShapeTopology/generateshapetopology.ui
	DomainDecomposition/RunTriangle/runtriangle.ui
	DomainDecomposition/CreateTINs/createtinsdialog.ui
	DataModelLoader/MeshFile/mshfile.ui
	DataModelLoader/AttFile/attfile.ui
	DataModelLoader/RivFile/rivfile.ui
	DataModelLoader/ParaFile/parafile.ui
	DataModelLoader/SoilFile/soilfile.ui
	DataModelLoader/GeolFile/geolfile.ui
	DataModelLoader/LCFile/lcfile.ui
	DataModelLoader/CalibFile/calibfile.ui
	DataModelLoader/InitFile/initfile.ui
	DataModelLoader/IbcFile/ibcfile.ui
	RunPIHM/runpihm.ui
	Analysis/TimeSeries/timeseries.ui
	Analysis/SpatialPlot/spatialplot.ui
	Help/About/about.ui
)

SET (pihmgis_MOC_HDRS
	pihmgis.h
	Project/DefineProject/defineproject.h
	RasterProcessing/RunAllRaster/runallraster.h
	pihmLIBS/helpDialog/helpdialog.h
	RasterProcessing/FillPits/fillpits.h
	RasterProcessing/FlowGrid/flowgrid.h
	RasterProcessing/StreamGrid/streamgrid.h
	RasterProcessing/LinkGrid/linkgrid.h
	RasterProcessing/StreamPolyLine/streampolyline.h
	RasterProcessing/CatchmentGrid/catchmentgrid.h
	RasterProcessing/CatchmentPolygon/catchmentpolygon.h
	VectorProcessing/Dissolve/dissolve.h
	VectorProcessing/PolygonToPolyLine/polygontopolylinedialog.h
	VectorProcessing/SimplifyLine/simplifylinedialog.h
	VectorProcessing/SplitLine/splitlinedialog.h
	VectorProcessing/MergeFeatures/mergefeaturesdialog.h
	DomainDecomposition/RunAllDomain/runnalldomain.h
	DomainDecomposition/GenerateShapeTopology/generateshapetopology.h
	DomainDecomposition/RunTriangle/runtriangle.h
	DomainDecomposition/CreateTINs/createtins.h
	DataModelLoader/MeshFile/mshfile.h
	DataModelLoader/AttFile/attfile.h
	DataModelLoader/RivFile/rivfile.h
	DataModelLoader/ParaFile/parafile.h
	DataModelLoader/SoilFile/soilfile.h
	DataModelLoader/GeolFile/geolfile.h
	DataModelLoader/LCFile/lcfile.h
	DataModelLoader/CalibFile/calibfile.h
	DataModelLoader/InitFile/initfile.h
	DataModelLoader/IbcFile/ibcfile.h
	RunPIHM/runpihm.h
	Analysis/TimeSeries/plotTS.h
	Analysis/TimeSeries/timeseries.h
	Analysis/SpatialPlot/spatialplot.h
	Help/About/about.h
)


SET (pihmgis_RCCS  pihmgis.qrc
	VectorProcessing/PolygonToPolyLine/polygontopolylinedialog.qrc
)

########################################################
# Build

find_package(GDAL REQUIRED)
find_package(Qt4 REQUIRED COMPONENTS Qt3Support QtCore QtGui)
include(${QT_USE_FILE})

QT4_WRAP_UI (pihmgis_UIS_H  ${pihmgis_UIS})

QT4_WRAP_CPP (pihmgis_MOC_SRCS  ${pihmgis_MOC_HDRS})

QT4_ADD_RESOURCES(pihmgis_RCC_SRCS ${pihmgis_RCCS})

add_definitions( -DGUI_EXPORT= -DCORE_EXPORT= )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} ${GDAL_INCLUDE_DIR} )

if( WIN32 )
	add_definitions( -DNO_TIMER=1 -D_CRT_SECURE_NO_WARNINGS=1 )
	link_directories(${OSGEO_DIR}\\lib ${OSGEO_DIR}\\apps\\qgis\\lib ${SUNDIALS_DIR}\\lib)
	include_directories(
     ${OSGEO_DIR}\\apps\\qgis\\include
     ${OSGEO_DIR}\\include\\qwt
     ${OSGEO_DIR}\\include
     ${SUNDIALS_DIR}\\include
	)
	set ( QWT_LIB "qwt5" )
	set ( LIBSHP "shapelib_i" )
else() # Works for Ubuntu 11.04
	include_directories( /usr/include/qgis /usr/include/qwt-qt4 )
	link_directories( /usr/lib/lapack )
	set ( QWT_LIB "qwt-qt4" )
	set ( LIBSHP "shp" )
endif()

ADD_LIBRARY (pihmgisplugin MODULE ${pihmgis_SRCS} ${pihmgis_MOC_SRCS} ${pihmgis_RCC_SRCS} ${pihmgis_UIS_H})
# debug only
#set(LIBRARY_OUTPUT_PATH /usr/lib/qgis/plugins) 

target_link_libraries(pihmgisplugin
	${GDAL_LIBRARY}
	qgis_core
	${QWT_LIB}
	${QT_LIBRARIES}
	${LIBSHP}
	#	sundials_cvode
	#	sundials_nvecserial
	#	${SUNDIALS_LINUX_EXTRA}
)

# Standalone
add_executable(pihm ${pihm_SRCS})
set_target_properties(pihm
	PROPERTIES COMPILE_DEFINITIONS NOQT=1
)
target_link_libraries(pihm
	sundials_cvode
	sundials_nvecserial
	${SUNDIALS_LINUX_EXTRA}
)

########################################################
# Install


if( WIN32 )
#set( CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP true )
set( CMAKE_INSTALL_DEBUG_LIBRARIES true )
install(TARGETS pihm pihmgisplugin
LIBRARY DESTINATION "apps\\qgis\\plugins"
RUNTIME DESTINATION "bin")
else()
install(TARGETS pihm pihmgisplugin
LIBRARY DESTINATION "lib/qgis/plugins"
RUNTIME DESTINATION "bin")
endif()

#INSTALL(TARGETS pihmgisplugin
#  RUNTIME DESTINATION ${QGIS_PLUGIN_DIR}
#  LIBRARY DESTINATION ${QGIS_PLUGIN_DIR})

INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "modified PIHM & QGis plug-in")
SET(CPACK_PACKAGE_VENDOR "mlt")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "2")
SET(CPACK_PACKAGE_VERSION_MINOR "3")
SET(CPACK_PACKAGE_VERSION_PATCH "1")
#set( CMAKE_INSTALL_PREFIX "apps\\qgis\\plugins" CACHE PATH "" FORCE )
#SET(CPACK_SET_DESTDIR ON)
#SET(CPACK_PACKAGE_INSTALL_DIRECTORY "C:\\\\OSGeo4W\\\\apps\\\\qgis\\\\plugins")
IF( WIN32 )
  set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} )
  set( CPACK_NSIS_INSTALL_ROOT "C:" )
  set( CPACK_OUTPUT_FILE_NAME "pihm_mod_2.3.1-qgis_1.7-win32.exe" )
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
