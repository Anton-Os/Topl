#define ASSETS_DIR "/home/antonos/Documents/Codex/Topl_New/assets/"
#define FONTS_DIR "/home/antonos/Documents/Codex/Topl_New/assets/fonts/"
#define IMAGES_DIR "/home/antonos/Documents/Codex/Topl_New/assets/images/"
#define MODELS_DIR "/home/antonos/Documents/Codex/Topl_New/assets/models/"
#define RESOURCE_DIR "/home/antonos/Documents/Codex/Topl_New/assets/res/"
#define SHADERS_DIR "/home/antonos/Documents/Codex/Topl_New/shaders/"

#define TRUE 1
#define FALSE 0

#if TRUE
    #define TOPL_ENABLE_VULKAN
#endif
#if 0
    #define RASTERON_ENABLE_ANIM // includes animation support
    #define RASTERON_ENABLE_FONT // includes font support
    
    // #include "Toolbox.h"
    #include "Rasteron.h" // includes Rasteron main modules and features
#endif
#if 0
	#define TOPL_ENABLE_MODELS
    // #include <assimp/scene.h>
    // #include <assimp/Importer.hpp>
    // #include <assimp/postprocess.h>
#endif
#if TRUE
    #define TOPL_ENABLE_PHYSICS
#endif
