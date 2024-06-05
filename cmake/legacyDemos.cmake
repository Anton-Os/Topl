# DirectX Demos

if(WIN32)
    add_executable(Legacy_DX11_Pawns
        geometry/constructs/Humanoid.cpp

        demos/legacy/DX11_Pawns.cpp
    )
    target_link_libraries(Legacy_DX11_Pawns PRIVATE DX11_ENGINE)

    add_executable(Legacy_DX11_Planetary demos/legacy/DX11_Planetary.cpp)
	target_link_libraries(Legacy_DX11_Planetary PRIVATE DX11_ENGINE)
    
    add_executable(Legacy_DX11_Surface demos/legacy/DX11_Surface.cpp)
	target_link_libraries(Legacy_DX11_Surface PRIVATE DX11_ENGINE)

    add_executable(Legacy_DX11_Overlay 
        geometry/constructs/Pane.cpp

        demos/legacy/DX11_Overlay.cpp
    )
	target_link_libraries(Legacy_DX11_Overlay PRIVATE DX11_ENGINE)
endif()

# OpenGL Demos

add_executable(Legacy_GL4_Pawns
    geometry/constructs/Humanoid.cpp

    demos/legacy/GL4_Pawns.cpp
)
target_link_libraries(Legacy_GL4_Pawns PRIVATE GL4_ENGINE)

add_executable(Legacy_GL4_Planetary demos/legacy/GL4_Planetary.cpp)
target_link_libraries(Legacy_GL4_Planetary PRIVATE GL4_ENGINE)

add_executable(Legacy_GL4_Surface demos/legacy/GL4_Surface.cpp)
target_link_libraries(Legacy_GL4_Surface PRIVATE GL4_ENGINE)

add_executable(Legacy_GL4_Overlay
    geometry/constructs/Pane.cpp

    demos/legacy/GL4_Overlay.cpp
)
target_link_libraries(Legacy_GL4_Overlay PRIVATE GL4_ENGINE)