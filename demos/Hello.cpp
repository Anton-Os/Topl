#include <iostream>

#include "Hello.hpp"

// #define TARGET_BACKEND BACKEND_GL4
// #define TARGET_BACKEND BACKEND_DX11
#define TARGET_BACKEND BACKEND_Vulkan

#define FRAME_AVG_TIME 100
#define FRAME_SPIKE_TIME 20

#define ACTOR_COUNT 1

static Geo_Triangle2D triangle = Geo_Triangle2D();
static Geo_Actor actor = Geo_Actor((Geo_Mesh*)&triangle);

static Topl_Scene scene = Topl_Scene();

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


// Hello Main Loop

int main(int argc, char** argv) {
	Platform platform(argv[0], "Hello");
	platform.createWindow(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);

	// Performance Testing

	Topl_Renderer* renderer = nullptr;

	if (TARGET_BACKEND == BACKEND_GL4) renderer = new Hello_Renderer_GL4(platform.getParentWindow());
	else if (TARGET_BACKEND == BACKEND_DX11) renderer = new Hello_Renderer_DX11(platform.getParentWindow());
	else if (TARGET_BACKEND == BACKEND_Vulkan) renderer = new Hello_Renderer_Vulkan(platform.getParentWindow());

	Timer_Persist _ticker;
	double frameTotal = 0.0;
	double swapTime = 0.0;

	if(TARGET_BACKEND != BACKEND_Vulkan){
		scene.addGeometry(&actor);
		renderer->buildScene(&scene);
	}

	while(1){
		platform.handleEvents();

		if(TARGET_BACKEND != BACKEND_Vulkan){
			// Frame Rate and Render Profiling
			double f1 = _ticker.getRelMillisecs();
			renderer->clear();
			double f2 = _ticker.getRelMillisecs();
			renderer->drawScene(nullptr);
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