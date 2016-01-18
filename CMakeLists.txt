cmake_minimum_required(VERSION 2.8)

project (Millenium-Cell)

set(BOOST_ROOT_DIR "C:/prog/Boost" CACHE PATH "")
set(BOOST_ROOT_DIR ${BOOST_ROOT_DIR} CACHE PATH "Root directory of Boost" FORCE)
include_directories(${BOOST_ROOT_DIR})
set(BOOST_ROOT ${BOOST_ROOT_DIR})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(Millenium-Cell  src/Graphics.cpp src/environment.cpp src/main.cpp)

#set(needed_features
    #cxx_thread_local)
  #target_compile_features(Millenium-Cell PRIVATE ${needed_features})

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

# Add "tags" target and make my_project depending on this target.
set_source_files_properties(tags PROPERTIES GENERATED true)
add_custom_target(tags
    COMMAND ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
  add_dependencies(Millenium-Cell tags)

