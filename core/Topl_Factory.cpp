#include "Topl_Factory.hpp"

Topl_Renderer_GL4* Topl_Factory::GL4_renderer = nullptr; 
Topl_Renderer_Drx11* Topl_Factory::Drx11_renderer = nullptr;
Topl_Renderer_Vulkan* Topl_Factory::Vulkan_renderer = nullptr;

Topl_Pipeline_GL4** Topl_Factory::GL4_pipelines = nullptr; 
unsigned Topl_Factory::GL4_pipeIndex = 0;
Topl_Pipeline_Drx11** Topl_Factory::Drx11_pipelines = nullptr;
unsigned Topl_Factory::Drx11_pipeIndex = 0;
Topl_Pipeline_Vulkan** Topl_Factory::Vulkan_pipelines = nullptr;
unsigned Topl_Factory::Vulkan_pipeIndex = 0;

Topl_Factory::~Topl_Factory(){
    // Renderer Cleanup
	if(GL4_renderer != nullptr) delete(GL4_renderer);
    if(Drx11_renderer != nullptr) delete(Drx11_renderer);
	if(Vulkan_renderer != nullptr) delete(Vulkan_renderer);

	// Pipeline Cleanup
    if(GL4_pipelines != nullptr){
        for(unsigned p = 0; p < GL4_pipeIndex; p++) delete(*(GL4_pipelines + p));
        free(GL4_pipelines);
    }
    if(Drx11_pipelines != nullptr){
        for(unsigned p = 0; p < Drx11_pipeIndex; p++) delete(*(Drx11_pipelines + p));
        free(Drx11_pipelines);
    }
	if (Vulkan_pipelines != nullptr) {
		for (unsigned p = 0; p < Vulkan_pipeIndex; p++) delete(*(Vulkan_pipelines + p));
		free(Vulkan_pipelines);
	}
}

Topl_Renderer* Topl_Factory::genRenderer(APP_Backend backend, Platform* platform){
    switch(backend){
    case APP_OpenGL_4:
        if(GL4_renderer == nullptr) GL4_renderer = new Topl_Renderer_GL4(platform->getParentWindow());
        return (Topl_Renderer*)GL4_renderer;
    case APP_DirectX_11:
        if(Drx11_renderer == nullptr) Drx11_renderer = new Topl_Renderer_Drx11(platform->getParentWindow());
        return (Topl_Renderer*)Drx11_renderer;
	case APP_Vulkan:
		if (Vulkan_renderer == nullptr) Vulkan_renderer = new Topl_Renderer_Vulkan(platform->getParentWindow());
		return (Topl_Renderer*)Vulkan_renderer;
    }
}

Topl_Renderer* Topl_Factory::genRenderer(APP_Backend backend, Platform* platform, std::initializer_list<Topl_Viewport> viewports){
    switch(backend){
    case APP_OpenGL_4:
        if(GL4_renderer == nullptr) GL4_renderer = new Topl_Renderer_GL4(platform->getParentWindow(), viewports);
        return (Topl_Renderer*)GL4_renderer;
    case APP_DirectX_11:
        if(Drx11_renderer == nullptr) Drx11_renderer = new Topl_Renderer_Drx11(platform->getParentWindow(), viewports);
        return (Topl_Renderer*)Drx11_renderer;
	case APP_Vulkan:
		if (Vulkan_renderer == nullptr) Vulkan_renderer = new Topl_Renderer_Vulkan(platform->getParentWindow(), viewports);
		return (Topl_Renderer*)Vulkan_renderer;
    }
}

