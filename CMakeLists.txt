cmake_minimum_required(VERSION 3.6)
project(ToplSkeleton DESCRIPTION "A skeleton for Topl")


if(NOT Rasteron_Install_Path)
    set(Rasteron_Install_Path "${CMAKE_INSTALL_PREFIX}/Rasteron" CACHE PATH "Rasteron install directory")
    set(Rasteron_Projects_Path "${CMAKE_INSTALL_PREFIX}/Rasteron" CACHE PATH "Rasteron external projects directory")
endif()

find_package(Rasteron PATHS ${Rasteron_Install_Path}/lib/Rasteron)
if(Rasteron_FOUND)
    include(${Rasteron_Install_Path}/lib/Rasteron/findDepends.cmake)

    message(STATUS "Rasteron loadding success!")
else()
    message(WARNING "Texturing and image modules cannot load, build Rasteron project and point Rasteron_Install_Path to its install directory")
endif()


include(ExternalProject)
set(EXTERNAL_PROJ_DIR "${CMAKE_BINARY_DIR}/Projects")

ExternalProject_Add(DXSDK
        GIT_REPOSITORY "https://github.com/Anton-Os/dxsdk.git"
        GIT_TAG "5fbd5e1158db72421bc32709136131451e6c8c37"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/dxsdk
        # SOURCE_DIR ${EXTERNAL_PROJ_DIR}/dxsdk
        # BINARY_DIR ${EXTERNAL_PROJ_DIR}/dxsdk
        # STAMP_DIR ${EXTERNAL_PROJ_DIR}/dxsdk/stamp
    )

