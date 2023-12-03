#include "Topl_Renderer_Vulkan.hpp"

namespace Vulkan {
	VKAPI_ATTR VkBool32 VKAPI_CALL debugReportLogCallback(
		VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
		uint64_t object, size_t location, int32_t messageCode,
		const char* pLayerPrefix, const char* pMessage, void* pUserData
	) {
		logMessage(MESSAGE_Exclaim, std::string(pMessage));
		return VK_FALSE;
	}

	static VkResult getExtensionProperties(std::vector<VkExtensionProperties>* extensions){
		VkResult result;

		unsigned extensionCount;
		result = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
		if(result != VK_SUCCESS) return result;

		extensions->resize(extensionCount);
		result = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensions->data());
		
		return result;
	}

	static VkResult createDebugReport(VkInstance* instance){
		VkResult result;

		PFN_vkCreateDebugReportCallbackEXT createDebugReportCallback = NULL;
		createDebugReportCallback = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(*instance, "vkCreateDebugReportCallbackEXT"));

		VkDebugReportCallbackCreateInfoEXT callbackCreateInfo = {};
		callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
		callbackCreateInfo.pNext = NULL;
		callbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		callbackCreateInfo.pfnCallback = debugReportLogCallback;
		callbackCreateInfo.pUserData = NULL;

		VkDebugReportCallbackEXT debugReportCallback;
		result = createDebugReportCallback(*instance, &callbackCreateInfo, NULL, &debugReportCallback);

		return result;
	}
}

void Topl_Renderer_Vulkan::init(NATIVE_WINDOW window) {
	VkResult result; // error checking variable

	// Instance Creation

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Topl";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Topl Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	result = Vulkan::getExtensionProperties(&_vulkanExtensions);
	if(result == VK_SUCCESS) logMessage("Extension loading success!\n");
	else return logMessage(MESSAGE_Exclaim, "Extension loading failure!\n");

	const unsigned instanceExtCount = 3;
	const char* instanceExtensions[instanceExtCount] = {
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME, // should be optional for debug?
		VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME, // Windows only
#else
		// VK_KHR_XCB_SURFACE_EXTENSION_NAME, // Unix only for XCB
		VK_KHR_XLIB_SURFACE_EXTENSION_NAME, // Unix Only for XLIB
#endif
	};

/* #ifdef ENABLE_DEBUG_LAYERS
	const unsigned instanceLayerCount = 1;
	const char* instanceLayers[instanceLayerCount] = { "VK_LAYER_LUNARG_standard_validation" };
#endif */

	VkInstanceCreateInfo instanceInfo = {};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = &appInfo;
/* #ifdef ENABLE_DEBUG_LAYERS
	instanceInfo.enabledLayerCount = instanceLayerCount;
	instanceInfo.ppEnabledLayerNames = instanceLayers;
#endif */
	instanceInfo.enabledExtensionCount = instanceExtCount;
	instanceInfo.ppEnabledExtensionNames = instanceExtensions;

	result = vkCreateInstance(&instanceInfo, NULL, &_instance);
	if(result == VK_SUCCESS) logMessage("Instance creation success!\n");
	else return logMessage(MESSAGE_Exclaim, "Instance creation failure!\n");

/* #ifdef ENABLE_DEBUG_LAYERS
	result = Vulkan::createDebugReport(&_instance);
	if(result == VK_SUCCESS) logMessage("Debug report success!\n");
	else return logMessage(MESSAGE_Exclaim, "Debug report failure!\n");
#endif */


	// Physical Device Enumeration

	unsigned physDeviceCount = 0;

	result = vkEnumeratePhysicalDevices(_instance, &physDeviceCount, NULL);
	if(physDeviceCount > 1) logMessage("Multiple phys devices detected!\n");
	else if(physDeviceCount == 0) return logMessage(MESSAGE_Exclaim, "No phys devices detected");

	_physicalDevices.resize(physDeviceCount);
	result = vkEnumeratePhysicalDevices(_instance, &physDeviceCount, _physicalDevices.data());
	if(result == VK_SUCCESS) logMessage("Physical device enumeration success!\n");
	else return logMessage(MESSAGE_Exclaim, "Physical device enumeration failure!\n");

	VkPhysicalDeviceProperties physDeviceProps;
	vkGetPhysicalDeviceProperties(_physicalDevices[0], &physDeviceProps);
	VkPhysicalDeviceFeatures physDeviceFeats;
	vkGetPhysicalDeviceFeatures(_physicalDevices[0], &physDeviceFeats);


	// Surface Creation

#ifdef _WIN32
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = NULL;
	surfaceCreateInfo.hwnd = _platformCtx.window;
	surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);

	result = vkCreateWin32SurfaceKHR(_instance, &surfaceCreateInfo, NULL, &_surface);
