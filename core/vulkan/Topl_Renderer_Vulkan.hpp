// Vulkan Specific Inclusions

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.h>

// Renderer Implementation

#include "Topl_Renderer.hpp"

struct Buffer_Vulkan : public Buffer { Buffer_Vulkan() : Buffer(){} };

struct Texture_Vulkan : public Texture { Texture_Vulkan() : Texture() {} };

struct Topl_Pipeline_Vulkan : public Topl_Pipeline {
	Topl_Pipeline_Vulkan() : Topl_Pipeline(){}
};

#define MAX_VULKAN_NAMES 50

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
	// void attachTexture(const Rasteron_Image* image, unsigned id) override;
	void attachTextureUnit(const Rasteron_Image* image, unsigned renderID, unsigned binding) override;
	void attachVolume(const Img_Volume* material, unsigned id) override;
#endif

	Topl_Pipeline_Vulkan* _pipeline;

	VkInstance _instance = VkInstance();
	std::vector<VkExtensionProperties> _vulkanExtensions;
	std::vector<VkLayerProperties> _vulkanLayers;
};
