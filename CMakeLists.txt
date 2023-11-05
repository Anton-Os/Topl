cmake_minimum_required(VERSION 3.7)
project(Topl DESCRIPTION "Topl Graphics Engine")

set (CMAKE_CXX_STANDARD 11)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/INSTALL" CACHE PATH "Install Path" FORCE)
    message("Install Prefix ${CMAKE_INSTALL_PREFIX}")
endif()

# Extrenal Libraries
include("cmake/externalTargets.cmake")
include("cmake/findVulkan.cmake")
if(UNIX AND NOT APPLE) # Unix Specific
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

list(APPEND support_src
	support/ValueGen.cpp

	support/IO.cpp
	support/Controls.cpp
    support/Timer.cpp
    support/Platform.cpp
    # core/File3D.cpp
)

list(APPEND geometry_src
	# meshes
    geometry/meshes/Geo_Shape2D.cpp
    geometry/meshes/Geo_Shape3D.cpp
    geometry/meshes/Geo_Cone.cpp
    geometry/meshes/Geo_Sphere.cpp
)
if(SUPPORT_MODELS AND Assimp_FOUND)
    message(STATUS, "Building 3D Model sources")
    list(APPEND geometry_src
        geometry/meshes/Geo_Node.cpp
        # geometry/constructs/Geo_Model.cpp
    )
endif()


list(APPEND core_src
    core/Topl_Main.cpp
    core/Topl_Factory.cpp
    core/Topl_Renderer.cpp
    core/Topl_Scene_Statics.cpp
)

set(SUPPORT_PHYSICS TRUE CACHE BOOL "Include physics calculations" FORCE)
if(SUPPORT_PHYSICS)
    list(APPEND core_src core/Topl_Scene_Dynamics.cpp)
endif()

# -------------- Core and Geometry Libraries ------------- #

add_library(CORELIB # Core Library
    ${support_src}
    ${core_src}
)
target_include_directories(CORELIB PRIVATE core support physics geometry)
set_target_properties(CORELIB PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS ON)
target_include_directories(CORELIB
    INTERFACE
    $<BUILD_INTERFACE:${interfaceHeaders}>
    $<INSTALL_INTERFACE:include/Topl>
)

if(WIN32)
    target_include_directories(CORELIB PRIVATE "${RESOURCE_DIR}") # resource file header
    target_sources(CORELIB PRIVATE "${RESOURCE_FILE}") # resource file compilation
elseif(UNIX AND NOT APPLE) # Unix Specific
    target_link_libraries(CORELIB PUBLIC ${X11_LIBRARIES})
endif()

add_library(GEOLIB ${geometry_src}) # Geometry Library
target_include_directories(GEOLIB PRIVATE core support physics geometry)
target_include_directories(GEOLIB INTERFACE
    $<BUILD_INTERFACE:${interfaceHeaders}>
    $<INSTALL_INTERFACE:include/Topl>
)

if(SUPPORT_MODELS AND Assimp_FOUND) # linking targets to Assimp
   # target_link_libraries(GEOLIB PUBLIC assimp::assimp ${irrXML_lib} ${zlib_lib}) # for release build
   target_link_libraries(GEOLIB PUBLIC assimp::assimp ${irrXMLd_lib} ${zlibd_lib}) # for debug build
   if(WIN32)
        set(irrXML_lib "${CMAKE_INSTALL_PREFIX}/lib/irrXML.lib") # locating release lib
        set(irrXMLd_lib "${CMAKE_INSTALL_PREFIX}/lib/irrXMLd.lib") # locating debug lib
        set(zlib_lib "${CMAKE_INSTALL_PREFIX}/lib/zlibstatic.lib") # locating release lib
        set(zlibd_lib "${CMAKE_INSTALL_PREFIX}/lib/zlibstaticd.lib") # locating debug lib
        # message("Assimp libraries zlib set to ${zlib_lib}, irrXML set to ${irrXML_lib}")
        message("Assimp libraries zlib set to ${zlibd_lib}, irrXML set to ${irrXMLd_lib}")
    
        # target_link_libraries(GEOLIB PUBLIC ${irrXML_lib} ${zlib_lib}) # for release build
        target_link_libraries(GEOLIB PUBLIC ${irrXMLd_lib} ${zlibd_lib}) # for debug build
    elseif(UNIX AND NOT APPLE) # Unix Specific
        find_package(ZLIB)
        find_library(irrXML_lib NAMES "irrXML" "IrrXML" PATHS ${CMAKE_INSTALL_PREFIX}/lib)
        message("Assimp libraries zlib set to ${ZLIB_LIBRARIES}, irrXML set to ${irrXML_lib}")

        target_link_libraries(GEOLIB PUBLIC ${ZLIB_LIBRARIES} ${irrXML_lib})
    endif()
endif()

if(Rasteron_FOUND) # linking targets to Rasteron
    target_link_libraries(CORELIB PUBLIC Rasteron)
    target_link_libraries(GEOLIB PUBLIC Rasteron)
