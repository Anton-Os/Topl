#include "Topl_Renderer_Vulkan.hpp"

namespace Vulkan {
	static void createShaderModule(VkDevice& device, VkShaderModule* shaderModule, std::string& shaderSrc){
		VkResult result; // error checking variable
		
		VkShaderModuleCreateInfo shaderCreateInfo = {};
		shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderCreateInfo.codeSize = shaderSrc.length() * 4; // multiply by 4 to get size in bytes
		shaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderSrc.c_str());

		logMessage("Shader source: " + shaderSrc); // TODO: Get past this point!!!
		result = vkCreateShaderModule(device, &shaderCreateInfo, nullptr, shaderModule);
		if(result == VK_SUCCESS) logMessage("Shader module creation success\n");
		else return logMessage(MESSAGE_Exclaim, "Shader module creation failure!\n");
	}
}

void Topl_Renderer_Vulkan::setPipeline(Topl_Pipeline_Vulkan* pipeline){
	_pipeline = pipeline;
	Topl_Renderer::setPipeline((Topl_Pipeline*)pipeline);
}

void Topl_Renderer_Vulkan::genPipeline(Topl_Pipeline_Vulkan* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader){
	if(pipeline == nullptr || vertexShader == nullptr || pixelShader == nullptr)
		return logMessage(MESSAGE_Exclaim, "Pipeline, vertex and pixel shaders cannot be null!");
	
	std::string vertexShaderSrc = readFile(vertexShader->getFilePath());
	VkShaderModule vertexShaderModule = {};
	Vulkan::createShaderModule(_logicDevice, &vertexShaderModule, vertexShaderSrc);

	pipeline->vertexStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipeline->vertexStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	pipeline->vertexStageInfo.module = vertexShaderModule;
	pipeline->vertexStageInfo.pName = "main";

	std::string pixelShaderSrc = readFile(pixelShader->getFilePath());
	VkShaderModule pixelShaderModule = {};
	Vulkan::createShaderModule(_logicDevice, &pixelShaderModule, pixelShaderSrc);

	pipeline->pixelStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipeline->pixelStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	pipeline->pixelStageInfo.module = pixelShaderModule;
	pipeline->pixelStageInfo.pName = "main";

	pipeline->entryShader = vertexShader;
	pipeline->isReady = true;
	setPipeline(pipeline);

	vkDestroyShaderModule(_logicDevice, vertexShaderModule, nullptr);
	vkDestroyShaderModule(_logicDevice, pixelShaderModule, nullptr);

	finalizePipeline(); // combine everything from init() and genPipeline() steps
}

void Topl_Renderer_Vulkan::genPipeline(Topl_Pipeline_Vulkan* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader, shader_cptr geomShader){
	if(pipeline == nullptr || vertexShader == nullptr || pixelShader == nullptr)
		return logMessage(MESSAGE_Exclaim, "Pipeline, vertex and pixel shaders cannot be null!");
	
	std::string vertexShaderSrc = readFile(vertexShader->getFilePath());
	VkShaderModule vertexShaderModule = {};
	Vulkan::createShaderModule(_logicDevice, &vertexShaderModule, vertexShaderSrc);

	pipeline->vertexStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipeline->vertexStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	pipeline->vertexStageInfo.module = vertexShaderModule;
	pipeline->vertexStageInfo.pName = "main";

	std::string pixelShaderSrc = readFile(pixelShader->getFilePath());
	VkShaderModule pixelShaderModule = {};
	Vulkan::createShaderModule(_logicDevice, &pixelShaderModule, pixelShaderSrc);

	pipeline->pixelStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipeline->pixelStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	pipeline->pixelStageInfo.module = pixelShaderModule;
	pipeline->pixelStageInfo.pName = "main";

	// TODO: Populate geometry shader
	// TODO: Populate tesselation control shader
	// TODO: Populate tesselation evaluation shader
	
	pipeline->entryShader = vertexShader;
	pipeline->isReady = true;
	setPipeline(pipeline);

	vkDestroyShaderModule(_logicDevice, vertexShaderModule, nullptr);
	vkDestroyShaderModule(_logicDevice, pixelShaderModule, nullptr);
	// TODO: Destroy any other shader modules

	finalizePipeline(); // combine everything from init() and genPipeline() steps
}

void Topl_Renderer_Vulkan::finalizePipeline(){
	VkPipelineShaderStageCreateInfo shaderStages[2] = { // TODO: Make configurable
		_pipeline->vertexStageInfo,
		_pipeline->pixelStageInfo
	};

	_pipeline->pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	_pipeline->pipelineInfo.stageCount = 2; // TODO: Make configurable
	_pipeline->pipelineInfo.pStages = shaderStages;
	_pipeline->pipelineInfo.pVertexInputState = &_vertexInputInfo;
	_pipeline->pipelineInfo.pInputAssemblyState = &_inputAssemblyInfo;
	_pipeline->pipelineInfo.pViewportState = &_viewportStateInfo;
	_pipeline->pipelineInfo.pRasterizationState = &_rasterStateInfo;
	_pipeline->pipelineInfo.pMultisampleState = &_multisampleInfo;
	_pipeline->pipelineInfo.pDepthStencilState = &_depthStencilInfo;
	_pipeline->pipelineInfo.pColorBlendState = &_colorBlendInfo;
	_pipeline->pipelineInfo.layout = _pipelineLayout;
	_pipeline->pipelineInfo.renderPass = _renderpass;
	_pipeline->pipelineInfo.subpass = 0;
	_pipeline->pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	_pipeline->pipelineInfo.basePipelineIndex = -1;

	if(vkCreateGraphicsPipelines(_logicDevice, VK_NULL_HANDLE, 1, &_pipeline->pipelineInfo, nullptr, &_pipeline->pipeline) == VK_SUCCESS)
		logMessage("Pipeline creation success\n");
	else return logMessage("Pipeline creation failure!");
}