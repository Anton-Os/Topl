#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "primitives/Geo_Flat.hpp"

namespace Topl {
	// Management Objects
	Topl_Scene scene;

    Geo_FlatTriangle trig
    Geo_Component trigGeo  = Geo_Component((Geo_RenderObj*)&trig);
}

// Shared functions

namespace Main {
    void init(Platform* platform) {
		platform->createWindow("Overlay");

        // add callbacks

		Topl::scene.addGeometry("triangle", &Topl::trigGeo);
		Topl::scene.setCamera(PROJECTION_Ortho, SpatialBounds3D(3.0f));
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