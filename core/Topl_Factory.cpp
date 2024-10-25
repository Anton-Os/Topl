#include "Topl_Factory.hpp"

Engine_Config_GL4 Topl_Factory::GL4_engine_cfg = Engine_Config_GL4();
#ifdef _WIN32
Engine_Config_DX11 Topl_Factory::DX11_engine_cfg = Engine_Config_DX11();
#endif
#if defined(TOPL_ENABLE_VULKAN) && !defined(__ANDROID__)
Engine_Config_VK Topl_Factory::VK_engine_cfg = Engine_Config_VK();
#endif

Topl_Factory::~Topl_Factory(){
    // Renderer Cleanup
	if(GL4_engine_cfg.renderer != nullptr) delete(GL4_engine_cfg.renderer);
#ifdef _WIN32
    if(DX11_engine_cfg.renderer != nullptr) delete(DX11_engine_cfg.renderer);
#endif
#if defined(TOPL_ENABLE_VULKAN) && !defined(__ANDROID__)
	if(VK_engine_cfg.renderer != nullptr) delete(VK_engine_cfg.renderer);
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
#if defined(TOPL_ENABLE_VULKAN) && !defined(__ANDROID__)
	if (VK_engine_cfg.pipelines != nullptr) {
		for (unsigned p = 0; p < VK_engine_cfg.pipeIndex; p++) delete(*(VK_engine_cfg.pipelines + p));
		free(VK_engine_cfg.pipelines);
	}
#endif
}

// Factory Renderer

Topl_Renderer* Topl_Factory::genRenderer(BACKEND_Target backend, Platform* platform){
    switch(backend){
    case BACKEND_GL4:
        if(GL4_engine_cfg.renderer == nullptr) 
#ifndef __ANDROID__
            GL4_engine_cfg.renderer = new Topl_Renderer_GL4(platform->getContext());
#else
            GL4_engine_cfg.renderer = new Droidl_Renderer(platform->getContext());
#endif
        return (Topl_Renderer*)GL4_engine_cfg.renderer;
#ifdef _WIN32
	case BACKEND_DX11:
        if(DX11_engine_cfg.renderer == nullptr) 
			DX11_engine_cfg.renderer = new Topl_Renderer_DX11(platform->getContext());
        return (Topl_Renderer*)DX11_engine_cfg.renderer;
#endif
#if defined(TOPL_ENABLE_VULKAN) && !defined(__ANDROID__)
	case BACKEND_VK:
		if (VK_engine_cfg.renderer == nullptr) 
			VK_engine_cfg.renderer = new Topl_Renderer_VK(platform->getContext());
		return (Topl_Renderer*)VK_engine_cfg.renderer;
#endif
	default: return nullptr; // Error
    }
}

// Factory Pipeline

void Topl_Factory::configPipelines() {
	if (GL4_engine_cfg.pipelines == nullptr) 
		GL4_engine_cfg.pipelines = (GL4::Pipeline**)malloc(MAX_PIPELINES * sizeof(GL4::Pipeline*));
#ifdef _WIN32
	if (DX11_engine_cfg.pipelines == nullptr) 
		DX11_engine_cfg.pipelines = (DX11::Pipeline**)malloc(MAX_PIPELINES * sizeof(DX11::Pipeline*));
#endif
#if defined(TOPL_ENABLE_VULKAN) && !defined(__ANDROID__)
	if (VK_engine_cfg.pipelines == nullptr) 
		VK_engine_cfg.pipelines = (VK::Pipeline**)malloc(MAX_PIPELINES * sizeof(VK::Pipeline*));
#endif
}

