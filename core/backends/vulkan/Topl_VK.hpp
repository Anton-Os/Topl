#include "Topl_Renderer.hpp"

namespace VK {
    // Buffer

    struct Buffer : public Buffer { 
        Buffer() : Buffer(){} 

        VkBuffer* buffer = {};
    };

    // Texture

    struct Texture : public Texture { 
        Texture() : Texture() {}

        VkSampler* sampler = {};
    };

    // Functions

    VKAPI_ATTR VkBool32 VKAPI_CALL debugReportLogCallback(
		VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
		uint64_t object, size_t location, int32_t messageCode,
		const char* pLayerPrefix, const char* pMessage, void* pUserData
	);

    VkResult getExtensionProperties(std::vector<VkExtensionProperties>* extensions);
    VkResult createDebugReport(VkInstance* instance);

    void createShaderModule(VkDevice* device, VkShaderModule* shaderModule, std::string& shaderSrc);
    void createShaderInfo(VkPipelineShaderStageCreateInfo* shaderInfo, VkShaderModule* shaderModule, VkShaderStageFlagBits stage);
    VkResult createPipelineLayout(VkDevice* device, VkPipelineLayout* pipelineLayout, VkPipelineVertexInputStateCreateInfo* vertexInputs, entry_shader_cptr entryShader)

    VkResult createBuff(VkPhysicalDevice* physDevice, VkDevice* device, VkBuffer* buffer, VkDeviceMemory* memory, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags props){;
}
