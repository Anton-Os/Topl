# -------------- OpenGL Engine ------------- #

add_library(GL4_ENGINE
    core/backends/opengl/Topl_Pipeline_GL4.cpp
    core/backends/opengl/Topl_Renderer_GL4.cpp
)
target_include_directories(GL4_ENGINE PRIVATE core support geometry)
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

# -------------- DirectX11 Engine ------------- #

if(WIN32)
    add_library(DX11_ENGINE
        core/backends/directx/Topl_Pipeline_DX11.cpp
        core/backends/directx/Topl_Renderer_DX11.cpp
    )
    target_include_directories(DX11_ENGINE PRIVATE core support geometry)
    target_include_directories(DX11_ENGINE INTERFACE
        $<BUILD_INTERFACE:${interfaceHeaders}>
        $<INSTALL_INTERFACE:include/Topl>
    )
    set_target_properties(DX11_ENGINE PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS ON)
endif()

# -------------- Vulkan Engine ------------- #

if(Vulkan_FOUND)
    add_library(VK_ENGINE
        core/backends/vulkan/Topl_Pipeline_VK.cpp
        core/backends/vulkan/Topl_Renderer_VK.cpp
    )
    target_include_directories(VK_ENGINE PUBLIC ${Vulkan_INCLUDE_DIRS})
    target_include_directories(VK_ENGINE PRIVATE core support geometry)
    target_include_directories(VK_ENGINE INTERFACE
        $<BUILD_INTERFACE:${interfaceHeaders}>
        $<INSTALL_INTERFACE:include/Topl>
    )
    target_link_libraries(VK_ENGINE PUBLIC ${Vulkan_LIBRARIES})
    # set_target_properties(VK_ENGINE PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS ON)
endif()


# -------------- Shared Linking ------------- #

list(APPEND engine_targets GL4_ENGINE) # OpenGL available cross-platform
if(WIN32)
    list(APPEND engine_targets DX11_ENGINE) # DirectX available on windows
endif()
if(Vulkan_FOUND)
    list(APPEND engine_targets VK_ENGINE) # VK available with library
endif()

foreach(target IN ITEMS ${engine_targets})
    if(WIN32)
        target_include_directories(${target} PRIVATE "${RESOURCE_DIR}")
    endif()
    target_include_directories(${target} PRIVATE "${CMAKE_INSTALL_PREFIX}/include")
    target_link_libraries(${target} PUBLIC GEOLIB CORELIB) # link both corelib and geolib
	# target_link_libraries(${target} PUBLIC GEOLIB) # link only geolib
	target_link_libraries(CORELIB PUBLIC ${target})
endforeach()
