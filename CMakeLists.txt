cmake_minimum_required(VERSION 3.6)
project(ToplSkeleton DESCRIPTION "A skeleton for Topl")

include(ExternalProject)

#include(Rasteron/InitProject.cmake) # need to fix Rasteron 
#add_subdirectory(Rasteron) # need to fix Rasteron

set(EXTERNAL_PROJ_DIR "${CMAKE_BINARY_DIR}/Projects")

if(FALSE) # need to fix Rasteron
    ExternalProject_Add(Rasteron
        GIT_REPOSITORY "https://github.com/Anton-Os/Rasteron.git"
        GIT_TAG "3045e0303332f122905a6014fac83688204c7f87" # Not important

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        DOWNLOAD_COMMAND "" 

        PREFIX ${CMAKE_SOURCE_DIR}/Rasteron
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/Rasteron
        BINARY_DIR ${CMAKE_BINARY_DIR}/Rasteron
    )
    find_package(Rasteron)
endif()

add_library(Topl STATIC core/Topl_Renderer_Drx12.cpp)

target_include_directories(Topl PUBLIC core helper)
# target_link_libraries(Topl Rasteron) # need to fix Rasteron