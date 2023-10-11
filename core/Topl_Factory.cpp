#include "Topl_Factory.hpp"

GL4_Engine_Config Topl_Factory::GL4_engine_cfg = GL4_Engine_Config();
#ifdef _WIN32
Drx11_Engine_Config Topl_Factory::Drx11_engine_cfg = Drx11_Engine_Config();
#endif
#ifdef VULKAN_H
Vulkan_Engine_Config Topl_Factory::Vulkan_engine_cfg = Vulkan_Engine_Config();
#endif

Topl_Factory::~Topl_Factory(){
    // Renderer Cleanup
	if(GL4_engine_cfg.renderer != nullptr) delete(GL4_engine_cfg.renderer);
#ifdef _WIN32
    if(Drx11_engine_cfg.renderer != nullptr) delete(Drx11_engine_cfg.renderer);
#endif
#ifdef VULKAN_H
	if(Vulkan_engine_cfg.renderer != nullptr) delete(Vulkan_engine_cfg.renderer);
#endif

	// Pipeline Cleanup
    if(GL4_engine_cfg.pipelines != nullptr){
        for(unsigned p = 0; p < GL4_engine_cfg.pipeIndex; p++) delete(*(GL4_engine_cfg.pipelines + p));
        free(GL4_engine_cfg.pipelines);
    }
#ifdef _WIN32
    if(Drx11_engine_cfg.pipelines != nullptr){
        for(unsigned p = 0; p < Drx11_engine_cfg.pipeIndex; p++) delete(*(Drx11_engine_cfg.pipelines + p));
        free(Drx11_engine_cfg.pipelines);
    }
#endif
#ifdef VULKAN_H
	if (Vulkan_engine_cfg.pipelines != nullptr) {
		for (unsigned p = 0; p < Vulkan_engine_cfg.pipeIndex; p++) delete(*(Vulkan_engine_cfg.pipelines + p));
		free(Vulkan_engine_cfg.pipelines);
	}
#endif
}

// Factory Renderer

Topl_Renderer* Topl_Factory::genRenderer(TARGET_Backend backend, Platform* platform){
    switch(backend){
    case TARGET_OpenGL:
        if(GL4_engine_cfg.renderer == nullptr) 
			GL4_engine_cfg.renderer = new Topl_Renderer_GL4(platform->getParentWindow());
        return (Topl_Renderer*)GL4_engine_cfg.renderer;
#ifdef _WIN32
	case TARGET_DirectX11:
        if(Drx11_engine_cfg.renderer == nullptr) 
			Drx11_engine_cfg.renderer = new Topl_Renderer_Drx11(platform->getParentWindow());
        return (Topl_Renderer*)Drx11_engine_cfg.renderer;
#endif
#ifdef VULKAN_H
	case TARGET_Vulkan:
		if (Vulkan_engine_cfg.renderer == nullptr) 
			Vulkan_engine_cfg.renderer = new Topl_Renderer_Vulkan(platform->getParentWindow());
		return (Topl_Renderer*)Vulkan_engine_cfg.renderer;
#endif
	default: return nullptr; // Error
    }
}

// Factory Pipeline

void Topl_Factory::configPipelines() {
	if (GL4_engine_cfg.pipelines == nullptr) 
		GL4_engine_cfg.pipelines = (Topl_Pipeline_GL4**)malloc(MAX_PIPELINES * sizeof(Topl_Pipeline_GL4*));
#ifdef _WIN32
	if (Drx11_engine_cfg.pipelines == nullptr) 
		Drx11_engine_cfg.pipelines = (Topl_Pipeline_Drx11**)malloc(MAX_PIPELINES * sizeof(Topl_Pipeline_Drx11*));
#endif
#ifdef VULKAN_H
	if (Vulkan_engine_cfg.pipelines == nullptr) 
		Vulkan_engine_cfg.pipelines = (Topl_Pipeline_Vulkan**)malloc(MAX_PIPELINES * sizeof(Topl_Pipeline_Vulkan*));
#endif
}

