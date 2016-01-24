cmake_minimum_required(VERSION 2.8)

project (Millenium-Cell)

set(BOOST_ROOT_DIR "C:/prog/Boost" CACHE PATH "")
set(BOOST_ROOT_DIR ${BOOST_ROOT_DIR} CACHE PATH "Root directory of Boost" FORCE)
include_directories(${BOOST_ROOT_DIR})
set(BOOST_ROOT ${BOOST_ROOT_DIR})

# Boost and its components
find_package( Boost REQUIRED system serialization)
if ( NOT Boost_FOUND )
  message(STATUS "This project requires the Boost library, and will not be compiled.")
  return()  
endif()

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(Millenium-Cell src/GraphManager.cpp src/main.cpp src/Graphics.cpp src/environment.cpp )

if(VTK_LIBRARIES)
  target_link_libraries(Millenium-Cell ${VTK_LIBRARIES})
else()
  target_link_libraries(Millenium-Cell vtkHybrid vtkWidgets)
endif()

if(Boost_LIBRARIES)
  TARGET_LINK_LIBRARIES(Millenium-Cell ${Boost_LIBRARIES})
endif()

 #Add "tags" target and make my_project depending on this target.
set_source_files_properties(tags PROPERTIES GENERATED true)
add_custom_target(tags
    COMMAND ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
  add_dependencies(Millenium-Cell tags)

add_custom_target(run
  COMMAND Millenium-Cell
  DEPENDS Millenium-Cell
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