endif()

# -------------- Building Engines ------------- #

# Building DirectX Engine

if(WIN32)
    add_library(DX11_ENGINE
        core/directx/Topl_Pipeline_DX11.cpp
        core/directx/Topl_Renderer_DX11.cpp
    )
    target_include_directories(DX11_ENGINE PRIVATE core support physics geometry)
    target_include_directories(DX11_ENGINE INTERFACE
        $<BUILD_INTERFACE:${interfaceHeaders}>
        $<INSTALL_INTERFACE:include/Topl>
    )
    set_target_properties(DX11_ENGINE PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS ON)
endif()

# Building Vulkan Engine

if(Vulkan_FOUND)
    add_library(Vulkan_ENGINE
        core/vulkan/Topl_Pipeline_Vulkan.cpp
        core/vulkan/Topl_Renderer_Vulkan.cpp
    )
    target_include_directories(Vulkan_ENGINE PUBLIC ${Vulkan_INCLUDE_DIRS})
    target_include_directories(Vulkan_ENGINE PRIVATE core support physics geometry)
    target_include_directories(Vulkan_ENGINE INTERFACE
        $<BUILD_INTERFACE:${interfaceHeaders}>
        $<INSTALL_INTERFACE:include/Topl>
    )
    target_link_libraries(Vulkan_ENGINE PUBLIC ${Vulkan_LIBRARIES})
    # set_target_properties(Vulkan_ENGINE PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS ON)
endif()

# Building OpenGL Engine

add_library(GL4_ENGINE
    core/opengl/Topl_Pipeline_GL4.cpp
    core/opengl/Topl_Renderer_GL4.cpp
)
target_include_directories(GL4_ENGINE PRIVATE core support physics geometry)
target_include_directories(GL4_ENGINE INTERFACE
    $<BUILD_INTERFACE:${interfaceHeaders}>
    $<INSTALL_INTERFACE:include/Topl>
)
target_link_libraries(GL4_ENGINE PUBLIC GLEW_lib)
# set_target_properties(GL4_ENGINE PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS ON)

if(UNIX AND NOT APPLE) # Unix Specific
    add_dependencies(GL4_ENGINE GLEW)

    set(OpenGL_GL_PREFERENCE GLVND)
    find_package(OpenGL REQUIRED)
    if(OpenGL_FOUND)
        message("OpenGL libraries found: ${OPENGL_opengl_LIBRARY} and ${OPENGL_glx_LIBRARY}")
        target_link_libraries(GL4_ENGINE PUBLIC ${OPENGL_opengl_LIBRARY})
        target_link_libraries(GL4_ENGINE PUBLIC ${OPENGL_glx_LIBRARY})
    endif()
endif()

list(APPEND engine_targets GL4_ENGINE) # OpenGL available cross-platform
if(WIN32)
    list(APPEND engine_targets DX11_ENGINE) # DirectX available on windows
endif()
if(Vulkan_FOUND)
    list(APPEND engine_targets Vulkan_ENGINE) # Vulkan available with library
endif()

foreach(target IN ITEMS ${engine_targets})
    if(WIN32)
        target_include_directories(${target} PRIVATE "${RESOURCE_DIR}")
    endif()
    target_link_libraries(${target} PUBLIC GEOLIB CORELIB) # link both corelib and geolib
	# target_link_libraries(${target} PUBLIC GEOLIB) # link only geolib
	target_link_libraries(CORELIB PUBLIC ${target})
endforeach()

# ------------- Demos and Tests ------------- #

add_executable(Diagnostic demos/Diagnostic.cpp)
add_executable(Sandbox demos/Sandbox.cpp)
# add_executable(Playground demos/Playground.cpp)
# add_executable(Animotion demos/Animotion.cpp)
# add_executable(Splitscreen demos/Splitscreen.cpp)
# add_executable(Subsurface demos/Subsurface.cpp)

list(APPEND demo_targets Diagnostic Sandbox) # Playground Animotion Splitscreen Subsurface) # add all demo targets to a list

foreach(target IN ITEMS ${demo_targets})
    target_include_directories(${target} PRIVATE core support physics geometry shaders)
    target_link_libraries(${target} PRIVATE CORELIB)
    if(WIN32) # adding resource file to demos
        target_include_directories(${target} PRIVATE "${RESOURCE_DIR}")
        target_sources(${target} PRIVATE "${RESOURCE_FILE}")
    endif()
endforeach()

# -------------- Custom Targets ------------- #
# Copying and managing implemented here with custom targets