Topl_Pipeline* Topl_Factory::genPipeline(TARGET_Backend backend, entry_shader_cptr vertexShader, shader_cptr pixelShader){
	configPipelines();

    switch(backend){
    case TARGET_OpenGL:
        if(GL4_engine_cfg.renderer == nullptr) return nullptr; // Error
        else {
            Topl_Pipeline_GL4** pipeline = GL4_engine_cfg.pipelines + GL4_engine_cfg.pipeIndex;
            *pipeline = new Topl_Pipeline_GL4();
            GL4_engine_cfg.renderer->genPipeline(*pipeline, vertexShader, pixelShader);
            GL4_engine_cfg.pipeIndex++;
            return *pipeline;
        }
#ifdef _WIN32
    case TARGET_DirectX11:
        if(Drx11_engine_cfg.renderer == nullptr) return nullptr; // Error
        else {
            Topl_Pipeline_Drx11** pipeline = Drx11_engine_cfg.pipelines + Drx11_engine_cfg.pipeIndex;
            *pipeline = new Topl_Pipeline_Drx11();
            Drx11_engine_cfg.renderer->genPipeline(*pipeline, vertexShader, pixelShader);
            Drx11_engine_cfg.pipeIndex++;
            return *pipeline;
        }
#endif
#ifdef VULKAN_H
	case TARGET_Vulkan:
		if(Vulkan_engine_cfg.renderer == nullptr) return nullptr; // Error
		else {
			Topl_Pipeline_Vulkan** pipeline = Vulkan_engine_cfg.pipelines + Vulkan_engine_cfg.pipeIndex;
			*pipeline = new Topl_Pipeline_Vulkan();
			Vulkan_engine_cfg.renderer->genPipeline(*pipeline, vertexShader, pixelShader);
			Vulkan_engine_cfg.pipeIndex++;
			return *pipeline;
		}
#endif
	default: return nullptr; // Error
    }
}

Topl_Pipeline* Topl_Factory::genPipeline(TARGET_Backend backend, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader, shader_cptr geomShader){
	configPipelines();

	switch (backend) {
	case TARGET_OpenGL:
		if (GL4_engine_cfg.renderer == nullptr) return nullptr; // Error
		else {
			Topl_Pipeline_GL4** pipeline = GL4_engine_cfg.pipelines + GL4_engine_cfg.pipeIndex;
			*pipeline = new Topl_Pipeline_GL4();
			GL4_engine_cfg.renderer->genPipeline(*pipeline, vertexShader, pixelShader, tessCtrlShader, tessEvalShader, geomShader);
			GL4_engine_cfg.pipeIndex++;
			return *pipeline;
		}
#ifdef _WIN32
	case TARGET_DirectX11:
		if (Drx11_engine_cfg.renderer == nullptr) return nullptr; // Error
		else {
			Topl_Pipeline_Drx11** pipeline = Drx11_engine_cfg.pipelines + Drx11_engine_cfg.pipeIndex;
			*pipeline = new Topl_Pipeline_Drx11();
			Drx11_engine_cfg.renderer->genPipeline(*pipeline, vertexShader, pixelShader, tessCtrlShader, tessEvalShader, geomShader);
			Drx11_engine_cfg.pipeIndex++;
			return *pipeline;
		}
#endif
#ifdef VULKAN_H
	case TARGET_Vulkan:
		if (Vulkan_engine_cfg.renderer == nullptr) return nullptr; // Error
		else {
			Topl_Pipeline_Vulkan** pipeline = Vulkan_engine_cfg.pipelines + Vulkan_engine_cfg.pipeIndex;
			*pipeline = new Topl_Pipeline_Vulkan();
			Vulkan_engine_cfg.renderer->genPipeline(*pipeline, vertexShader, pixelShader, tessCtrlShader, tessEvalShader, geomShader);
			Vulkan_engine_cfg.pipeIndex++;
			return *pipeline;
		}
#endif
	default: return nullptr; // Error
    }
}

void Topl_Factory::switchPipeline(TARGET_Backend backend, Topl_Renderer* renderer, Topl_Pipeline* pipeline) {
	if (backend == TARGET_OpenGL)
		((Topl_Renderer_GL4*)renderer)->setPipeline((Topl_Pipeline_GL4*)pipeline);
#ifdef _WIN32
	else if (backend == TARGET_DirectX11)
		((Topl_Renderer_Drx11*)renderer)->setPipeline((Topl_Pipeline_Drx11*)pipeline);
#endif
#ifdef VULKAN_H
	else if (backend == TARGET_Vulkan)
		((Topl_Renderer_Vulkan*)renderer)->setPipeline((Topl_Pipeline_Vulkan*)pipeline);
#endif
	else return; // Error
}