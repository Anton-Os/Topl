#ifndef TOPL_SUPPORT_H

#define ASSETS_DIR "D:/Codex/PROJECT/Topl/master/assets/"
#define FONTS_DIR "D:/Codex/PROJECT/Topl/master/assets/fonts/"
#define IMAGES_DIR "D:/Codex/PROJECT/Topl/master/assets/images/"
#define MODELS_DIR "D:/Codex/PROJECT/Topl/master/assets/models/"
#define RESOURCE_DIR "D:/Codex/PROJECT/Topl/master/assets/res/"
#define SHADERS_DIR "D:/Codex/PROJECT/Topl/master/shaders/"

#define TRUE 1
#define FALSE 0
#define ON 1
#define OFF 0

#if TRUE
    #define TOPL_ENABLE_VULKAN
#endif
#if 0 && !defined(__ANDROID__) && !defined(IGNORE_RASTER)
    #define TOPL_ENABLE_TEXTURES
    #define RASTERON_ENABLE_SPACE
    #define RASTERON_ENABLE_ANIM // includes animation support
    #define RASTERON_ENABLE_FONT // includes font support
    #include "Rasteron.h" // main library for Rasteron
    #include "Loader.h" // loader library for Rasteron
#endif
#if 0
	#define TOPL_ENABLE_MODELS
    // #include <assimp/scene.h>
    // #include <assimp/Importer.hpp>
    // #include <assimp/postprocess.h>
#endif
#if ON
    #define TOPL_ENABLE_PHYSICS
#endif

#ifdef _WIN32
#define DllExport __declspec(dllexport)
#elif defined(__APPLE__)
#define DllExport __attribute__(( visibility("default") ))
#else
#define DllExport
#endif

#define TOPL_SUPPORT_H
#endif
