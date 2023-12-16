#define ASSETS_DIR "F:/Codex/PROJECT/Topl/master/assets/"
#define FONTS_DIR "F:/Codex/PROJECT/Topl/master/assets/fonts/"
#define IMAGES_DIR "F:/Codex/PROJECT/Topl/master/assets/images/"
#define MODELS_DIR "F:/Codex/PROJECT/Topl/master/assets/models/"
#define RESOURCE_DIR "F:/Codex/PROJECT/Topl/master/assets/res/"
#define SHADERS_DIR "F:/Codex/PROJECT/Topl/master/shaders/"

#if TRUE
    #define TOPL_ENABLE_VULKAN
#endif
#if 1
    #define RASTERON_ENABLE_ANIM
    #define RASTERON_ENABLE_FONT
    
    #include "Rasteron.h" // includes Rasteron main modules and features
    // #include "Experimental.h" // included Rasteron experimental modules
#endif
#if 0
	#define TOPL_ENABLE_MODELS
#endif
#if TRUE
    #define TOPL_ENABLE_PHYSICS
#endif
