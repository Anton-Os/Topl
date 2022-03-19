cmake_minimum_required(VERSION 3.7)
project(Topl DESCRIPTION "Topl Graphics and Animation Engine")

# Extrenal Libraries
include(externalTargets.cmake)
include(locateVulkan.cmake)

list(APPEND helper_src
	helper/FileIO.cpp
	helper/Input.cpp
    helper/Timer.cpp
    helper/Platform.cpp
    helper/ValueGen.cpp
    # core/File3D.cpp
)

list(APPEND geometry_src
    geometry/primitives/Geo_Flat.cpp
    geometry/primitives/Geo_Conic.cpp
    geometry/primitives/Geo_Extruded.cpp
    geometry/primitives/Geo_Node.cpp
    geometry/primitives/Geo_Sphere.cpp

    geometry/Geo_Tree.cpp
    geometry/Geo_SpriteTable.cpp
    geometry/Geometry.cpp
    geometry/ShapesGen.cpp
)
if(SUPPORT_3D_ASSETS)
    list(APPEND geometry_src
        geometry/trees/Model.cpp
        geometry/primitives/Geo_Node.cpp
    )
endif()

if(Assimp_FOUND)
    message("Assimp built library irrXML_lib set to ${irrXML_lib}")
    message("Assimp built library zlib_lib set to ${zlib_lib}")

    #string(REPLACE ".lib" "d.lib" irrXMLd_lib ${irrXML_lib})
    set(irrXMLd_lib "${CMAKE_INSTALL_PREFIX}/lib/irrXMLd.lib")
    #string(REPLACE ".lib" "d.lib" zlibd_lib ${zlib_lib})
    set(zlibd_lib "${CMAKE_INSTALL_PREFIX}/lib/zlibstaticd.lib")
    message("Assimp built library irrXMLd_lib set to ${irrXMLd_lib}")
    message("Assimp built library zlibd_lib set to ${zlibd_lib}")
endif()

# DirectX Windows Dependent Demos
if(WIN32)
    add_library(Drx11_ENGINE
        ${helper_src}
    	${geometry_src}

        core/Topl_Scene_Statics.cpp
        core/Topl_Scene_Dynamics.cpp

        core/Topl_Pipeline_Drx11.cpp
        core/Topl_Renderer_Drx11.cpp

        core/Topl_App.cpp
    )
    target_include_directories(Drx11_ENGINE PUBLIC core helper geometry shaders/include)
    target_include_directories(Drx11_ENGINE PUBLIC
        ${CMAKE_INSTALL_PREFIX}/include/eigen3
    )
    if(Rasteron_FOUND)
        target_link_libraries(Drx11_ENGINE PUBLIC Rasteron)
    endif()
    if(Assimp_FOUND)
        # target_link_libraries(Drx11_ENGINE PUBLIC assimp::assimp ${irrXML_lib} ${zlib_lib})
        target_link_libraries(Drx11_ENGINE PUBLIC assimp::assimp ${irrXMLd_lib} ${zlibd_lib}) # for debug mode
    endif()
    set_target_properties(Drx11_ENGINE
        PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS ON
    )

    add_executable(Drx11_Pawns
        geometry/trees/Humanoid.cpp

        demos/legacy/Drx11_Pawns.cpp
    )
    target_link_libraries(Drx11_Pawns PRIVATE Drx11_ENGINE)

	add_executable(Drx11_Blocks
		geometry/trees/Chain.cpp
		geometry/trees/Grid.cpp

		demos/legacy/Drx11_Blocks.cpp
	)
	target_link_libraries(Drx11_Blocks PRIVATE Drx11_ENGINE)

    add_executable(Drx11_Planetary demos/legacy/Drx11_Planetary.cpp)
	target_link_libraries(Drx11_Planetary PRIVATE Drx11_ENGINE)
    
    add_executable(Drx11_Surface demos/legacy/Drx11_Surface.cpp)
	target_link_libraries(Drx11_Surface PRIVATE Drx11_ENGINE)

    add_executable(Drx11_Overlay 
        geometry/trees/Pane.cpp

        demos/legacy/Drx11_Overlay.cpp
    )
	target_link_libraries(Drx11_Overlay PRIVATE Drx11_ENGINE)

    add_executable(Drx11_Assets demos/legacy/Drx11_Assets.cpp)
	target_link_libraries(Drx11_Assets PRIVATE Drx11_ENGINE)

  # add_dependencies(Drx11_ENGINE DXSDK GLEW Eigen)
