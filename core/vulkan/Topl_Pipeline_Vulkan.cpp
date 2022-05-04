#include "Topl_Renderer_Vulkan.hpp"

namespace Renderer {
	// include debugging and compilation
}

void Topl_Renderer_Vulkan::setPipeline(Topl_Pipeline_Vulkan* pipeline){
	_pipeline = pipeline;
	_entryShader = pipeline->entryShader;
	_isPipelineReady = pipeline->isReady;
}

void Topl_Renderer_Vulkan::genPipeline(Topl_Pipeline_Vulkan* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader){
	pipeline->entryShader = vertexShader;
	pipeline->isReady = true;
	setPipeline(pipeline);
}

void Topl_Renderer_Vulkan::genPipeline(Topl_Pipeline_Vulkan* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader, shader_cptr geomShader){
	pipeline->entryShader = vertexShader;
	pipeline->isReady = true;
	setPipeline(pipeline);
}