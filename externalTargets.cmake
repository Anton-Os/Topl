if(NOT Rasteron_Install_Path)
    set(Rasteron_Install_Path "${CMAKE_INSTALL_PREFIX}/Rasteron" CACHE PATH "Rasteron install directory")
endif()

find_package(Rasteron PATHS ${Rasteron_Install_Path}/lib/Rasteron)
if(Rasteron_FOUND)
    include(${Rasteron_Install_Path}/lib/Rasteron/findDepends.cmake)
    message(STATUS "Rasteron loading success!")
else()
    message(WARNING "Texturing and Image modules cannot load! Build and install Rasteron submodule and point Rasteron_Install_Path variable to the Rasteron install directory")
endif()


include(ExternalProject)
set(EXTERNAL_PROJ_DIR "${CMAKE_BINARY_DIR}/Projects")

if(WIN32)
ExternalProject_Add(DXSDK
        GIT_REPOSITORY "https://github.com/Anton-Os/dxsdk.git"
        GIT_TAG "5fbd5e1158db72421bc32709136131451e6c8c37"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/dxsdk
    )
endif()

ExternalProject_Add(GLEW
        GIT_REPOSITORY "https://github.com/Anton-Os/GLEW.git"
        GIT_TAG "e9aa3dd0e3e82b4fa656d282ea1f652cf6575296"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/GLEW
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    )

ExternalProject_Add(Eigen
        GIT_REPOSITORY "https://github.com/eigenteam/eigen-git-mirror.git"
        GIT_TAG "36b95962756c1fce8e29b1f8bc45967f30773c00"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/Eigen
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    )

set(SUPPORT_3D_ASSETS true CACHE BOOL "Include 3d asset loading module" FORCE)
if(SUPPORT_3D_ASSETS)
ExternalProject_Add(Assimp # 3D Model loading
        GIT_REPOSITORY "https://github.com/assimp/assimp.git"
        GIT_TAG "2d2889f73fa1b2ca09ba9f43c9785402d3a7fdd"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/Assimp
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    )
endif()

set(SUPPORT_AUDIO false CACHE BOOL "Include audio playback module" FORCE)
if(SUPPORT_AUDIO)
ExternalProject_Add(OpenAL # Audio File Loading
        GIT_REPOSITORY "https://github.com/kcat/openal-soft.git"
        GIT_TAG "6406cc614130ba5f04555ba46e849c685ae6eae0"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/OpenAL
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    )
endif()

if(WIN32)
  find_package(dxsdk_interface PATHS ${CMAKE_INSTALL_PREFIX}/lib/dxsdk)
endif()
find_package(Eigen3 PATHS ${CMAKE_INSTALL_PREFIX}/share/eigen3/cmake)
find_package(GLEW PATHS ${CMAKE_INSTALL_PREFIX}/lib/GLEW)
find_package(Assimp PATHS ${CMAKE_INSTALL_PREFIX}/lib/cmake/assimp-5.0)