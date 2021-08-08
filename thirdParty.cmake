# Trying to use as few modules as possible but these are worth considering in the near future

ExternalProject_Add(Assimp # 3D Model loading
        GIT_REPOSITORY "https://github.com/assimp/assimp.git"
        GIT_TAG "2d2889f73fa1b2ca09ba9f43c9785402d3a7fdd"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/Assimp
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    )

ExternalProject_Add(OpenAL # Audio File Loading
        GIT_REPOSITORY "https://github.com/kcat/openal-soft.git"
        GIT_TAG "6406cc614130ba5f04555ba46e849c685ae6eae0"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/OpenAL
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    )

ExternalProject_Add(FreeType # Font Loading
        GIT_REPOSITORY "https://gitlab.freedesktop.org/freetype/freetype.git"
        GIT_TAG "801cd842e27c85cb1d5000f6397f382ffe295daa"

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

        PREFIX ${EXTERNAL_PROJ_DIR}/FreeType
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    )