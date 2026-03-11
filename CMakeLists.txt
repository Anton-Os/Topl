cmake_minimum_required(VERSION 3.7...3.10)
project(Topl DESCRIPTION "Topl Graphics Engine")

set (CMAKE_CXX_STANDARD 11)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/INSTALL" CACHE PATH "Install Path" FORCE)
    message("Install Prefix ${CMAKE_INSTALL_PREFIX}")
endif()

set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets/" )
set(SHADERS_DIR "${CMAKE_SOURCE_DIR}/shaders/" )
if(WIN32)
    set(RESOURCE_DIR "${ASSETS_DIR}/res" CACHE PATH "Path to directory with resource.h file")
    set(RESOURCE_FILE "${RESOURCE_DIR}/resource.rc" CACHE FILEPATH "Resource (.rc) file")
    # set(RESOURCE_ICON "${RESOURCE_DIR}/TOPL.ico" CACHE FILEPATH "Icon (.ico) file")
endif()

set(SUPPORT_TEXTURES TRUE CACHE BOOL "Include texturing")
set(SUPPORT_PHYSICS TRUE CACHE BOOL "Include physics calculations")
set(SUPPORT_MODELS TRUE CACHE BOOL "Include 3D model loading")
# set(SUPPORT_AUDIO ON CACHE BOOL "Include audio support")
# set(SUPPORT_SAVES ON CACHE BOOL "Include custom saves")

# ------------- Libraries, Engines, and Procedures ------------- #

if(ANDROID OR APPLE)
    set(IGNORE_EXTLIBS TRUE)
    set(IGNORE_NATIVELIBS TRUE)
    set(IGNORE_DEMOS TRUE)
    message("Ignoring additional targets for Android and Apple targets")
endif()

if(NOT DEFINED IGNORE_EXTLIBS)
    set(EXTERNAL_PROJ_DIR "${CMAKE_BINARY_DIR}/PROJECTS")
    include("cmake/externalTargets.cmake") # For building dependencies
endif()

include("cmake/internalTargets.cmake") # Builds CORELIB and GEOLIB

if(NOT DEFINED IGNORE_NATIVELIBS)
    include("cmake/findVulkan.cmake") # For locating Vulkan
    include("cmake/engineTargets.cmake") # For building engine specific libraries
    include("cmake/shaderTargets.cmake") # For copying and compiling shaders
    include("cmake/resourceTargets.cmake") # For generating assets compile-time
endif()

include("cmake/installTargets.cmake") # For installation

# ------------- Demos and Tests ------------- #

configure_file("${CMAKE_SOURCE_DIR}/support/support_def.h.in" "${CMAKE_SOURCE_DIR}/support/support_def.h")

if(NOT DEFINED IGNORE_DEMOS)
add_executable(Hello demos/hello/Hello.cpp)
add_executable(Molecular demos/molecular/Molecular.cpp)
add_executable(Meshform demos/meshform/Meshform.cpp)
add_executable(Penscape demos/penscape/Penscape.cpp)
add_executable(Kaleidoscope demos/kaleidoscope/Kaleidoscope.cpp)
add_executable(Animatrix demos/animatrix/Animatrix.cpp)
add_executable(Genesis demos/genesis/Genesis.cpp)
add_executable(Traversal demos/traversal/Traversal.cpp)
add_executable(Cubegate demos/cubegate/Cubegate.cpp)

list(APPEND demo_targets Hello) # Sandbox Molecular Meshform Penscape Kaleidoscope Animatrix Genesis)
list(APPEND demo_targets Molecular Meshform Penscape Kaleidoscope Animatrix Genesis Traversal Cubegate)

foreach(target IN ITEMS ${demo_targets})
    target_include_directories(${target} PRIVATE core support geometry shaders)
    target_link_libraries(${target} PRIVATE CORELIB)
    if(WIN32) # adding resource file to demos
        target_include_directories(${target} PRIVATE "${RESOURCE_DIR}")
        target_sources(${target} PRIVATE "${RESOURCE_FILE}")
    endif()
endforeach()
endif()
