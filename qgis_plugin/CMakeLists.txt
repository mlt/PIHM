cmake_minimum_required(VERSION 2.8)

if( WIN32 )
# Get OSGeo4W with dev packages for QGis, Qt & Qwt. Get VC++ 2008 Express Edition
# Start command line for VC++ 2008 Express
# > cd to source code
# > mkdir build
# > cd build
# > "\Program Files\CMake 2.8\bin\cmake.exe" ..
# > nmake
# > nmake install
  set( OSGEO_DIR "C:\\OSGeo4W" )
	set( ENV{GDAL_ROOT} ${OSGEO_DIR} )
	set( ENV{QTDIR} ${OSGEO_DIR} )
	set( SUNDIALS_DIR "C:\\Progra~1\\sundials" )
	set( CMAKE_INSTALL_PREFIX "${OSGEO_DIR}\\apps\\qgis\\plugins" CACHE PATH "" FORCE )
endif()

# set( CMAKE_BUILD_TYPE Release FORCE)

project(pihmgisplugin)

########################################################
# Files

SET (pihmgis_SRCS
     pihmgis.cpp
     pihmgisgui.cpp
	Project/DefineProject/defineproject.cpp
	Project/OpenProject/openproject.cpp
	Project/CloseProject/closeproject.cpp
	Project/ImportProject/importproject.cpp
	RasterProcessing/RunAllRaster/runallraster.cpp
	pihmLIBS/helpDialog/helpdialog.cpp
	pihmLIBS/fileStruct.h
	pihmLIBS/fileStruct.cpp
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
	pihmLIBS/simplifyPolyShp.h
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
	DomainDecomposition/RunTriangle/triangle.cpp
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
	MyNewThread.cpp
	RunPIHM/runpihm.cpp
	pihmLIBS/generateShape.h
	pihmLIBS/generateShape.cpp
	Analysis/TimeSeries/plotTS.cpp
	Analysis/TimeSeries/timeseries.cpp
	Analysis/Spatialplot/spatialplot.cpp
	Help/About/about.cpp
)

SET (pihmgis_UIS pihmgisgui.ui
	Project/DefineProject/defineproject.ui
	Project/OpenProject/openproject.ui
	Project/CloseProject/closeproject.ui
	Project/ImportProject/importproject.ui
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
     pihmgisgui.h
	Project/DefineProject/defineproject.h
	Project/OpenProject/openproject.h
	Project/CloseProject/closeproject.h
	Project/ImportProject/importproject.h
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

add_definitions( -DGUI_EXPORT= -DCORE_EXPORT= -DSUNDIALS_240=1 )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

if( WIN32 )
	add_definitions( -DNO_TIMER=1 -D_CRT_SECURE_NO_WARNINGS=1 )
	link_directories(${OSGEO_DIR}\\lib ${OSGEO_DIR}\\apps\\qgis\\lib ${SUNDIALS_DIR}\\lib)
	include_directories(
     ${OSGEO_DIR}\\apps\\qgis\\include
     ${OSGEO_DIR}\\include\\qwt
     ${OSGEO_DIR}\\include
     ${SUNDIALS_DIR}\\include
	)
endif()

ADD_LIBRARY (pihmgisplugin MODULE ${pihmgis_SRCS} ${pihmgis_MOC_SRCS} ${pihmgis_RCC_SRCS} ${pihmgis_UIS_H})

target_link_libraries(pihmgisplugin
  ${GDAL_LIBRARY}
  qgis_core
  qwt5
  ${QT_LIBRARIES}
	sundials_cvode
	sundials_nvecserial
)

#  -static-libgcc
#  -L/C/OSGeo4W/lib
#	-L/usr/local/lib

########################################################
# Install

install(TARGETS pihmgisplugin LIBRARY DESTINATION ".")

#INSTALL(TARGETS pihmgisplugin
#  RUNTIME DESTINATION ${QGIS_PLUGIN_DIR}
#  LIBRARY DESTINATION ${QGIS_PLUGIN_DIR})

