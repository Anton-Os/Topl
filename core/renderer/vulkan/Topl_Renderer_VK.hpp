#include "Topl_VK.hpp"

// Renderer

#define ENABLE_VULKAN_DEPTH true

class Topl_Renderer_VK : public Topl_Renderer {
public:
	Topl_Renderer_VK(NATIVE_PLATFORM_CONTEXT* context) : Topl_Renderer(context){
		init(context->window);

		setViewport(&_activeViewport); // viewport creation
		setDrawMode(DRAW_Triangles);
	}
	~Topl_Renderer_VK();

	void draw(const Geo_Actor* actor) override;
	void clear() override;
	void setViewport(const Topl_Viewport* viewport) override;
	void swapBuffers(double frameTime) override;
	void setDrawMode(enum DRAW_Mode mode) override;

	void setPipeline(VK::Pipeline* pipeline);
	void genPipeline(VK::Pipeline* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader);
	void genPipeline(VK::Pipeline* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader, shader_cptr geomShader);
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

	VK::Pipeline* _pipeline;

	std::map<unsigned long, VK::Buffer> _vertexBufferMap, _indexBufferMap, _blockBufferMap;
	std::map<unsigned long, VK::Texture[8]> _textureMap;
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
	VkCommandBufferBeginInfo _commandBufferInfo = {};
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
	VkSemaphore _imageReadySemaphore = {};
	VkSemaphore _renderFinishSemaphore = {};
	VkFence _inFlightFence;
};
