# DirectX Demos

if(WIN32)
    add_executable(Drx11_Pawns
        geometry/trees/Humanoid.cpp

        demos/legacy/Drx11_Pawns.cpp
    )
    target_link_libraries(Drx11_Pawns PRIVATE Drx11_ENGINE)

	add_executable(Drx11_Blocks
		geometry/trees/Chain.cpp
		geometry/trees/Grid.cpp

		demos/legacy/Drx11_Blocks.cpp
	)
	target_link_libraries(Drx11_Blocks PRIVATE Drx11_ENGINE)

    add_executable(Drx11_Planetary demos/legacy/Drx11_Planetary.cpp)
	target_link_libraries(Drx11_Planetary PRIVATE Drx11_ENGINE)
    
    add_executable(Drx11_Surface demos/legacy/Drx11_Surface.cpp)
	target_link_libraries(Drx11_Surface PRIVATE Drx11_ENGINE)

    add_executable(Drx11_Overlay 
        geometry/trees/Pane.cpp

        demos/legacy/Drx11_Overlay.cpp
    )
	target_link_libraries(Drx11_Overlay PRIVATE Drx11_ENGINE)

    add_executable(Drx11_Assets demos/legacy/Drx11_Assets.cpp)
	target_link_libraries(Drx11_Assets PRIVATE Drx11_ENGINE)
endif()

# OpenGL Demos

add_executable(GL4_Pawns
    geometry/trees/Humanoid.cpp

    demos/legacy/GL4_Pawns.cpp
)
target_link_libraries(GL4_Pawns PRIVATE GL4_ENGINE)

add_executable(GL4_Blocks
	geometry/trees/Chain.cpp
    geometry/trees/Grid.cpp

    demos/legacy/GL4_Blocks.cpp
)
target_link_libraries(GL4_Blocks PRIVATE GL4_ENGINE)

add_executable(GL4_Planetary demos/legacy/GL4_Planetary.cpp)
target_link_libraries(GL4_Planetary PRIVATE GL4_ENGINE)

add_executable(GL4_Surface demos/legacy/GL4_Surface.cpp)
target_link_libraries(GL4_Surface PRIVATE GL4_ENGINE)

add_executable(GL4_Overlay
    geometry/trees/Pane.cpp

    demos/legacy/GL4_Overlay.cpp
)
target_link_libraries(GL4_Overlay PRIVATE GL4_ENGINE)

add_executable(GL4_Assets demos/legacy/GL4_Assets.cpp)
target_link_libraries(GL4_Assets PRIVATE GL4_ENGINE)