#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "primitives/Geo_Flat.hpp"

namespace App {
	// Management Objects
	Topl_Scene scene;
	Topl_Camera camera = Topl_Camera(PROJECTION_Ortho, SpatialBounds3D(3.0f));

	Geo_Plane plane = Geo_Plane(Vec3f({ 0.0f, 1.0f, 0.0f }), Vec3f({ 1.0f, 0.0, 0.0 }));
    Geo_Actor planeActor  = Geo_Actor((Geo_RenderObj*)&plane);
	Geo_FlatCircle circle = Geo_FlatCircle(0.1f);
	Geo_Actor circleActor = Geo_Actor((Geo_RenderObj*)&circle);

	unsigned clicks = 0; // tracks presses with mouse
}

void mouseCallback_l(float x, float y){ 
	if(App::clicks > 0) App::clicks--; 
	puts("Left Button Down!");
}
void mouseCallback_r(float x, float y){ 
	App::clicks++; 
	puts("Right Button Down");
}

// Shared functions

namespace Main {
    void init(Platform* platform) {
		platform->createWindow();

		Platform::mouseControl.addCallback(MOUSE_LeftBtn_Down, mouseCallback_l);
		Platform::mouseControl.addCallback(MOUSE_RightBtn_Down, mouseCallback_r);
		// Platform::mouseControl.addCallback(MOUSE_LeftBtn_Up, mouseCallback_l2);
		// Platform::mouseControl.addCallback(MOUSE_RightBtn_Up, mouseCallback_r2);

		App::scene.addGeometry("plane", &App::planeActor);
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer) {
		float mouseX, mouseY;

		while (1) {
			renderer->clearView();
			renderer->updateScene(&App::scene);
			renderer->renderScene(&App::scene);
			renderer->switchFramebuff();

			platform->handleEvents(true);
		}
	}
}