cmake_minimum_required(VERSION 3.6)
project(Topl DESCRIPTION "Topl Graphics and Animation Engine")

# Extrenal Libraries

if(NOT Rasteron_Install_Path)
    set(Rasteron_Install_Path "${CMAKE_INSTALL_PREFIX}/Rasteron" CACHE PATH "Rasteron install directory")
endif()

find_package(Rasteron PATHS ${Rasteron_Install_Path}/lib/Rasteron)
if(Rasteron_FOUND)
    include(${Rasteron_Install_Path}/lib/Rasteron/findDepends.cmake)
    message(STATUS "Rasteron loadding success!")
else()
    message(WARNING "Texturing and Image modules cannot load! Build and install Rasteron submodule and point Rasteron_Install_Path variable to the Rasteron install directory")
endif()


include(ExternalProject)
set(EXTERNAL_PROJ_DIR "${CMAKE_BINARY_DIR}/Projects")

if(WIN32)
ExternalProject_Add(DXSDK
        GIT_REPOSITORY "https://github.com/Anton-Os/dxsdk.git"
        GIT_TAG "5fbd5e1158db72421bc32709136131451e6c8c37"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/dxsdk
    )
endif()

ExternalProject_Add(GLEW
        GIT_REPOSITORY "https://github.com/Anton-Os/GLEW.git"
        GIT_TAG "e9aa3dd0e3e82b4fa656d282ea1f652cf6575296"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/GLEW
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    )

ExternalProject_Add(Eigen
        GIT_REPOSITORY "https://github.com/eigenteam/eigen-git-mirror.git"
        GIT_TAG "36b95962756c1fce8e29b1f8bc45967f30773c00"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/Eigen
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    )

# 3rd Party Additions

### CMake Install Issues
ExternalProject_Add(Assimp # 3D Model loading
        GIT_REPOSITORY "https://github.com/assimp/assimp.git"
        GIT_TAG "2d2889f73fa1b2ca09ba9f43c9785402d3a7fdd"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/Assimp
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    )

ExternalProject_Add(OpenAL # Audio File Loading
        GIT_REPOSITORY "https://github.com/kcat/openal-soft.git"
        GIT_TAG "6406cc614130ba5f04555ba46e849c685ae6eae0"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/OpenAL
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    )

ExternalProject_Add(FreeType # Font Loading
        GIT_REPOSITORY "https://gitlab.freedesktop.org/freetype/freetype.git"
        GIT_TAG "801cd842e27c85cb1d5000f6397f382ffe295daa"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/FreeType
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    )




if(WIN32)
  find_package(dxsdk_interface PATHS ${CMAKE_INSTALL_PREFIX}/lib/dxsdk)
endif()
find_package(Eigen3 PATHS ${CMAKE_INSTALL_PREFIX}/share/eigen3/cmake)
find_package(GLEW PATHS ${CMAKE_INSTALL_PREFIX}/lib/GLEW)
find_package(Assimp PATHS ${CMAKE_INSTALL_PREFIX}/lib/assimp)

list(APPEND common_Sources
	core/FileIO.cpp
	core/Input.cpp
    core/Timer.cpp
    core/Platform.cpp
    # core/File3D.cpp
    # core/ValueGen.cpp
)

list(APPEND geo_Sources
    geo/primitives/Geo_Flat.cpp
    geo/primitives/Geo_Conic.cpp
    geo/primitives/Geo_Extruded.cpp
    geo/primitives/Geo_Model3D.cpp
    geo/primitives/Geo_Sphere.cpp
)

# DirectX Windows Dependent Demos
if(WIN32)

    add_library(Drx11_Engine
        ${common_Sources}
    	${geo_Sources}
        core/Topl_Scene_Builder.cpp
        core/Topl_Scene_Physics.cpp
        # core/File3D.cpp
        core/ValueGen.cpp

        core/Topl_Renderer_Drx11.cpp
    )
    target_include_directories(Drx11_Engine PUBLIC core geo)
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

    add_executable(Drx11_MovingSprite
        geo/composites/Humanoid.cpp

        demos/Drx11_MovingSprite.cpp
    )
    target_link_libraries(Drx11_MovingSprite PRIVATE Drx11_Engine)

	add_executable(Drx11_SimpleShapes
		geo/composites/Chain.cpp
		geo/composites/Grid.cpp

		demos/Drx11_SimpleShapes.cpp
	)
	target_link_libraries(Drx11_SimpleShapes PRIVATE Drx11_Engine)

    add_executable(Drx11_Planetary demos/Drx11_Planetary.cpp)
	target_link_libraries(Drx11_Planetary PRIVATE Drx11_Engine)

  # add_dependencies(Drx11_Engine DXSDK GLEW Eigen)
endif()

# OpenGL Rendering Demos

add_library(GL4_Engine
    ${common_Sources}
    ${geo_Sources}
    core/Topl_Scene_Builder.cpp
    core/Topl_Scene_Physics.cpp
    # core/File3D.cpp
    core/ValueGen.cpp

    core/Topl_Renderer_GL4.cpp
)
target_include_directories(GL4_Engine PUBLIC core geo)
target_include_directories(GL4_Engine PUBLIC
    # ${CMAKE_INSTALL_PREFIX}/include/dxsdk
    ${CMAKE_INSTALL_PREFIX}/include/eigen3
)
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

add_executable(GL4_MovingSprite
    geo/composites/Humanoid.cpp

    demos/GL4_MovingSprite.cpp
)
# target_include_directories(GL4_MovingSprite PRIVATE demos)
target_link_libraries(GL4_MovingSprite PRIVATE GL4_Engine)

add_executable(GL4_SimpleShapes
	geo/composites/Chain.cpp
    geo/composites/Grid.cpp

    demos/GL4_SimpleShapes.cpp
)
target_link_libraries(GL4_SimpleShapes PRIVATE GL4_Engine)

add_executable(GL4_Planetary demos/GL4_Planetary.cpp)
target_link_libraries(GL4_Planetary PRIVATE GL4_Engine)


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
