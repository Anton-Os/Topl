# Runtime Libraries
# LRTLIB carries everything the legacy demos in demos/legacy need, MRTLIB carries the shared modern runtime
# (Topl_Runtime.hpp and Topl_Runtime.cpp) that every demo in demos/modern builds on

add_library(LRTLIB # Legacy Runtime
    demos/Topl_Factory.cpp
    demos/legacy/Topl_Demo.cpp
    demos/legacy/Topl_Demo_Loop.cpp
    demos/legacy/Topl_Demo_Pipelines.cpp
)

add_library(MRTLIB # Modern Runtime
    demos/Topl_Factory.cpp
    demos/modern/Topl_Runtime.cpp
)

list(APPEND runtimes LRTLIB MRTLIB)

foreach(rt IN ITEMS ${runtimes})
    target_include_directories(${rt} PRIVATE core support geometry shaders demos)
    set_target_properties(${rt} PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS ON) # Topl_Factory.cpp uses generic lambdas
    set_target_properties(${rt} PROPERTIES FOLDER "Runtime Libraries")
    target_link_libraries(${rt} PUBLIC GLEW_lib)
    if(Vulkan_FOUND)
        target_include_directories(${rt} PUBLIC ${Vulkan_INCLUDE_DIRS})
        target_link_libraries(${rt} PUBLIC ${Vulkan_LIBRARIES})
    endif()

    if(WIN32)
        target_include_directories(${rt} PRIVATE "${RESOURCE_DIR}") # resource file header
        target_sources(${rt} PRIVATE "${RESOURCE_FILE}") # resource file compilation
    elseif(UNIX AND NOT APPLE) # Unix Specific
        target_link_libraries(${rt} PUBLIC ${X11_LIBRARIES})
    endif()

    if(SUPPORT_TEXTURES AND Rasteron_FOUND) # linking CORELIB to Rasteron
        target_link_libraries(${rt} PUBLIC Rasteron)
    endif()

    if(SUPPORT_AUDIO) # legacy demos and every modern demo play or capture audio
        target_include_directories(${rt} PUBLIC ${miniaudio_h})
        target_link_libraries(${rt} PUBLIC ${miniaudio_lib})
        target_include_directories(${rt} PUBLIC ${EXTERNAL_PROJ_DIR}/kissfft/src)
    endif()
endforeach()

# The modern runtime carries the engine so modern demos only link MRTLIB
target_include_directories(MRTLIB PRIVATE demos/modern)
target_link_libraries(MRTLIB PUBLIC CORELIB)
