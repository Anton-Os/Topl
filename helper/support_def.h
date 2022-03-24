// #define PARENT_DIR "C:/AntonDocs/Codex/Ao-Project/Topl/MSVC_BUILD_2/Debug/" // Change Debug directory to $(Configuration)
#define ASSETS_DIR "C:/AntonDocs/Codex/Ao-Project/Topl/master/assets/"
#define SHADERS_DIR "C:/AntonDocs/Codex/Ao-Project/Topl/master/shaders/"
#if 1
    #define RASTERON_ENABLE_PLUGIN
    #define RASTERON_ENABLE_FONT
    
    #include "Rasteron.h" // includes Rasteron main modules
#endif
#if 1
    #include <assimp/Importer.hpp>      // C++ importer interface
    #include <assimp/scene.h>           // Output data structure
    #include <assimp/postprocess.h>     // Post processing flags
#endif
