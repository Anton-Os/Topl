#include "Topl_Renderer_VK.hpp"

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

	VkResult getExtensionProperties(std::vector<VkExtensionProperties>* extensions){
		unsigned extensionCount;
		result = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
		if(result != VK_SUCCESS) return result;

		extensions->resize(extensionCount);
		result = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensions->data());
		
		return result;
	}

	VkResult createDebugReport(VkInstance* instance) {
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

	/* VkResult createInstance(VkInstance* instance, VkInstanceCreateInfo* createInfo){ }
	VkResult createSurface(VkInstance* instance, VkSurfaceKHR* surface){} // TODO: Win32 and Linux Version Required
	VkResult queryPhysicalDevices(VkInstance* instance, VkSurfaceKHR* surface, std::vector<VkPhysicalDevice>& physDevices, std::vector<VkQueueFamilyProperties>& queueProps){ }
	VkResult createLogicDevice(VkDevice* device, std::vector<VkPhysicalDevice>& physDevices){} */

	VkResult createBuff(VkPhysicalDevice* physDevice, VkDevice* device, VkBuffer* buffer, VkDeviceMemory* buffMemory, VkDeviceSize size, VkBufferUsageFlags usage){
		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		// bufferCreateInfo.pNext = nullptr;
		// bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = usage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		// bufferCreateInfo.queueFamilyIndexCount = 0;
		// bufferCreateInfo.pQueueFamilyIndices = nullptr;

		result = vkCreateBuffer(*device, &bufferCreateInfo, nullptr, buffer);
		if(result != VK_SUCCESS) logMessage(MESSAGE_Exclaim, "Buffer creation failure!");

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(*device, *buffer, &memRequirements);
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(*physDevice, &memProperties);

        uint32_t memTypeIdx = 0;
        VkMemoryPropertyFlags memFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        for(unsigned i = 0; i < memProperties.memoryTypeCount; i++) 
            if((memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & memFlags) == memFlags){ 
                memTypeIdx = i;
                logMessage("Matching memory type index is " + std::to_string(memTypeIdx) + '\n');
                break;
            }

        VkMemoryAllocateInfo memAllocInfo = {};
        memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memAllocInfo.allocationSize = memRequirements.size;
        memAllocInfo.memoryTypeIndex = memTypeIdx;

        result = vkAllocateMemory(*device, &memAllocInfo, nullptr, buffMemory);
        if(result != VK_SUCCESS) logMessage(MESSAGE_Exclaim, "Memory allocation failure!");
        else vkBindBufferMemory(*device, *buffer, *buffMemory, 0);

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

    const unsigned instanceExtCount = 2;
    const char* instanceExtensions[instanceExtCount] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME, // Windows only
#else
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
    // instanceInfo.enabledLayerCount = instanceLayerCount;
    // instanceInfo.ppEnabledLayerNames = instanceLayers;
#else
    instanceInfo.enabledLayerCount = 0;
    instanceInfo.ppEnabledLayerNames = nullptr;
#endif

    result = vkCreateInstance(&instanceInfo, NULL, &_instance);
    if(result == VK_SUCCESS) logMessage("Instance creation success!\n");
    else return logMessage(MESSAGE_Exclaim, "Instance creation failure!\n");

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

#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.pNext = NULL;
    surfaceCreateInfo.hwnd = _platformCtx->window;
    surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);

    result = vkCreateWin32SurfaceKHR(_instance, &surfaceCreateInfo, NULL, &_surface);
#elif defined(__linux__)
    logMessage("Defining data for surface\n");
    VkXlibSurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.pNext = NULL;
    surfaceCreateInfo.flags = 0;
    surfaceCreateInfo.dpy = _platformCtx->display;
    surfaceCreateInfo.window = _platformCtx->window;

    // printf("Display is %d and windows is %d", _platformCtx->display, _platformCtx->window);

    logMessage("About to create surface\n");
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

    VkSubpassDependency subpassDependency = {};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &colorAttachment;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDesc;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &subpassDependency;

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
    if(result == VK_SUCCESS) logMessage("Command buffers creation success! \n");
    else return logMessage(MESSAGE_Exclaim, "Command buffers creation failure! \n");

    // VkCommandBufferBeginInfo commandBufferInfo = {};
    _commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    _commandBufferInfo.pNext = nullptr;
    _commandBufferInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    _commandBufferInfo.pInheritanceInfo = nullptr;

    // Synchronization Object Creation

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    result = vkCreateSemaphore(_logicDevice, &semaphoreInfo, nullptr, &_imageReadySemaphore);
    result = vkCreateSemaphore(_logicDevice, &semaphoreInfo, nullptr, &_renderFinishSemaphore);

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    result = vkCreateFence(_logicDevice, &fenceInfo, nullptr, &_inFlightFence);
    if(result == VK_SUCCESS) logMessage("Sync object creation success \n");
    else return logMessage(MESSAGE_Exclaim, "Sync object creation failure! \n");
}

