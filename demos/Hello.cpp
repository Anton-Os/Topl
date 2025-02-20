#include <iostream>

#include "Hello.hpp"

#define TARGET_BACKEND 1 // BACKEND_GL4
// #define TARGET_BACKEND 2 // BACKEND_DX11
// #define TARGET_BACKEND 3 // BACKEND_VK

#define FRAME_AVG_TIME 100
#define FRAME_SPIKE_TIME 20

#define ACTOR_COUNT 1

static Geo_Trig2D triangle;
static Geo_Actor actor = Geo_Actor((Geo_Mesh*)&triangle);
std::vector<Vec3f> calcPoints(1, VEC_3F_ONES);

static Topl_Scene scene = Topl_Scene();

// Hello Main Loop

static void logFrameRate(double f1, double f2, double f3, double f4){
    static unsigned frameCount = 0;
    static double frameTotal = 0.0;

    frameCount++;
    frameTotal += f1 + f2 + f3 + f4;

    std::cout << "Frame times: "
        << f1 << " on start, " << f2 << " on clear, " << f3 << " on render, " << f4 << " on present, "
        << " Total: " << f1 + f2 + f3 + f4
        << " | Average: " << frameTotal / frameCount << std::endl;
}

MAIN_ENTRY {
#ifndef __ANDROID__
    Platform platform(argv[0], "Hello");
	platform.createWindow(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);
#else
    Platform platform(pApp);
    while(platform.getParentWindow() == nullptr && platform.handleEvents() && !pApp->destroyRequested)
        platform.awaitWindow(); // waiting for window on Android
#endif

    std::cout << "Creating backend" << std::endl;
#if TARGET_BACKEND==1
	Hello_Renderer_GL4* renderer = new Hello_Renderer_GL4(platform.getContext());
#elif defined(_WIN32) && TARGET_BACKEND==2
	Hello_Renderer_DX11* renderer = new Hello_Renderer_DX11(platform.getContext());
#elif defined(TOPL_ENABLE_VULKAN) && TARGET_BACKEND==3
	Hello_Renderer_VK* renderer = new Hello_Renderer_VK(platform.getContext());
#endif

	Timer_Persist _ticker;
	double frameTotal = 0.0;
	double swapTime = 0.0;

	std::cout << "Scene building" << std::endl;
	scene.addGeometry(&actor);
    // triangle.instanceCount = 10;
	// triangle.tessLevel = 10;
	renderer->buildScene(&scene);

	std::cout << "Rendering loop" << std::endl;
	while(platform.handleEvents()){
		double f1 = _ticker.getRelMillisecs();
		renderer->clear();
		double f2 = _ticker.getRelMillisecs();
#if !defined(__ANDROID__) && TARGET_BACKEND!=3
		renderer->setDrawPipeline(false);
		renderer->dispatch(&calcPoints);
		renderer->setDrawPipeline(true);
#endif
        renderer->update(&actor);
		renderer->draw(&actor);
		double f3 = _ticker.getRelMillisecs();
		renderer->present();
        double f4 = _ticker.getRelMillisecs();
        // logFrameRate(f1, f2, f3, f4);
	}

	if(renderer != nullptr) delete(renderer);
	// return 0;
}