cmake_minimum_required(VERSION 2.8)

project (Millenium-Cell)
set (VTK_DIR "/Users/Musky/Dev/VTK-6.3.0-build/")

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(Millenium-Cell  src/environment.cpp src/main.cpp)

 
IF(BOOST)
FIND_PACKAGE(Boost REQUIRED)
ENDIF(BOOST)


if(VTK_LIBRARIES)
  target_link_libraries(Millenium-Cell ${VTK_LIBRARIES})
else()
  target_link_libraries(Millenium-Cell vtkHybrid vtkWidgets)
endif()


set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )

# Include directories

INCLUDE_DIRECTORIES(
  /opt/local/include
  ${Boost_INCLUDE_DIRS}
  )

#add_executable(Millenium-Cell src/environment.cpp src/main.cpp)


# Boost and its components
find_package( Boost REQUIRED )

if ( NOT Boost_FOUND )

  message(STATUS "This project requires the Boost library, and will not be compiled.")

  return()  

endif()


