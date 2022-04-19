#include "opengl/Topl_Renderer_GL4.hpp"
#ifdef _WIN32
#include "directx/Topl_Renderer_Drx11.hpp"
#endif
#include "vulkan/Topl_Renderer_Vulkan.hpp"

enum APP_Backend {
    APP_OpenGL_4,
    APP_DirectX_11,
    APP_Vulkan
};

class Topl_Factory { // supplies and generates all interfaces for Topl_App class
public:
	// Create a constructor that allocates data

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
	// Internally Managed Structures
	static Topl_Renderer_GL4* GL4_renderer;
	static Topl_Renderer_Drx11* Drx11_renderer;
	static Topl_Renderer_Vulkan* Vulkan_renderer;

	static Topl_Pipeline_GL4** GL4_pipelines; static unsigned GL4_pipeIndex;
	static Topl_Pipeline_Drx11** Drx11_pipelines; static unsigned Drx11_pipeIndex;
	static Topl_Pipeline_Vulkan** Vulkan_pipelines; static unsigned Vulkan_pipeIndex;
};