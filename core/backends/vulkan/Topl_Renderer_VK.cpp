#include "Topl_Renderer_VK.hpp"

#define ENABLE_DEBUG_LAYERS

namespace VK {
	VkResult result;

	VKAPI_ATTR VkBool32 VKAPI_CALL debugReportLogCallback(
		VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
		uint64_t object, size_t location, int32_t messageCode,
		const char* pLayerPrefix, const char* pMessage, void* pUserData
	) {
		logMessage(MESSAGE_Exclaim, std::string(pMessage));
		return VK_FALSE;
	}

	static VkResult getExtensionProperties(std::vector<VkExtensionProperties>* extensions){
		unsigned extensionCount;
		result = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
		if(result != VK_SUCCESS) return result;

		extensions->resize(extensionCount);
		result = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensions->data());
		
		return result;
	}

	static VkResult createDebugReport(VkInstance* instance){
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

	static VkResult createInstance(VkInstance* instance, VkInstanceCreateInfo* createInfo){ }
	static VkResult createSurface(VkInstance* instance, VkSurfaceKHR* surface){} // TODO: Win32 and Linux Version Required
	static VkResult queryPhysicalDevices(VkInstance* instance, VkSurfaceKHR* surface, std::vector<VkPhysicalDevice>& physDevices, std::vector<VkQueueFamilyProperties>& queueProps){ }
	static VkResult createLogicDevice(VkDevice* device, std::vector<VkPhysicalDevice>& physDevices){}

	static VkResult createBuff(VkPhysicalDevice* physDevice, VkDevice* device, VkBuffer* buffer, VkDeviceMemory* memory, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags props){
		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = nullptr;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = usage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0;
		bufferCreateInfo.pQueueFamilyIndices = nullptr;

		result = vkCreateBuffer(*device, &bufferCreateInfo, nullptr, buffer);
		if(result != VK_SUCCESS) logMessage("Buffer creation failure!");

		return result;
	}
}

