#include "Topl_Renderer_Vulkan.hpp"

namespace Vulkan {
	static void createShaderModule(VkDevice& device, VkShaderModule* shaderModule, std::string& shaderSrc){
		VkResult result; // error checking variable
		
		VkShaderModuleCreateInfo shaderCreateInfo = {};
		shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderCreateInfo.codeSize = shaderSrc.length();
		shaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderSrc.c_str());

		// logMessage("Shader source: " + shaderSrc); // TODO: Get past this point!!!
		result = vkCreateShaderModule(device, &shaderCreateInfo, nullptr, shaderModule);
		if(result == VK_SUCCESS) logMessage(" shader module creation success\n");
		else return logMessage(MESSAGE_Exclaim, " shader module creation failure!\n");
	}
}

void Topl_Renderer_Vulkan::setPipeline(Topl_Pipeline_Vulkan* pipeline){
	_pipeline = pipeline;
	Topl_Renderer::setPipeline((Topl_Pipeline*)pipeline);
}

void Topl_Renderer_Vulkan::genPipeline(Topl_Pipeline_Vulkan* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader){
	if(pipeline == nullptr || vertexShader == nullptr || pixelShader == nullptr)
		return logMessage(MESSAGE_Exclaim, "Pipeline, vertex and pixel shaders cannot be null!");
	
	std::string vertexShaderSrc = readFileBinary(vertexShader->getFilePath());
	std::cout << "Vertex Shader path: " << vertexShader->getFilePath(); // for testing
	VkShaderModule vertexShaderModule = {};
	Vulkan::createShaderModule(_logicDevice, &vertexShaderModule, vertexShaderSrc);

	pipeline->vertexSInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipeline->vertexSInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	pipeline->vertexSInfo.module = vertexShaderModule;
	pipeline->vertexSInfo.pName = "main";

	std::string pixelShaderSrc = readFileBinary(pixelShader->getFilePath());
	std::cout << "Pixel Shader path: " << pixelShader->getFilePath(); // for testing
	VkShaderModule pixelShaderModule = {};
	Vulkan::createShaderModule(_logicDevice, &pixelShaderModule, pixelShaderSrc);

	pipeline->pixelSInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipeline->pixelSInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	pipeline->pixelSInfo.module = pixelShaderModule;
	pipeline->pixelSInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[2] = { // TODO: Make configurable
		pipeline->vertexSInfo,
		pipeline->pixelSInfo
	};

	pipeline->pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline->pipelineInfo.stageCount = 2; // TODO: Make configurable
	pipeline->pipelineInfo.pStages = shaderStages;
	pipeline->pipelineInfo.pVertexInputState = &_vertexInputInfo;
	pipeline->pipelineInfo.pInputAssemblyState = &_inputAssemblyInfo;
	pipeline->pipelineInfo.pViewportState = &_viewportStateInfo;
	pipeline->pipelineInfo.pRasterizationState = &_rasterStateInfo;
	pipeline->pipelineInfo.pMultisampleState = &_multisampleInfo;
	pipeline->pipelineInfo.pDepthStencilState = nullptr; // &_depthStencilInfo;
	pipeline->pipelineInfo.pColorBlendState = &_colorBlendInfo;
	pipeline->pipelineInfo.pDynamicState = &_dynamicStateInfo;
	pipeline->pipelineInfo.layout = _pipelineLayout;
	pipeline->pipelineInfo.renderPass = _renderpass;
	pipeline->pipelineInfo.subpass = 0;
	pipeline->pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipeline->pipelineInfo.basePipelineIndex = 0;

	/* VkPipelineCacheCreateInfo pipelineCacheInfo;
    pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    pipelineCacheInfo.pNext = NULL;
    pipelineCacheInfo.initialDataSize = 0;
    pipelineCacheInfo.pInitialData = NULL;
    pipelineCacheInfo.flags = 0;

	VkPipelineCache pipelineCache;
    if(vkCreatePipelineCache(info.device, &pipelineCacheInfo, NULL, &pipelineCache) == VK_SUCCESS)
		logMessage("Pipeline cache creation success\n");
	else return logMessage("Pipeline cache creation failure"); */

	std::cout << "Pipeline Creation Checkpoint!" << std::endl;

	if(vkCreateGraphicsPipelines(_logicDevice, VK_NULL_HANDLE, 1, &pipeline->pipelineInfo, nullptr, &pipeline->pipeline) == VK_SUCCESS)
		logMessage("Pipeline creation success\n");
	else return logMessage("Pipeline creation failure!");

	vkDestroyShaderModule(_logicDevice, vertexShaderModule, nullptr);
	vkDestroyShaderModule(_logicDevice, pixelShaderModule, nullptr);

	pipeline->entryShader = vertexShader;
	pipeline->isReady = true;
	setPipeline(pipeline);
}

