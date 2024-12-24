#include "Topl_Renderer_VK.hpp"

namespace VK {
    VkFormat getShaderFormat(enum SHDR_ValueType type) {
        switch (type) {
        case SHDR_float_vec4: return VK_FORMAT_R32G32B32A32_SFLOAT;
        case SHDR_float_vec3: return VK_FORMAT_R32G32B32_SFLOAT;
        case SHDR_float_vec2: return VK_FORMAT_R32G32_SFLOAT;
        case SHDR_float: return VK_FORMAT_R32_SFLOAT;
        case SHDR_uint_vec4: return VK_FORMAT_R8G8B8A8_UINT;
        case SHDR_uint_vec3: return VK_FORMAT_R8G8B8_UINT;
        case SHDR_uint_vec2: return VK_FORMAT_R8G8_UINT;
        case SHDR_uint: return VK_FORMAT_R8_UINT;
        default:
            logMessage(MESSAGE_Exclaim, "VK Shader Input Type Not Supported!");
            return VK_FORMAT_UNDEFINED; // error
        }
    }

	VkResult createShaderModule(VkDevice* device, VkShaderModule* shaderModule, std::string& shaderSrc){
		VkResult result; // error checking variable
		
		VkShaderModuleCreateInfo moduleCreateInfo = {};
		moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		moduleCreateInfo.codeSize = shaderSrc.length();
		moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderSrc.c_str());

		result = vkCreateShaderModule(*device, &moduleCreateInfo, nullptr, shaderModule);
		if(result == VK_SUCCESS) logMessage(" shader module creation success\n");
		else logMessage(MESSAGE_Exclaim, " shader module creation failure!\n");

		return result;
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
		VkVertexInputBindingDescription vertexBindDesc = {};

        vertexBindDesc.binding = 0;
        vertexBindDesc.stride = sizeof(Geo_Vertex);
        vertexBindDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		// vertexBindDesc.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

        VkVertexInputAttributeDescription* vertexAttribDescs = (VkVertexInputAttributeDescription*)malloc(sizeof(VkVertexInputAttributeDescription) * MAX_SHADER_INPUTS);

        unsigned short inputOffset = 0;
        for(unsigned inputNum = 0; inputNum < entryShader->getInputCount() && inputNum < MAX_SHADER_INPUTS; inputNum++){
            const Shader_Type* shaderType = entryShader->getInputAtIndex(inputNum);

            *(vertexAttribDescs + inputNum) = {
                0, inputNum,
                getShaderFormat(shaderType->type),
                inputOffset
            };

            inputOffset += Topl_Pipeline::getOffset(shaderType->type);
            logMessage("Created input with name " + shaderType->name + " at offset " + std::to_string(inputOffset) + '\n');
        }

		vertexInputs->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputs->vertexBindingDescriptionCount = 1;
        vertexInputs->pVertexBindingDescriptions = &vertexBindDesc;
        vertexInputs->vertexAttributeDescriptionCount = entryShader->getInputCount();
        vertexInputs->pVertexAttributeDescriptions = vertexAttribDescs;

		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.setLayoutCount = 0;
		layoutInfo.pSetLayouts = nullptr;
		layoutInfo.pushConstantRangeCount = 0;
		layoutInfo.pPushConstantRanges = nullptr;

		VkResult result = vkCreatePipelineLayout(*device, &layoutInfo, nullptr, pipelineLayout);
		free(vertexAttribDescs);
		return result;
	} 
}

void Topl_Renderer_VK::setPipeline(VK::Pipeline* pipeline){
	_pipeline = pipeline;
	Topl_Renderer::setPipeline((Topl_Pipeline*)pipeline);

	if(vkBeginCommandBuffer(_commandBuffers[0], &_commandBufferInfo) != VK_SUCCESS) logMessage(MESSAGE_Exclaim, "Command buffer begin failure!\n");
	vkCmdBindPipeline(_commandBuffers[0], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline->pipeline);
	if(vkEndCommandBuffer(_commandBuffers[0]) != VK_SUCCESS) logMessage(MESSAGE_Exclaim, "Command buffer ending failure!\n");
}

