#include <iostream>

#include "Diagnostic.hpp"

#define APP_BACKEND APP_OpenGL_4
// #define APP_BACKEND APP_DirectX_11
// #define APP_BACKEND App_Vulkan

#define FRAME_AVG_TIME 500
#define FRAME_SPIKE_TIME 10

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
	else if(APP_BACKEND == APP_DirectX_11) renderer = new Diagnostic_Renderer_Drx11(platform.getParentWindow());

	Timer_Ticker _ticker;
	double frameAvg = 0.0;
	std::cout << "Frame Time: " << std::endl;
	while(1){
		double frameTime = _ticker.getRelMillisecs();
		if (frameTime > FRAME_SPIKE_TIME) std::cout << frameTime << ", "; // print frame time
		
		frameAvg += frameTime;
		if (renderer->getFrameCount() % FRAME_AVG_TIME == 0) {
			std::cout << "\nAverage: " << frameAvg / FRAME_AVG_TIME << std::endl; // print frame average
			frameAvg = 0.0; // reset
		}

		// platform.handleEvents(DISABLE_CURSOR_UPDATE);
		
		renderer->clearView();
		renderer->renderScene(nullptr);
		renderer->switchFramebuff();
	}

	if(renderer != nullptr) delete(renderer);
	return 0;
}