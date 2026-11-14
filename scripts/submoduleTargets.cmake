# Builds Rasteron and Soundscape from the sources inside of their submodules
#
#   Each project is configured, built and installed by its own CMake scripts into CMAKE_INSTALL_PREFIX,
#   the engine then locates the installed package the same way as any other dependency.
#   Options for a project are passed through CMAKE_ARGS, sources are tracked so that a change inside
#   of a submodule rebuilds and installs it with the next build of the engine.
#   Geometron is compiled directly into GEOLIB, see internalTargets.cmake

set(SUBMODULE_BUILD_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/buildSubmodule.cmake")

set(submodule_args -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX})
if(CMAKE_BUILD_TYPE)
    list(APPEND submodule_args -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE})
endif()

function(add_submodule target sourceDir package isBootstrap) # remaining arguments are passed to the project as CMAKE_ARGS
    set(prefix "${EXTERNAL_PROJ_DIR}/${package}")

    ExternalProject_Add(${target}
        SOURCE_DIR ${sourceDir}
        PREFIX ${prefix}
        BINARY_DIR ${prefix}/build
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}

        CMAKE_ARGS ${submodule_args} ${ARGN}

        BUILD_COMMAND ${CMAKE_COMMAND}
            -DNAME=${target} -DBINARY_DIR=${prefix}/build -DCONFIG=$<CONFIG>
            -DGENERATOR=${CMAKE_GENERATOR} -DBOOTSTRAP=${isBootstrap}
            -P ${SUBMODULE_BUILD_SCRIPT}
    )
    set_target_properties(${target} PROPERTIES FOLDER "Submodules")

    # Steps of an external project are skipped once complete, tracking the sources repeats them after a change
    file(GLOB_RECURSE tracked
        "${sourceDir}/CMakeLists.txt" "${sourceDir}/*.cmake" "${sourceDir}/*.in"
        "${sourceDir}/*.c" "${sourceDir}/*.cpp" "${sourceDir}/*.h" "${sourceDir}/*.hpp"
    )
    list(FILTER tracked EXCLUDE REGEX "support_def\\.h$") # generated while configuring
    ExternalProject_Add_Step(${target} sources
        COMMAND ${CMAKE_COMMAND} -E echo "Sources of ${target} changed since its last build"
        DEPENDS ${tracked} ${SUBMODULE_BUILD_SCRIPT}
        DEPENDEES mkdir
        DEPENDERS configure
    )

    # Headers and libraries that are installed already are declared as outputs of the project,
    # which lets Ninja recompile and link whatever depends on them within the same build
    file(GLOB installed "${CMAKE_INSTALL_PREFIX}/include/${package}/*.h")
    foreach(library IN ITEMS ${${package}_LIBRARY_NAMES})
        set(library_file "${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${library}${CMAKE_STATIC_LIBRARY_SUFFIX}")
        if(EXISTS ${library_file})
            list(APPEND installed ${library_file})
        endif()
    endforeach()
    if(installed)
        ExternalProject_Add_Step(${target} publish
            COMMAND ${CMAKE_COMMAND} -E echo "Installed ${package}"
            BYPRODUCTS ${installed}
            DEPENDEES install
        )
    endif()
endfunction()

# -------------- Rasteron ------------- #

if(SUPPORT_TEXTURES AND NOT EXISTS "${RASTERON_DIR}/CMakeLists.txt")
    message(WARNING "Rasteron sources are missing, run: git submodule update --init Rasteron. Texturing and Image modules cannot load!")
    set(Rasteron_FOUND FALSE)
elseif(SUPPORT_TEXTURES)
    # Every option of the Rasteron project is enabled, dependencies are built on a first pass and located on a second
    set(rasteron_options SUPPORT_FONT_BAKING SUPPORT_MEDIA_EXPORT)
    set(rasteron_args "")
    foreach(option IN ITEMS ${rasteron_options})
        list(APPEND rasteron_args -D${option}:BOOL=TRUE)
    endforeach()

    set(Rasteron_LIBRARY_NAMES Rasteron)
    add_submodule(Rasteron_Source ${RASTERON_DIR} Rasteron TRUE ${rasteron_args})
    list(APPEND depend_targets Rasteron_Source)
    set(Rasteron_BINARY_DIR "${EXTERNAL_PROJ_DIR}/Rasteron/build") # demo tools such as TexTool are built here

    find_package(Rasteron PATHS ${CMAKE_INSTALL_PREFIX}/lib/Rasteron NO_DEFAULT_PATH)
    if(Rasteron_FOUND AND EXISTS "${CMAKE_INSTALL_PREFIX}/lib/Rasteron/findDepends.cmake")
        include(${CMAKE_INSTALL_PREFIX}/lib/Rasteron/findDepends.cmake) # image libraries that Rasteron links against
        message(STATUS "Rasteron loading success!")
    else()
        list(APPEND pending_depends Rasteron)
    endif()
else()
    set(Rasteron_FOUND FALSE)
endif()

# -------------- Soundscape ------------- #

set(Soundscape_MIDI FALSE)
if(SUPPORT_AUDIO AND NOT EXISTS "${SOUNDSCAPE_DIR}/CMakeLists.txt")
    message(WARNING "Soundscape sources are missing, run: git submodule update --init Soundscape. Audio modules cannot load!")
    set(Soundscape_FOUND FALSE)
elseif(SUPPORT_AUDIO)
    set(Soundscape_LIBRARY_NAMES SDLIB MIDILIB)
    add_submodule(Soundscape_Source ${SOUNDSCAPE_DIR} Soundscape FALSE -DSUPPORT_MIDI:BOOL=${SUPPORT_MIDI})
    list(APPEND depend_targets Soundscape_Source)
    set(Soundscape_BINARY_DIR "${EXTERNAL_PROJ_DIR}/Soundscape/build")

    find_package(Soundscape PATHS ${CMAKE_INSTALL_PREFIX}/lib/Soundscape NO_DEFAULT_PATH)
    if(Soundscape_FOUND)
        # Soundscape.h includes miniaudio and kissfft, which the Soundscape project installs and downloads
        set(Soundscape_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/include" "${Soundscape_BINARY_DIR}/PROJECTS/kissfft/src")
        set(Soundscape_LIBRARIES SDLIB)
        message(STATUS "Soundscape loading success!")

        if(SUPPORT_MIDI AND TARGET MIDILIB)
            list(APPEND Soundscape_LIBRARIES MIDILIB)
            set(Soundscape_MIDI TRUE)
        elseif(SUPPORT_MIDI)
            list(APPEND pending_depends "Soundscape MIDI plugin")
        endif()
    else()
        list(APPEND pending_depends Soundscape)
    endif()
else()
    set(Soundscape_FOUND FALSE)
endif()

# -------------- Pending Dependencies ------------- #

if(pending_depends) # the next build configures again once a dependency has installed
    set(depends_stamp "${CMAKE_BINARY_DIR}/depends.stamp")
    if(NOT EXISTS ${depends_stamp})
        file(WRITE ${depends_stamp} "")
    endif()
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${depends_stamp})

    foreach(target IN ITEMS ${depend_targets})
        ExternalProject_Add_Step(${target} notify
            COMMAND ${CMAKE_COMMAND} -E touch ${depends_stamp}
            DEPENDEES install
        )
    endforeach()
endif()
