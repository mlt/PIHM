cmake_minimum_required(VERSION 2.8)

if( WIN32 )
	add_definitions( -DNO_TIMER -DCPU86 -D_CRT_SECURE_NO_WARNINGS=1 )
endif()

add_executable( triangle triangle.c )

install(TARGETS triangle
RUNTIME DESTINATION "bin")
