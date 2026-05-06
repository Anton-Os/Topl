include(ExternalProject)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(NOT DEFINED IGNORE_NATIVELIBS)
ExternalProject_Add(GLEW
    GIT_REPOSITORY "https://github.com/Anton-Os/GLEW.git"
    GIT_TAG "e9aa3dd0e3e82b4fa656d282ea1f652cf6575296"

    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

    PREFIX ${EXTERNAL_PROJ_DIR}/GLEW
    INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
)
find_package(GLEW PATHS ${CMAKE_INSTALL_PREFIX}/lib/GLEW)
endif()

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
else()
    set(Rasteron_FOUND FALSE)
endif()

#TODO: Build Rasteron from Submodle or from Git if detected

if(SUPPORT_MODELS)
    ExternalProject_Add(ASSIMP # 3D Model loading
        GIT_REPOSITORY "https://github.com/assimp/assimp.git"
        GIT_TAG "2d2889f73fa1b2ca09ba9f43c9785402d3a7fdd"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX} -DBUILD_SHARED_LIBS:BOOL=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5

        PREFIX ${EXTERNAL_PROJ_DIR}/Assimp
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    )
    find_package(Assimp PATHS ${CMAKE_INSTALL_PREFIX}/lib/cmake/assimp-5.0)
    if(Assimp_FOUND)
        message(STATUS "Assimp loading success!")
    else()
        message(WARNING "Assimp loading failure, 3D models support disabled")
    endif()
else()
    set(Assimp_FOUND FALSE)
endif()

if(SUPPORT_AUDIO)
    ExternalProject_Add(JUCE
        GIT_REPOSITORY "https://github.com/juce-framework/JUCE.git"
        GIT_TAG "501c07674e1ad693085a7e7c398f205c2677f5da"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/JUCE
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    )

    find_package(JUCE PATHS ${CMAKE_INSTALL_PREFIX}/lib/JUCE)
    if(JUCE_FOUND)
        message(STATUS "JUCE loading success!")
    else()
        message(WARNING "JUCE loading failure, audio playback non-functional")
    endif()
endif()

# set(SUPPORT_SAVES ${IS_FALSE} CACHE INT "Allow saving capabilities in .topl format" FORCE)
# if(SUPPORT_SAVES)   
#    message(STATUS, "Saves supported")
#    # TODO: Add Google Protobuff?
# endif()
