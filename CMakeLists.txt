cmake_minimum_required(VERSION 3.6)
project(ToplSkeleton DESCRIPTION "A skeleton for Topl")

# Extrenal Libraries

if(NOT Rasteron_Install_Path)
    set(Rasteron_Install_Path "${CMAKE_INSTALL_PREFIX}/Rasteron" CACHE PATH "Rasteron install directory")
    # set(Rasteron_Projects_Path "${CMAKE_INSTALL_PREFIX}/Rasteron" CACHE PATH "Rasteron external projects directory")
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
<<<<<<< refs/remotes/origin/linux_port
    add_executable(Img demos/Img.cpp)
=======
    add_executable(Img
        demos/Demo_Img.cpp
    )
>>>>>>> local
    target_link_libraries(Img RasteronLib) # finding Rasteron but not tiff
endif()


list(APPEND common_Sources
	core/FileIO.cpp
	core/Input.cpp
<<<<<<< refs/remotes/origin/linux_port
  core/Timer.cpp
=======
    core/Timer.cpp
    core/Platform.cpp
    # core/ValueGen.cpp
>>>>>>> local
)

list(APPEND geo_Sources
    geo/primitives/Geo_Rect2D.cpp
    geo/primitives/Geo_Box3D.cpp
    geo/primitives/Geo_NGon2D.cpp
)

list(APPEND scene_Sources
    scene/Topl_Scene_Builder.cpp
    scene/Topl_Scene_Physics.cpp
)

# Common Feature Demos

add_executable(TickTick
    ${common_Sources}
    demos/Demo_TickTick.cpp
)
target_include_directories(TickTick PUBLIC core helper geo)
if(UNIX)
    target_link_libraries(TickTick PUBLIC X11 GL) # Links UNIX OS libs
endif()

add_executable(Presser
    ${common_Sources}
    demos/Demo_Presser.cpp
)
target_include_directories(Presser PUBLIC core helper geo)
if(UNIX)
    target_link_libraries(Presser PUBLIC X11 GL) # Links UNIX OS libs
endif()

add_executable(Cursor
    ${common_Sources}
    demos/Demo_Cursor.cpp
)
target_include_directories(Cursor PUBLIC core helper geo)
if(UNIX)
    target_link_libraries(Cursor PUBLIC X11 GL) # Links UNIX OS libs
endif()

add_executable(ParseFBX
    ${common_Sources}
    demos/Demo_ParseFBX.cpp
)
target_include_directories(ParseFBX PUBLIC core helper geo)

if(WIN32)

    # DirectX Rendering Engine

    add_library(Drx11_Engine
        ${common_Sources}
<<<<<<< refs/remotes/origin/linux_port
		${geo_Sources}
=======
    	${geo_Sources}
        ${scene_Sources}
        core/ValueGen.cpp
>>>>>>> local

        core/Topl_Renderer_Drx11.cpp
    )
    target_include_directories(Drx11_Engine PUBLIC core geo scene)
    target_include_directories(Drx11_Engine PUBLIC
        ${CMAKE_INSTALL_PREFIX}/include/dxsdk
        ${CMAKE_INSTALL_PREFIX}/include/eigen3
    )
    target_link_libraries(Drx11_Engine PUBLIC RasteronLib)
    set_target_properties(Drx11_Engine
        PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS ON
    )
    add_dependencies(Drx11_Engine DXSDK Eigen) # Build external projects before engine

    # DirectX Rendering Demos

    add_executable(Drx11_MovingSprite
        geo/composites/Humanoid.cpp

        demos/Drx11_MovingSprite.cpp
    )
    target_include_directories(Drx11_MovingSprite PRIVATE demos)
    target_link_libraries(Drx11_MovingSprite PRIVATE Drx11_Engine)

	add_executable(Drx11_SimpleShapes
		geo/composites/Chain.cpp
		geo/composites/Grid.cpp

		demos/Drx11_SimpleShapes.cpp
	)
	target_link_libraries(Drx11_SimpleShapes PRIVATE Drx11_Engine)
<<<<<<< refs/remotes/origin/linux_port
=======

  # add_dependencies(Drx11_Engine DXSDK GLEW Eigen)
>>>>>>> local
endif()

# OpenGL Rendering Engine

add_library(GL4_Engine
<<<<<<< refs/remotes/origin/linux_port
        ${common_Sources}
		${geo_Sources}
=======
    ${common_Sources}
    ${geo_Sources}
    ${scene_Sources}
    core/ValueGen.cpp
>>>>>>> local

    core/Topl_Renderer_GL4.cpp
)
target_include_directories(GL4_Engine PUBLIC core geo scene)
target_include_directories(GL4_Engine PUBLIC
    # ${CMAKE_INSTALL_PREFIX}/include/dxsdk
    ${CMAKE_INSTALL_PREFIX}/include/eigen3
)
<<<<<<< refs/remotes/origin/linux_port
add_dependencies(GL4_Engine GLEW Eigen) # Build external projects before engine

# OpenGL Rendering Demos
=======
target_link_libraries(GL4_Engine PUBLIC RasteronLib GLEW_lib)
set_target_properties(GL4_Engine
    PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)
if(NOT WIN32) # decreases Windows build time
    add_dependencies(GL4_Engine GLEW Eigen)
endif()
if(UNIX)
    target_link_libraries(GL4_Engine PUBLIC X11 GL) # Links UNIX OS libs
endif()
>>>>>>> local

add_executable(GL4_MovingSprite
    geo/composites/Humanoid.cpp

    demos/GL4_MovingSprite.cpp
)
target_include_directories(GL4_MovingSprite PRIVATE demos)
target_link_libraries(GL4_MovingSprite PRIVATE GL4_Engine)

add_executable(GL4_SimpleShapes
	geo/composites/Chain.cpp
    geo/composites/Grid.cpp

    demos/GL4_SimpleShapes.cpp
)
target_link_libraries(GL4_SimpleShapes PRIVATE GL4_Engine)


# Copying and managing implemented here with custom targets

file(GLOB drx11_Shaders ${CMAKE_SOURCE_DIR}/shaders/*.hlsl)
file(GLOB gl4_Shaders ${CMAKE_SOURCE_DIR}/shaders/*.glsl)
add_custom_target(ShaderCopy
                  SOURCES ${drx11_Shaders} ${gl4_Shaders})
                  # Add GLSL shaders too
if(WIN32)
    foreach(file IN ITEMS ${drx11_Shaders})
        add_custom_command(TARGET Drx11_Engine PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${file}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/")
    endforeach()
endif()

foreach(file IN ITEMS ${gl4_Shaders})
    add_custom_command(TARGET GL4_Engine PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${file}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/")
endforeach()

add_dependencies(ShaderCopy GL4_Engine)
if(WIN32)
  add_dependencies(ShaderCopy Drx11_Engine)
endif()

file(GLOB res_images
    ${CMAKE_SOURCE_DIR}/res/images/*.png # Rasteron Supported
    ${CMAKE_SOURCE_DIR}/res/images/*.tiff # Rasteron Supported
    ${CMAKE_SOURCE_DIR}/res/images/*.bmp # Rasteron Supported
)

file(COPY ${res_images} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/") # Copies all Image Resources