void Topl_Renderer_VK::genPipeline(VK::Pipeline* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, std::initializer_list<shader_cptr> shaders){
	VkResult result;
	
	if(pipeline == nullptr || ((vertexShader == nullptr || pixelShader == nullptr) && shaders.size() == 0))
		return logMessage(MESSAGE_Exclaim, "Pipeline, vertex and pixel shaders cannot be null!");
	
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	VK::createPipelineLayout(&_logicDevice, &_pipelineLayout, &vertexInputInfo, vertexShader);

	unsigned short shaderStageCount = 2;
	VkPipelineShaderStageCreateInfo shaderStages[MAX_PIPELINE_STAGES];

	VkShaderModule vertexShaderModule = {}; // Vertex Shader
	std::string vertexShaderSrc = readFileBinary(vertexShader->getFilePath().c_str());
	std::cout << "Vertex Shader path: " << vertexShader->getFilePath().c_str(); // for testing
	result = VK::createShaderModule(&_logicDevice, &vertexShaderModule, vertexShaderSrc);
	if(result == VK_SUCCESS) VK::createShaderInfo(&pipeline->vertexSInfo, &vertexShaderModule, VK_SHADER_STAGE_VERTEX_BIT);
	else { pipeline->isReady = false; return; }
	shaderStages[0] = pipeline->vertexSInfo;
	

	VkShaderModule pixelShaderModule = {}; // Pixel Shader
	std::string pixelShaderSrc = readFileBinary(pixelShader->getFilePath().c_str());
	std::cout << "Pixel Shader path: " << pixelShader->getFilePath().c_str(); // for testing
	result = VK::createShaderModule(&_logicDevice, &pixelShaderModule, pixelShaderSrc);
	if(result == VK_SUCCESS) VK::createShaderInfo(&pipeline->pixelSInfo, &pixelShaderModule, VK_SHADER_STAGE_FRAGMENT_BIT);
	else { pipeline->isReady = false; return; }
	shaderStages[1] = pipeline->pixelSInfo;

	auto geomShader = std::find_if(shaders.begin(), shaders.end(), [](const shader_cptr& s){ return s->getType() == SHDR_Geom; });
	auto tessCtrlShader = std::find_if(shaders.begin(), shaders.end(), [](const shader_cptr& s){ return s->getType() == SHDR_TessCtrl; });
	auto tessEvalShader = std::find_if(shaders.begin(), shaders.end(), [](const shader_cptr& s){ return s->getType() == SHDR_TessEval; });
	auto computeShader = std::find_if(shaders.begin(), shaders.end(), [](const shader_cptr& s){ return s->getType() == SHDR_Compute; });

	VkShaderModule geomShaderModule = {}; // Geometry Shader
	if(geomShader != shaders.end()){
		shaderStageCount++;

		std::string geomShaderSrc = readFileBinary((*geomShader)->getFilePath().c_str());
		result = VK::createShaderModule(&_logicDevice, &geomShaderModule, geomShaderSrc);
		if(result == VK_SUCCESS) VK::createShaderInfo(&pipeline->geomSInfo, &geomShaderModule, VK_SHADER_STAGE_GEOMETRY_BIT);
		else { pipeline->isReady = false; return; }
		shaderStages[shaderStageCount] = pipeline->geomSInfo;
	}

	VkShaderModule tessCtrlShaderModule = {}; // Tess. Control Shader
	if(tessCtrlShader != shaders.end()){
		shaderStageCount++;

		std::string tessCtrlShaderSrc = readFileBinary((*tessCtrlShader)->getFilePath().c_str());
		result = VK::createShaderModule(&_logicDevice, &tessCtrlShaderModule, tessCtrlShaderSrc);
		if(result == VK_SUCCESS) VK::createShaderInfo(&pipeline->tessCtrlSInfo, &tessCtrlShaderModule, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
		else { pipeline->isReady = false; return; }
		shaderStages[shaderStageCount] = pipeline->tessCtrlSInfo;
	}

	VkShaderModule tessEvalShaderModule = {}; // Tess. Evaluation Shader
	if(tessEvalShader != shaders.end()){
		shaderStageCount++;

		std::string tessEvalShaderSrc = readFileBinary((*tessEvalShader)->getFilePath().c_str());
		result = VK::createShaderModule(&_logicDevice, &tessEvalShaderModule, tessEvalShaderSrc);
		if(result == VK_SUCCESS) VK::createShaderInfo(&pipeline->tessEvalSInfo, &tessEvalShaderModule, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
		else { pipeline->isReady = false; return; }
		shaderStages[shaderStageCount] = pipeline->tessEvalSInfo;
	}

	VkShaderModule computeShaderModule = {}; // Compute Shader
	if(tessEvalShader != shaders.end()){
		shaderStageCount++;

		std::string computeShaderSrc = readFileBinary((*computeShader)->getFilePath().c_str());
		result = VK::createShaderModule(&_logicDevice, &computeShaderModule, computeShaderSrc);
		if(result == VK_SUCCESS) VK::createShaderInfo(&pipeline->computeSInfo, &computeShaderModule, VK_SHADER_STAGE_COMPUTE_BIT);
		else { pipeline->isReady = false; return; }
		shaderStages[shaderStageCount] = pipeline->computeSInfo;
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

	std::cout << "About to create pipeline" << std::endl;
	if(vkCreateGraphicsPipelines(_logicDevice, VK_NULL_HANDLE, 1, &pipeline->pipelineInfo, nullptr, &pipeline->pipeline) == VK_SUCCESS)
		logMessage("Pipeline creation success\n");
	else return logMessage("Pipeline creation failure!");

	vkDestroyShaderModule(_logicDevice, vertexShaderModule, nullptr);
	vkDestroyShaderModule(_logicDevice, pixelShaderModule, nullptr);
	if(tessCtrlShader != shaders.end()) vkDestroyShaderModule(_logicDevice, geomShaderModule, nullptr);
	if(tessEvalShader != shaders.end()) vkDestroyShaderModule(_logicDevice, tessCtrlShaderModule, nullptr);
	if(geomShader != shaders.end()) vkDestroyShaderModule(_logicDevice, tessEvalShaderModule, nullptr);

	pipeline->setShaders(vertexShader, pixelShader, shaders);
	setPipeline(pipeline);
}
