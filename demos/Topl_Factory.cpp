#include "Topl_Factory.hpp"

// Engine instances
Engine_Config_GL4 Topl_Factory::GL4_engine_cfg{};
#ifdef _WIN32
Engine_Config_DX11 Topl_Factory::DX11_engine_cfg{};
#endif
#if defined(TOPL_ENABLE_VULKAN) && !defined(__ANDROID__)
Engine_Config_VK Topl_Factory::VK_engine_cfg{};
#endif

// -----------------------------------------------------------------------------
// Cleanup
// -----------------------------------------------------------------------------

Topl_Factory::~Topl_Factory() {
    auto cleanupRenderer = [](auto*& r) {
        if (r) { delete r; r = nullptr; }
    };

    auto cleanupPipelines = [](auto*& arr, unsigned count) {
        if (!arr) return;
        for (unsigned i = 0; i < count; i++) delete arr[i];
        free(arr);
        arr = nullptr;
    };

    cleanupRenderer(GL4_engine_cfg.renderer);
    cleanupPipelines(GL4_engine_cfg.pipelines, GL4_engine_cfg.pipeIndex);

#ifdef _WIN32
    cleanupRenderer(DX11_engine_cfg.renderer);
    cleanupPipelines(DX11_engine_cfg.pipelines, DX11_engine_cfg.pipeIndex);
#endif

#if defined(TOPL_ENABLE_VULKAN) && !defined(__ANDROID__)
    cleanupRenderer(VK_engine_cfg.renderer);
    cleanupPipelines(VK_engine_cfg.pipelines, VK_engine_cfg.pipeIndex);
#endif
}

// -----------------------------------------------------------------------------
// Renderer Factory
// -----------------------------------------------------------------------------

Topl_Renderer* Topl_Factory::genRenderer(BACKEND_Target backend, Platform* platform) {
    switch (backend) {
        case BACKEND_GL4:
            if (!GL4_engine_cfg.renderer) {
#ifndef __ANDROID__
                GL4_engine_cfg.renderer = new Topl_Renderer_GL4(platform->getContext());
#else
                GL4_engine_cfg.renderer = new Droidl_Renderer(platform->getContext());
#endif
            }
            return GL4_engine_cfg.renderer;

#ifdef _WIN32
        case BACKEND_DX11:
            if (!DX11_engine_cfg.renderer)
                DX11_engine_cfg.renderer = new Topl_Renderer_DX11(platform->getContext());
            return DX11_engine_cfg.renderer;
#endif

#if defined(TOPL_ENABLE_VULKAN) && !defined(__ANDROID__)
        case BACKEND_VK:
            if (!VK_engine_cfg.renderer)
                VK_engine_cfg.renderer = new Topl_Renderer_VK(platform->getContext());
            return VK_engine_cfg.renderer;
#endif

        default:
            return nullptr;
    }
}

// -----------------------------------------------------------------------------
// Pipeline Allocation
// -----------------------------------------------------------------------------

void Topl_Factory::configPipelines() {
    if (!GL4_engine_cfg.pipelines) 
		GL4_engine_cfg.pipelines = (GL4::Pipeline**)malloc(MAX_PIPELINES * sizeof(GL4::Pipeline*));
#ifdef _WIN32
    if (!DX11_engine_cfg.pipelines) 
		DX11_engine_cfg.pipelines = (DX11::Pipeline**)malloc(MAX_PIPELINES * sizeof(DX11::Pipeline*));
#endif
#if defined(TOPL_ENABLE_VULKAN) && !defined(__ANDROID__)
    if (!VK_engine_cfg.pipelines) 
		VK_engine_cfg.pipelines = (VK::Pipeline**)malloc(MAX_PIPELINES * sizeof(VK::Pipeline*));
#endif
}

// -----------------------------------------------------------------------------
// Pipeline Factory
// -----------------------------------------------------------------------------

template <typename Engine_Config, typename Renderer, typename PipelineType>
Topl_Pipeline* Topl_Factory::createPipeline(Engine_Config& cfg, Renderer* renderer, entry_shader_cptr vShader, shader_cptr pShader, std::initializer_list<shader_cptr> shaders){
	if (!renderer)
        return nullptr;

    unsigned idx = cfg.pipeIndex;
    PipelineType** arr = cfg.pipelines;

    PipelineType* pipe = new PipelineType();
    arr[idx] = pipe;
    cfg.pipeIndex++;

    renderer->genPipeline(pipe, vShader, pShader, shaders);
    return pipe;
}

Topl_Pipeline* Topl_Factory::genPipeline(
    BACKEND_Target backend,
    entry_shader_cptr vShader,
    shader_cptr pShader,
    std::initializer_list<shader_cptr> shaders){
    configPipelines();

    switch (backend) {
case BACKEND_GL4:
	return createPipeline<Engine_Config_GL4, Topl_Renderer_GL4, GL4::Pipeline>(GL4_engine_cfg, GL4_engine_cfg.renderer, vShader, pShader, shaders);
#ifdef _WIN32
	case BACKEND_DX11:
		return createPipeline<Engine_Config_DX11, Topl_Renderer_DX11, DX11::Pipeline>(DX11_engine_cfg,DX11_engine_cfg.renderer,vShader, pShader, shaders);
#endif
#if defined(TOPL_ENABLE_VULKAN) && !defined(__ANDROID__)
	case BACKEND_VK:
		return createPipeline<Engine_Config_VK, Topl_Renderer_VK, VK::Pipeline>(VK_engine_cfg, VK_engine_cfg.renderer, vShader, pShader, shaders);
#endif
	default:
		return nullptr;
	}
}

// -----------------------------------------------------------------------------
// Pipeline Switching
// -----------------------------------------------------------------------------

void Topl_Factory::switchPipeline(Topl_Renderer* renderer, Topl_Pipeline* pipeline) {
    if (auto* r = dynamic_cast<Topl_Renderer_GL4*>(renderer))
        r->setPipeline(static_cast<GL4::Pipeline*>(pipeline));

#ifdef _WIN32
    else if (auto* r = dynamic_cast<Topl_Renderer_DX11*>(renderer))
        r->setPipeline(static_cast<DX11::Pipeline*>(pipeline));
#endif

#if defined(TOPL_ENABLE_VULKAN) && !defined(__ANDROID__)
    else if (auto* r = dynamic_cast<Topl_Renderer_VK*>(renderer))
        r->setPipeline(static_cast<VK::Pipeline*>(pipeline));
#endif

    else
        logMessage(MESSAGE_Exclaim, "Invalid Backend");
}
