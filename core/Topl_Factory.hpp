#include "opengl/Topl_Renderer_GL4.hpp" // at least 1 backend required
#ifdef _WIN32
#include "directx/Topl_Renderer_Drx11.hpp"
#endif
#ifdef VULKAN_H
#include "vulkan/Topl_Renderer_Vulkan.hpp"
#endif

// Backends List

enum TARGET_Backend {
	TARGET_OpenGL,
	TARGET_DirectX11,
	TARGET_Vulkan
};

// Engine Configuration

struct Engine_Config { unsigned pipeIndex; };

struct GL4_Engine_Config : public Engine_Config {
	Topl_Renderer_GL4* renderer;
	Topl_Pipeline_GL4** pipelines;
};

#ifdef _WIN32
struct Drx11_Engine_Config : public Engine_Config {
	Topl_Renderer_Drx11* renderer;
	Topl_Pipeline_Drx11** pipelines;
};
#endif

#ifdef VULKAN_H
struct Vulkan_Engine_Config : public Engine_Config {
	Topl_Renderer_Vulkan* renderer;
	Topl_Pipeline_Vulkan** pipelines;
};
#endif

// Factory

class Topl_Factory {
public:
	~Topl_Factory();
	static Topl_Renderer* genRenderer(TARGET_Backend backend, Platform* platform);

	static Topl_Pipeline* genPipeline(TARGET_Backend backend, entry_shader_cptr vertexShader, shader_cptr pixelShader);
	static Topl_Pipeline* genPipeline(TARGET_Backend backend,
		entry_shader_cptr vertexSource,
		shader_cptr pixelSource,
		shader_cptr tessCtrlSource,
		shader_cptr tessEvalSource,
		shader_cptr geomSource
	);

	static void switchPipeline(TARGET_Backend backend, Topl_Renderer* renderer, Topl_Pipeline* pipeline);
private:
	static void configPipelines(); // config helper function

	// Engine Instances

	static GL4_Engine_Config GL4_engine_cfg;
#ifdef _WIN32
	static Drx11_Engine_Config Drx11_engine_cfg;
#endif
#ifdef VULKAN_H
	static Vulkan_Engine_Config Vulkan_engine_cfg;
#endif
};