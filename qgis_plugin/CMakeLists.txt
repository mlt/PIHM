cmake_minimum_required(VERSION 2.8)

if( WIN32 )
	set( OSGEO_DIR "C:\\OSGeo4W" )
	set( ENV{GDAL_ROOT} ${OSGEO_DIR} )
	set( ENV{QTDIR} ${OSGEO_DIR} )
endif()

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

find_package(GDAL REQUIRED)
find_package(Qt4 REQUIRED COMPONENTS Qt3Support QtCore QtGui)
include(${QT_USE_FILE})

QT4_WRAP_UI (pihmgis_UIS_H  ${pihmgis_UIS})

QT4_WRAP_CPP (pihmgis_MOC_SRCS  ${pihmgis_MOC_HDRS})

QT4_ADD_RESOURCES(pihmgis_RCC_SRCS ${pihmgis_RCCS})

add_definitions( -DGUI_EXPORT= -DCORE_EXPORT= )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} ${GDAL_INCLUDE_DIR} )

if( WIN32 )
	add_definitions( -D_CRT_SECURE_NO_WARNINGS=1 )
	link_directories(${OSGEO_DIR}\\lib ${OSGEO_DIR}\\apps\\qgis\\lib )
	include_directories(
     ${OSGEO_DIR}\\apps\\qgis\\include
     ${OSGEO_DIR}\\include\\qwt
     ${OSGEO_DIR}\\include
	)
	set ( QWT_LIB "qwt5" )
	set ( LIBSHP "shapelib_i" )
else() # Works for Ubuntu 11.04
	include_directories( /usr/include/qgis /usr/include/qwt-qt4 )
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

if( WIN32 )
	#set( CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP true )
	set( CMAKE_INSTALL_DEBUG_LIBRARIES true )
	install(TARGETS pihmgisplugin
	LIBRARY DESTINATION "apps\\qgis\\plugins")
else() # GNU/Linux
	install(TARGETS pihmgisplugin
	LIBRARY DESTINATION "lib/qgis/plugins")
endif()