void Topl_Renderer_VK::init(NATIVE_WINDOW window) {
	VkResult result; // error checking variable

	// Instance Creation

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Topl";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Topl Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	result = VK::getExtensionProperties(&_vulkanExtensions);
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

#ifdef ENABLE_DEBUG_LAYERS
	const unsigned instanceLayerCount = 1;
	const char* instanceLayers[instanceLayerCount] = { "VK_LAYER_KHRONOS_validation" };
#endif

/* #ifdef ENABLE_DEBUG_LAYERS
	const char* validationLayers[] = { "VK_LAYER_KHRONOS_validation" };

	unsigned instanceLayerCount;
	vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);

	printf("Validation layer count is: %d\n", instanceLayerCount);
	VkLayerProperties* instanceLayers = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * instanceLayerCount);
	for(unsigned l = 0; l < instanceLayerCount; l++) 
		printf("Layer name is: %s", (*(instanceLayers + l)).layerName);
	vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayers);
#endif */

	VkInstanceCreateInfo instanceInfo = {};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledExtensionCount = instanceExtCount;
	instanceInfo.ppEnabledExtensionNames = instanceExtensions;
#ifdef ENABLE_DEBUG_LAYERS
	instanceInfo.enabledLayerCount = instanceLayerCount;
	instanceInfo.ppEnabledLayerNames = instanceLayers;
	// free(instanceLayers);
#endif

	result = vkCreateInstance(&instanceInfo, NULL, &_instance);
	if(result == VK_SUCCESS) logMessage("Instance creation success!\n");
	else return logMessage(MESSAGE_Exclaim, "Instance creation failure!\n");

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

/* #ifdef ENABLE_DEBUG_LAYERS
	result = VK::createDebugReport(&_instance);
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

	// Queue Family Enumeration

	unsigned queueFamilyCount = 0;

	vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevices[0], &queueFamilyCount, NULL);
	logMessage("Physical device queue family count: " + std::to_string(queueFamilyCount) + '\n');

	_queueFamilyProps.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevices[0], &queueFamilyCount, _queueFamilyProps.data());
	if(result == VK_SUCCESS) logMessage("Queue family enumeration success! Family count: " + std::to_string(queueFamilyCount) + "\n");
	else return logMessage(MESSAGE_Exclaim, "Queue family enumeration failure!\n");


	// Queue Family Selection

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

	// Logical Device Creation

	VkDeviceQueueCreateInfo queueInfo = {};
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.pNext = NULL;
	queueInfo.queueFamilyIndex = gSupportIdx;
	queueInfo.queueCount = 1;
	queueInfo.pQueuePriorities = queuePriorities; // &queuePriority;

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
	swapchainCreateInfo.imageExtent.width = _surfaceCaps.maxImageExtent.width; // TODO: Get the actual width extent
	swapchainCreateInfo.imageExtent.height = _surfaceCaps.maxImageExtent.height; // TODO: Get the actual height extent
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

	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &colorAttachment;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpassDesc;

	result = vkCreateRenderPass(_logicDevice, &renderPassCreateInfo, nullptr, &_renderPass);
	if(result == VK_SUCCESS) logMessage("RenderPass creation success \n");
	else return logMessage(MESSAGE_Exclaim, "RenderPass creation failure! \n");

	// Framebuffer Creation

	_framebuffers.resize(_swapchainImageViews.size());

	for(unsigned short s = 0; s < _swapchainImageViews.size(); s++){
		VkImageView attachments[] = { _swapchainImageViews[s] };

		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = _renderPass;
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = attachments;
		framebufferCreateInfo.width = _surfaceCaps.currentExtent.width;
		framebufferCreateInfo.height = _surfaceCaps.currentExtent.height;
		framebufferCreateInfo.layers = 1;

		result = vkCreateFramebuffer(_logicDevice, &framebufferCreateInfo, nullptr, &_framebuffers[s]);
		if(result == VK_SUCCESS) logMessage("Framebuffer creation success at no. " + std::to_string(s) + "\n");
		else return logMessage(MESSAGE_Exclaim, "Framebuffer creation failure! \n");
	}

	// Rasetrizer State

	VkPipelineRasterizationStateCreateInfo _rasterStateInfo = {};
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
	_depthStencilInfo.maxDepthBounds = 1.0F; // 0;
	_depthStencilInfo.stencilTestEnable = VK_FALSE;
	_depthStencilInfo.front = _depthStencilInfo.back;

	// Color Blending State

	_colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	_colorBlendAttachment.blendEnable = VK_TRUE;
	_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	_colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	_colorBlendInfo.logicOpEnable = VK_FALSE; // VK_TRUE;
	_colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
	_colorBlendInfo.attachmentCount = 1;
	_colorBlendInfo.pAttachments = &_colorBlendAttachment;
	_colorBlendInfo.blendConstants[0] = 0.0f;
	_colorBlendInfo.blendConstants[1] = 0.0f;
	_colorBlendInfo.blendConstants[2] = 0.0f;
	_colorBlendInfo.blendConstants[3] = 0.0f;


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

	// Command Buffer Execution

	/* VkCommandBufferBeginInfo commandBufferInfo = {};
	commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferInfo.pNext = nullptr;
	commandBufferInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	commandBufferInfo.pInheritanceInfo = nullptr;

	for(unsigned c = 0; c < _commandBuffers.size(); c++)
		result = vkBeginCommandBuffer(_commandBuffers[c], &commandBufferInfo);
	if(result == VK_SUCCESS) logMessage("Command buffer execution success \n");
	else return logMessage(MESSAGE_Exclaim, "Command buffer execution failure! \n");

	VkClearValue clearValues[2];
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
	
}

Topl_Renderer_VK::~Topl_Renderer_VK() {
	vkDestroyPipelineLayout(_logicDevice, _pipelineLayout, nullptr);
	vkDestroyRenderPass(_logicDevice, _renderPass, nullptr);
	vkFreeCommandBuffers(_logicDevice, _commandPool, _commandBuffers.size(), _commandBuffers.data());
	vkDestroyCommandPool(_logicDevice, _commandPool, nullptr);
	for(unsigned i = 0; i < _swapchainImageViews.size(); i++) vkDestroyImageView(_logicDevice, _swapchainImageViews[i], NULL);
	vkDestroySwapchainKHR(_logicDevice, _swapchain, NULL);
	vkDestroySurfaceKHR(_instance, _surface, nullptr);
	vkDestroyDevice(_logicDevice, nullptr);
	vkDestroyInstance(_instance, nullptr);
}

void Topl_Renderer_VK::clear(){
	// Implement clearing operation
}

void Topl_Renderer_VK::setViewport(const Topl_Viewport* viewport) {
	_viewport.x = 0;
	_viewport.y = 0;
	_viewport.width = _surfaceCaps.currentExtent.width; // TODO: Adjust to viewport argument
	_viewport.height = _surfaceCaps.currentExtent.height; // TODO: Adjust to viewport argument
	_viewport.minDepth = 0.0f;
	_viewport.maxDepth = 1.0f;

	_scissorRect.offset = { 0, 0 };
	_scissorRect.extent.width = _surfaceCaps.currentExtent.width; // TODO: Adjust to viewport argument
	_scissorRect.extent.height = _surfaceCaps.currentExtent.height; // TODO: Adjust to viewport argument

	_dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	_dynamicStateInfo.dynamicStateCount = 3;
	_dynamicStateInfo.pDynamicStates = &_dynamicStates[0];

	_viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	_viewportStateInfo.viewportCount = 1; // should be adjustable?
	_viewportStateInfo.pViewports = &_viewport;
	_viewportStateInfo.scissorCount = 1;
	_viewportStateInfo.pScissors = &_scissorRect;
}

void Topl_Renderer_VK::swapBuffers(double frameTime){ 
	// Implement swapping buffers
	_flags[DRAWN_BIT] = true;
}

void Topl_Renderer_VK::build(const Topl_Scene* scene) {
	// TODO: Create buffers for scene

	// TODO: Create buffers for render objects
	for(unsigned g = 0; g < scene->getActorCount(); g++){
		actor_cptr actor = scene->getGeoActor(g);
		Geo_Mesh* mesh = (Geo_Mesh*)actor->getMesh();
		unsigned long renderID = getRenderID(actor);

		if(renderID == INVALID_RENDERID){ // Actor will not be duplicated
			_renderIDs++;
			_renderObjMap.insert({ _renderIDs, scene->getGeoActor(g) });
			_renderTargetMap.insert({ scene->getGeoActor(g), _renderIDs });
			renderID = getRenderID(actor);
		} else logMessage(MESSAGE_Exclaim, "Override required!"); // TODO: Implement else to override
		
		/* VkBuffer* vertexBuff = nullptr;
		VkDeviceMemory* memory = nullptr; // TODO: Assign data from vertices
		VkDeviceSize size = 1; // TODO: AAssign size from vertices
		VkBufferUsageFlags flags = 0;
		VkMemoryPropertyFlags props = 0;
		_flags[BUILD_BIT] = VK::createBuff(&_physicalDevices[0], &_logicDevice, vertexBuff, memory, size, flags, props); */
	}

	_flags[BUILD_BIT] = true;
}

#ifdef RASTERON_H

Img_Base Topl_Renderer_VK::frame() {
	_frameImage = Img_Base();
	_frameImage.setColorImage(CLEAR_COLOR_CODE);
	return _frameImage;
}

void Topl_Renderer_VK::attachTexAt(const Rasteron_Image* image, unsigned renderID, unsigned binding) {
	// Implement texture attaching
}

void Topl_Renderer_VK::attachTex3D(const Img_Volume* volumeTex, unsigned renderID) {
	// Implement Body
}

#endif

void Topl_Renderer_VK::update(const Topl_Scene* scene){
	// Implement update operations
}

void Topl_Renderer_VK::setDrawMode(enum DRAW_Mode mode) {
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

/* void Topl_Renderer_VK::render(const Topl_Scene* scene){
	// Implement render operation
	_isDrawn = true;
} */

void Topl_Renderer_VK::draw(const Geo_Actor* actor){
	unsigned long renderID = _renderTargetMap[actor];
	// if(renderID == SCENE_RENDERID) { }
	// else {}
}