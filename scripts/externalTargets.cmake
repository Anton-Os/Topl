include(ExternalProject)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(pending_depends "") # dependencies that are requested but not built and installed yet, see CMakeLists.txt
set(depend_targets "") # targets that build and install the dependencies

if(IS_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib64)
    set(EXTERNAL_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib64)
else()
    set(EXTERNAL_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)
endif()

if(NOT DEFINED IGNORE_NATIVELIBS)
ExternalProject_Add(GLEW
    GIT_REPOSITORY "https://github.com/Anton-Os/GLEW.git"
    GIT_TAG "e9aa3dd0e3e82b4fa656d282ea1f652cf6575296"

    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

    PREFIX ${EXTERNAL_PROJ_DIR}/GLEW
    INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
)
set_target_properties(GLEW PROPERTIES FOLDER "Dependencies")

list(APPEND depend_targets GLEW)

find_package(GLEW PATHS ${CMAKE_INSTALL_PREFIX}/lib/GLEW)
if(NOT TARGET GLEW_lib)
    list(APPEND pending_depends GLEW)
endif()
endif()

# Rasteron, Geometron and Soundscape are built from their submodules, see submoduleTargets.cmake and internalTargets.cmake

if(SUPPORT_MODELS)
    ExternalProject_Add(ASSIMP # 3D Model loading and exporting for Geometron
        GIT_REPOSITORY "https://github.com/assimp/assimp.git"
        GIT_TAG "f6ee58e2489f3cbaee328655599f50c5c386a08e"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX} -DCMAKE_POLICY_VERSION_MINIMUM=3.5
            -DASSIMP_BUILD_TESTS=OFF # unit tests take most of the build time

        PREFIX ${EXTERNAL_PROJ_DIR}/Assimp
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    )
    set_target_properties(ASSIMP PROPERTIES FOLDER "Dependencies")
    list(APPEND depend_targets ASSIMP)

    set(assimp_paths ${CMAKE_INSTALL_PREFIX}/lib/scripts/assimp-6.0 ${EXTERNAL_LIB_DIR}/cmake/assimp-6.0 ${CMAKE_INSTALL_PREFIX}/lib/cmake/assimp-6.0)
    find_package(assimp PATHS ${assimp_paths} NO_DEFAULT_PATH)
    if(assimp_FOUND)
        message(STATUS "Assimp loading success!")
    else()
        list(APPEND pending_depends Assimp)
    endif()
    set(Assimp_FOUND ${assimp_FOUND})
else()
    set(Assimp_FOUND FALSE)
endif()

# set(SUPPORT_SAVES ${IS_FALSE} CACHE INT "Allow saving capabilities in .topl format" FORCE)
# if(SUPPORT_SAVES)
#    message(STATUS, "Saves supported")
#    # TODO: Add Google Protobuff?
# endif()
