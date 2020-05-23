cmake_minimum_required(VERSION 3.6)
project(ToplSkeleton DESCRIPTION "A skeleton for Topl")

include(ExternalProject)

if(NOT Rasteron_Install_Path)
    set(Rasteron_Install_Path "${CMAKE_INSTALL_PREFIX}/Rasteron" CACHE PATH "Rasteron install directory")
    set(Rasteron_Projects_Path "${CMAKE_INSTALL_PREFIX}/Rasteron" CACHE PATH "Rasteron external projects directory")
endif()

set(EXTERNAL_PROJ_DIR "${CMAKE_BINARY_DIR}/Projects")

# NOT SURE THIS IS A FIX TO BUILD DURING CONFIG TIME
execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/Rasteron
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Rasteron
)

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

ExternalProject_Add(Eigen
        GIT_REPOSITORY "https://github.com/eigenteam/eigen-git-mirror.git"
        GIT_TAG "36b95962756c1fce8e29b1f8bc45967f30773c00"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/Eigen
        # SOURCE_DIR ${EXTERNAL_PROJ_DIR}/Eigen
        # BINARY_DIR ${EXTERNAL_PROJ_DIR}/Eigen
        # STAMP_DIR ${EXTERNAL_PROJ_DIR}/Eigen
    )

# add_library(Topl STATIC core/Topl_Renderer_Drx12.cpp)
#target_include_directories(Topl PUBLIC core helper) # for local dependencies
# target_link_libraries(Topl Rasteron) # need to fix Rasteron

add_executable(DemoTickTick  
    core/Timer.cpp
    demos/DemoTickTick.cpp
)
target_include_directories(DemoTickTick PUBLIC core helper)

add_executable(DemoPresser
    core/Timer.cpp
    core/Input.cpp
    demos/DemoPresser.cpp
)
target_include_directories(DemoPresser PUBLIC core helper)

find_package(dxsdk_interface PATHS ${CMAKE_INSTALL_PREFIX}/lib/dxsdk)
find_package(Eigen3 PATHS ${CMAKE_INSTALL_PREFIX}/share/eigen3/cmake)

add_executable(DemoDrx11 
    core/Geometry.cpp
    core/Topl_SceneGraph.cpp
    core/Topl_Renderer_Drx11.cpp

    demos/DemoDrx11_Init.cpp
)
target_include_directories(DemoDrx11 PUBLIC core helper)

target_include_directories(DemoDrx11 PUBLIC 
    ${CMAKE_INSTALL_PREFIX}/include/dxsdk
    ${CMAKE_INSTALL_PREFIX}/include/eigen3
) # REMOVE THIS LATER
target_link_libraries(DemoDrx11 INTERFACE dxsdk_interface Eigen3) # FIX THIS

add_executable(DemoDrx11_Shapes
    core/Geometry.cpp
    core/Geometry/RandShapes.cpp
    core/Physics.cpp
    core/Topl_SceneGraph.cpp
    core/Topl_Renderer_Drx11.cpp

    demos/DemoDrx11_Shapes.cpp
)
target_include_directories(DemoDrx11_Shapes PUBLIC core helper)

target_include_directories(DemoDrx11_Shapes PUBLIC 
    ${CMAKE_INSTALL_PREFIX}/include/dxsdk
    ${CMAKE_INSTALL_PREFIX}/include/eigen3
) # REMOVE THIS LATER
target_link_libraries(DemoDrx11_Shapes INTERFACE dxsdk_interface Eigen3) # FIX THIS




add_executable(DemoGL4 
    core/Geometry.cpp
    
    demos/DemoGL4_Init.cpp 
    core/Topl_Renderer_GL4.cpp
)

target_include_directories(DemoGL4 PUBLIC core helper)
target_include_directories(DemoGL4 PUBLIC 
    ${CMAKE_INSTALL_PREFIX}/include/dxsdk
    ${CMAKE_INSTALL_PREFIX}/include/eigen3
) # REMOVE THIS LATER
target_link_libraries(DemoGL4 INTERFACE dxsdk_interface Eigen3) # REMOVE THIS LATER



file(GLOB drx11_Shaders ${CMAKE_SOURCE_DIR}/shaders/*.hlsl)
add_custom_target(ShaderCopy 
                  SOURCES ${drx11_Shaders} )
                  # Add GLSL shaders too 
if(WIN32)
    foreach(file IN ITEMS ${drx11_Shaders})
        add_custom_command(TARGET DemoDrx11 PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy 
            "${file}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/")
    endforeach()
endif()

add_dependencies(DemoDrx11 ShaderCopy) # Repeat for all targets