#elif defined(__linux__)
	VkXlibSurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = NULL;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.dpy = _platformCtx.display;
	surfaceCreateInfo.window = _platformCtx.window;

	result = vkCreateXlibSurfaceKHR(_instance, &surfaceCreateInfo, NULL, &_surface);
#endif
	if(result == VK_SUCCESS) logMessage("Surface creation sucess\n");
	else return logMessage(MESSAGE_Exclaim, "Surface creation failure!\n");


	// Queue Family Enumeration

	unsigned queueFamilyCount = 0;

	vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevices[0], &queueFamilyCount, NULL);
	logMessage("Physical device queue family count: " + std::to_string(queueFamilyCount) + '\n');

	_queueFamilyProps.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevices[0], &queueFamilyCount, _queueFamilyProps.data());
	if(result == VK_SUCCESS) logMessage("Queue family enumeration success! Family count: " + std::to_string(queueFamilyCount) + "\n");
	else return logMessage(MESSAGE_Exclaim, "Queue family enumeration failure!\n");


	// Queue Family Selection

	VkDeviceQueueCreateInfo queueInfo = {};
	float queuePriorities[1] = { 0.0f }; // float queuePriority = 1.0;
	unsigned gSupportIdx, pSupportIdx; // graphics and presentation support indices

	for(unsigned q = 0; q < queueFamilyCount; q++){ // queue graphics bit detection
		if(_queueFamilyProps[q].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			gSupportIdx = q; // sets the graphics support index
			logMessage("Graphics bit detected for queue family no. " + std::to_string(q) + "\n");
		}
		else logMessage("Graphics bit not detected for queue family no. " + std::to_string(q) + "\n");
	}

	VkBool32 isPresentable = false;
	for(unsigned q = 0; q < queueFamilyCount; q++){ // queue graphics bit detection
		vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevices[0], q, _surface, &isPresentable);
		if(isPresentable) {
			pSupportIdx = q; // sets the presentation support index
			logMessage("Presentation support for queue family no. " + std::to_string(q) + "\n");
		}
		else logMessage("Presentation support not detected for queue family no. " + std::to_string(q) + "\n");
	}

	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.pNext = NULL;
	queueInfo.queueFamilyIndex = gSupportIdx;
	queueInfo.queueCount = 1;
	queueInfo.pQueuePriorities = queuePriorities; // &queuePriority;


	// Logical Device Creation

	VkPhysicalDeviceFeatures deviceFeatures = {};
	const char* deviceExtensions[1] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	VkDeviceCreateInfo deviceInfo = {};
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pNext = NULL;
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pQueueCreateInfos = &queueInfo;
	deviceInfo.enabledExtensionCount = 1; // for swapchain support
	deviceInfo.ppEnabledExtensionNames = deviceExtensions; // for swapchain support
	deviceInfo.enabledLayerCount = 0;
	deviceInfo.ppEnabledLayerNames = NULL;
	deviceInfo.pEnabledFeatures = /* NULL; */ &deviceFeatures;
	
	result = vkCreateDevice(_physicalDevices[0], &deviceInfo, NULL, &_logicDevice);
	if(result == VK_SUCCESS) logMessage("Logical device creation success\n");
	else return logMessage(MESSAGE_Exclaim, "Logical device creation failure!\n");

	vkGetDeviceQueue(_logicDevice, pSupportIdx, 0, &_graphicsQueue);


	// Swapchain Support

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevices[0], _surface, &_surfaceCaps);

	unsigned formatCount;

	vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevices[0], _surface, &formatCount, nullptr);
	logMessage("Surface format count: " + std::to_string(formatCount) + "\n");

	bool formatFound = false;
	_surfaceFormats.resize(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevices[0], _surface, &formatCount, _surfaceFormats.data());
	for(unsigned f = 0; f < formatCount; f++)
		if(_surfaceFormats[f].format == VK_FORMAT_B8G8R8A8_UNORM && _surfaceFormats[f].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			formatFound = true;

	if(formatFound) logMessage("Format found: {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR} \n");
	else return logMessage(MESSAGE_Exclaim, "Format not found! \n");


	unsigned presentModeCount;

	vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevices[0], _surface, &presentModeCount, nullptr);
	logMessage("Present modes count: " + std::to_string(presentModeCount) + "\n");

	bool presentModeFount = false;
	_presentModes.resize(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevices[0], _surface, &presentModeCount, _presentModes.data());
	for(unsigned p = 0; p < presentModeCount; p++)
		if(_presentModes[p] == VK_PRESENT_MODE_MAILBOX_KHR) presentModeFount = true;

	if(presentModeFount) logMessage("Present mode found: VK_PRESENT_MODE_MAILBOX_KHR \n");
	else return logMessage(MESSAGE_Exclaim, "Present mode not found! \n");

	
	unsigned queueFamIndices[2] = { gSupportIdx, pSupportIdx };
	VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.pNext = NULL;
	swapchainCreateInfo.flags = 0;
	swapchainCreateInfo.surface = _surface;
	swapchainCreateInfo.minImageCount = _surfaceCaps.minImageCount + 1;
	swapchainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM; // preselected
	swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; // preselected
	swapchainCreateInfo.imageExtent.width = TOPL_WIN_WIDTH;
	swapchainCreateInfo.imageExtent.height = TOPL_WIN_HEIGHT;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
	swapchainCreateInfo.queueFamilyIndexCount = 1;
	swapchainCreateInfo.pQueueFamilyIndices = queueFamIndices;
	swapchainCreateInfo.preTransform = _surfaceCaps.currentTransform;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR; // preselected

	result = vkCreateSwapchainKHR(_logicDevice, &swapchainCreateInfo, NULL, &_swapchain);
	if(result == VK_SUCCESS) logMessage("Swapchain creation success\n");
	else return logMessage(MESSAGE_Exclaim, "Swapchain creation failure!\n");

	
	// Get Images from Swapchain

	unsigned imageCount;

	vkGetSwapchainImagesKHR(_logicDevice, _swapchain, &imageCount, NULL);

	_swapchainImages.resize(imageCount);
	_swapchainImageViews.resize(imageCount);
	result = vkGetSwapchainImagesKHR(_logicDevice, _swapchain, &imageCount, _swapchainImages.data());
	if(result == VK_SUCCESS) logMessage(std::to_string(imageCount) + " Swapchain images retrieved " + "\n");
	else return logMessage(MESSAGE_Exclaim, "Swapchain image retrieval failure!\n");

	VkImageViewCreateInfo imageViewCreateInfo = {};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.pNext = NULL;
	imageViewCreateInfo.flags = 0;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;
	for(unsigned i = 0; i < imageCount; i++){
		imageViewCreateInfo.image = _swapchainImages[i];
		result = vkCreateImageView(_logicDevice, &imageViewCreateInfo, nullptr, &_swapchainImageViews[i]);
		if(result == VK_SUCCESS) logMessage("Swapchain image view creation success at no. " + std::to_string(i) + "\n");
		else return logMessage(MESSAGE_Exclaim, "Swapchain image view creation failure! \n");
	}


	// Command Pool and Command Buffers creation

	VkCommandPoolCreateInfo commandPoolInfo = {};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
	commandPoolInfo.flags = 0;
	commandPoolInfo.queueFamilyIndex = gSupportIdx;
	
	result = vkCreateCommandPool(_logicDevice, &commandPoolInfo, nullptr, &_commandPool);
	if(result == VK_SUCCESS) logMessage("Command pool creation success \n");
	else return logMessage(MESSAGE_Exclaim, "Command pool creation failure! \n");

	VkCommandBufferAllocateInfo commandBufferAllocInfo = {};
	commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocInfo.pNext = NULL;
	commandBufferAllocInfo.commandPool = _commandPool;
	commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocInfo.commandBufferCount = imageCount;

	_commandBuffers.resize(imageCount);
	result = vkAllocateCommandBuffers(_logicDevice, &commandBufferAllocInfo, _commandBuffers.data());
	if(result == VK_SUCCESS) logMessage("Command buffers creation success \n");
	else return logMessage(MESSAGE_Exclaim, "Command buffers creation failure! \n");

	// Render Pass Creation

	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	
	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDesc = {};
	subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDesc.colorAttachmentCount = 1;
	subpassDesc.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderpassCreateInfo = {};
	renderpassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderpassCreateInfo.attachmentCount = 1;
	renderpassCreateInfo.pAttachments = &colorAttachment;
	renderpassCreateInfo.subpassCount = 1;
	renderpassCreateInfo.pSubpasses = &subpassDesc;

	result = vkCreateRenderPass(_logicDevice, &renderpassCreateInfo, nullptr, &_renderpass);
	if(result == VK_SUCCESS) logMessage("Render pass creation success \n");
	else return logMessage(MESSAGE_Exclaim, "Render pass creation failure! \n");

	// Command Buffer Execution

	VkCommandBufferBeginInfo commandBufferInfo = {};
	commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferInfo.pNext = nullptr;
	commandBufferInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	commandBufferInfo.pInheritanceInfo = nullptr;

	for(unsigned c = 0; c < _commandBuffers.size(); c++)
		result = vkBeginCommandBuffer(_commandBuffers[c], &commandBufferInfo);
	if(result == VK_SUCCESS) logMessage("Command buffer execution success \n");
	else return logMessage(MESSAGE_Exclaim, "Command buffer execution failure! \n");

	/* VkClearValue clearValues[2];
	clearValues[0].color = { CLEAR_R, CLEAR_G, CLEAR_B, CLEAR_A };
	clearValues[1].depthStencil = { 1.0f, 0 };
	VkRect2D screenRect = {{0, 0}, { TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT }};
	
	VkRenderPassBeginInfo renderPassInfo;
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.pNext = nullptr;
	// renderPassInfo.renderPass = XXXX; // NEED RENDER PASS!!!
	renderPassInfo.renderArea = screenRect;
	renderPassInfo.clearValueCount = 2;
	renderPassInfo.pClearValues = clearValues;
	for(unsigned c = 0; c < _commandBuffers.size(); c++){
		// renderPassInfo.framebuffer = XXXX; // NEED FRAMEBUFFERS!!!
		result = vkBeginCommandBuffer(_commandBuffers[c], &commandBufferInfo);
	} */

	// Vertex Input State
	
	_vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	_vertexInputInfo.vertexBindingDescriptionCount = 0; // TODO: Add vertex bindings
	_vertexInputInfo.pVertexBindingDescriptions = nullptr;
	_vertexInputInfo.vertexAttributeDescriptionCount = 0; // TODO: Add vertex attributes
	_vertexInputInfo.pVertexAttributeDescriptions = nullptr;

	// Rasetrizer State

	_rasterStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	_rasterStateInfo.depthClampEnable = VK_FALSE;
	_rasterStateInfo.rasterizerDiscardEnable = VK_FALSE;
	_rasterStateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	_rasterStateInfo.cullMode = VK_CULL_MODE_NONE;
	_rasterStateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	_rasterStateInfo.depthBiasEnable = VK_FALSE;
	_rasterStateInfo.depthBiasConstantFactor = 0.0;
	_rasterStateInfo.depthBiasClamp = 0.0;
	_rasterStateInfo.depthBiasSlopeFactor = 0.0;

	// Multisampling State

	_multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	_multisampleInfo.sampleShadingEnable = VK_FALSE;
	_multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	_multisampleInfo.minSampleShading = 1.0f;
	_multisampleInfo.pSampleMask = nullptr;
	_multisampleInfo.alphaToCoverageEnable = VK_FALSE;
	_multisampleInfo.alphaToOneEnable = VK_FALSE;

	// Depth and Stencil State

	_depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	_depthStencilInfo.pNext = nullptr;
	_depthStencilInfo.flags = 0;
	_depthStencilInfo.depthTestEnable = ENABLE_VULKAN_DEPTH;
	_depthStencilInfo.depthWriteEnable = ENABLE_VULKAN_DEPTH;
	_depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	_depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	_depthStencilInfo.stencilTestEnable = VK_FALSE;
	_depthStencilInfo.back.failOp = VK_STENCIL_OP_KEEP;
	_depthStencilInfo.back.passOp = VK_STENCIL_OP_KEEP;
	_depthStencilInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
	_depthStencilInfo.back.compareMask = 0;
	_depthStencilInfo.back.reference = 0;
	_depthStencilInfo.back.depthFailOp = VK_STENCIL_OP_KEEP;
	_depthStencilInfo.back.writeMask = 0;
	_depthStencilInfo.minDepthBounds = 0;
	_depthStencilInfo.maxDepthBounds = 0;
	_depthStencilInfo.stencilTestEnable = VK_FALSE;
	_depthStencilInfo.front = _depthStencilInfo.back;

	// Color Blending State

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	_colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	_colorBlendInfo.logicOpEnable = VK_TRUE;
	_colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
	_colorBlendInfo.attachmentCount = 1;
	_colorBlendInfo.pAttachments = &colorBlendAttachment;
	_colorBlendInfo.blendConstants[0] = 0.0f;
	_colorBlendInfo.blendConstants[1] = 0.0f;
	_colorBlendInfo.blendConstants[2] = 0.0f;
	_colorBlendInfo.blendConstants[3] = 0.0f;

	// Pipeline Layout Creation

	_pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	_pipelineLayoutInfo.setLayoutCount = 0;
	_pipelineLayoutInfo.pSetLayouts = nullptr;
	_pipelineLayoutInfo.pushConstantRangeCount = 0;
	_pipelineLayoutInfo.pPushConstantRanges = nullptr;

	result = vkCreatePipelineLayout(_logicDevice, &_pipelineLayoutInfo, nullptr, &_pipelineLayout);
	if(result == VK_SUCCESS) logMessage("Pipeline layout creation success\n");
	else return logMessage(MESSAGE_Exclaim, "Pipeline layout creation failure!\n");
}

