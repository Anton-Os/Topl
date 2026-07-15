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
    ExternalProject_Add(miniaudio
        GIT_REPOSITORY "https://github.com/mackron/miniaudio.git"
        GIT_TAG "9634bedb5b5a2ca38c1ee7108a9358a4e233f14d"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/miniaudio
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    )

    ExternalProject_Add(kissfft
        GIT_REPOSITORY "https://github.com/mborgerding/kissfft.git"
        GIT_TAG "6398d8a1d0c92486b5ece8a456fd5e6a97ad1f08"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/kissfft
        CONFIGURE_COMMAND "" # Manual Configure?
        BUILD_COMMAND "" # Manual Build?
        INSTALL_COMMAND "" # Manual Install?
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    )

    # TODO: Check CUDA for cuFFT support?

    set(miniaudio_h "${CMAKE_INSTALL_PREFIX}/include/miniaudio")
    find_library(miniaudio_lib NAMES miniaudio PATHS ${CMAKE_INSTALL_PREFIX}/lib)
    # find_file(kissfft_header NAMES kiss_fft.h PATHS ${EXTERNAL_PROJ_DIR}/kissfft/src/kissfft ${CMAKE_INSTALL_PREFIX}/include/kissfft)
    # find_file(kissfft_src NAMES kiss_fft.c PATHS ${EXTERNAL_PROJ_DIR}/kissfft/src/kissfft)

    message(STATUS "miniaudio library is ${miniaudio_lib} and header is ${miniaudio_h}")
    message(STATUS "kissfft directory is ${EXTERNAL_PROJ_DIR}/kissfft")    
else()
    set(miniaudio_lib "")
    set(miniaudio_h "")
endif()

# set(SUPPORT_SAVES ${IS_FALSE} CACHE INT "Allow saving capabilities in .topl format" FORCE)
# if(SUPPORT_SAVES)   
#    message(STATUS, "Saves supported")
#    # TODO: Add Google Protobuff?
# endif()
