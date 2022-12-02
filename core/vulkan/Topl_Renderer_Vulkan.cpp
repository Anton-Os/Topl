#include "Topl_Renderer_Vulkan.hpp"

namespace Vulkan {
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( 
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData
	) {
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			logMessage(MESSAGE_Exclaim, std::string(pCallbackData->pMessage) + '\n');
			return VK_FALSE;
		}
		return VK_TRUE;
	}
}

void Topl_Renderer_Vulkan::init(NATIVE_WINDOW window) {
	VkResult result; // error checking variable

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Topl";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Topl Vulkan Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	unsigned vulkanExtCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtCount, nullptr);
	_vulkanExtensions = std::vector<VkExtensionProperties>(vulkanExtCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtCount, _vulkanExtensions.data());
	const char* extensionNames[MAX_VULKAN_NAMES];
	for (unsigned e = 0; e < vulkanExtCount; e++) extensionNames[e] = _vulkanExtensions[e].extensionName;
	/*const char* extensions[2] = { // see glfw window specific code
		"VK_KHR_surface",
#ifdef _WIN32
		"VK_KHR_win32_surface"
#elif defined(__linux)
		"VK_KHR_xlib_surface"
#endif
	}; */

	unsigned vulkanLayerCount;
	vkEnumerateInstanceLayerProperties(&vulkanLayerCount, nullptr);
	_vulkanLayers = std::vector<VkLayerProperties>(vulkanLayerCount);
	vkEnumerateInstanceLayerProperties(&vulkanLayerCount, _vulkanLayers.data());
	const char* layerNames[MAX_VULKAN_NAMES] = {}; // enough to contain all extensions
	for (unsigned l = 0; l < vulkanLayerCount; l++) layerNames[l] = _vulkanLayers[l].layerName;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = vulkanExtCount;
	createInfo.ppEnabledExtensionNames = &extensionNames[0];
	createInfo.enabledLayerCount = vulkanLayerCount;
	createInfo.ppEnabledLayerNames = &layerNames[0];
#ifndef NDEBUG
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
	debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugCreateInfo.pfnUserCallback = &Vulkan::debugCallback;
	createInfo.pNext = &debugCreateInfo;
#endif

	result = vkCreateInstance(&createInfo, nullptr, &_instance);
	if (result != VK_SUCCESS) return logMessage(MESSAGE_Exclaim, "Vulkan Instance creation failed");
}

Topl_Renderer_Vulkan::~Topl_Renderer_Vulkan() {
	vkDestroyInstance(_instance, nullptr);
}

void Topl_Renderer_Vulkan::clearView(){
	// Implement clearing operation
}

void Topl_Renderer_Vulkan::setViewport(const Topl_Viewport* viewport) {
	// Implement viewport setting
}

void Topl_Renderer_Vulkan::swapBuffers(double frameTime){ 
	// Implement swapping buffers
	_isPresented = true;
}

void Topl_Renderer_Vulkan::build(const Topl_Scene* scene) {
	// Implement building operation
	_isBuilt = true;
}

#ifdef RASTERON_H

Img_Base Topl_Renderer_Vulkan::frame() {
	_frameImage = Img_Base();
	_frameImage.setColorImage(CLEAR_COLOR_CODE);
	return _frameImage;
}

void Topl_Renderer_Vulkan::attachTextureUnit(const Rasteron_Image* image, unsigned renderID, unsigned binding) {
	// Implement texture attaching
}

void Topl_Renderer_Vulkan::attachVolume(const Img_Volume* material, unsigned renderID) {
	// Implement Body
}

#endif

void Topl_Renderer_Vulkan::update(const Topl_Scene* scene){
	// Implement update operations
}

void Topl_Renderer_Vulkan::setDrawMode(enum DRAW_Mode mode) {
	_drawMode = mode;

	switch(_drawMode) {
	case DRAW_Points: break;
	case DRAW_Lines: break;
	case DRAW_Triangles: break;
	case DRAW_Fan: break;
	case DRAW_Strip: break;
	default: return logMessage(MESSAGE_Exclaim, "Draw type not supported yet!");
	}
}

/* void Topl_Renderer_Vulkan::render(const Topl_Scene* scene){
	// Implement render operation
	_isDrawn = true;
} */

void Topl_Renderer_Vulkan::renderTarget(unsigned long renderID){
	// if(renderID == SPECIAL_SCENE_RENDER_ID) { }
	// else {}
}