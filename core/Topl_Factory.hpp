#include <typeinfo>

#ifdef __ANDROID__
#include "../../Droidl_New/app/src/main/cpp/Droidl_Renderer.hpp" // TODO: Correct this path
#else
#include "renderer/opengl/Topl_Renderer_GL4.hpp"
#endif
#ifdef _WIN32
#include "renderer/directx/Topl_Renderer_DX11.hpp"
#endif
#ifdef TOPL_ENABLE_VULKAN
#include "renderer/vulkan/Topl_Renderer_VK.hpp"
#endif

// Backends List

enum BACKEND_Target { 
	BACKEND_GL4,
#ifdef _WIN32
	BACKEND_DX11,
#endif
#ifdef TOPL_ENABLE_VULKAN 
	BACKEND_VK 
#endif
};

// Engine Configuration

struct Engine_Config { unsigned pipeIndex; };

#ifndef __ANDROID__
struct Engine_Config_GL4 : public Engine_Config {
	Topl_Renderer_GL4* renderer;
	GL4::Pipeline** pipelines;
};

#ifdef _WIN32
struct Engine_Config_DX11 : public Engine_Config {
	Topl_Renderer_DX11* renderer;
	DX11::Pipeline** pipelines;
};
#endif

#ifdef TOPL_ENABLE_VULKAN
struct Engine_Config_VK : public Engine_Config {
	Topl_Renderer_VK* renderer;
	VK::Pipeline** pipelines;
};
#endif
#else
struct Engine_Config_GL4 : public Engine_Config {
    Droidl_Renderer* renderer;
    GL4::Pipeline** pipelines;
};
#endif

// Factory

class Topl_Factory {
public:
	~Topl_Factory();
	static Topl_Renderer* genRenderer(BACKEND_Target backend, Platform* platform);

	static Topl_Pipeline* genPipeline(BACKEND_Target backend, entry_shader_cptr vertexShader, shader_cptr pixelShader);
	static Topl_Pipeline* genPipeline(BACKEND_Target backend,
		entry_shader_cptr vertexSource,
		shader_cptr pixelSource,
		shader_cptr tessCtrlSource,
		shader_cptr tessEvalSource,
		shader_cptr geomSource
	);

	static void switchPipeline(Topl_Renderer* renderer, Topl_Pipeline* pipeline);
private:
	static void configPipelines(); // config helper function

	// Engine Instances

	static Engine_Config_GL4 GL4_engine_cfg;
#ifndef __ANDROID__
#ifdef _WIN32
	static Engine_Config_DX11 DX11_engine_cfg;
#endif
#ifdef TOPL_ENABLE_VULKAN
	static Engine_Config_VK VK_engine_cfg;
#endif
#endif
};
