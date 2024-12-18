#include <iostream>

#include "Hello.hpp"

// #define TARGET_BACKEND BACKEND_GL4
#define TARGET_BACKEND BACKEND_DX11
// #define TARGET_BACKEND BACKEND_VK

#define FRAME_AVG_TIME 100
#define FRAME_SPIKE_TIME 20

#define ACTOR_COUNT 1

static Geo_Trig2D triangle;
static Geo_Actor actor = Geo_Actor((Geo_Mesh*)&triangle);

static Topl_Scene scene = Topl_Scene();

// Hello Main Loop

MAIN_ENTRY {
	Platform platform(argv[0], "Hello");
	std::cout << "Window creation" << std::endl;
	platform.createWindow(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);

#if TARGET_BACKEND == BACKEND_GL4
	Hello_Renderer_GL4* renderer = new Hello_Renderer_GL4(platform.getContext());
#elif defined(_WIN32) && TARGET_BACKEND == BACKEND_DX11
	Hello_Renderer_DX11* renderer = new Hello_Renderer_DX11(platform.getContext());
#elif defined(TOPL_ENABLE_VULKAN) && TARGET_BACKEND == BACKEND_VK
	Hello_Renderer_VK* renderer = new Hello_Renderer_VK(platform.getContext());
#endif

	Timer_Persist _ticker;
	double frameTotal = 0.0;
	double swapTime = 0.0;

	std::cout << "Scene building" << std::endl;
	scene.addGeometry(&actor);
	triangle.instanceCount = 10;
	triangle.tessLevel = 10;
	renderer->buildScene(&scene);

	std::cout << "Rendering loop" << std::endl;
	while(platform.handleEvents()){
		double f1 = _ticker.getRelMillisecs();
		renderer->clear();
		double f2 = _ticker.getRelMillisecs();
		renderer->setDrawPipeline(false);
		// renderer->dispatch(100);
		renderer->setDrawPipeline(true);
		renderer->draw(&actor);
		double f3 = _ticker.getRelMillisecs();
		renderer->present();
		double f4 = _ticker.getRelMillisecs();

		/* frameTotal += f1 + f2 + f3 + f4;
			if(f1 + f2 + f3 + f4 > frameTotal / renderer->getFrameCount())
			std::cout << "Frame times: "
			<< f1 << " on start, " << f2 << " on clear, " << f3 << " on render, " << f4 << " on present, "
			<< " Total: " << f1 + f2 + f3 + f4
			<< " | Average: " << frameTotal / renderer->getFrameCount() << std::endl; */
	}

	if(renderer != nullptr) delete(renderer);
	return 0;
}
