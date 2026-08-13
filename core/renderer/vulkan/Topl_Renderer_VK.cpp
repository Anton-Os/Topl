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

    VkResult createDescriptorLayout(VkDevice* device, VkDescriptorSetLayout* layout, std::initializer_list<std::pair<uint32_t, VkDescriptorType>> bindingsDesc){
        VkDescriptorSetLayoutBinding* layoutBindings = (VkDescriptorSetLayoutBinding*)malloc(sizeof(VkDescriptorSetLayoutBinding) * bindingsDesc.size());

        unsigned b = 0;
        for(auto desc = bindingsDesc.begin(); desc != bindingsDesc.end(); desc++){
            (layoutBindings + b)->binding = desc->first;
            (layoutBindings + b)->descriptorType = desc->second;
            (layoutBindings + b)->descriptorCount = 1;
            (layoutBindings + b)->stageFlags = VK_SHADER_STAGE_ALL;
            (layoutBindings + b)->pImmutableSamplers = nullptr; // Should this be conditional based on descriptor type?
            b++;
        }

        VkDescriptorSetLayoutCreateInfo descLayoutInfo = {};
        descLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descLayoutInfo.pNext = nullptr;
        descLayoutInfo.flags = 0;
        descLayoutInfo.bindingCount = bindingsDesc.size();
        descLayoutInfo.pBindings = layoutBindings;

        result = vkCreateDescriptorSetLayout(*device, &descLayoutInfo, nullptr, layout);
        if(result != VK_SUCCESS) logMessage(MESSAGE_Exclaim, "Failed to create descriptor set layout!");

        free(layoutBindings);
        return result;
    }
}

void Topl_Renderer_VK::init(NATIVE_WINDOW window) {
    // 1. Create instance
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Topl Vulkan Renderer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Topl";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    const char* extensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#endif
    };

    VkInstanceCreateInfo instInfo{};
    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pApplicationInfo = &appInfo;
    instInfo.enabledExtensionCount = sizeof(extensions) / sizeof(extensions[0]);
    instInfo.ppEnabledExtensionNames = extensions;
    instInfo.enabledLayerCount = 0;

    if (vkCreateInstance(&instInfo, nullptr, &_instance) != VK_SUCCESS)
        throw std::runtime_error("Failed to create Vulkan instance");

    // 2. Create surface (Win32)
#ifdef _WIN32
    HWND hwnd = (HWND)window;
    HINSTANCE hinst = GetModuleHandle(nullptr);

    VkWin32SurfaceCreateInfoKHR surfInfo{};
    surfInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfInfo.hinstance = hinst;
    surfInfo.hwnd = hwnd;

    if (vkCreateWin32SurfaceKHR(_instance, &surfInfo, nullptr, &_surface) != VK_SUCCESS)
        throw std::runtime_error("Failed to create Win32 surface");