void Topl_Renderer_Vulkan::genPipeline(Topl_Pipeline_Vulkan* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr geomShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader){
	if(pipeline == nullptr || vertexShader == nullptr || pixelShader == nullptr)
		return logMessage(MESSAGE_Exclaim, "Pipeline, vertex and pixel shaders cannot be null!");
	
	unsigned short shaderStageCount = 0;
	VkPipelineShaderStageCreateInfo shaderStages[5];

	std::string vertexShaderSrc = readFileBinary(vertexShader->getFilePath());
	VkShaderModule vertexShaderModule = {};
	Vulkan::createShaderModule(_logicDevice, &vertexShaderModule, vertexShaderSrc);

	pipeline->vertexSInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipeline->vertexSInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	pipeline->vertexSInfo.module = vertexShaderModule;
	pipeline->vertexSInfo.pName = "main";
	shaderStages[0] = pipeline->vertexSInfo;

	std::string pixelShaderSrc = readFileBinary(pixelShader->getFilePath());
	VkShaderModule pixelShaderModule = {};
	Vulkan::createShaderModule(_logicDevice, &pixelShaderModule, pixelShaderSrc);

	pipeline->pixelSInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipeline->pixelSInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	pipeline->pixelSInfo.module = pixelShaderModule;
	pipeline->pixelSInfo.pName = "main";
	shaderStages[1] = pipeline->pixelSInfo;

	VkShaderModule geomShaderModule = {};
	if(geomShader != nullptr){
		shaderStageCount++;

		std::string geomShaderSrc = readFileBinary(geomShader->getFilePath());
		VkShaderModule geomShaderModule = {};
		Vulkan::createShaderModule(_logicDevice, &geomShaderModule, geomShaderSrc);

		pipeline->geomSInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipeline->geomSInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
		pipeline->geomSInfo.module = geomShaderModule;
		pipeline->geomSInfo.pName = "main";
		shaderStages[shaderStageCount] = pipeline->geomSInfo;
	}

	VkShaderModule tessCtrlShaderModule = {};
	if(tessCtrlShader != nullptr){
		shaderStageCount++;

		std::string tessCtrlShaderSrc = readFileBinary(tessCtrlShader->getFilePath());
		VkShaderModule tessCtrlShaderModule = {};
		Vulkan::createShaderModule(_logicDevice, &tessCtrlShaderModule, tessCtrlShaderSrc);

		pipeline->tessCtrlSInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipeline->tessCtrlSInfo.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		pipeline->tessCtrlSInfo.module = tessCtrlShaderModule;
		pipeline->tessCtrlSInfo.pName = "main";
		shaderStages[shaderStageCount] = pipeline->tessCtrlSInfo;
	}

	VkShaderModule tessEvalShaderModule = {};
	if(tessEvalShader != nullptr){
		shaderStageCount++;

		std::string tessEvalShaderSrc = readFileBinary(tessEvalShader->getFilePath());
		VkShaderModule tessEvalShaderModule = {};
		Vulkan::createShaderModule(_logicDevice, &tessEvalShaderModule, tessEvalShaderSrc);

		pipeline->tessEvalSInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipeline->tessEvalSInfo.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		pipeline->tessEvalSInfo.module = tessEvalShaderModule;
		pipeline->tessEvalSInfo.pName = "main";
		shaderStages[shaderStageCount] = pipeline->tessEvalSInfo;
	}

	pipeline->pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline->pipelineInfo.stageCount = shaderStageCount;
	pipeline->pipelineInfo.pStages = shaderStages;
	pipeline->pipelineInfo.pVertexInputState = &_vertexInputInfo;
	pipeline->pipelineInfo.pInputAssemblyState = &_inputAssemblyInfo;
	pipeline->pipelineInfo.pViewportState = &_viewportStateInfo;
	pipeline->pipelineInfo.pRasterizationState = &_rasterStateInfo;
	pipeline->pipelineInfo.pMultisampleState = &_multisampleInfo;
	pipeline->pipelineInfo.pDepthStencilState = &_depthStencilInfo;
	pipeline->pipelineInfo.pColorBlendState = &_colorBlendInfo;
	pipeline->pipelineInfo.layout = _pipelineLayout;
	pipeline->pipelineInfo.renderPass = _renderpass;
	pipeline->pipelineInfo.subpass = 0;
	pipeline->pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipeline->pipelineInfo.basePipelineIndex = -1; // 0

	if(vkCreateGraphicsPipelines(_logicDevice, VK_NULL_HANDLE, 1, &pipeline->pipelineInfo, nullptr, &pipeline->pipeline) == VK_SUCCESS)
		logMessage("Pipeline creation success\n");
	else return logMessage("Pipeline creation failure!");

	vkDestroyShaderModule(_logicDevice, vertexShaderModule, nullptr);
	vkDestroyShaderModule(_logicDevice, pixelShaderModule, nullptr);

	pipeline->entryShader = vertexShader;
	pipeline->isReady = true;
	setPipeline(pipeline);
}