Topl_Renderer_Vulkan::~Topl_Renderer_Vulkan() {
	vkDestroyPipelineLayout(_logicDevice, _pipelineLayout, nullptr);
	vkDestroyRenderPass(_logicDevice, _renderpass, nullptr);
	vkFreeCommandBuffers(_logicDevice, _commandPool, _commandBuffers.size(), _commandBuffers.data());
	vkDestroyCommandPool(_logicDevice, _commandPool, nullptr);
	for(unsigned i = 0; i < _swapchainImageViews.size(); i++) vkDestroyImageView(_logicDevice, _swapchainImageViews[i], NULL);
	vkDestroySwapchainKHR(_logicDevice, _swapchain, NULL);
	vkDestroySurfaceKHR(_instance, _surface, nullptr);
	vkDestroyDevice(_logicDevice, nullptr);
	vkDestroyInstance(_instance, nullptr);
}

void Topl_Renderer_Vulkan::clearView(){
	// Implement clearing operation
}

void Topl_Renderer_Vulkan::setViewport(const Topl_Viewport* viewport) {
	VkViewport vp = {};
	vp.x = 0;
	vp.y = 0;
	vp.width = _surfaceCaps.currentExtent.width; // TODO: Adjust to viewport argument
	vp.height = _surfaceCaps.currentExtent.height; // TODO: Adjust to viewport argument
	vp.minDepth = 0.0f;
	vp.maxDepth = 1.0f;

	VkRect2D scissorRect = {};
	scissorRect.offset = { 0, 0 };
	scissorRect.extent.width = _surfaceCaps.currentExtent.width; // TODO: Adjust to viewport argument
	scissorRect.extent.height = _surfaceCaps.currentExtent.height; // TODO: Adjust to viewport argument

	VkDynamicState dynamicStates[2] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	_dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	_dynamicStateInfo.dynamicStateCount = 2;
	_dynamicStateInfo.pDynamicStates = dynamicStates;

	_viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	_viewportStateInfo.viewportCount = 1; // should be adjustable?
	_viewportStateInfo.pViewports = &vp;
	_viewportStateInfo.scissorCount = 1;
	_viewportStateInfo.pScissors = &scissorRect;
}

