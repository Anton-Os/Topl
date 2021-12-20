cmake_minimum_required(VERSION 3.6)
project(Topl DESCRIPTION "Topl Graphics and Animation Engine")

# Extrenal Libraries
include(externalTargets.cmake)

list(APPEND common_Src
	core/FileIO.cpp
	core/Input.cpp
    core/Timer.cpp
    core/Platform.cpp
    # core/File3D.cpp
    # core/ValueGen.cpp
)

list(APPEND geo_Src
    geo/primitives/Geo_Flat.cpp
    geo/primitives/Geo_Conic.cpp
    geo/primitives/Geo_Extruded.cpp
    # geo/primitives/Geo_Node.cpp
    geo/primitives/Geo_Sphere.cpp

    geo/Geo_Tree.cpp
    geo/Geo_SpriteTable.cpp
    geo/Geometry.cpp
    geo/ShapesGen.cpp
)
if(SUPPORT_3D_ASSETS)
    list(APPEND geo_Src
        geo/trees/Model.cpp
        geo/primitives/Geo_Node.cpp
    )
endif()

if(Assimp_FOUND)
    # ExternalProject_Get_Property(Assimp BINARY_DIR)
    message("Assimp dependency irrXML_lib set to ${irrXML_lib}")
    message("Assimp dependency zlib_lib set to ${zlib_lib}")
endif()

# DirectX Windows Dependent Demos
if(WIN32)

    add_library(Drx11_Engine
        ${common_Src}
    	${geo_Src}
        core/Topl_Scene_Builder.cpp
        core/Topl_Scene_Physics.cpp
        # core/File3D.cpp
        core/ValueGen.cpp

        core/Topl_Renderer_Drx11.cpp
    )
    target_include_directories(Drx11_Engine PUBLIC core geo shaders/include)
    target_include_directories(Drx11_Engine PUBLIC
        ${CMAKE_INSTALL_PREFIX}/include/dxsdk
        ${CMAKE_INSTALL_PREFIX}/include/eigen3
    )
    if(Rasteron_FOUND)
        target_link_libraries(Drx11_Engine PUBLIC Rasteron)
    endif()
    if(Assimp_FOUND)
        target_link_libraries(Drx11_Engine PUBLIC assimp::assimp)
        target_link_libraries(Drx11_Engine PUBLIC assimp::assimp ${irrXML_lib} ${zlib_lib})
    endif()
    set_target_properties(Drx11_Engine
        PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS ON
    )

    add_executable(Drx11_MovingSprite
        geo/trees/Humanoid.cpp

        demos/Drx11_MovingSprite.cpp
    )
    target_link_libraries(Drx11_MovingSprite PRIVATE Drx11_Engine)

	add_executable(Drx11_SimpleShapes
		geo/trees/Chain.cpp
		geo/trees/Grid.cpp

		demos/Drx11_SimpleShapes.cpp
	)
	target_link_libraries(Drx11_SimpleShapes PRIVATE Drx11_Engine)

    add_executable(Drx11_Planetary demos/Drx11_Planetary.cpp)
	target_link_libraries(Drx11_Planetary PRIVATE Drx11_Engine)
    
    add_executable(Drx11_Canvas demos/Drx11_Canvas.cpp)
	target_link_libraries(Drx11_Canvas PRIVATE Drx11_Engine)

    add_executable(Drx11_Overlay 
        geo/trees/Pane.cpp

        demos/Drx11_Overlay.cpp
    )
	target_link_libraries(Drx11_Overlay PRIVATE Drx11_Engine)

    add_executable(Drx11_Assets demos/Drx11_Assets.cpp)
	target_link_libraries(Drx11_Assets PRIVATE Drx11_Engine)

  # add_dependencies(Drx11_Engine DXSDK GLEW Eigen)
endif()

# OpenGL Rendering Demos

add_library(GL4_Engine
    ${common_Src}
    ${geo_Src}
    core/Topl_Scene_Builder.cpp
    core/Topl_Scene_Physics.cpp
    # core/File3D.cpp
    core/ValueGen.cpp

    core/Topl_Renderer_GL4.cpp
)
target_include_directories(GL4_Engine PUBLIC core geo shaders/include)
target_include_directories(GL4_Engine PUBLIC
    ${CMAKE_INSTALL_PREFIX}/include/eigen3
)
target_link_libraries(GL4_Engine PUBLIC GLEW_lib)
if(Rasteron_FOUND)
    target_link_libraries(GL4_Engine PUBLIC Rasteron)
endif()
if(Assimp_FOUND)
    target_link_libraries(GL4_Engine PUBLIC assimp::assimp)
    target_link_libraries(GL4_Engine PUBLIC assimp::assimp ${irrXML_lib} ${zlib_lib})
endif()
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
    geo/trees/Humanoid.cpp

    demos/GL4_MovingSprite.cpp
)
# target_include_directories(GL4_MovingSprite PRIVATE demos)
target_link_libraries(GL4_MovingSprite PRIVATE GL4_Engine)

add_executable(GL4_SimpleShapes
	geo/trees/Chain.cpp
    geo/trees/Grid.cpp

    demos/GL4_SimpleShapes.cpp
)
target_link_libraries(GL4_SimpleShapes PRIVATE GL4_Engine)

add_executable(GL4_Planetary demos/GL4_Planetary.cpp)
target_link_libraries(GL4_Planetary PRIVATE GL4_Engine)

add_executable(GL4_Canvas demos/GL4_Canvas.cpp)
target_link_libraries(GL4_Canvas PRIVATE GL4_Engine)

add_executable(GL4_Overlay
    geo/trees/Pane.cpp

    demos/GL4_Overlay.cpp
)
target_link_libraries(GL4_Overlay PRIVATE GL4_Engine)

add_executable(GL4_Assets demos/GL4_Assets.cpp)
target_link_libraries(GL4_Assets PRIVATE GL4_Engine)


# Copying and managing implemented here with custom targets

file(GLOB drx11_Shaders ${CMAKE_SOURCE_DIR}/shaders/hlsl/*.hlsl)
file(GLOB gl4_Shaders ${CMAKE_SOURCE_DIR}/shaders/glsl/*.glsl)
add_custom_target(ShaderEdit
                  SOURCES ${drx11_Shaders} ${gl4_Shaders})
                  # Add GLSL shaders too

add_dependencies(ShaderEdit GL4_Engine)
if(WIN32)
  add_dependencies(ShaderEdit Drx11_Engine)
endif()

set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets/")
set(SHADERS_DIR "${CMAKE_SOURCE_DIR}/shaders/")
configure_file("${CMAKE_SOURCE_DIR}/core/support_def.h.in" "${CMAKE_SOURCE_DIR}/core/support_def.h")