// Vulkan Specific Inclusions

// #include "vulkan/vulkan.h"

// Renderer Implementation

#include "Topl_Renderer.hpp"

struct Buffer_Vulkan : public Buffer { Buffer_Vulkan() : Buffer(){} };

struct Texture_Vulkan : public Texture { Texture_Vulkan() : Texture() {} };

struct Topl_Pipeline_Vulkan : public Topl_Pipeline {
	Topl_Pipeline_Vulkan() : Topl_Pipeline(){}
};

struct Topl_RenderContext_Vulkan : public Topl_RenderContext {
	Topl_RenderContext_Vulkan(const Topl_Scene *const s) : Topl_RenderContext(s){}
	Topl_RenderContext_Vulkan(const Topl_Scene *const s, unsigned idCount) : Topl_RenderContext(s, idCount){}

	std::vector<Buffer_Vulkan> buffers;
	std::vector<Texture_Vulkan> textures;
};

class Topl_Renderer_Vulkan : public Topl_Renderer {
public:
	Topl_Renderer_Vulkan(NATIVE_WINDOW window) : Topl_Renderer(){ 
		init(window);
		drawMode(); // set default draw mode

		_renderCtx_Vulkan = (Topl_RenderContext_Vulkan**)malloc(sizeof(Topl_RenderContext_Vulkan*) * MAX_RENDERER_CONTEXTS);
	}
	~Topl_Renderer_Vulkan(){}

	void clearView() override;
	void switchFramebuff() override;
	void build(const Topl_Scene* scene) override;
	void texturize(const Topl_Scene* scene) override;

	void setPipeline(Topl_Pipeline_Vulkan* pipeline);
	void genPipeline(Topl_Pipeline_Vulkan* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader);
	void genPipeline(Topl_Pipeline_Vulkan* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader, shader_cptr geomShader);
#ifdef RASTERON_H
	Rasteron_Image* frame() override;
#endif
private:
  	void init(NATIVE_WINDOW window) override;
	// void init(NATIVE_WINDOW window, std::initializer_list<Topl_Viewport> viewports) override;
	void update(const Topl_Scene* scene) override;
	void drawMode(void) override;
	void render(const Topl_Scene* scene) override;
#ifdef RASTERON_H
	void attachTexture(const Rasteron_Image* image, unsigned id) override;
	void attachMaterial(const Topl_Material* material, unsigned id) override;
#endif

	Topl_Pipeline_Vulkan* _pipeline;
	Topl_RenderContext_Vulkan** _renderCtx_Vulkan; // stores multiple render contexts with unique scenes and ids

	Topl_RenderContext_Vulkan* getRenderContext(const Topl_Scene* scene) { // finds render context matching input
		for (unsigned r = 0; r < _renderCtxIndex; r++)
			if ((*(_renderCtx_Vulkan + r))->scene == scene) 
				return *(_renderCtx_Vulkan + r);
		return nullptr; // error
	}

	// enum _drawMode_Vulkan; // Vulkan specific draw mode
};
