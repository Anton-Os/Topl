#include <iostream>

#include "Hello.hpp"

#define TARGET_BACKEND BACKEND_GL4
// #define TARGET_BACKEND BACKEND_DX11
// #define TARGET_BACKEND BACKEND_VK

#define FRAME_AVG_TIME 100
#define FRAME_SPIKE_TIME 20

#define ACTOR_COUNT 1

static Geo_Triangle2D triangle = Geo_Triangle2D();
static Geo_Actor actor = Geo_Actor((Geo_Mesh*)&triangle);

static Topl_Scene scene = Topl_Scene();

// Hello Main Loop

int main(int argc, char** argv) {
	Platform platform(argv[0], "Hello");
	std::cout << "Window creation" << std::endl;
	platform.createWindow(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);

    // Performance Testing

    Topl_Renderer* renderer = nullptr;

    std::cout << "Renderer creation" << std::endl;
    if (TARGET_BACKEND == BACKEND_GL4) renderer = new Hello_Renderer_GL4(platform.getContext());
#ifdef _WIN32
    else if (TARGET_BACKEND == BACKEND_DX11) renderer = new Hello_Renderer_DX11(platform.getContext());
#endif
#ifdef TOPL_ENABLE_VULKAN
    else if (TARGET_BACKEND == BACKEND_VK) renderer = new Hello_Renderer_VK(platform.getContext());
#endif

	Timer_Persist _ticker;
	double frameTotal = 0.0;
	double swapTime = 0.0;

    std::cout << "Scene building" << std::endl;
	if(TARGET_BACKEND != BACKEND_VK){
        triangle.isTesselated = true;
		scene.addGeometry(&actor);
        renderer->setDrawMode(DRAW_Triangles);
		renderer->buildScene(&scene);
	}

    std::cout << "Rendering loop" << std::endl;
    while(platform.handleEvents()){
		if(TARGET_BACKEND != BACKEND_VK){
			// Frame Rate and Render Profiling
			double f1 = _ticker.getRelMillisecs();
			renderer->clear();
			double f2 = _ticker.getRelMillisecs();
            if(renderer->drawScene(nullptr) != true) return 0;
			double f3 = _ticker.getRelMillisecs();
			renderer->present();
			double f4 = _ticker.getRelMillisecs();

			frameTotal += f1 + f2 + f3 + f4;
            /* // if(f1 + f2 + f3 + f4 > frameTotal / renderer->getFrameCount())
				std::cout << "Frame times: " 
				<< f1 << " on start, " << f2 << " on clear, " << f3 << " on render, " << f4 << " on present, " 
				<< " Total: " << f1 + f2 + f3 + f4 
                << " | Average: " << frameTotal / renderer->getFrameCount() << std::endl; */
		}
	}

	if(renderer != nullptr) delete(renderer);
	return 0;
}
