cmake_minimum_required(VERSION 2.8)

add_definitions( -DSUNDIALS_240=1 )

if( WIN32 )
	set( SUNDIALS_DIR "C:\\Progra~1\\sundials" )
else() # Should work for Ubuntu 11.04 with sudo. QGis doesn't work with symlinked plugin :( Give a try to QGIS_PLUGINPATH
	set ( SUNDIALS_LINUX_EXTRA lapack )
endif()

# set( CMAKE_BUILD_TYPE Release FORCE)

set( pihm_SRCS
	update.cpp
	f.cpp
	pihm.h
	pihm.cpp
	print.cpp
	initialize.cpp
	read_alloc.cpp
	is_sm_et.cpp
)

#include_directories( ${GDAL_INCLUDE_DIR} ) #${CMAKE_CURRENT_BINARY_DIR}

if( WIN32 )
	add_definitions( -D_CRT_SECURE_NO_WARNINGS=1 )
	link_directories( ${SUNDIALS_DIR}\\lib)
	include_directories( ${SUNDIALS_DIR}\\include )
else() # Works for Ubuntu 11.04
	link_directories( /usr/lib/lapack )
endif()

add_executable(pihm ${pihm_SRCS})
target_link_libraries(pihm
	sundials_cvode
	sundials_nvecserial
	${SUNDIALS_LINUX_EXTRA}
)

install(TARGETS pihm
RUNTIME DESTINATION "bin")
