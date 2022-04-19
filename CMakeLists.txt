cmake_minimum_required(VERSION 3.7)
project(Topl DESCRIPTION "Topl Graphics and Animation Engine")

# Extrenal Libraries
include("cmake/externalTargets.cmake")
include("cmake/findVulkan.cmake")
if(UNIX)
    include(FindX11)
endif()

# ------------- Sources ------------- #

set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets/" )
set(SHADERS_DIR "${CMAKE_SOURCE_DIR}/shaders/" )

if(WIN32)
    set(RESOURCE_DIR "${ASSETS_DIR}/res" CACHE PATH "Path to directory with resource.h file")
    set(RESOURCE_FILE "${RESOURCE_DIR}/resource.rc" CACHE FILEPATH "Resource (.rc) file")
    # set(RESOURCE_ICON "${RESOURCE_DIR}/TOPL.ico" CACHE FILEPATH "Icon (.ico) file")
endif()

list(APPEND helper_src
	helper/IO.cpp
	helper/Controls.cpp
    helper/Timer.cpp
    helper/Platform.cpp
    helper/ValueGen.cpp
    # core/File3D.cpp
)

list(APPEND geometry_src
    geometry/Geometry.cpp
    geometry/ShapesGen.cpp

    geometry/primitives/Geo_Flat.cpp
    geometry/primitives/Geo_Conic.cpp
    geometry/primitives/Geo_Extruded.cpp
    geometry/primitives/Geo_Sphere.cpp

    geometry/Geo_Tree.cpp
)
if(Assimp_FOUND)
    list(APPEND geometry_src
        geometry/trees/Model.cpp
        geometry/primitives/Geo_Node.cpp
    )
endif()
if(Rasteron_FOUND)
    list(APPEND geometry_src geometry/Geo_SpriteTable.cpp)
endif()

list(APPEND core_src
    core/Topl_Renderer.cpp
    # core/Topl_Factory.cpp

    core/Topl_Scene_Statics.cpp
    core/Topl_Scene_Dynamics.cpp
)

if(Assimp_FOUND)
    set(irrXMLd_lib "${CMAKE_INSTALL_PREFIX}/lib/irrXMLd.lib") # locating debug lib
    set(zlibd_lib "${CMAKE_INSTALL_PREFIX}/lib/zlibstaticd.lib") # locating debug lib
    message("Assimp libraries irrXML_lib set to ${irrXML_lib}, irrXMLd_lib set to ${irrXMLd_lib}")
    message("Assimp libraries zlib_lib set to ${zlib_lib}, zlibd_lib set to ${zlibd_lib}")
endif()

# -------------- Renderers and Libraries ------------- #

add_library(Topl_CORELIB
    ${helper_src}
    ${core_src}
)
target_include_directories(Topl_CORELIB PUBLIC core helper geometry)
set_target_properties(Topl_CORELIB PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS ON)

if(WIN32)
    target_include_directories(Topl_CORELIB PUBLIC "${RESOURCE_DIR}") # resource file header
    target_sources(Topl_CORELIB PRIVATE "${RESOURCE_FILE}") # resource file compilation
elseif(UNIX)
    target_link_libraries(Topl_CORELIB PUBLIC ${X11_LIBRARIES})
endif()

add_library(Topl_GEOLIB ${geometry_src})
target_include_directories(Topl_GEOLIB PUBLIC core helper geometry)

if(Assimp_FOUND) # linking targets to Assimp
   # target_link_libraries(Topl_GEOLIB PUBLIC assimp::assimp ${irrXML_lib} ${zlib_lib}) # for release build
   target_link_libraries(Topl_GEOLIB PUBLIC assimp::assimp ${irrXMLd_lib} ${zlibd_lib}) # for debug build
endif()

if(Rasteron_FOUND) # linking targets to Rasteron
    target_link_libraries(Topl_CORELIB PUBLIC Rasteron)
    target_link_libraries(Topl_GEOLIB PUBLIC Rasteron)
endif()

# Building DirectX Engine

if(WIN32)
    add_library(Drx11_ENGINE
        core/directx/Topl_Pipeline_Drx11.cpp
        core/directx/Topl_Renderer_Drx11.cpp
    )
    target_include_directories(Drx11_ENGINE PUBLIC core helper geometry shaders/include)
    set_target_properties(Drx11_ENGINE PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS ON)
endif()

# Building Vulkan Engine