Topl_Pipeline* Topl_Factory::genPipeline(BACKEND_Target backend, entry_shader_cptr vertexShader, shader_cptr pixelShader){
	configPipelines();

    switch(backend){
    case BACKEND_GL4:
        if(GL4_engine_cfg.renderer == nullptr) return nullptr; // Error
        else {
            GL4::Pipeline** pipeline = GL4_engine_cfg.pipelines + GL4_engine_cfg.pipeIndex;
            *pipeline = new GL4::Pipeline();
            GL4_engine_cfg.renderer->genPipeline(*pipeline, vertexShader, pixelShader);
            GL4_engine_cfg.pipeIndex++;
            return *pipeline;
        }
#ifdef _WIN32
    case BACKEND_DX11:
        if(DX11_engine_cfg.renderer == nullptr) return nullptr; // Error
        else {
            DX11::Pipeline** pipeline = DX11_engine_cfg.pipelines + DX11_engine_cfg.pipeIndex;
            *pipeline = new DX11::Pipeline();
            DX11_engine_cfg.renderer->genPipeline(*pipeline, vertexShader, pixelShader);
            DX11_engine_cfg.pipeIndex++;
            return *pipeline;
        }
#endif
#if defined(TOPL_ENABLE_VULKAN) && !defined(__ANDROID__)
	case BACKEND_VK:
		if(VK_engine_cfg.renderer == nullptr) return nullptr; // Error
		else {
			VK::Pipeline** pipeline = VK_engine_cfg.pipelines + VK_engine_cfg.pipeIndex;
			*pipeline = new VK::Pipeline();
			VK_engine_cfg.renderer->genPipeline(*pipeline, vertexShader, pixelShader);
			VK_engine_cfg.pipeIndex++;
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
			GL4::Pipeline** pipeline = GL4_engine_cfg.pipelines + GL4_engine_cfg.pipeIndex;
			*pipeline = new GL4::Pipeline();
			GL4_engine_cfg.renderer->genPipeline(*pipeline, vertexShader, pixelShader, tessCtrlShader, tessEvalShader, geomShader);
			GL4_engine_cfg.pipeIndex++;
			return *pipeline;
		}
#ifdef _WIN32
	case BACKEND_DX11:
		if (DX11_engine_cfg.renderer == nullptr) return nullptr; // Error
		else {
			DX11::Pipeline** pipeline = DX11_engine_cfg.pipelines + DX11_engine_cfg.pipeIndex;
			*pipeline = new DX11::Pipeline();
			DX11_engine_cfg.renderer->genPipeline(*pipeline, vertexShader, pixelShader, tessCtrlShader, tessEvalShader, geomShader);
			DX11_engine_cfg.pipeIndex++;
			return *pipeline;
		}
#endif
#if defined(TOPL_ENABLE_VULKAN) && !defined(__ANDROID__)
	case BACKEND_VK:
		if (VK_engine_cfg.renderer == nullptr) return nullptr; // Error
		else {
			VK::Pipeline** pipeline = VK_engine_cfg.pipelines + VK_engine_cfg.pipeIndex;
			*pipeline = new VK::Pipeline();
			VK_engine_cfg.renderer->genPipeline(*pipeline, vertexShader, pixelShader, tessCtrlShader, tessEvalShader, geomShader);
			VK_engine_cfg.pipeIndex++;
			return *pipeline;
		}
#endif
	default: return nullptr; // Error
    }
}

void Topl_Factory::switchPipeline(Topl_Renderer* renderer, Topl_Pipeline* pipeline) {
	if (typeid(*renderer) == typeid(Topl_Renderer_GL4)) ((Topl_Renderer_GL4*)renderer)->setPipeline((GL4::Pipeline*)pipeline);
#ifdef _WIN32
	else if(typeid(*renderer) == typeid(Topl_Renderer_DX11)) ((Topl_Renderer_DX11*)renderer)->setPipeline((DX11::Pipeline*)pipeline);
#endif
#if defined(TOPL_ENABLE_VULKAN) && !defined(__ANDROID__)
	else if(typeid(*renderer) == typeid(Topl_Renderer_VK)) ((Topl_Renderer_VK*)renderer)->setPipeline((VK::Pipeline*)pipeline);
#endif
	else return logMessage(MESSAGE_Exclaim, "Invalid Backend"); // Error
}
