#include "Topl_Renderer_Vulkan.hpp"

namespace Renderer {
	// include buffer creation and more functions
}


void Topl_Renderer_Vulkan::init(NATIVE_WINDOW window) {
	// Implement initialization
}

void Topl_Renderer_Vulkan::clearView(){
	// Implement clearing operation
}

void Topl_Renderer_Vulkan::setViewport(const Topl_Viewport* viewport) {
	// Implement viewport setting
}

void Topl_Renderer_Vulkan::switchFramebuff(){ 
	// Implement framebuffer switching operation
	_isDrawn = false;
}

void Topl_Renderer_Vulkan::build(const Topl_Scene* scene) {
	// Implement building operation
	_isBuilt = true;
}

#ifdef RASTERON_H

Rasteron_Image* Topl_Renderer_Vulkan::frame() {
	Rasteron_Image* image = createSolidImg({ TOPL_WIN_HEIGHT, TOPL_WIN_WIDTH }, CLEAR_COLOR_HEX);
	return image;
}

void Topl_Renderer_Vulkan::texturize(const Topl_Scene* scene) {
#ifdef RASTERON_H // Rasteron dependency required for updating textures
	// Implement texturizing operations
#endif
}

void Topl_Renderer_Vulkan::attachTexture(const Rasteron_Image* image, unsigned id){
	// Implement texture attaching
}

void Topl_Renderer_Vulkan::attachMaterial(const Topl_Material* material, unsigned id) {
	// Implement Body
}

#endif

void Topl_Renderer_Vulkan::update(const Topl_Scene* scene){
	// Implement update operations
}

void Topl_Renderer_Vulkan::drawMode(){
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