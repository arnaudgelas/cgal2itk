cmake_minimum_required( VERSION 2.8 )

project( itk2cgal )

find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )

find_package( CGAL REQUIRED )
include( ${CGAL_USE_FILE} )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

include( CTest )

if( BUILD_TESTING )
  add_subdirectory( tests )
endif()
