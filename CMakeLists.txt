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
    set(RESOURCE_FILE "${CMAKE_SOURCE_DIR}/support/resource.rc" CACHE FILEPATH "Resource (.rc) file" FORCE)
    # set(RESOURCE_ICON "${RESOURCE_DIR}/TOPL.ico" CACHE FILEPATH "Icon (.ico) file")
endif()

set(SUPPORT_TEXTURES TRUE CACHE BOOL "Include texturing")
set(SUPPORT_PHYSICS TRUE CACHE BOOL "Include physics calculations")
set(SUPPORT_MODELS TRUE CACHE BOOL "Include 3D model loading")
set(SUPPORT_AUDIO FALSE CACHE BOOL "Include audio support")
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
    include("scripts/externalTargets.cmake") # For building dependencies
endif()

include("scripts/internalTargets.cmake") # Builds CORELIB and GEOLIB

if(NOT DEFINED IGNORE_NATIVELIBS)
    include("scripts/findVulkan.cmake") # For locating Vulkan
    include("scripts/engineTargets.cmake") # For building engine specific libraries
    include("scripts/shaderTargets.cmake") # For copying and compiling shaders
    include("scripts/resourceTargets.cmake") # For generating assets compile-time
endif()

include("scripts/installTargets.cmake") # For installation

# ------------- Demos and Tests ------------- #

configure_file("${CMAKE_SOURCE_DIR}/support/support_def.h.in" "${CMAKE_SOURCE_DIR}/support/support_def.h")

if(NOT DEFINED IGNORE_DEMOS)
include("scripts/runtimeTargets.cmake") # For building runtime libraries
include("scripts/demoTargets.cmake") # For building demos
endif()
