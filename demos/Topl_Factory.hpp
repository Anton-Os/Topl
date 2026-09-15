#pragma once
#include <typeinfo>

// Backend-specific renderer includes
#if defined(__ANDROID__)
    #include "../../Droidl_New/app/src/main/cpp/Droidl_Renderer.hpp"
#else
    #include "renderers/opengl/Topl_Renderer_GL4.hpp"
#endif

#if defined(_WIN32)
    #include "renderers/directx/Topl_Renderer_DX11.hpp"
#endif

#if defined(TOPL_ENABLE_VULKAN)
    #include "renderers/vulkan/Topl_Renderer_VK.hpp"
#endif

// -----------------------------------------------------------------------------
// Backend Types
// -----------------------------------------------------------------------------

enum BACKEND_Target {
    BACKEND_GL4,
#if defined(_WIN32)
    BACKEND_DX11,
#endif
#if defined(TOPL_ENABLE_VULKAN)
    BACKEND_VK
#endif
};

// -----------------------------------------------------------------------------
// Engine Config Base
// -----------------------------------------------------------------------------

struct Engine_Config {
    unsigned pipeIndex{};
};

// -----------------------------------------------------------------------------
// Backend-specific Engine Configs
// -----------------------------------------------------------------------------

#if defined(__ANDROID__)

struct Engine_Config_GL4 : Engine_Config {
    Droidl_Renderer* renderer{};
    GL4::Pipeline** pipelines{};
};

#else

struct Engine_Config_GL4 : Engine_Config {
    Topl_Renderer_GL4* renderer{};
    GL4::Pipeline** pipelines{};
};

#if defined(_WIN32)
struct Engine_Config_DX11 : Engine_Config {
    Topl_Renderer_DX11* renderer{};
    DX11::Pipeline** pipelines{};
};
#endif

#if defined(TOPL_ENABLE_VULKAN)
struct Engine_Config_VK : Engine_Config {
    Topl_Renderer_VK* renderer{};
    VK::Pipeline** pipelines{};
};
#endif

#endif // __ANDROID__

// -----------------------------------------------------------------------------
// Factory
// -----------------------------------------------------------------------------

class Topl_Factory {
public:
	~Topl_Factory();
    static Topl_Renderer* genRenderer(BACKEND_Target backend, Platform* platform);

	template <typename Engine_Config, typename Renderer, typename PipelineType>
	static Topl_Pipeline* createPipeline(
		Engine_Config& cfg, Renderer* renderer, 
		entry_shader_cptr vShader, shader_cptr pShader, std::initializer_list<shader_cptr> shaders
	);
    static Topl_Pipeline* genPipeline(BACKEND_Target backend, entry_shader_cptr vShader,shader_cptr pShader){
        return genPipeline(backend, vShader, pShader, {});
    }
    static Topl_Pipeline* genPipeline(BACKEND_Target backend, entry_shader_cptr vShader, shader_cptr pShader, std::initializer_list<shader_cptr> shaders);

    static void switchPipeline(Topl_Renderer* renderer, Topl_Pipeline* pipeline);
private:
    static void configPipelines();

    // Engine Instances
    static Engine_Config_GL4 GL4_engine_cfg;

#if !defined(__ANDROID__)
    #if defined(_WIN32)
        static Engine_Config_DX11 DX11_engine_cfg;
    #endif
    #if defined(TOPL_ENABLE_VULKAN)
        static Engine_Config_VK VK_engine_cfg;
    #endif
#endif
};