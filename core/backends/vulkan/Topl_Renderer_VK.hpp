// VK Specific Inclusions
#undef ENABLE_DEBUG_LAYERS

#define VK_PROTOTYPES
#ifdef _WIN32
	#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(__linux__)
        #define VK_USE_PLATFORM_XLIB_KHR
#endif 

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.h>

#include "Topl_Renderer.hpp"

// Buffer

struct Buffer_VK : public Buffer { 
	Buffer_VK() : Buffer(){} 

	VkBuffer* buffer = {};
};

// Texture

struct Texture_VK : public Texture { 
	Texture_VK() : Texture() {}

	VkSampler* sampler = {};
};

// Pipeline

struct Topl_Pipeline_VK : public Topl_Pipeline {
	Topl_Pipeline_VK() : Topl_Pipeline(){}
	~Topl_Pipeline_VK(){ }

	VkPipelineShaderStageCreateInfo vertexSInfo, pixelSInfo, geomSInfo, tessCtrlSInfo, tessEvalSInfo;

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	VkPipeline pipeline = {};
};

// Renderer

#define ENABLE_VULKAN_DEPTH true

class Topl_Renderer_VK : public Topl_Renderer {
public:
    Topl_Renderer_VK(NATIVE_PLATFORM_CONTEXT* context) : Topl_Renderer(context){
        init(context->window);

		setViewport(&_defaultViewport); // viewport creation
		setDrawMode(DRAW_Triangles);
	}
	~Topl_Renderer_VK();

	void draw(const Geo_Actor* actor) override;
	void clear() override;
	void setViewport(const Topl_Viewport* viewport) override;
	void swapBuffers(double frameTime) override;
	void setDrawMode(enum DRAW_Mode mode) override;

	void setPipeline(Topl_Pipeline_VK* pipeline);
	void genPipeline(Topl_Pipeline_VK* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader);
	void genPipeline(Topl_Pipeline_VK* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader, shader_cptr geomShader);
#ifdef RASTERON_H
	Img_Base frame() override;
#endif
protected:
  	void init(NATIVE_WINDOW window) override;
	void update(const Geo_Actor* actor) override;
	void build(const Geo_Actor* actor) override;
#ifdef RASTERON_H
	void attachTexAt(const Rasteron_Image* image, unsigned renderID, unsigned binding) override;
	void attachTex3D(const Img_Volume* volumeTex, unsigned id) override;
#endif

	Topl_Pipeline_VK* _pipeline;

	std::map<unsigned long, Buffer_VK> _vertexBufferMap, _indexBufferMap, _blockBufferMap;
	std::map<unsigned long, Texture_VK[8]> _textureMap;
private:
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
	std::vector<VkFramebuffer> _framebuffers = {};
	VkAttachmentReference colorAttachmentRef = {};
	VkRenderPass _renderPass;
	VkPipelineLayout _pipelineLayout;
	VkCommandPool _commandPool;
	std::vector<VkCommandBuffer> _commandBuffers = {};
	VkViewport _viewport;
	VkPipelineViewportStateCreateInfo _viewportStateInfo = {};
	VkRect2D _scissorRect = {};
	VkDynamicState _dynamicStates[3] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_LINE_WIDTH };
	VkPipelineDynamicStateCreateInfo _dynamicStateInfo = {};
	VkPipelineInputAssemblyStateCreateInfo _inputAssemblyInfo = {};
	VkPipelineRasterizationStateCreateInfo _rasterStateInfo = {};
	VkPipelineMultisampleStateCreateInfo _multisampleInfo = {};
	VkPipelineDepthStencilStateCreateInfo _depthStencilInfo = {};
	VkPipelineColorBlendAttachmentState _colorBlendAttachment = {};
	VkPipelineColorBlendStateCreateInfo _colorBlendInfo = {};
	VkPipelineLayoutCreateInfo _pipelineLayoutInfo = {};
};