ExternalProject_Add(GLEW
        GIT_REPOSITORY "https://github.com/Anton-Os/GLEW.git"
        GIT_TAG "6de325e4393213d703483e3acd52e8890fd4b191"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/GLEW
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


find_package(dxsdk_interface PATHS ${CMAKE_INSTALL_PREFIX}/lib/dxsdk)
find_package(Eigen3 PATHS ${CMAKE_INSTALL_PREFIX}/share/eigen3/cmake)
find_package(GLEW PATHS ${CMAKE_INSTALL_PREFIX}/lib/GLEW)



if(Rasteron_FOUND)

    add_executable(DemoImg
        demos/DemoImg.cpp
    )   
    target_link_libraries(DemoImg RasteronLib) # finding Rasteron but not tiff
endif()

add_executable(DemoTickTick  
    core/Timer.cpp
    demos/DemoTickTick.cpp
)
target_include_directories(DemoTickTick PUBLIC core helper geo)

add_executable(DemoPresser
    core/Timer.cpp
    core/Input.cpp
    demos/DemoPresser.cpp
)
target_include_directories(DemoPresser PUBLIC core helper geo)

add_executable(DemoDrx11
    core/FileIO.cpp
    core/Geometry.cpp
    core/Timer.cpp
    core/Topl_SceneManager.cpp
    core/Topl_Renderer_Drx11.cpp

    demos/DemoDrx11_Init.cpp
)
target_include_directories(DemoDrx11 PUBLIC core geo)

target_include_directories(DemoDrx11 PUBLIC 
    ${CMAKE_INSTALL_PREFIX}/include/dxsdk
    ${CMAKE_INSTALL_PREFIX}/include/eigen3
) # REMOVE THIS LATER
target_link_libraries(DemoDrx11 INTERFACE dxsdk_interface Eigen3) # FIX THIS

add_executable(DemoDrx11_Shapes
    core/FileIO.cpp
    core/Geometry.cpp
    core/Timer.cpp
    geo/CircleUp.cpp
    core/Topl_SceneManager.cpp
    core/Topl_Renderer_Drx11.cpp

    demos/DemoDrx11_Shapes.cpp
)
target_include_directories(DemoDrx11_Shapes PUBLIC core geo)

target_include_directories(DemoDrx11_Shapes PUBLIC 
    ${CMAKE_INSTALL_PREFIX}/include/dxsdk
    ${CMAKE_INSTALL_PREFIX}/include/eigen3
) # REMOVE THIS LATER
target_link_libraries(DemoDrx11_Shapes INTERFACE dxsdk_interface Eigen3) # FIX THIS

add_executable(DemoDrx11_MovingSprite
    core/FileIO.cpp
    core/Geometry.cpp
    core/Timer.cpp
    geo/Character1.cpp
    core/Topl_SceneManager.cpp
    core/Topl_Renderer_Drx11.cpp

    demos/DemoDrx11_MovingSprite.cpp
)
target_include_directories(DemoDrx11_MovingSprite PUBLIC core geo)

target_include_directories(DemoDrx11_MovingSprite PUBLIC 
    ${CMAKE_INSTALL_PREFIX}/include/dxsdk
    ${CMAKE_INSTALL_PREFIX}/include/eigen3
) # REMOVE THIS LATER
target_link_libraries(DemoDrx11_MovingSprite INTERFACE dxsdk_interface Eigen3) # FIX THIS
target_link_libraries(DemoDrx11_MovingSprite PRIVATE RasteronLib  GLEW_lib) # GLEW lib shouldnt be linked

add_executable(DemoDrx11_Capture
    core/FileIO.cpp
    core/Geometry.cpp
    core/Timer.cpp
    geo/Character1.cpp
    core/Topl_SceneManager.cpp
    core/Topl_Renderer_Drx11.cpp

    demos/DemoDrx11_Capture.cpp
)
target_include_directories(DemoDrx11_Capture PUBLIC core geo)

target_include_directories(DemoDrx11_Capture PUBLIC 
    ${CMAKE_INSTALL_PREFIX}/include/dxsdk
    ${CMAKE_INSTALL_PREFIX}/include/eigen3
) # REMOVE THIS LATER
target_link_libraries(DemoDrx11_Capture INTERFACE dxsdk_interface Eigen3) # FIX THIS
target_link_libraries(DemoDrx11_Capture PRIVATE RasteronLib  GLEW_lib) # GLEW lib shouldnt be linked
 



add_executable(DemoGL4
    core/FileIO.cpp
    core/Geometry.cpp
    core/Timer.cpp
    core/Topl_SceneManager.cpp
    core/Topl_Renderer_GL4.cpp
    
    demos/DemoGL4_Init.cpp 
)

target_include_directories(DemoGL4 PUBLIC core geo)
target_include_directories(DemoGL4 PUBLIC 
    ${CMAKE_INSTALL_PREFIX}/include/dxsdk # REMOVE THIS LATER
    ${CMAKE_INSTALL_PREFIX}/include/eigen3
)
target_link_libraries(DemoGL4 INTERFACE dxsdk_interface Eigen3) # REMOVE THIS LATER
target_link_libraries(DemoGL4 PRIVATE GLEW_lib)

add_executable(DemoGL4_Shapes
    core/FileIO.cpp
    core/Geometry.cpp
    core/Timer.cpp
    geo/CircleUp.cpp
    core/Topl_SceneManager.cpp
    core/Topl_Renderer_GL4.cpp
    
    demos/DemoGL4_Shapes.cpp 
)

target_include_directories(DemoGL4_Shapes PUBLIC core geo)
target_include_directories(DemoGL4_Shapes PUBLIC 
    ${CMAKE_INSTALL_PREFIX}/include/dxsdk # REMOVE THIS LATER
    ${CMAKE_INSTALL_PREFIX}/include/eigen3
)
target_link_libraries(DemoGL4_Shapes INTERFACE dxsdk_interface Eigen3) # REMOVE THIS LATER
target_link_libraries(DemoGL4_Shapes PRIVATE GLEW_lib)

add_executable(DemoGL4_MovingSprite
    core/FileIO.cpp
    core/Geometry.cpp
    core/Timer.cpp
    geo/Character1.cpp
    core/Topl_SceneManager.cpp
    core/Topl_Renderer_GL4.cpp
    
    demos/DemoGL4_MovingSprite.cpp 
)
target_include_directories(DemoGL4_MovingSprite PUBLIC core geo)

target_include_directories(DemoGL4_MovingSprite PUBLIC 
    ${CMAKE_INSTALL_PREFIX}/include/dxsdk # REMOVE THIS LATER
    ${CMAKE_INSTALL_PREFIX}/include/eigen3
)
target_link_libraries(DemoGL4_MovingSprite INTERFACE dxsdk_interface Eigen3) # FIX THIS
target_link_libraries(DemoGL4_MovingSprite PRIVATE RasteronLib GLEW_lib)

add_executable(DemoGL4_Capture
    core/FileIO.cpp
    core/Geometry.cpp
    core/Timer.cpp
    geo/Character1.cpp
    core/Topl_SceneManager.cpp
    core/Topl_Renderer_GL4.cpp
    
    demos/DemoGL4_Capture.cpp 
)
target_include_directories(DemoGL4_Capture PUBLIC core geo)

target_include_directories(DemoGL4_Capture PUBLIC 
    ${CMAKE_INSTALL_PREFIX}/include/dxsdk # REMOVE THIS LATER
    ${CMAKE_INSTALL_PREFIX}/include/eigen3
)
target_link_libraries(DemoGL4_Capture INTERFACE dxsdk_interface Eigen3) # FIX THIS
target_link_libraries(DemoGL4_Capture PRIVATE RasteronLib GLEW_lib)
 



file(GLOB drx11_Shaders ${CMAKE_SOURCE_DIR}/shaders/*.hlsl)
file(GLOB gl4_Shaders ${CMAKE_SOURCE_DIR}/shaders/*.glsl)
add_custom_target(ShaderCopy 
                  SOURCES ${drx11_Shaders} ${gl4_Shaders})
                  # Add GLSL shaders too 
if(WIN32)
    foreach(file IN ITEMS ${drx11_Shaders})
        add_custom_command(TARGET DemoDrx11 PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy 
            "${file}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/")
    endforeach()
endif()

foreach(file IN ITEMS ${gl4_Shaders})
    add_custom_command(TARGET DemoGL4 PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
        "${file}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/")
endforeach()


add_dependencies(DemoDrx11 ShaderCopy) # Repeat for all targets
add_dependencies(DemoGL4 ShaderCopy)