#else
#error Implement surface creation for your platform
#endif

    // 3. Pick physical device
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
    if (deviceCount == 0)
        throw std::runtime_error("No Vulkan devices found");

    _physicalDevices.resize(deviceCount);
    vkEnumeratePhysicalDevices(_instance, &deviceCount, _physicalDevices.data());

    VkPhysicalDevice phys = VK_NULL_HANDLE;
    uint32_t graphicsQueueFamily = UINT32_MAX;
    uint32_t presentQueueFamily = UINT32_MAX;

    for (auto d : _physicalDevices) {
        uint32_t qCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(d, &qCount, nullptr);
        std::vector<VkQueueFamilyProperties> qProps(qCount);
        vkGetPhysicalDeviceQueueFamilyProperties(d, &qCount, qProps.data());

        for (uint32_t i = 0; i < qCount; ++i) {
            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(d, i, _surface, &presentSupport);

            if ((qProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && presentSupport) {
                phys = d;
                graphicsQueueFamily = i;
                presentQueueFamily = i;
                break;
            }
        }
        if (phys != VK_NULL_HANDLE) break;
    }

    if (phys == VK_NULL_HANDLE)
        throw std::runtime_error("Failed to find suitable physical device");

    // 4. Create logical device and queues
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo qInfo{};
    qInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    qInfo.queueFamilyIndex = graphicsQueueFamily;
    qInfo.queueCount = 1;
    qInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures features{};
    VkDeviceCreateInfo devInfo{};
    devInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    devInfo.queueCreateInfoCount = 1;
    devInfo.pQueueCreateInfos = &qInfo;
    devInfo.pEnabledFeatures = &features;

    const char* devExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    devInfo.enabledExtensionCount = 1;
    devInfo.ppEnabledExtensionNames = devExtensions;

    if (vkCreateDevice(phys, &devInfo, nullptr, &_logicDevice) != VK_SUCCESS)
        throw std::runtime_error("Failed to create logical device");

    vkGetDeviceQueue(_logicDevice, graphicsQueueFamily, 0, &_graphicsQueue);
    VkQueue presentQueue;
    vkGetDeviceQueue(_logicDevice, presentQueueFamily, 0, &presentQueue);

    // 5. Swapchain
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phys, _surface, &_surfaceCaps);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(phys, _surface, &formatCount, nullptr);
    _surfaceFormats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(phys, _surface, &formatCount, _surfaceFormats.data());

    VkSurfaceFormatKHR chosenFormat = _surfaceFormats[0];
    for (auto& f : _surfaceFormats) {
        if (f.format == VK_FORMAT_B8G8R8A8_UNORM &&
            f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            chosenFormat = f;
            break;
        }
    }

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(phys, _surface, &presentModeCount, nullptr);
    _presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(phys, _surface, &presentModeCount, _presentModes.data());

    VkPresentModeKHR chosenPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (auto m : _presentModes) {
        if (m == VK_PRESENT_MODE_MAILBOX_KHR) {
            chosenPresentMode = m;
            break;
        }
    }

    VkExtent2D extent = _surfaceCaps.currentExtent;
    uint32_t imageCount = _surfaceCaps.minImageCount + 1;
    if (_surfaceCaps.maxImageCount > 0 && imageCount > _surfaceCaps.maxImageCount)
        imageCount = _surfaceCaps.maxImageCount;

    VkSwapchainCreateInfoKHR swapInfo{};
    swapInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapInfo.surface = _surface;
    swapInfo.minImageCount = imageCount;
    swapInfo.imageFormat = chosenFormat.format;
    swapInfo.imageColorSpace = chosenFormat.colorSpace;
    swapInfo.imageExtent = extent;
    swapInfo.imageArrayLayers = 1;
    swapInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapInfo.preTransform = _surfaceCaps.currentTransform;
    swapInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapInfo.presentMode = chosenPresentMode;
    swapInfo.clipped = VK_TRUE;
    swapInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(_logicDevice, &swapInfo, nullptr, &_swapchain) != VK_SUCCESS)
        throw std::runtime_error("Failed to create swapchain");

    vkGetSwapchainImagesKHR(_logicDevice, _swapchain, &imageCount, nullptr);
    _swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(_logicDevice, _swapchain, &imageCount, _swapchainImages.data());

    // 6. Image views
    _swapchainImageViews.resize(imageCount);
    for (uint32_t i = 0; i < imageCount; ++i) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = _swapchainImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = chosenFormat.format;
        viewInfo.components = {
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY
        };
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(_logicDevice, &viewInfo, nullptr, &_swapchainImageViews[i]) != VK_SUCCESS)
            throw std::runtime_error("Failed to create image view");
    }

    // 7. Render pass
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = chosenFormat.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo rpInfo{};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpInfo.attachmentCount = 1;
    rpInfo.pAttachments = &colorAttachment;
    rpInfo.subpassCount = 1;
    rpInfo.pSubpasses = &subpass;
    rpInfo.dependencyCount = 1;
    rpInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(_logicDevice, &rpInfo, nullptr, &_renderPass) != VK_SUCCESS)
        throw std::runtime_error("Failed to create render pass");

    // 8. Framebuffers
    _framebuffers.resize(imageCount);
    for (uint32_t i = 0; i < imageCount; ++i) {
        VkImageView attachments[] = { _swapchainImageViews[i] };

        VkFramebufferCreateInfo fbInfo{};
        fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbInfo.renderPass = _renderPass;
        fbInfo.attachmentCount = 1;
        fbInfo.pAttachments = attachments;
        fbInfo.width = extent.width;
        fbInfo.height = extent.height;
        fbInfo.layers = 1;

        if (vkCreateFramebuffer(_logicDevice, &fbInfo, nullptr, &_framebuffers[i]) != VK_SUCCESS)
            throw std::runtime_error("Failed to create framebuffer");
    }

    // 9. Command pool & buffers
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = graphicsQueueFamily;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(_logicDevice, &poolInfo, nullptr, &_commandPool) != VK_SUCCESS)
        throw std::runtime_error("Failed to create command pool");

    _commandBuffers.resize(imageCount);
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = _commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = imageCount;

    if (vkAllocateCommandBuffers(_logicDevice, &allocInfo, _commandBuffers.data()) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate command buffers");

    _commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    _commandBufferInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    // 10. Sync objects
    VkSemaphoreCreateInfo semInfo{};
    semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(_logicDevice, &semInfo, nullptr, &_imageReadySemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(_logicDevice, &semInfo, nullptr, &_renderFinishSemaphore) != VK_SUCCESS)
        throw std::runtime_error("Failed to create semaphores");

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateFence(_logicDevice, &fenceInfo, nullptr, &_inFlightFence) != VK_SUCCESS)
        throw std::runtime_error("Failed to create fence");

    // 11. Default viewport & scissor
    _viewport.x = 0.0f;
    _viewport.y = 0.0f;
    _viewport.width = static_cast<float>(extent.width);
    _viewport.height = static_cast<float>(extent.height);
    _viewport.minDepth = 0.0f;
    _viewport.maxDepth = 1.0f;

    _scissorRect.offset = { 0, 0 };
    _scissorRect.extent = extent;
}

