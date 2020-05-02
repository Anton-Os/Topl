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

ExternalProject_Add(DXSDK
        GIT_REPOSITORY "https://github.com/Anton-Os/dxsdk.git"
        GIT_TAG "5fbd5e1158db72421bc32709136131451e6c8c37"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/dxsdk
        # SOURCE_DIR ${EXTERNAL_PROJ_DIR}/dxsdk
        # BINARY_DIR ${EXTERNAL_PROJ_DIR}/dxsdk
        # STAMP_DIR ${EXTERNAL_PROJ_DIR}/dxsdk/stamp
    )

# add_library(Topl STATIC core/Topl_Renderer_Drx12.cpp)
#target_include_directories(Topl PUBLIC core helper) # for local dependencies
# target_link_libraries(Topl Rasteron) # need to fix Rasteron

add_executable(DemoTickTick  demos/DemoTickTick.cpp)
target_include_directories(DemoTickTick PUBLIC core helper)

add_executable(DemoDrx11 
    core/Geometry.cpp

    demos/DemoDrx11_Init.cpp
    core/Topl_Renderer_Drx11.cpp
)

target_include_directories(DemoDrx11 PUBLIC core helper)

find_package(dxsdk_interface PATHS ${CMAKE_INSTALL_PREFIX}/lib/dxsdk)
target_include_directories(DemoDrx11 PUBLIC ${CMAKE_INSTALL_PREFIX}/include/dxsdk) # UPDATE THIS
target_link_libraries(DemoDrx11 INTERFACE dxsdk_interface) # FIX THIS

add_executable(DemoGL4 
    core/Geometry.cpp
    
    demos/DemoGL4_Init.cpp 
    core/Topl_Renderer_GL4.cpp
)

target_include_directories(DemoGL4 PUBLIC core helper)
target_include_directories(DemoGL4 PUBLIC ${CMAKE_INSTALL_PREFIX}/include/dxsdk) # REMOVE THIS LATER
target_link_libraries(DemoGL4 INTERFACE dxsdk_interface) # REMOVE THIS LATER

if(WIN32)
    file(GLOB drx11_Shaders ${CMAKE_SOURCE_DIR}/shaders/*.hlsl)

    foreach(file IN ITEMS ${drx11_Shaders})
        add_custom_command(TARGET DemoDrx11 PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy 
            "${file}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/")
    endforeach()
endif()