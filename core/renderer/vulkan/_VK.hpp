// VK Specific Inclusions
#ifdef _WIN32
#define ENABLE_DEBUG_LAYERS
#endif

#define ENABLE_VULKAN_DEPTH true

#define VK_PROTOTYPES
#ifdef _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#elif defined(__ANDROID__)
    #define VK_USE_PLATFORM_ANDROID_KHR
#elif defined(__linux__)
	#define VK_USE_PLATFORM_XLIB_KHR
#endif

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.h>

#include "renderer/Topl_Renderer.hpp"

namespace VK {
    // Buffer

    struct Buffer : public Topl_Buffer { 
        Buffer() : Topl_Buffer(){} 
        Buffer(unsigned id, enum BUFF_Type t, VkBuffer b, VkDeviceMemory m) : Topl_Buffer(id, t){ 
            buffer = b; 
            memory = m;
        }

        VkBuffer buffer = {};
        VkDeviceMemory memory = {};
    };

    // Texture

    struct Texture : public Topl_Texture { 
        Texture() : Topl_Texture() {}

        VkSampler* sampler = {};
        VkDeviceMemory memory = {};
    };

    // Pipeline

    struct Pipeline : public Topl_Pipeline {
        Pipeline() : Topl_Pipeline(){}
        ~Pipeline(){ }

        VkPipelineShaderStageCreateInfo vertexSInfo, pixelSInfo, geomSInfo, tessCtrlSInfo, tessEvalSInfo, computeSInfo;

        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        VkPipeline pipeline = {};
    };

    // Functions

    VKAPI_ATTR VkBool32 VKAPI_CALL debugReportLogCallback(
		VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
		uint64_t object, size_t location, int32_t messageCode,
		const char* pLayerPrefix, const char* pMessage, void* pUserData
	);

    VkResult getExtensionProperties(std::vector<VkExtensionProperties>* extensions);
    VkResult createDebugReport(VkInstance* instance);

    VkFormat getShaderFormat(enum SHDR_ValueType type);
    VkResult createShaderModule(VkDevice* device, VkShaderModule* shaderModule, std::string& shaderSrc);
    void createShaderInfo(VkPipelineShaderStageCreateInfo* shaderInfo, VkShaderModule* shaderModule, VkShaderStageFlagBits stage);
    VkResult createPipelineLayout(VkDevice* device, VkPipelineLayout* pipelineLayout, VkPipelineVertexInputStateCreateInfo* vertexInputs, entry_shader_cptr entryShader);

    VkResult createBuff(VkPhysicalDevice* physDevice, VkDevice* device, VkBuffer* buffer, VkDeviceMemory* memory, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags props);
}
