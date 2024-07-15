cmake_minimum_required(VERSION 3.7)
project(Topl DESCRIPTION "Topl Graphics Engine")

set (CMAKE_CXX_STANDARD 11)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/INSTALL" CACHE PATH "Install Path" FORCE)
    message("Install Prefix ${CMAKE_INSTALL_PREFIX}")
endif()

set(EXTERNAL_PROJ_DIR "${CMAKE_BINARY_DIR}/Projects")

set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets/" )
set(SHADERS_DIR "${CMAKE_SOURCE_DIR}/shaders/" )
if(WIN32)
    set(RESOURCE_DIR "${ASSETS_DIR}/res" CACHE PATH "Path to directory with resource.h file")
    set(RESOURCE_FILE "${RESOURCE_DIR}/resource.rc" CACHE FILEPATH "Resource (.rc) file")
    # set(RESOURCE_ICON "${RESOURCE_DIR}/TOPL.ico" CACHE FILEPATH "Icon (.ico) file")
endif()

set(SUPPORT_TEXTURES TRUE CACHE BOOL "Include texturing" FORCE)
set(SUPPORT_PHYSICS TRUE CACHE BOOL "Include physics calculations" FORCE)
set(SUPPORT_MODELS ON CACHE BOOL "Include 3D model loading" FORCE)
# set(SUPPORT_AUDIO ON CAHCE BOOL "Include audio support" FORCE)
# set(SUPPORT_SAVES ON CACHE BOOL "Include custom saves" FORCE)

# ------------- Libraries, Engines, and Procedures ------------- #

include("cmake/externalTargets.cmake") # For building dependencies
include("cmake/findVulkan.cmake") # For locating Vulkan

if(UNIX AND NOT APPLE) # Unix Specific
    include(FindX11)
endif()

include("cmake/internalTargets.cmake") # For building core and geometry libraries
include("cmake/engineTargets.cmake") # For building engine specific libraries
include("cmake/shaderTargets.cmake") # For copying and compiling shaders
include ("cmake/resourceTargets.cmake") # For generating assets compile-time
include("cmake/installTargets.cmake") # For installation procedures

# ------------- Demos and Tests ------------- #

add_executable(Hello demos/Hello.cpp)
add_executable(Playground demos/Playground.cpp)
add_executable(FirstPerson demos/FirstPerson.cpp)
add_executable(MathArt demos/MathArt.cpp)
add_executable(Entropy demos/Entropy.cpp)

list(APPEND demo_targets Hello Playground FirstPerson MathArt Entropy)

foreach(target IN ITEMS ${demo_targets})
    target_include_directories(${target} PRIVATE core support geometry shaders)
    target_link_libraries(${target} PRIVATE CORELIB)
    if(WIN32) # adding resource file to demos
        target_include_directories(${target} PRIVATE "${RESOURCE_DIR}")
        target_sources(${target} PRIVATE "${RESOURCE_FILE}")
    endif()
endforeach()