Topl_Renderer_VK::~Topl_Renderer_VK() {
    vkDeviceWaitIdle(_logicDevice);

    vkDestroyFence(_logicDevice, _inFlightFence, nullptr);
    vkDestroySemaphore(_logicDevice, _imageReadySemaphore, nullptr);
    vkDestroySemaphore(_logicDevice, _renderFinishSemaphore, nullptr);

    for (auto fb : _framebuffers)
        vkDestroyFramebuffer(_logicDevice, fb, nullptr);

    vkDestroyRenderPass(_logicDevice, _renderPass, nullptr);

    for (auto view : _swapchainImageViews)
        vkDestroyImageView(_logicDevice, view, nullptr);

    vkDestroySwapchainKHR(_logicDevice, _swapchain, nullptr);
    vkDestroyCommandPool(_logicDevice, _commandPool, nullptr);

    vkDestroyDevice(_logicDevice, nullptr);
    vkDestroySurfaceKHR(_instance, _surface, nullptr);
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

void Topl_Renderer_VK::swapBuffers(double /*frameTime*/) {
    vkWaitForFences(_logicDevice, 1, &_inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(_logicDevice, 1, &_inFlightFence);

    VkResult res = vkAcquireNextImageKHR(
        _logicDevice,
        _swapchain,
        UINT64_MAX,
        _imageReadySemaphore,
        VK_NULL_HANDLE,
        &_swapImgIdx
    );
    if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("Failed to acquire swapchain image");

    VkCommandBuffer cmd = _commandBuffers[_swapImgIdx];
    vkResetCommandBuffer(cmd, 0);
    vkBeginCommandBuffer(cmd, &_commandBufferInfo);

    VkClearValue clearColor{};
    clearColor.color = { _clearColors[0], _clearColors[1], _clearColors[2], _clearColors[3] };

    VkRenderPassBeginInfo rpBegin{};
    rpBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBegin.renderPass = _renderPass;
    rpBegin.framebuffer = _framebuffers[_swapImgIdx];
    rpBegin.renderArea.offset = { 0, 0 };
    rpBegin.renderArea.extent = _surfaceCaps.currentExtent;
    rpBegin.clearValueCount = 1;
    rpBegin.pClearValues = &clearColor;

    vkCmdBeginRenderPass(cmd, &rpBegin, VK_SUBPASS_CONTENTS_INLINE);

    // No pipeline / draw calls yet: just clear
    vkCmdEndRenderPass(cmd);

    if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
        throw std::runtime_error("Failed to record command buffer");

    VkSemaphore waitSemaphores[] = { _imageReadySemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore signalSemaphores[] = { _renderFinishSemaphore };

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _inFlightFence) != VK_SUCCESS)
        throw std::runtime_error("Failed to submit draw command buffer");

    VkSwapchainKHR swapchains[] = { _swapchain };
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &_swapImgIdx;

    VkResult presRes = vkQueuePresentKHR(_graphicsQueue, &presentInfo);
    if (presRes != VK_SUCCESS && presRes != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("Failed to present swapchain image");
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
    if(actor == SCENE_RENDERID) logMessage("Handle scene data!");
    else {
        static VkDeviceSize offsets[] = { 0 };
        unsigned long renderID = _renderTargetMap[actor];

        std::cout << "Resetting command buffer"  << std::endl;
        vkResetCommandBuffer(_commandBuffers[0], 0);
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

            std::cout << "Frame IDs is " << std::to_string(_frameIDs) << "image index is " << std::to_string(_swapImgIdx) << std::endl;
            vkCmdDraw(_commandBuffers[0], actor->getMesh()->getVertexCount(), 1, 0, 0);
        }

        vkCmdEndRenderPass(_commandBuffers[0]);

        std::cout << "Finishing command buffer"  << std::endl;
        if(vkEndCommandBuffer(_commandBuffers[0]) != VK_SUCCESS) logMessage(MESSAGE_Exclaim, "Command buffer ending failure!\n");
    }
}

#ifdef TOPL_ENABLE_TEXTURES

Sampler_2D Topl_Renderer_VK::frame() {
	_frameImage = Sampler_2D();
	// _frameImage.setColorImage(CLEAR_COLOR_CODE);
	return _frameImage;
}

void Topl_Renderer_VK::attachTexAt(const Sampler_2D* image, unsigned renderID, unsigned binding) {
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

void Topl_Renderer_VK::attachTex3D(const Sampler_3D* volumeTex, unsigned renderID) {
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