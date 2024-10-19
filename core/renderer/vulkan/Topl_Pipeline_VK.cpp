#include "Topl_Renderer_VK.hpp"

namespace VK {
	void createShaderModule(VkDevice* device, VkShaderModule* shaderModule, std::string& shaderSrc){
		VkResult result; // error checking variable
		
		VkShaderModuleCreateInfo moduleCreateInfo = {};
		moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		moduleCreateInfo.codeSize = shaderSrc.length();
		moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderSrc.c_str());

		result = vkCreateShaderModule(*device, &moduleCreateInfo, nullptr, shaderModule);
		if(result == VK_SUCCESS) logMessage(" shader module creation success\n");
		else return logMessage(MESSAGE_Exclaim, " shader module creation failure!\n");
	}

	void createShaderInfo(VkPipelineShaderStageCreateInfo* shaderInfo, VkShaderModule* shaderModule, VkShaderStageFlagBits stage){
		shaderInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderInfo->stage = stage;
		shaderInfo->pNext = 0;
		shaderInfo->flags = 0;
		shaderInfo->pSpecializationInfo = nullptr;
		shaderInfo->module = *shaderModule;
		shaderInfo->pName = "main";
	}

	VkResult createPipelineLayout(VkDevice* device, VkPipelineLayout* pipelineLayout, VkPipelineVertexInputStateCreateInfo* vertexInputs, entry_shader_cptr entryShader){
		VkPipelineLayoutCreateInfo layoutInfo = {};

		vertexInputs->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputs->vertexBindingDescriptionCount = 0; // TODO: Add vertex bindings
		vertexInputs->pVertexBindingDescriptions = nullptr;
		vertexInputs->vertexAttributeDescriptionCount = 0; // TODO: Add vertex attributes
		vertexInputs->pVertexAttributeDescriptions = nullptr;

		// TODO: Use entry shader to determine layout

		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.setLayoutCount = 0;
		layoutInfo.pSetLayouts = nullptr;
		layoutInfo.pushConstantRangeCount = 0;
		layoutInfo.pPushConstantRanges = nullptr;

		return vkCreatePipelineLayout(*device, &layoutInfo, nullptr, pipelineLayout);
	}
}

void Topl_Renderer_VK::setPipeline(Topl_Pipeline_VK* pipeline){
	_pipeline = pipeline;
	Topl_Renderer::setPipeline((Topl_Pipeline*)pipeline);

	if(vkBeginCommandBuffer(_commandBuffers[0], &_commandBufferInfo) != VK_SUCCESS) logMessage(MESSAGE_Exclaim, "Command buffer begin failure!\n");
	vkCmdBindPipeline(_commandBuffers[0], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline->pipeline);
	if(vkEndCommandBuffer(_commandBuffers[0]) != VK_SUCCESS) logMessage(MESSAGE_Exclaim, "Command buffer ending failure!\n");
}

void Topl_Renderer_VK::genPipeline(Topl_Pipeline_VK* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader){
	if(pipeline == nullptr || vertexShader == nullptr || pixelShader == nullptr)
		return logMessage(MESSAGE_Exclaim, "Pipeline, vertex and pixel shaders cannot be null!");
	
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	VK::createPipelineLayout(&_logicDevice, &_pipelineLayout, &vertexInputInfo, vertexShader);

	std::string vertexShaderSrc = readFileBinary(vertexShader->getFilePath().c_str());
	std::cout << "Vertex Shader path: " << vertexShader->getFilePath().c_str(); // for testing
	VkShaderModule vertexShaderModule = {};
	VK::createShaderModule(&_logicDevice, &vertexShaderModule, vertexShaderSrc);
	VK::createShaderInfo(&pipeline->vertexSInfo, &vertexShaderModule, VK_SHADER_STAGE_VERTEX_BIT);

	std::string pixelShaderSrc = readFileBinary(pixelShader->getFilePath().c_str());
	std::cout << "Pixel Shader path: " << pixelShader->getFilePath().c_str(); // for testing
	VkShaderModule pixelShaderModule = {};
	VK::createShaderModule(&_logicDevice, &pixelShaderModule, pixelShaderSrc);
	VK::createShaderInfo(&pipeline->pixelSInfo, &pixelShaderModule, VK_SHADER_STAGE_FRAGMENT_BIT);

	VkPipelineShaderStageCreateInfo shaderStages[2] = { // TODO: Make configurable
		pipeline->vertexSInfo,
		pipeline->pixelSInfo
	};

	pipeline->pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline->pipelineInfo.stageCount = 2; // TODO: Make configurable
	pipeline->pipelineInfo.pStages = shaderStages;
	pipeline->pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipeline->pipelineInfo.pInputAssemblyState = &_inputAssemblyInfo;
	pipeline->pipelineInfo.pViewportState = &_viewportStateInfo;
	pipeline->pipelineInfo.pRasterizationState = &_rasterStateInfo;
	pipeline->pipelineInfo.pMultisampleState = &_multisampleInfo;
	pipeline->pipelineInfo.pDepthStencilState = nullptr; // &_depthStencilInfo;
	pipeline->pipelineInfo.pColorBlendState = &_colorBlendInfo;
	pipeline->pipelineInfo.pDynamicState = &_dynamicStateInfo;
	pipeline->pipelineInfo.layout = _pipelineLayout;
	pipeline->pipelineInfo.renderPass = _renderPass;
	pipeline->pipelineInfo.subpass = 0;
	pipeline->pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipeline->pipelineInfo.basePipelineIndex = 0;

	if(vkCreateGraphicsPipelines(_logicDevice, VK_NULL_HANDLE, 1, &pipeline->pipelineInfo, nullptr, &pipeline->pipeline) == VK_SUCCESS)
		logMessage("Pipeline creation success\n");
	else return logMessage("Pipeline creation failure!");

	vkDestroyShaderModule(_logicDevice, vertexShaderModule, nullptr);
	vkDestroyShaderModule(_logicDevice, pixelShaderModule, nullptr);

	pipeline->entryShader = vertexShader;
	pipeline->isReady = true;
	setPipeline(pipeline);
}