file(GLOB gl4_shaders ${SHADERS_DIR}/glsl/*.glsl)
add_custom_target(GL4_ShaderEdit SOURCES ${gl4_shaders})
add_dependencies(GL4_ShaderEdit GL4_ENGINE)

if(WIN32)
	file(GLOB drx11_shaders ${SHADERS_DIR}/hlsl/*.hlsl)
	add_custom_target(DX11_ShaderEdit SOURCES ${drx11_shaders})
	add_dependencies(DX11_ShaderEdit DX11_ENGINE)
endif()

if(Vulkan_FOUND)
	# function(compileShader shaderFile shaderType)
	#	get_filename_component(shader_name ${shaderFile} NAME)
	#	message(STATUS "Compiling ${shaderType} shader ${shader_name}")

	#	string(FIND ${shader_name} shaderType isShaderType)
	#	message("${isShaderType}")
	#	if(NOT isShaderType EQUAL -1)
	#		message("Compiling ${shaderType} shader ${shaderFile}")
	#		exec_program(
	#			${glslc_exec} 
	#			${CMAKE_BINARY_DIR} 
	#			ARGS "-fshader-stage=vertex ${shaderFile} -o ${shader_name}.spv"
	#		)
	#	endif()
	# endfunction()

	message(STATUS "Searching inside ${VULKAN_PATH}/Bin/")

    find_file(glslc_exec "glslc" "glslc.exe" PATHS "${VULKAN_PATH}/Bin/") # for compiling shader code into bytecode
	if(NOT ${glslc_exec} STREQUAL "glslc_exec-NOTFOUND")
		message("${glslc_exec} executing")
        foreach(shader IN ITEMS ${gl4_shaders})
			get_filename_component(shader_name ${shader} NAME)

			string(FIND ${shader_name} "Vert" isVertexShader)
			if(NOT isVertexShader EQUAL -1)
				message("Compiling vertex shader ${shader_name} inside of ${shader}")
				# exec_program(${glslc_exec} ${CMAKE_CURRENT_BINARY_DIR} ARGS "-fshader-stage=vertex ${shader} -o ${shader_name}.spv")
                exec_program(${glslc_exec} ${SHADERS_DIR}/spirv ARGS "-fshader-stage=vertex ${shader} -o ${shader_name}.spv")
			endif()
			
			string(FIND ${shader_name} "Frag" isFragShader)
			if(NOT isFragShader EQUAL -1)
				message("Compiling fragment shader ${shader_name} inside of ${shader}")
				# exec_program(${glslc_exec} ${CMAKE_CURRENT_BINARY_DIR} ARGS "-fshader-stage=frag ${shader} -o ${shader_name}.spv")
                exec_program(${glslc_exec} ${SHADERS_DIR}/spirv ARGS "-fshader-stage=frag ${shader} -o ${shader_name}.spv")
			endif()

            string(FIND ${shader_name} "Geo" isGeomShader)
			if(NOT isGeomShader EQUAL -1)
				message("Compiling geometry shader ${shader_name} inside of ${shader}")
				# exec_program(${glslc_exec} ${CMAKE_CURRENT_BINARY_DIR} ARGS "-fshader-stage=geom ${shader} -o ${shader_name}.spv")
                exec_program(${glslc_exec} ${SHADERS_DIR}/spirv ARGS "-fshader-stage=geom ${shader} -o ${shader_name}.spv")
			endif()

            string(FIND ${shader_name} "Tess" isTessShader)
			if(NOT isTessShader EQUAL -1)
				string(FIND ${shader_name} "Eval" isTessEvalShader)
                if(NOT isTessEvalShader EQUAL -1)
                    message("Compiling tesselation evaluation shader ${shader_name} inside of ${shader}")
                    # exec_program(${glslc_exec} ${CMAKE_CURRENT_BINARY_DIR} ARGS "-fshader-stage=tesseval ${shader} -o ${shader_name}.spv")
                    exec_program(${glslc_exec} ${SHADERS_DIR}/spirv ARGS "-fshader-stage=tesseval ${shader} -o ${shader_name}.spv")
                else()
                    message("Compiling tesselation control shader ${shader_name} inside of ${shader}")
                    # exec_program(${glslc_exec} ${CMAKE_CURRENT_BINARY_DIR} ARGS "-fshader-stage=tesscontrol ${shader} -o ${shader_name}.spv")
                    exec_program(${glslc_exec} ${SHADERS_DIR}/spirv ARGS "-fshader-stage=tesscontrol ${shader} -o ${shader_name}.spv")
                endif()
			endif()
        endforeach()
    endif()
endif()

configure_file("${CMAKE_SOURCE_DIR}/support/support_def.h.in" "${CMAKE_SOURCE_DIR}/support/support_def.h")

# Install Procedures

install(TARGETS CORELIB GEOLIB ${engine_targets}
    EXPORT Topl
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    FRAMEWORK DESTINATION lib
)

file(GLOB_RECURSE interfaceHeaders 
    ${CMAKE_CURRENT_SOURCE_DIR}/core/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/physics*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*h
)

install(FILES ${interfaceHeaders} DESTINATION include/Topl)
install(EXPORT Topl DESTINATION lib/Topl FILE ToplConfig.cmake)