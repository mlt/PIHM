cmake_minimum_required(VERSION 2.8)

# set( CMAKE_BUILD_TYPE Release FORCE)

#add_definitions( -DSUNDIALS_240=1 )

project(pihm)

########################################################
# Files

SET ( pihm_SRCS f initialize is_sm_et pihm print read_alloc update )

########################################################
# Build

if ( WIN32 )
	if( MSVC )
		set( SUNDIALS_DIR "C:\\Progra~1\\sundials" )
		add_definitions( -D_CRT_SECURE_NO_WARNINGS=1 )
		link_directories( ${SUNDIALS_DIR}\\lib )
		include_directories( ${SUNDIALS_DIR}\\include )
	else() # MinGW
		link_directories( /usr/local/lib )
		include_directories( /usr/local/include )
	endif()
endif() # WIN32

add_executable( pihm ${pihm_SRCS} )

target_link_libraries(pihm
	sundials_cvode
	sundials_nvecserial
)

########################################################
# Install

