// Vulkan Specific Inclusions

#define VK_PROTOTYPES
#ifdef _WIN32
	#define VK_USE_PLATFORM_WIN32_KHR
#endif 

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.h>


// Renderer Implementation

#include "Topl_Renderer.hpp"

struct Buffer_Vulkan : public Buffer { Buffer_Vulkan() : Buffer(){} };

struct Texture_Vulkan : public Texture { Texture_Vulkan() : Texture() {} };

struct Topl_Pipeline_Vulkan : public Topl_Pipeline {
	Topl_Pipeline_Vulkan() : Topl_Pipeline(){}
	~Topl_Pipeline_Vulkan(){ }

	VkPipelineShaderStageCreateInfo vertexStageInfo, pixelStageInfo;

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	VkPipeline pipeline = {};
};

#define ENABLE_VULKAN_DEPTH true

class Topl_Renderer_Vulkan : public Topl_Renderer {
public:
	Topl_Renderer_Vulkan(NATIVE_WINDOW window) : Topl_Renderer(window){ 
		init(window); 
	}
	~Topl_Renderer_Vulkan();

	void clearView() override;
	void setViewport(const Topl_Viewport* viewport) override;
	void swapBuffers(double frameTime) override;
	void setDrawMode(enum DRAW_Mode mode) override;
	void build(const Topl_Scene* scene) override;

	void setPipeline(Topl_Pipeline_Vulkan* pipeline);
	void genPipeline(Topl_Pipeline_Vulkan* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader);
	void genPipeline(Topl_Pipeline_Vulkan* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader, shader_cptr geomShader);
#ifdef RASTERON_H
	Img_Base frame() override;
#endif
protected:
  	void init(NATIVE_WINDOW window) override;
	void update(const Topl_Scene* scene) override;
	void renderTarget(unsigned long renderID) override;
#ifdef RASTERON_H
	// void attachTexAt(const Rasteron_Image* image, unsigned id) override;
	void attachTexAt(const Rasteron_Image* image, unsigned renderID, unsigned binding) override;
	void attachVolume(const Img_Volume* material, unsigned id) override;
#endif
	void finalizePipeline(); // operation to finalize pipeline creation

	Topl_Pipeline_Vulkan* _pipeline;

	std::vector<VkExtensionProperties> _vulkanExtensions;
	std::vector<VkLayerProperties> _vulkanLayers;
	// unsigned _physicalDeviceIdx = 0; // TODO: Make configurable if more than one device
	std::vector<VkPhysicalDevice> _physicalDevices = {};
	std::vector<VkQueueFamilyProperties> _queueFamilyProps = {};
	VkInstance _instance;
	VkDevice _logicDevice;
	VkQueue _graphicsQueue;
	VkSurfaceKHR _surface;
	VkSurfaceCapabilitiesKHR _surfaceCaps = {};
	std::vector<VkSurfaceFormatKHR> _surfaceFormats = {};
	std::vector<VkPresentModeKHR> _presentModes = {};
	VkSwapchainKHR _swapchain;
	std::vector<VkImage> _swapchainImages = {};
	std::vector<VkImageView> _swapchainImageViews = {};
	VkRenderPass _renderpass;
	VkPipelineLayout _pipelineLayout;
	VkCommandPool _commandPool;
	std::vector<VkCommandBuffer> _commandBuffers = {};
	VkPipelineViewportStateCreateInfo _viewportStateInfo = {};
	VkPipelineDynamicStateCreateInfo _dynamicStateInfo = {};
	VkPipelineVertexInputStateCreateInfo _vertexInputInfo = {};
	VkPipelineInputAssemblyStateCreateInfo _inputAssemblyInfo = {};
	VkPipelineRasterizationStateCreateInfo _rasterStateInfo = {};
	VkPipelineMultisampleStateCreateInfo _multisampleInfo = {};
	VkPipelineDepthStencilStateCreateInfo _depthStencilInfo = {};
	VkPipelineColorBlendStateCreateInfo _colorBlendInfo = {};
	VkPipelineLayoutCreateInfo _pipelineLayoutInfo = {};
};
