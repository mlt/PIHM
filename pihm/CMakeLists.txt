cmake_minimum_required(VERSION 2.8)

add_definitions( -DSUNDIALS_240=1 )


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

# Will make no harm on non-windows platforms.
# link_directories shall come before add_executable but extra dependency after
set( SUNDIALS_DIR "${CMAKE_BINARY_DIR}/sundials" )
link_directories( ${SUNDIALS_DIR}/lib)

add_executable(pihm ${pihm_SRCS})

if( WIN32 )
  include(ExternalProject)
  ExternalProject_Add(sundials
    URL "http://computation.llnl.gov/casc/sundials/download/code/sundials-2.5.0.tar.gz"
    # PREFIX "${CMAKE_CURRENT_BINARY_DIR}/sundials"
    # INSTALL_DIR "${SUNDIALS_DIR}"
    CMAKE_CACHE_ARGS "-DCMAKE_INSTALL_PREFIX:PATH=${SUNDIALS_DIR}"
    # INSTALL_COMMAND ""
    )
  add_dependencies(pihm sundials)

	add_definitions( -D_CRT_SECURE_NO_WARNINGS=1 )
	include_directories( ${SUNDIALS_DIR}\\include )
else() # Works for Ubuntu 11.04
	link_directories( /usr/lib/lapack )
	set ( SUNDIALS_LINUX_EXTRA lapack )
endif()

target_link_libraries(pihm
	sundials_cvode
	sundials_nvecserial
	${SUNDIALS_LINUX_EXTRA}
)

install(TARGETS pihm
COMPONENT pihm
RUNTIME DESTINATION "bin")
