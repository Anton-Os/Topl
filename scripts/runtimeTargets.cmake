add_library(RTLIB # Legacy Runtime Library
    demos/Topl_Factory.cpp
    demos/legacy/Topl_Demo.cpp
    demos/legacy/Topl_Demo_Loop.cpp
    demos/legacy/Topl_Demo_Pipelines.cpp
)

# add_library(VSRTLIB # Visual Runtime Library
    # demos/Topl_Factory.cpp
    # demos/visual/Topl_Visual_Demo.cpp
# )
# add_library(AVRTLIB # Audio-Visual Runtime Library
    # demos/Topl_Factory.cpp
    # demos/audiovisual/Topl_AudioVisual_Demo.cpp
# )

target_include_directories(RTLIB PRIVATE core support geometry shaders demos)
set_target_properties(RTLIB PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS ON)
set_target_properties(RTLIB PROPERTIES FOLDER "Runtime Libraries")
if(WIN32) # adding resource file to demos
        target_include_directories(RTLIB PRIVATE "${RESOURCE_DIR}")
        target_sources(RTLIB PRIVATE "${RESOURCE_FILE}")
    endif()
target_link_libraries(RTLIB PUBLIC GLEW_lib)
if(Vulkan_FOUND)
    target_include_directories(RTLIB PUBLIC ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(RTLIB PUBLIC ${Vulkan_LIBRARIES})
endif()

if(WIN32)
    target_include_directories(RTLIB PRIVATE "${RESOURCE_DIR}") # resource file header
    target_sources(RTLIB PRIVATE "${RESOURCE_FILE}") # resource file compilation
elseif(UNIX AND NOT APPLE) # Unix Specific
    target_link_libraries(RTLIB PUBLIC ${X11_LIBRARIES})
endif()