void Topl_Renderer_Vulkan::swapBuffers(double frameTime){ 
	// Implement swapping buffers
	_isPresented = true;
}

void Topl_Renderer_Vulkan::build(const Topl_Scene* scene) {
	_isBuilt = true;
}

#ifdef RASTERON_H

Img_Base Topl_Renderer_Vulkan::frame() {
	_frameImage = Img_Base();
	_frameImage.setColorImage(CLEAR_COLOR_CODE);
	return _frameImage;
}

void Topl_Renderer_Vulkan::attachTexAt(const Rasteron_Image* image, unsigned renderID, unsigned binding) {
	// Implement texture attaching
}

void Topl_Renderer_Vulkan::attachTex3D(const Img_Volume* volumeTex, unsigned renderID) {
	// Implement Body
}

#endif

void Topl_Renderer_Vulkan::update(const Topl_Scene* scene){
	// Implement update operations
}

void Topl_Renderer_Vulkan::setDrawMode(enum DRAW_Mode mode) {
	_drawMode = mode;

	_inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	_inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	switch(_drawMode) {
	case DRAW_Points: _inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST; break;
	case DRAW_Lines: _inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST; break;
	case DRAW_Triangles: _inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; break;
	case DRAW_Fan: _inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN; break;
	case DRAW_Strip: _inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; break;
	default: return logMessage(MESSAGE_Exclaim, "Draw type not supported yet!\n");
	}
}

/* void Topl_Renderer_Vulkan::render(const Topl_Scene* scene){
	// Implement render operation
	_isDrawn = true;
} */

void Topl_Renderer_Vulkan::renderTarget(unsigned long renderID){
	// if(renderID == SCENE_RENDERID) { }
	// else {}
}