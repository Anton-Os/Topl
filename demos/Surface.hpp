#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "primitives/Geo_Flat.hpp"

namespace Topl {
	// Management Objects
	Topl_Scene scene;
	Topl_Camera camera = Topl_Camera(PROJECTION_Ortho, SpatialBounds3D(3.0f));

    Geo_Plane plane = Geo_Plane(Eigen::Vector3f(0.0f, 1.0f, 0.0f), Eigen::Vector3f(1.0f, 0.0, 0.0));
    Geo_Actor planeActor  = Geo_Actor((Geo_RenderObj*)&plane);
	Geo_FlatCircle circle = Geo_FlatCircle(0.1f);
	Geo_Actor circleActor = Geo_Actor((Geo_RenderObj*)&circle);

	unsigned clicks = 0; // tracks presses with mouse
}

void mouseCallback_l(void){ if(Topl::clicks > 0) Topl::clicks--; }
void mouseCallback_r(void){ Topl::clicks++; }

// Shared functions

namespace Main {
    void init(Platform* platform) {
		platform->createWindow();

		Platform::mouseLogger.addCallback(MOUSE_LeftBtn_Down, mouseCallback_l);
		Platform::mouseLogger.addCallback(MOUSE_RightBtn_Down, mouseCallback_r);

		Topl::scene.addGeometry("plane", &Topl::planeActor);
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer) {
		float mouseX, mouseY;

		while (1) {
			renderer->clearView();
			renderer->updateScene(&Topl::scene);
			renderer->renderScene(DRAW_Triangles);

            bool isInWindowBounds = platform->getCursorCoords(&mouseX, &mouseY);
			platform->handleEvents();
		}
	}
}