Topl_Pipeline* Topl_Factory::genPipeline(APP_Backend backend, entry_shader_cptr vertexShader, shader_cptr pixelShader){
    if(GL4_pipelines == nullptr) GL4_pipelines = (Topl_Pipeline_GL4**)malloc(MAX_PIPELINES * sizeof(Topl_Pipeline_GL4*));
    if(Drx11_pipelines == nullptr) Drx11_pipelines = (Topl_Pipeline_Drx11**)malloc(MAX_PIPELINES * sizeof(Topl_Pipeline_Drx11*));
	if(Vulkan_pipelines == nullptr) Vulkan_pipelines = (Topl_Pipeline_Vulkan**)malloc(MAX_PIPELINES * sizeof(Topl_Pipeline_Vulkan*));

    switch(backend){
    case APP_OpenGL_4:
        if(GL4_renderer == nullptr) return nullptr; // error
        else {
            Topl_Pipeline_GL4* pipeline = *(GL4_pipelines + GL4_pipeIndex);
            pipeline = new Topl_Pipeline_GL4();
            GL4_renderer->genPipeline(pipeline, vertexShader, pixelShader);
            GL4_pipeIndex++;
            return pipeline;
        }
    case APP_DirectX_11:
        if(Drx11_renderer == nullptr) return nullptr; // error
        else {
            Topl_Pipeline_Drx11* pipeline = *(Drx11_pipelines + Drx11_pipeIndex);
            pipeline = new Topl_Pipeline_Drx11();
            Drx11_renderer->genPipeline(pipeline, vertexShader, pixelShader);
            Drx11_pipeIndex++;
            return pipeline;
        }
	case APP_Vulkan:
		if(Vulkan_renderer == nullptr) return nullptr; // error
		else {
			Topl_Pipeline_Vulkan* pipeline = *(Vulkan_pipelines + Vulkan_pipeIndex);
			pipeline = new Topl_Pipeline_Vulkan();
			Vulkan_renderer->genPipeline(pipeline, vertexShader, pixelShader);
			Vulkan_pipeIndex++;
			return pipeline;
		}
    }
}

Topl_Pipeline* Topl_Factory::genPipeline(APP_Backend backend, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader, shader_cptr geomShader){
    if(GL4_pipelines == nullptr) GL4_pipelines = (Topl_Pipeline_GL4**)malloc(MAX_PIPELINES * sizeof(Topl_Pipeline_GL4*));
    if(Drx11_pipelines == nullptr) Drx11_pipelines = (Topl_Pipeline_Drx11**)malloc(MAX_PIPELINES * sizeof(Topl_Pipeline_Drx11*));
	if(Vulkan_pipelines == nullptr) Vulkan_pipelines = (Topl_Pipeline_Vulkan**)malloc(MAX_PIPELINES * sizeof(Topl_Pipeline_Vulkan*));

	switch (backend) {
	case APP_OpenGL_4:
		if (GL4_renderer == nullptr) return nullptr; // error
		else {
			Topl_Pipeline_GL4* pipeline = *(GL4_pipelines + GL4_pipeIndex);
			pipeline = new Topl_Pipeline_GL4();
			GL4_renderer->genPipeline(pipeline, vertexShader, pixelShader, tessCtrlShader, tessEvalShader, geomShader);
			GL4_pipeIndex++;
			return pipeline;
		}
	case APP_DirectX_11:
		if (Drx11_renderer == nullptr) return nullptr; // error
		else {
			Topl_Pipeline_Drx11* pipeline = *(Drx11_pipelines + Drx11_pipeIndex);
			pipeline = new Topl_Pipeline_Drx11();
			Drx11_renderer->genPipeline(pipeline, vertexShader, pixelShader, tessCtrlShader, tessEvalShader, geomShader);
			Drx11_pipeIndex++;
			return pipeline;
		}
	case APP_Vulkan:
		if (Vulkan_renderer == nullptr) return nullptr; // error
		else {
			Topl_Pipeline_Vulkan* pipeline = *(Vulkan_pipelines + Vulkan_pipeIndex);
			pipeline = new Topl_Pipeline_Vulkan();
			Vulkan_renderer->genPipeline(pipeline, vertexShader, pixelShader, tessCtrlShader, tessEvalShader, geomShader);
			Vulkan_pipeIndex++;
			return pipeline;
		}
    }
}
