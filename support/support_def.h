#ifndef TOPL_SUPPORT_H

#define ASSETS_DIR "Z:/PROJECT/Topl/master/assets/"
#define FONTS_DIR "Z:/PROJECT/Topl/master/assets/fonts/"
#define IMAGES_DIR "Z:/PROJECT/Topl/master/assets/images/"
#define MODELS_DIR "Z:/PROJECT/Topl/master/assets/models/"
#define RESOURCE_DIR "Z:/PROJECT/Topl/master/assets/res/"
#define AUDIO_DIR "Z:/PROJECT/Topl/master/assets/audio/"
#define SHADERS_DIR "Z:/PROJECT/Topl/master/shaders/"

#define TRUE 1
#define FALSE 0
#define ON 1
#define OFF 0

#if TRUE
    #define TOPL_ENABLE_VULKAN
#endif
#if 1 && !defined(__ANDROID__) && !defined(IGNORE_RASTER)
    #define RASTERON_ENABLE_QUEUE TRUE
    #define RASTERON_ENABLE_SPACE TRUE
    #define RASTERON_ENABLE_FONT TRUE
    #include "Rasteron.h" // main library for Rasteron
    #include "Loader.h" // loader library for Rasteron
    #define TOPL_ENABLE_TEXTURES
#endif
#if 1
    // #include <assimp/scene.h>
    // #include <assimp/Importer.hpp>
    // #include <assimp/postprocess.h>
    #define TOPL_ENABLE_MODELS
#endif
#if ON
    #define TOPL_ENABLE_PHYSICS
#endif
/* #if 
    #define TOPL_ENABLE_CUDA
#endif */
#if ON
#ifndef TOPL_ENABLE_AUDIO // ensures only one audio library is enabled
    // #include <miniaudio/miniaudio.h> // Audio Library
    // #include <kissfft/kiss_fft.h> // FFT Library
    // #include <kissfft/kiss_fftr.h> // FFT Library
#define TOPL_ENABLE_AUDIO
#endif
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
