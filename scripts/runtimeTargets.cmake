add_library(RTLIB # Legacy Demos
    demos/Topl_Factory.cpp
    demos/legacy/Topl_Demo.cpp
    demos/legacy/Topl_Demo_Loop.cpp
    demos/legacy/Topl_Demo_Pipelines.cpp
)

add_library(VRTLIB # Visual Runtime
    demos/modern/Topl_Runtime.cpp
    demos/modern/visual/Topl_Visualizer.cpp
)
add_library(SVRTLIB # Spacetime Runtime
    demos/modern/Topl_Runtime.cpp
    demos/modern/spacetime/Topl_Spacetime_Visualizer.cpp
)
add_library(AVRTLIB # Audio-Visual Runtime
    demos/modern/Topl_Runtime.cpp
    demos/modern/audiovisual/Topl_Audio_Visualizer.cpp
)

list(APPEND runtimes
    RTLIB
    VRTLIB
    SVRTLIB
    AVRTLIB
)

foreach(rt IN ITEMS ${runtimes})
    target_include_directories(${rt} PRIVATE core support geometry shaders demos)
    set_target_properties(${rt} PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS ON)
    set_target_properties(${rt} PROPERTIES FOLDER "Runtime Libraries")
    if(WIN32) # adding resource file to demos
        target_include_directories(${rt} PRIVATE "${RESOURCE_DIR}")
        target_sources(${rt} PRIVATE "${RESOURCE_FILE}")
    endif()
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
endforeach()

if(SUPPORT_AUDIO)
    target_include_directories(RTLIB PUBLIC ${miniaudio_h})
    target_link_libraries(RTLIB PUBLIC ${miniaudio_lib})
    target_include_directories(RTLIB PUBLIC ${EXTERNAL_PROJ_DIR}/kissfft/src)
    target_include_directories(AVRTLIB PUBLIC ${miniaudio_h})
    target_link_libraries(AVRTLIB PUBLIC ${miniaudio_lib})
    target_include_directories(AVRTLIB PUBLIC ${EXTERNAL_PROJ_DIR}/kissfft/src)
endif()