void Topl_Renderer_VK::genPipeline(Topl_Pipeline_VK* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr geomShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader){
	if(pipeline == nullptr || vertexShader == nullptr || pixelShader == nullptr)
		return logMessage(MESSAGE_Exclaim, "Pipeline, vertex and pixel shaders cannot be null!");
	
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	VK::createPipelineLayout(&_logicDevice, &_pipelineLayout, &vertexInputInfo, vertexShader);

	unsigned short shaderStageCount = 0;
	VkPipelineShaderStageCreateInfo shaderStages[5];

	std::string vertexShaderSrc = readFileBinary(vertexShader->getFilePath().c_str());
	std::cout << "Vertex Shader path: " << vertexShader->getFilePath().c_str(); // for testing
	VkShaderModule vertexShaderModule = {};
	VK::createShaderModule(&_logicDevice, &vertexShaderModule, vertexShaderSrc);
	VK::createShaderInfo(&pipeline->vertexSInfo, &vertexShaderModule, VK_SHADER_STAGE_VERTEX_BIT);
	shaderStages[0] = pipeline->vertexSInfo;

	std::string pixelShaderSrc = readFileBinary(pixelShader->getFilePath().c_str());
	std::cout << "Pixel Shader path: " << pixelShader->getFilePath().c_str(); // for testing
	VkShaderModule pixelShaderModule = {};
	VK::createShaderModule(&_logicDevice, &pixelShaderModule, pixelShaderSrc);
	VK::createShaderInfo(&pipeline->pixelSInfo, &pixelShaderModule, VK_SHADER_STAGE_FRAGMENT_BIT);
	shaderStages[1] = pipeline->pixelSInfo;

	VkShaderModule geomShaderModule = {};
	if(geomShader != nullptr){
		shaderStageCount++;

		std::string geomShaderSrc = readFileBinary(geomShader->getFilePath().c_str());
		VkShaderModule geomShaderModule = {};
		VK::createShaderModule(&_logicDevice, &geomShaderModule, geomShaderSrc);
		VK::createShaderInfo(&pipeline->geomSInfo, &geomShaderModule, VK_SHADER_STAGE_GEOMETRY_BIT);
		shaderStages[shaderStageCount] = pipeline->geomSInfo;
	}

	VkShaderModule tessCtrlShaderModule = {};
	if(tessCtrlShader != nullptr){
		shaderStageCount++;

		std::string tessCtrlShaderSrc = readFileBinary(tessCtrlShader->getFilePath().c_str());
		VkShaderModule tessCtrlShaderModule = {};
		VK::createShaderModule(&_logicDevice, &tessCtrlShaderModule, tessCtrlShaderSrc);
		VK::createShaderInfo(&pipeline->tessCtrlSInfo, &tessCtrlShaderModule, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
		shaderStages[shaderStageCount] = pipeline->tessCtrlSInfo;
	}

	VkShaderModule tessEvalShaderModule = {};
	if(tessEvalShader != nullptr){
		shaderStageCount++;

		std::string tessEvalShaderSrc = readFileBinary(tessEvalShader->getFilePath().c_str());
		VkShaderModule tessEvalShaderModule = {};
		VK::createShaderModule(&_logicDevice, &tessEvalShaderModule, tessEvalShaderSrc);
		VK::createShaderInfo(&pipeline->tessEvalSInfo, &tessEvalShaderModule, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
		shaderStages[shaderStageCount] = pipeline->tessEvalSInfo;
	}

	pipeline->pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline->pipelineInfo.stageCount = shaderStageCount;
	pipeline->pipelineInfo.pStages = shaderStages;
	pipeline->pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipeline->pipelineInfo.pInputAssemblyState = &_inputAssemblyInfo;
	pipeline->pipelineInfo.pViewportState = &_viewportStateInfo;
	pipeline->pipelineInfo.pRasterizationState = &_rasterStateInfo;
	pipeline->pipelineInfo.pMultisampleState = &_multisampleInfo;
	pipeline->pipelineInfo.pDepthStencilState = nullptr; // &_depthStencilInfo;
	pipeline->pipelineInfo.pColorBlendState = &_colorBlendInfo;
	pipeline->pipelineInfo.pDynamicState = &_dynamicStateInfo;
	pipeline->pipelineInfo.layout = _pipelineLayout;
	pipeline->pipelineInfo.renderPass = _renderPass;
	pipeline->pipelineInfo.subpass = 0;
	pipeline->pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipeline->pipelineInfo.basePipelineIndex = 0;

	if(vkCreateGraphicsPipelines(_logicDevice, VK_NULL_HANDLE, 1, &pipeline->pipelineInfo, nullptr, &pipeline->pipeline) == VK_SUCCESS)
		logMessage("Pipeline creation success\n");
	else return logMessage("Pipeline creation failure!");

	vkDestroyShaderModule(_logicDevice, vertexShaderModule, nullptr);
	vkDestroyShaderModule(_logicDevice, pixelShaderModule, nullptr);

	pipeline->entryShader = vertexShader;
	pipeline->isReady = true;
	setPipeline(pipeline);
}