if(Vulkan_FOUND)
    add_library(Vulkan_ENGINE
        core/vulkan/Topl_Pipeline_Vulkan.cpp
        core/vulkan/Topl_Renderer_Vulkan.cpp
    )
    target_include_directories(Vulkan_ENGINE PUBLIC ${Vulkan_INCLUDE_DIRS})
    target_include_directories(Vulkan_ENGINE PUBLIC core helper geometry shaders/include)
    target_link_libraries(Vulkan_ENGINE PUBLIC ${Vulkan_LIBRARIES})
    # set_target_properties(Vulkan_ENGINE PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS ON)
endif()

# Building OpenGL Engine

add_library(GL4_ENGINE
    core/opengl/Topl_Pipeline_GL4.cpp
    core/opengl/Topl_Renderer_GL4.cpp
)
target_include_directories(GL4_ENGINE PUBLIC core helper geometry shaders/include)
target_link_libraries(GL4_ENGINE PUBLIC GLEW_lib)
# set_target_properties(GL4_ENGINE PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS ON)

if(UNIX)
    add_dependencies(GL4_ENGINE GLEW)
endif()

# add all engine targets to a list
list(APPEND engine_targets GL4_ENGINE) # OpenGL available on all systems
if(WIN32)
    list(APPEND engine_targets Drx11_ENGINE)
endif()
if(Vulkan_FOUND)
    list(APPEND engine_targets Vulkan_ENGINE)
endif()

foreach(target IN ITEMS ${engine_targets})
    target_link_libraries(${target} PUBLIC Topl_CORELIB Topl_GEOLIB) # linking Topl libraries
endforeach()

# ------------- Demos ------------- #

include("cmake/legacyDemos.cmake")
include("cmake/runTests.cmake")

# Modern Demos and Tests

add_executable(App_Playground
    geometry/trees/Chain.cpp
    geometry/trees/Grid.cpp
    geometry/trees/Humanoid.cpp
    geometry/trees/Model.cpp
    geometry/trees/Pane.cpp

    core/Topl_Factory.cpp
    core/Topl_App.cpp
    demos/Playground.cpp
)
target_link_libraries(App_Playground PRIVATE ${engine_targets})

add_executable(App_Splitscreen
    core/Topl_Factory.cpp
    core/Topl_App.cpp
    demos/Splitscreen.cpp
)
target_link_libraries(App_Splitscreen PRIVATE ${engine_targets})


# add all demo targets to a list
list(APPEND demo_targets App_Playground App_Splitscreen)

if(WIN32) # adding resource file to demos
   foreach(target IN ITEMS ${demo_targets})
       target_include_directories(${target} PUBLIC "${RESOURCE_DIR}")
       target_sources(${target} PRIVATE "${RESOURCE_FILE}")
   endforeach()
endif()

# -------------- Custom Targets ------------- #
# Copying and managing implemented here with custom targets

file(GLOB gl4_Shaders ${SHADERS_DIR}/glsl/*.glsl)
file(GLOB drx11_Shaders ${SHADERS_DIR}/hlsl/*.hlsl)
add_custom_target(ShaderEdit SOURCES ${gl4_Shaders} ${drx11_Shaders})

add_dependencies(ShaderEdit GL4_ENGINE)
if(WIN32)
  add_dependencies(ShaderEdit Drx11_ENGINE)
endif()
if(Vulkan_FOUND)
    find_file(glslc_exec "glslc" PATHS "${VULKAN_PATH}/Bin/") # for compiling shader code into bytecode
    # if(NOT ${glslc_exec} EQUAL "glslc_exec-NOTFOUND")
        # foreach(shader IN ITEMS ${gl4_Shaders})
            # Compile shaders to spirv and store in binary directory
        #endforeach()
    #endif()
endif()

configure_file("${CMAKE_SOURCE_DIR}/helper/support_def.h.in" "${CMAKE_SOURCE_DIR}/helper/support_def.h")

# Install Procedures

install(TARGETS Topl_CORELIB Topl_GEOLIB ${engine_targets}
    EXPORT Topl
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    FRAMEWORK DESTINATION lib
)

file(GLOB_RECURSE interfaceHeaders 
    ${CMAKE_CURRENT_SOURCE_DIR}/core/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/helper/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/*.h
)

# install(FILES ${interfaceHeaders} DESTINATION include/Topl)
# install(EXPORT Topl DESTINATION lib/Topl FILE ToplConfig.cmake)