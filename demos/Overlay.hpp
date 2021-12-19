#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "primitives/Geo_Flat.hpp"
#include "trees/Pane.hpp"

namespace Topl {
	Topl_Scene scene;
	Topl_Camera camera = Topl_Camera(PROJECTION_Ortho, SpatialBounds3D(3.0f));
    std::string assetsPath = ASSETS_DIR;
	std::string imagesSubPath = "images/";

    Geo_FlatTriangle trig = Geo_FlatTriangle(1.0f);
    Geo_Actor trigGeo  = Geo_Actor((Geo_RenderObj*)&trig);
}

// Shared functions

namespace Main {
    void init(Platform* platform) {
		platform->createWindow();
		// platform->createWindow(); // creates first child
		// platform->createWindow(); // creates second child
		// platform->createWindow(); // creates third child
		// platform->createWindow(); // creates fourth child

		Topl::scene.addGeometry("triangle", &Topl::trigGeo);
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