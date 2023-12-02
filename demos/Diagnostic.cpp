#include <iostream>

#include "Diagnostic.hpp"

#define TARGET_BACKEND BACKEND_GL4
// #define TARGET_BACKEND BACKEND_DX11
// #define TARGET_BACKEND BACKEND_Vulkan

#define FRAME_AVG_TIME 100
#define FRAME_SPIKE_TIME 20

/* // Constructor and Destructor Testing
{
	Topl_Renderer_GL4 renderer_GL = Topl_Renderer_GL4(platform.getParentWindow());
#ifdef _WIN32
	Topl_Renderer_DX11 renderer_Drx = Topl_Renderer_DX11(platform.getParentWindow());
#endif
#ifdef TOPL_ENABLE_VULKAN
	Topl_Renderer_Vulkan renderer_Vk = Topl_Renderer_Vulkan(platform.getParentWindow());
#endif
} */

// Diagnostic Main Loop

int main(int argc, char** argv) {
	Platform platform(argv[0], "Diagnostic");
	platform.createWindow(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);

	// Performance Testing

	Topl_Renderer* renderer = nullptr;

	if (TARGET_BACKEND == BACKEND_GL4) renderer = new Diagnostic_Renderer_GL4(platform.getParentWindow());
	else if (TARGET_BACKEND == BACKEND_DX11) renderer = new Diagnostic_Renderer_DX11(platform.getParentWindow());
	else if (TARGET_BACKEND == BACKEND_Vulkan) renderer = new Diagnostic_Renderer_Vulkan(platform.getParentWindow());

	Timer_Ticker _ticker;
	double frameTotal = 0.0;
	double swapTime = 0.0;

	if(TARGET_BACKEND != BACKEND_Vulkan){
		test_scene.addGeometry(&actor);
		renderer->buildScene(&test_scene);
	}

	while(1){
		platform.handleEvents(DISABLE_CURSOR_UPDATE);

		if(TARGET_BACKEND != BACKEND_Vulkan){
			// Frame Rate and Render Profiling
			double f1 = _ticker.getRelMillisecs();
			renderer->clearView();
			double f2 = _ticker.getRelMillisecs();
			renderer->renderScene(nullptr);
			double f3 = _ticker.getRelMillisecs();
			renderer->present();
			double f4 = _ticker.getRelMillisecs();

			frameTotal += f1 + f2 + f3 + f4;
			if(f1 + f2 + f3 + f4 > frameTotal / renderer->getFrameCount())
				std::cout << "Frame times: " 
				<< f1 << " on start, " << f2 << " on clear, " << f3 << " on render, " << f4 << " on present, " 
				<< " Total: " << f1 + f2 + f3 + f4 
				<< " | Average: " << frameTotal / renderer->getFrameCount() << std::endl;
		}
	}

	if(renderer != nullptr) delete(renderer);
	return 0;
}