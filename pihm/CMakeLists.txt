cmake_minimum_required(VERSION 2.8)

# set( CMAKE_BUILD_TYPE Release FORCE)

if( WIN32 )
	set( SUNDIALS_DIR "C:\\Progra~1\\sundials" )
	add_definitions( -DSUNDIALS_240=1 )
endif()

project(pihm)

########################################################
# Files

SET ( pihm_SRCS f initialize is_sm_et pihm print read_alloc update )

########################################################
# Build

if( WIN32 )
	add_definitions( -DNO_TIMER=1 -D_CRT_SECURE_NO_WARNINGS=1 )
	link_directories( ${SUNDIALS_DIR}\\lib )
	include_directories( ${SUNDIALS_DIR}\\include )
endif()

add_executable( pihm ${pihm_SRCS} )

target_link_libraries(pihm
	sundials_cvode
	sundials_nvecserial
)

########################################################
# Install

