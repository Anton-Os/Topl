if(NOT DEFINED IGNORE_NATIVELIBS)

if(UNIX AND NOT APPLE) # Unix Specific
    include(FindX11)
endif()

# -------------- Core Library ------------- #

list(APPEND support_src
	support/IO.cpp
	support/Controls.cpp
    support/Timer.cpp
    support/Platform.cpp
    # core/File3D.cpp
)

list(APPEND core_src
    core/Topl_Factory.cpp
    core/renderer/Topl_Renderer.cpp
    core/program/Topl_Program.cpp
    core/program/Topl_Program_Timeline.cpp
    core/program/Topl_Program_Runner.cpp
    core/scene/Topl_Scene.cpp
)

if(SUPPORT_PHYSICS)
    list(APPEND core_src core/scene/Topl_Scene_Dynamics.cpp)
endif()

add_library(CORELIB # SHARED # Core Library
    ${support_src}
    ${core_src}
)
target_include_directories(CORELIB PRIVATE core support geometry shaders)
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

endif() # IGORE_NATIVELIBS

# -------------- Geometry Library ------------- #

list(APPEND geometry_src
	# meshes
    geometry/Geo_Vertex.cpp
    geometry/Geo_Mesh.cpp
    geometry/meshes/Geo_Surface.cpp
    geometry/meshes/Geo_Volume.cpp
    geometry/meshes/Geo_Cone.cpp
    geometry/meshes/Geo_Orboid.cpp
    geometry/meshes/Geo_Fractal.cpp
)
if(SUPPORT_MODELS AND Assimp_FOUND)
    list(APPEND geometry_src geometry/meshes/Geo_Node.cpp)
endif()

add_library(GEOLIB SHARED ${geometry_src}) # Geometry Library
target_include_directories(GEOLIB PRIVATE core support geometry)
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


# -------------- Shared Linking ------------- #


if(SUPPORT_TEXTURES AND Rasteron_FOUND) # linking targets to Rasteron
    if(NOT DEFINED IGNORE_NATIVELIBS)
        target_link_libraries(CORELIB PUBLIC Rasteron)
    endif()
    # target_include_directories(GEOLIB PRIVATE ${Rasteron_Install_Path}/include/Rasteron)
    target_link_libraries(GEOLIB PUBLIC Rasteron)
endif()