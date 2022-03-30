cmake_minimum_required(VERSION 3.7)
project(Topl DESCRIPTION "Topl Graphics and Animation Engine")

# Extrenal Libraries
include(externalTargets.cmake)
include(findVulkan.cmake)

# ------------- Sources ------------- #

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

list(APPEND core_src
    core/Topl_Scene_Statics.cpp
    core/Topl_Scene_Dynamics.cpp
    core/Topl_App.cpp
)

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

# -------------- Renderers ------------- #

# Building DirectX Engine

if(WIN32)
    add_library(Drx11_ENGINE
        ${helper_src}
    	${geometry_src}
        ${core_src}

        core/Topl_Pipeline_Drx11.cpp
        core/Topl_Renderer_Drx11.cpp
    )
    target_include_directories(Drx11_ENGINE PUBLIC core helper geometry shaders/include)
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
endif()

# Building Vulkan Engine

if(Vulkan_FOUND)
    add_library(Vulkan_ENGINE
        ${helper_src}
        ${geometry_src}
        ${core_src}

        core/Topl_Pipeline_Vulkan.cpp
        core/Topl_Renderer_Vulkan.cpp
    )
    target_include_directories(Vulkan_ENGINE PUBLIC ${Vulkan_INCLUDE_DIRS})
    target_include_directories(Vulkan_ENGINE PUBLIC core helper geometry shaders/include)
    target_link_libraries(Vulkan_ENGINE PUBLIC ${Vulkan_LIBRARIES})
    if(Rasteron_FOUND)
        target_link_libraries(Vulkan_ENGINE PUBLIC Rasteron)
    endif()
    if(Assimp_FOUND)
        # target_link_libraries(Vulkan_ENGINE PUBLIC assimp::assimp ${irrXML_lib} ${zlib_lib}) # for release mode
        target_link_libraries(Vulkan_ENGINE PUBLIC assimp::assimp ${irrXMLd_lib} ${zlibd_lib}) # for debug mode
    endif()
    set_target_properties(Vulkan_ENGINE
        PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS ON
    )
endif()

# Building OpenGL Engine

add_library(GL4_ENGINE
    ${helper_src}
    ${geometry_src}
    ${core_src}

    core/Topl_Pipeline_GL4.cpp
    core/Topl_Renderer_GL4.cpp
)
target_include_directories(GL4_ENGINE PUBLIC core helper geometry shaders/include)
target_link_libraries(GL4_ENGINE PUBLIC GLEW_lib)
if(Rasteron_FOUND)
    target_link_libraries(GL4_ENGINE PUBLIC Rasteron)
endif()
if(Assimp_FOUND)
    # target_link_libraries(GL4_ENGINE PUBLIC assimp::assimp ${irrXML_lib} ${zlib_lib}) # for release mode
    target_link_libraries(GL4_ENGINE PUBLIC assimp::assimp ${irrXMLd_lib} ${zlibd_lib}) # for debug mode
endif()
set_target_properties(GL4_ENGINE
    PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

# ------------- Demos ------------- #

include(legacyDemos.cmake)
# add_executable(Test_Image demos/test/ImageTest.cpp)

# Modern Demos and Tests

add_executable(Test_MathOps demos/test/MathOps.cpp)
target_include_directories(Test_MathOps PUBLIC core)

add_executable(Test_HelloVulkan demos/test/HelloVulkan.cpp)
target_include_directories(Test_HelloVulkan PUBLIC core)

add_executable(Demo_Playground
    geometry/trees/Chain.cpp
    geometry/trees/Grid.cpp
    geometry/trees/Humanoid.cpp
    geometry/trees/Model.cpp
    geometry/trees/Pane.cpp

    demos/Playground.cpp
)
target_link_libraries(Demo_Playground GL4_ENGINE Drx11_ENGINE Vulkan_ENGINE)

# -------------- Custom Targets ------------- #
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

if(WIN32) # adding resource file to engines
    target_sources(GL4_ENGINE PRIVATE "${ASSETS_DIR}/others/resource.rc")
    target_sources(Drx11_ENGINE PRIVATE "${ASSETS_DIR}/others/resource.rc")
endif()