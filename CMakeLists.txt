cmake_minimum_required(VERSION 2.8)

project (Viab-Cell2)
set (VTK_DIR "/Users/Musky/Dev/VTK-6.3.0-build/")

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(Viab-Cell2  src/environment.cpp src/main.cpp)


if(VTK_LIBRARIES)
  target_link_libraries(Viab-Cell2 ${VTK_LIBRARIES})
else()
  target_link_libraries(Viab-Cell2 vtkHybrid vtkWidgets)
endif()



set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )
 
IF(BOOST)
FIND_PACKAGE(Boost REQUIRED)
ENDIF(BOOST)

# Include directories

INCLUDE_DIRECTORIES(
  /opt/local/include
  ${Boost_INCLUDE_DIRS}
  )

#add_executable(Viab-Cell2 src/environment.cpp src/main.cpp)


# Boost and its components
find_package( Boost REQUIRED )

if ( NOT Boost_FOUND )

  message(STATUS "This project requires the Boost library, and will not be compiled.")

  return()  

endif()


