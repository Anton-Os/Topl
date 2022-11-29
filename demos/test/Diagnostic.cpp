#include <iostream>

#include "Diagnostic.hpp"

// #define APP_BACKEND APP_OpenGL_4
#define APP_BACKEND APP_DirectX_11
// #define APP_BACKEND App_Vulkan

#define FRAME_AVG_TIME 100
#define FRAME_SPIKE_TIME 20

// Diagnose Main Loop

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

	Topl_Renderer* renderer = nullptr;

	if (APP_BACKEND == APP_OpenGL_4) renderer = new Diagnostic_Renderer_GL4(platform.getParentWindow());
	else if (APP_BACKEND == APP_DirectX_11) renderer = new Diagnostic_Renderer_Drx11(platform.getParentWindow());
	else if (APP_BACKEND == APP_Vulkan) renderer = new Diagnostic_Renderer_Vulkan(platform.getParentWindow());

	Timer_Ticker _ticker;
	double frameTotal = 0.0;
	while(1){
		// platform.handleEvents(DISABLE_CURSOR_UPDATE);
		
		double f1 = _ticker.getRelMillisecs();
		renderer->clearView();
		double f2 = _ticker.getRelMillisecs();
		renderer->renderAll();
		double f3 = _ticker.getRelMillisecs();
		renderer->switchFramebuff();
		double f4 = _ticker.getRelMillisecs();

		frameTotal += f1 + f2 + f3 + f4;
		if(f1 + f2 + f3 + f4 > frameTotal / renderer->getFrameCount())
			std::cout << "Frame times: " << f1 << ", " << f2 << ", " << f3 << ", " << f4 << ", " 
			<< " Total: " << f1 + f2 + f3 + f4 << " | Average: " << frameTotal / renderer->getFrameCount() << std::endl;
	}

	if(renderer != nullptr) delete(renderer);
	return 0;
}