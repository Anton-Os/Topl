#include "Topl_Factory.hpp"

Engine_Config_GL4 Topl_Factory::GL4_engine_cfg = Engine_Config_GL4();
#ifdef _WIN32
Engine_Config_DX11 Topl_Factory::DX11_engine_cfg = Engine_Config_DX11();
#endif
#ifdef TOPL_ENABLE_VULKAN
Engine_Config_Vulkan Topl_Factory::Vulkan_engine_cfg = Engine_Config_Vulkan();
#endif

Topl_Factory::~Topl_Factory(){
    // Renderer Cleanup
	if(GL4_engine_cfg.renderer != nullptr) delete(GL4_engine_cfg.renderer);
#ifdef _WIN32
    if(DX11_engine_cfg.renderer != nullptr) delete(DX11_engine_cfg.renderer);
#endif
#ifdef TOPL_ENABLE_VULKAN
	if(Vulkan_engine_cfg.renderer != nullptr) delete(Vulkan_engine_cfg.renderer);
#endif

	// Pipeline Cleanup
    if(GL4_engine_cfg.pipelines != nullptr){
        for(unsigned p = 0; p < GL4_engine_cfg.pipeIndex; p++) delete(*(GL4_engine_cfg.pipelines + p));
        free(GL4_engine_cfg.pipelines);
    }
#ifdef _WIN32
    if(DX11_engine_cfg.pipelines != nullptr){
        for(unsigned p = 0; p < DX11_engine_cfg.pipeIndex; p++) delete(*(DX11_engine_cfg.pipelines + p));
        free(DX11_engine_cfg.pipelines);
    }
#endif
#ifdef TOPL_ENABLE_VULKAN
	if (Vulkan_engine_cfg.pipelines != nullptr) {
		for (unsigned p = 0; p < Vulkan_engine_cfg.pipeIndex; p++) delete(*(Vulkan_engine_cfg.pipelines + p));
		free(Vulkan_engine_cfg.pipelines);
	}
#endif
}

// Factory Renderer

Topl_Renderer* Topl_Factory::genRenderer(BACKEND_Target backend, Platform* platform){
    switch(backend){
    case BACKEND_GL4:
        if(GL4_engine_cfg.renderer == nullptr) 
			GL4_engine_cfg.renderer = new Topl_Renderer_GL4(platform->getParentWindow());
        return (Topl_Renderer*)GL4_engine_cfg.renderer;
#ifdef _WIN32
	case BACKEND_DX11:
        if(DX11_engine_cfg.renderer == nullptr) 
			DX11_engine_cfg.renderer = new Topl_Renderer_DX11(platform->getParentWindow());
        return (Topl_Renderer*)DX11_engine_cfg.renderer;
#endif
#ifdef TOPL_ENABLE_VULKAN
	case BACKEND_Vulkan:
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
	if (DX11_engine_cfg.pipelines == nullptr) 
		DX11_engine_cfg.pipelines = (Topl_Pipeline_DX11**)malloc(MAX_PIPELINES * sizeof(Topl_Pipeline_DX11*));
#endif
#ifdef TOPL_ENABLE_VULKAN
	if (Vulkan_engine_cfg.pipelines == nullptr) 
		Vulkan_engine_cfg.pipelines = (Topl_Pipeline_Vulkan**)malloc(MAX_PIPELINES * sizeof(Topl_Pipeline_Vulkan*));
#endif
}

Topl_Pipeline* Topl_Factory::genPipeline(BACKEND_Target backend, entry_shader_cptr vertexShader, shader_cptr pixelShader){
	configPipelines();

    switch(backend){
    case BACKEND_GL4:
        if(GL4_engine_cfg.renderer == nullptr) return nullptr; // Error
        else {
            Topl_Pipeline_GL4** pipeline = GL4_engine_cfg.pipelines + GL4_engine_cfg.pipeIndex;
            *pipeline = new Topl_Pipeline_GL4();
            GL4_engine_cfg.renderer->genPipeline(*pipeline, vertexShader, pixelShader);
            GL4_engine_cfg.pipeIndex++;
            return *pipeline;
        }
#ifdef _WIN32
    case BACKEND_DX11:
        if(DX11_engine_cfg.renderer == nullptr) return nullptr; // Error
        else {
            Topl_Pipeline_DX11** pipeline = DX11_engine_cfg.pipelines + DX11_engine_cfg.pipeIndex;
            *pipeline = new Topl_Pipeline_DX11();
            DX11_engine_cfg.renderer->genPipeline(*pipeline, vertexShader, pixelShader);
            DX11_engine_cfg.pipeIndex++;
            return *pipeline;
        }
#endif
#ifdef TOPL_ENABLE_VULKAN
	case BACKEND_Vulkan:
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

Topl_Pipeline* Topl_Factory::genPipeline(BACKEND_Target backend, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader, shader_cptr geomShader){
	configPipelines();

	switch (backend) {
	case BACKEND_GL4:
		if (GL4_engine_cfg.renderer == nullptr) return nullptr; // Error
		else {
			Topl_Pipeline_GL4** pipeline = GL4_engine_cfg.pipelines + GL4_engine_cfg.pipeIndex;
			*pipeline = new Topl_Pipeline_GL4();
			GL4_engine_cfg.renderer->genPipeline(*pipeline, vertexShader, pixelShader, tessCtrlShader, tessEvalShader, geomShader);
			GL4_engine_cfg.pipeIndex++;
			return *pipeline;
		}
#ifdef _WIN32
	case BACKEND_DX11:
		if (DX11_engine_cfg.renderer == nullptr) return nullptr; // Error
		else {
			Topl_Pipeline_DX11** pipeline = DX11_engine_cfg.pipelines + DX11_engine_cfg.pipeIndex;
			*pipeline = new Topl_Pipeline_DX11();
			DX11_engine_cfg.renderer->genPipeline(*pipeline, vertexShader, pixelShader, tessCtrlShader, tessEvalShader, geomShader);
			DX11_engine_cfg.pipeIndex++;
			return *pipeline;
		}
#endif
#ifdef TOPL_ENABLE_VULKAN
	case BACKEND_Vulkan:
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

void Topl_Factory::switchPipeline(BACKEND_Target backend, Topl_Renderer* renderer, Topl_Pipeline* pipeline) {
	if (backend == BACKEND_GL4)
		((Topl_Renderer_GL4*)renderer)->setPipeline((Topl_Pipeline_GL4*)pipeline);
#ifdef _WIN32
	else if (backend == BACKEND_DX11)
		((Topl_Renderer_DX11*)renderer)->setPipeline((Topl_Pipeline_DX11*)pipeline);
#endif
#ifdef TOPL_ENABLE_VULKAN
	else if (backend == BACKEND_Vulkan)
		((Topl_Renderer_Vulkan*)renderer)->setPipeline((Topl_Pipeline_Vulkan*)pipeline);
#endif
	else return; // Error
}