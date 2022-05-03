#include "opengl/Topl_Renderer_GL4.hpp" // at least 1 backend required
#ifdef _WIN32
#include "directx/Topl_Renderer_Drx11.hpp"
#endif
#ifdef VULKAN_H
#include "vulkan/Topl_Renderer_Vulkan.hpp"
#endif

enum APP_Backend {
    APP_OpenGL_4,
    APP_DirectX_11,
    APP_Vulkan
};

class Topl_Factory { // factory that generates backend-specific data
public:
	~Topl_Factory();
	static Topl_Renderer* genRenderer(APP_Backend backend, Platform* platform);
    static Topl_Renderer* genRenderer(APP_Backend backend, Platform* plaform, std::initializer_list<Topl_Viewport> viewports);

	static Topl_Pipeline* genPipeline(APP_Backend backend, entry_shader_cptr vertexShader, shader_cptr pixelShader);
	static Topl_Pipeline* genPipeline(APP_Backend backend,
		entry_shader_cptr vertexSource,
		shader_cptr pixelSource,
		shader_cptr tessCtrlSource,
		shader_cptr tessEvalSource,
		shader_cptr geomSource
	);
private:
	// static void allocPipelines();
	// static bool isCheckBackend; // makes sure backends are checked for use

	static Topl_Renderer_GL4* GL4_renderer;
	static Topl_Pipeline_GL4** GL4_pipelines; 
	static unsigned GL4_pipeIndex; // tracks pipeline in use

#ifdef _WIN32
	static Topl_Renderer_Drx11* Drx11_renderer;
	static Topl_Pipeline_Drx11** Drx11_pipelines; 
	static unsigned Drx11_pipeIndex; // tracks pipeline in use
#endif

#ifdef VULKAN_H
	static Topl_Renderer_Vulkan* Vulkan_renderer;
	static Topl_Pipeline_Vulkan** Vulkan_pipelines; 
	static unsigned Vulkan_pipeIndex; // tracks pipeline in use
#endif
};