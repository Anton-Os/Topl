# Building Legacy  Demos

if(SUPPORT_TEXTURES AND Rasteron_FOUND) # linking CORELIB to Rasteron
    target_link_libraries(CORELIB PUBLIC Rasteron)
endif()

add_executable(Hello demos/legacy/hello/Hello.cpp)
add_executable(Molecular demos/legacy/molecular/Molecular.cpp)
add_executable(Meshform demos/legacy/meshform/Meshform.cpp)
add_executable(Penscape demos/legacy/penscape/Penscape.cpp)
add_executable(Kaleidoscope demos/legacy/kaleidoscope/Kaleidoscope.cpp)
add_executable(Animatrix demos/legacy/animatrix/Animatrix.cpp)
add_executable(Genesis demos/legacy/genesis/Genesis.cpp)
add_executable(Traversal demos/legacy/traversal/Traversal.cpp)
add_executable(Cubegate demos/legacy/cubegate/Cubegate.cpp)

list(APPEND demo_targets Hello) # Sandbox Molecular Meshform Penscape Kaleidoscope Animatrix Genesis)
list(APPEND demo_targets Molecular Meshform Penscape Kaleidoscope Animatrix Genesis Traversal Cubegate)

foreach(target IN ITEMS ${demo_targets})
    target_include_directories(${target} PRIVATE core support geometry shaders demos demos/legacy)
    target_link_libraries(${target} PRIVATE CORELIB RTLIB)
    if(WIN32) # adding resource file to demos
        target_include_directories(${target} PRIVATE "${RESOURCE_DIR}")
        target_sources(${target} PRIVATE "${RESOURCE_FILE}")
    endif()
    set_target_properties(${target} PROPERTIES FOLDER "Starters Demos")
endforeach()

if(SUPPORT_AUDIO)
    target_include_directories(Hello PUBLIC ${miniaudio_h})
    target_link_libraries(Hello PUBLIC ${miniaudio_lib})
    target_include_directories(Hello PUBLIC ${EXTERNAL_PROJ_DIR}/kissfft/src)
endif()


# TODO: add Audio & Visual Demos