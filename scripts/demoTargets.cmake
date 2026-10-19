# Building Legacy  Demos

if(SUPPORT_TEXTURES AND Rasteron_FOUND) # linking CORELIB to Rasteron
    target_link_libraries(CORELIB PUBLIC Rasteron)
endif()

add_executable(Hello demos/Hello.cpp)

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
    target_include_directories(${target} PRIVATE core support geometry shaders demos demos/legacy ${CMAKE_SOURCE_DIR})
    target_link_libraries(${target} PRIVATE CORELIB LRTLIB)
    if(WIN32) # adding resource file to demos
        target_include_directories(${target} PRIVATE "${RESOURCE_DIR}")
        target_sources(${target} PRIVATE "${RESOURCE_FILE}")
    endif()
    set_target_properties(${target} PROPERTIES FOLDER "Legacy Demos")
    if(SUPPORT_TEXTURES AND Rasteron_FOUND) # linking CORELIB to Rasteron
        target_link_libraries(${target} PUBLIC Rasteron)
    endif()
endforeach()

if(SUPPORT_AUDIO)
    target_include_directories(Hello PUBLIC ${miniaudio_h})
    target_link_libraries(Hello PUBLIC ${miniaudio_lib})
    target_include_directories(Hello PUBLIC ${EXTERNAL_PROJ_DIR}/kissfft/src)
endif()


# Building Modern Demos
# Each demo compiles its own class next to its entry point and links the shared modern runtime

add_executable(Visualizer demos/modern/visual/Visualizer.cpp demos/modern/visual/Topl_Visualizer.cpp)
add_executable(Spacetime demos/modern/spacetime/Spacetime.cpp demos/modern/spacetime/Topl_Spacetime_Visualizer.cpp)
add_executable(Audiovisual demos/modern/audiovisual/Audiovisual.cpp demos/modern/audiovisual/Topl_Audio_Visualizer.cpp)
add_executable(Ragdolls demos/modern/ragdolls/Ragdolls.cpp demos/modern/ragdolls/Topl_Ragdolls.cpp)
add_executable(Audiostream demos/modern/audiostream/Audiostream.cpp demos/modern/audiostream/Topl_Audiostream.cpp)
add_executable(Fractile demos/modern/fractile/Fractile.cpp demos/modern/fractile/Topl_Fractile.cpp)
add_executable(Torspin demos/modern/torspin/Torspin.cpp demos/modern/torspin/Topl_Torspin.cpp)
add_executable(Planetos demos/modern/planetos/Planetos.cpp demos/modern/planetos/Topl_Planetos.cpp)

list(APPEND modern_demo_targets Visualizer Spacetime Audiovisual Ragdolls Audiostream Fractile Torspin Planetos)

foreach(target IN ITEMS ${modern_demo_targets})
    target_include_directories(${target} PRIVATE core support geometry shaders demos demos/modern ${CMAKE_SOURCE_DIR})
    target_link_libraries(${target} PRIVATE MRTLIB)
    set_target_properties(${target} PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED ON)
    if(WIN32) # adding resource file to demos
        target_include_directories(${target} PRIVATE "${RESOURCE_DIR}")
        target_sources(${target} PRIVATE "${RESOURCE_FILE}")
    endif()
    set_target_properties(${target} PROPERTIES FOLDER "Modern Demos")
    if(SUPPORT_TEXTURES AND Rasteron_FOUND)
        target_link_libraries(${target} PUBLIC Rasteron)
    endif()
endforeach() # audio support reaches every modern demo through MRTLIB
