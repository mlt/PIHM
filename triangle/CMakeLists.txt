cmake_minimum_required(VERSION 2.8)

include(ExternalProject)
ExternalProject_Add(triangle-ext
  URL "http://www.netlib.org/voronoi/triangle.zip"
  PREFIX "${CMAKE_BINARY_DIR}/triangle"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  BUILD_IN_SOURCE 1
  )

add_definitions( -DNO_TIMER -D_CRT_SECURE_NO_WARNINGS=1 )
if( CMAKE_SIZEOF_VOID_P EQUAL 4 )
  add_definitions( -DCPU86 )
endif()

add_executable( triangle ${CMAKE_CURRENT_BINARY_DIR}/src/triangle-ext/triangle.c )
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/src/triangle-ext/triangle.c GENERATED)

install(TARGETS triangle
COMPONENT triangle
RUNTIME DESTINATION "bin")
