#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "primitives/Geo_Flat.hpp"

namespace Topl {
	// Management Objects
	Topl_Scene scene;
	Topl_Camera camera = Topl_Camera(PROJECTION_Ortho, SpatialBounds3D(3.0f));

    Geo_Plane plane = Geo_Plane(Eigen::Vector3f(0.0f, 1.0f, 0.0f), Eigen::Vector3f(1.0f, 0.0, 0.0));
    Geo_Actor planeGeo  = Geo_Actor((Geo_RenderObj*)&plane);
}

// Shared functions

namespace Main {
    void init(Platform* platform) {
		platform->createWindow();

		Topl::scene.addGeometry("plane", &Topl::planeGeo);
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