Topl_Renderer_VK::~Topl_Renderer_VK() {
	vkDestroySemaphore(_logicDevice, _imageReadySemaphore, nullptr);
	vkDestroySemaphore(_logicDevice, _renderFinishSemaphore, nullptr);
	vkDestroyFence(_logicDevice, _inFlightFence, nullptr);
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
    vkResetCommandBuffer(_commandBuffers[0], 0);
    vkBeginCommandBuffer(_commandBuffers[0], &_commandBufferInfo);

    VkClearColorValue clearColor = { _clearColors[0], _clearColors[1], _clearColors[2], _clearColors[3] };
    VkClearValue clearValue = {};
    clearValue.color = clearColor;
    clearValue.depthStencil = { 1.0f, 0 };
    // VkRect2D screenRect = {{0, 0}, { TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT }};

    VkImageSubresourceRange imgRange = {};
    imgRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imgRange.levelCount = 1;
    imgRange.layerCount = 1;

    for(unsigned i = 0; i < _swapchainImages.size(); i++)
        vkCmdClearColorImage(_commandBuffers[0], _swapchainImages[i], VK_IMAGE_LAYOUT_GENERAL, &clearColor, 1, &imgRange);

    if(vkEndCommandBuffer(_commandBuffers[0]) != VK_SUCCESS) logMessage(MESSAGE_Exclaim, "Command buffer ending failure!\n");
}

void Topl_Renderer_VK::setViewport(const Topl_Viewport* viewport) {
	if(vkBeginCommandBuffer(_commandBuffers[0], &_commandBufferInfo) != VK_SUCCESS) logMessage(MESSAGE_Exclaim, "Command buffer begin failure!\n");

	_viewport.x = 0;
	_viewport.y = 0;
	_viewport.width = _surfaceCaps.currentExtent.width; // TODO: Adjust to viewport argument
	_viewport.height = _surfaceCaps.currentExtent.height; // TODO: Adjust to viewport argument
	_viewport.minDepth = 0.0f;
	_viewport.maxDepth = 1.0f;
	vkCmdSetViewport(_commandBuffers[0], 0, 1, &_viewport);

	_scissorRect.offset = { 0, 0 };
	_scissorRect.extent.width = _surfaceCaps.currentExtent.width; // TODO: Adjust to viewport argument
	_scissorRect.extent.height = _surfaceCaps.currentExtent.height; // TODO: Adjust to viewport argument
	vkCmdSetScissor(_commandBuffers[0], 0, 1, &_scissorRect);

	_dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	_dynamicStateInfo.dynamicStateCount = 3;
	_dynamicStateInfo.pDynamicStates = &_dynamicStates[0];

	_viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	_viewportStateInfo.viewportCount = 1; // should be adjustable?
	_viewportStateInfo.pViewports = &_viewport;
	_viewportStateInfo.scissorCount = 1;
	_viewportStateInfo.pScissors = &_scissorRect;

	if(vkEndCommandBuffer(_commandBuffers[0]) != VK_SUCCESS) logMessage(MESSAGE_Exclaim, "Command buffer ending failure!\n");
}

