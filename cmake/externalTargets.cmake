include(ExternalProject)

if(SUPPORT_TEXTURES)
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
endif()

#TODO: Build Rasteron from Submodle or from Git if detected

ExternalProject_Add(GLEW
        GIT_REPOSITORY "https://github.com/Anton-Os/GLEW.git"
        GIT_TAG "e9aa3dd0e3e82b4fa656d282ea1f652cf6575296"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/GLEW
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    )

find_package(GLEW PATHS ${CMAKE_INSTALL_PREFIX}/lib/GLEW)

if(SUPPORT_MODELS)
ExternalProject_Add(Assimp # 3D Model loading
        GIT_REPOSITORY "https://github.com/assimp/assimp.git"
        GIT_TAG "2d2889f73fa1b2ca09ba9f43c9785402d3a7fdd"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX} -DBUILD_SHARED_LIBS:BOOL=OFF

        PREFIX ${EXTERNAL_PROJ_DIR}/Assimp
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    )
endif()

if(NOT SUPPORT_MODELS)
    set(Assimp_FOUND 0) # override variable to not found
else()
    find_package(Assimp PATHS ${CMAKE_INSTALL_PREFIX}/lib/cmake/assimp-5.0)
endif()

# set(SUPPORT_SAVES ${IS_FALSE} CACHE INT "Allow saving capabilities in .topl format" FORCE)
# if(SUPPORT_SAVES)   
#    message(STATUS, "Saves supported")
#    # TODO: Add Google Protobuff?
# endif()

# set(SUPPORT_MEDIA OFF CACHE BOOL "Include audio module" FORCE)
# if(SUPPORT_MEDIA)
# ExternalProject_Add(OpenAL # Change to FFMPEG?
#        GIT_REPOSITORY "https://github.com/kcat/openal-soft.git"
#        GIT_TAG "6406cc614130ba5f04555ba46e849c685ae6eae0"

#        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

#        PREFIX ${EXTERNAL_PROJ_DIR}/OpenAL
#        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
#    )
# endif()