endif()

# OpenGL Rendering Demos

add_library(GL4_ENGINE
    ${helper_src}
    ${geometry_src}

    core/Topl_Scene_Statics.cpp
    core/Topl_Scene_Dynamics.cpp

    core/Topl_Pipeline_GL4.cpp
    core/Topl_Renderer_GL4.cpp
    
    core/Topl_App.cpp
)
target_include_directories(GL4_ENGINE PUBLIC core helper geometry shaders/include)
target_include_directories(GL4_ENGINE PUBLIC
    ${CMAKE_INSTALL_PREFIX}/include/eigen3
)
target_link_libraries(GL4_ENGINE PUBLIC GLEW_lib)
if(Rasteron_FOUND)
    target_link_libraries(GL4_ENGINE PUBLIC Rasteron)
endif()
if(Assimp_FOUND)
    # target_link_libraries(GL4_ENGINE PUBLIC assimp::assimp ${irrXML_lib} ${zlib_lib})
    target_link_libraries(GL4_ENGINE PUBLIC assimp::assimp ${irrXMLd_lib} ${zlibd_lib}) # for debug mode
endif()
set_target_properties(GL4_ENGINE
    PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)
if(NOT WIN32) # decreases Windows build time
    add_dependencies(GL4_ENGINE GLEW Eigen)
endif()

add_executable(GL4_Pawns
    geometry/trees/Humanoid.cpp

    demos/legacy/GL4_Pawns.cpp
)
# target_include_directories(GL4_Pawns PRIVATE demos)
target_link_libraries(GL4_Pawns PRIVATE GL4_ENGINE)

add_executable(GL4_Blocks
	geometry/trees/Chain.cpp
    geometry/trees/Grid.cpp

    demos/legacy/GL4_Blocks.cpp
)
target_link_libraries(GL4_Blocks PRIVATE GL4_ENGINE)

add_executable(GL4_Planetary demos/legacy/GL4_Planetary.cpp)
target_link_libraries(GL4_Planetary PRIVATE GL4_ENGINE)

add_executable(GL4_Surface demos/legacy/GL4_Surface.cpp)
target_link_libraries(GL4_Surface PRIVATE GL4_ENGINE)

add_executable(GL4_Overlay
    geometry/trees/Pane.cpp

    demos/legacy/GL4_Overlay.cpp
)
target_link_libraries(GL4_Overlay PRIVATE GL4_ENGINE)

add_executable(GL4_Assets demos/legacy/GL4_Assets.cpp)
target_link_libraries(GL4_Assets PRIVATE GL4_ENGINE)

# Copying and managing implemented here with custom targets

file(GLOB drx11_Shaders ${CMAKE_SOURCE_DIR}/shaders/hlsl/*.hlsl)
file(GLOB gl4_Shaders ${CMAKE_SOURCE_DIR}/shaders/glsl/*.glsl)
add_custom_target(ShaderEdit SOURCES ${drx11_Shaders} ${gl4_Shaders})

add_dependencies(ShaderEdit GL4_ENGINE)
if(WIN32)
  add_dependencies(ShaderEdit Drx11_ENGINE)
endif()

set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets/")
set(SHADERS_DIR "${CMAKE_SOURCE_DIR}/shaders/")
configure_file("${CMAKE_SOURCE_DIR}/helper/support_def.h.in" "${CMAKE_SOURCE_DIR}/helper/support_def.h")

# Modern Demos

add_executable(Exec
    geometry/trees/Chain.cpp
    geometry/trees/Grid.cpp
    geometry/trees/Humanoid.cpp
    geometry/trees/Model.cpp
    geometry/trees/Pane.cpp

    demos/Exec.cpp
)
target_link_libraries(Exec GL4_ENGINE Drx11_ENGINE)
