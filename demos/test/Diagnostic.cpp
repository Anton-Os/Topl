#include "Topl_App.hpp"

#include "Idle_Shader.hpp"
#include "Advance_Shader.hpp"

// #define APP_BACKEND APP_OpenGL_4
#define APP_BACKEND APP_DirectX_11
// #defint APP_BACKEND App_Vulkan

int main(int argc, char** argv) {
	Platform platform(argv[0], "Diagnostic");
	platform.createWindow(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);

	// Constructor and Destructor Testing
	{
		Topl_Renderer_GL4 renderer_GL = Topl_Renderer_GL4(platform.getParentWindow());
#ifdef _WIN32
		Topl_Renderer_Drx11 renderer_Drx = Topl_Renderer_Drx11(platform.getParentWindow());
#endif
#ifdef VULKAN_H
		Topl_Renderer_Vulkan renderer_Vk = Topl_Renderer_Vulkan(platform.getParentWindow());
#endif
	}

	// Performance Testing

	// Idle_VertexShader* vertexShader;
	// Idle_FragmentShader* fragShader;
	Topl_Renderer* renderer = nullptr;

	if (APP_BACKEND == APP_OpenGL_4) renderer = new Topl_Renderer_GL4(platform.getParentWindow());
	else if(APP_BACKEND == APP_DirectX_11) renderer = new Topl_Renderer_Drx11(platform.getParentWindow());
	else if (APP_BACKEND == APP_Vulkan) renderer = new Topl_Renderer_Vulkan(platform.getParentWindow());

	while(1){
		platform.handleEvents(DISABLE_CURSOR_UPDATE);
		
		renderer->clearView();
		renderer->renderScene(nullptr);
		renderer->switchFramebuff();
	}

	if(renderer != nullptr) delete(renderer);
	return 0;
}