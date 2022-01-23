#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "primitives/Geo_Flat.hpp"
#include "trees/Pane.hpp"

namespace Topl {
	Topl_Scene scene;
	Topl_Camera camera = Topl_Camera(); // identity matrix
    std::string assetsPath = ASSETS_DIR;
	std::string imagesSubPath = "images/";

    Geo_FlatTriangle trig = Geo_FlatTriangle(0.5f);
    Geo_Actor trigGeo  = Geo_Actor((Geo_RenderObj*)&trig);

	Geo_PaneLayout paneLayout("layout", &scene, 5, 5);
}

// Shared functions

namespace Main {
    void init(Platform* platform) {
		platform->createWindow();
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer) {
		while (1) {
			renderer->clearView();
			renderer->updateScene(&Topl::scene);
			renderer->renderScene(DRAW_Triangles);

			platform->handleEvents();
		}
	}
}