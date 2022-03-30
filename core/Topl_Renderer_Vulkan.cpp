#include "Topl_Renderer_Vulkan.hpp"

namespace Renderer {
	// include buffer creation and more
}


void Topl_Renderer_Vulkan::init(NATIVE_WINDOW window) {
	_platformCtx.window = window;
}

void Topl_Renderer_Vulkan::clearView(){
	// Implement clearing operation
}

void Topl_Renderer_Vulkan::switchFramebuff(){ 
	// Implement framebuffer switching operation
	_isSceneDrawn = false;
}

void Topl_Renderer_Vulkan::build(const Topl_Scene* scene) {
	// Implement building operation
	_isSceneReady = true;
}

#ifdef RASTERON_H

Rasteron_Image* Topl_Renderer_Vulkan::frame() {
	Rasteron_Image* image = createImgBlank(TOPL_WIN_HEIGHT, TOPL_WIN_WIDTH, 0xFF191919); // 
	return image;
}

void Topl_Renderer_Vulkan::texturize(const Topl_Scene* scene) {
	Topl_RenderContext_Vulkan* activeCtx = getRenderContext(scene);
	if (activeCtx == nullptr) { // TODO: Generalize this code
		perror("Render Context could not be found!");
		return;
	}
#ifdef RASTERON_H // Rasteron dependency required for updating textures
	// Implement texturizing operations
#endif
}

void Topl_Renderer_Vulkan::attachTexture(const Rasteron_Image* image, unsigned id){
	// TODO: find id corresponding to proper render context
	// Topl_RenderContext_Vulkan* activeCtx = getRenderContext(scene);
	Topl_RenderContext_Vulkan* activeCtx = *(_renderCtx_Vulkan); // for now gets the first available render context
	if (activeCtx == nullptr) {
		perror("Render Context could not be found!");
		return;
	}

	// Implement texture attaching
}

void Topl_Renderer_Vulkan::attachMaterial(const Topl_Material* material, unsigned id) {
	// Implement Body
}

#endif

void Topl_Renderer_Vulkan::update(const Topl_Scene* scene){
	Topl_RenderContext_Vulkan* activeCtx = getRenderContext(scene);
	if (activeCtx == nullptr) {
		perror("Render Context could not be found!");
		return;
	}

	// Implement update operations
}

void Topl_Renderer_Vulkan::drawMode(){
	switch(_drawMode) {
	case DRAW_Points: break;
	case DRAW_Lines: break;
	case DRAW_Triangles: break;
	case DRAW_Fan: break;
	case DRAW_Strip: break;
	default:
		perror("Draw type not supported yet!");
		_isSceneDrawn = false; return;
	}
}

void Topl_Renderer_Vulkan::render(const Topl_Scene* scene){
	Topl_RenderContext_Vulkan* activeCtx = getRenderContext(scene);
	if (activeCtx == nullptr) {
		perror("Render Context could not be found!");
		return;
	}

	// Implement render operation

	_isSceneDrawn = true;
}