void Topl_Renderer_VK::swapBuffers(double frameTime){ 
    VkSemaphore waitSemaphores[] = { _imageReadySemaphore };
	VkSemaphore signalSemaphores[] = { _renderFinishSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT }; // { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = _commandBuffers.data();

	if(vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _inFlightFence) != VK_SUCCESS) logMessage(MESSAGE_Exclaim, "Queue submission failure!\n");

	// Presentation

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapchains[] = { _swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &_swapImgIdx;
	presentInfo.pResults = nullptr;

	if(vkQueuePresentKHR(_graphicsQueue, &presentInfo) != VK_SUCCESS) logMessage(MESSAGE_Exclaim, "Queue presentation failure!\n");

    vkWaitForFences(_logicDevice, 1, &_inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(_logicDevice, 1, &_inFlightFence);
	_flags[DRAWN_BIT] = true;
}

void Topl_Renderer_VK::build(const Geo_Actor* actor){
    if(actor == SCENE_RENDERID) logMessage("Handle scene data!");
    else {
        unsigned long renderID = getRenderID(actor);
		Geo_Mesh* mesh = (Geo_Mesh*)actor->getMesh();

        VkBuffer vertexBuff;
        VkDeviceMemory vertexBuffMemory;
        _flags[BUILD_BIT] = VK::createBuff(&_physicalDevices[0], &_logicDevice, &vertexBuff, &vertexBuffMemory, mesh->getVertexCount() * sizeof(Geo_Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

        void* data;
        vkMapMemory(_logicDevice, vertexBuffMemory, 0, mesh->getVertexCount() * sizeof(Geo_Vertex), 0, &data);
        memcpy(data, mesh->getVertices(), mesh->getVertexCount() * sizeof(Geo_Vertex));
        vkUnmapMemory(_logicDevice, vertexBuffMemory);

        _vertexBufferMap.insert({ renderID, VK::Buffer(renderID, BUFF_Vertex_Type, vertexBuff, vertexBuffMemory) });
    }
}

void Topl_Renderer_VK::update(const Geo_Actor* actor){
	// TODO: Implement this
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

void Topl_Renderer_VK::draw(const Geo_Actor* actor){
    if(vkAcquireNextImageKHR(_logicDevice, _swapchain, UINT64_MAX, _imageReadySemaphore, VK_NULL_HANDLE, &_swapImgIdx) != VK_SUCCESS) // Should this be here
        logMessage(MESSAGE_Exclaim, "Aquire next image failure!\n");

    if(actor == SCENE_RENDERID) logMessage("Handle scene data!");
    else {
        static VkDeviceSize offsets[] = { 0 };
        unsigned long renderID = _renderTargetMap[actor];

        // vkResetCommandBuffer(_commandBuffers[0], 0);

        if(vkBeginCommandBuffer(_commandBuffers[0], &_commandBufferInfo) != VK_SUCCESS) logMessage(MESSAGE_Exclaim, "Command buffer begin failure!\n");

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.framebuffer = _framebuffers[_swapImgIdx];
        renderPassInfo.renderPass = _renderPass;
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = _surfaceCaps.currentExtent;
        VkClearValue clearColor = {{{ _clearColors[0], _clearColors[1], _clearColors[2], _clearColors[3] }}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(_commandBuffers[0], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        if(renderID == SCENE_RENDERID) logMessage("Handle scene data!");
        else if(actor->isShown && actor->getMesh() != nullptr) {
            if(_vertexBufferMap.find(renderID) != _vertexBufferMap.end())
                vkCmdBindVertexBuffers(_commandBuffers[0], 0, 1, &_vertexBufferMap.at(renderID).buffer, offsets);

            // std::cout << "Number of vertices: " << std::to_string(actor->getMesh()->getVertexCount()) << std::endl;
            std::cout << "Frame IDs is " << std::to_string(_frameIDs) << "image index is " << std::to_string(_swapImgIdx) << std::endl;
            if(_frameIDs > 10) vkCmdDraw(_commandBuffers[0], actor->getMesh()->getVertexCount(), 1, 0, 0);
        }

        vkCmdEndRenderPass(_commandBuffers[0]);

        if(vkEndCommandBuffer(_commandBuffers[0]) != VK_SUCCESS) logMessage(MESSAGE_Exclaim, "Command buffer ending failure!\n");
    }
}

#ifdef RASTERON_H

Img_Base Topl_Renderer_VK::frame() {
	_frameImage = Img_Base();
	// _frameImage.setColorImage(CLEAR_COLOR_CODE);
	return _frameImage;
}

void Topl_Renderer_VK::attachTexAt(const Img_Base* image, unsigned renderID, unsigned binding) {
	// Create Data
    
    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = image->getImage()->height;
    imageCreateInfo.extent.height = image->getImage()->width;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.flags = 0;

    // Create Image
}

void Topl_Renderer_VK::attachTex3D(const Img_Volume* volumeTex, unsigned renderID) {
	// Create Data
    
    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_3D;
    imageCreateInfo.extent.width = volumeTex->getWidth();
    imageCreateInfo.extent.height = volumeTex->getHeight();
    imageCreateInfo.extent.depth = volumeTex->getDepth();
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.flags = 0;

    // Create Image
}

#endif
