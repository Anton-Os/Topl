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
	ValueGen valueGen = ValueGen(); // seeds random number generation

    Geo_FlatTriangle trig = Geo_FlatTriangle(0.5f); 
    Geo_Actor trigGeo  = Geo_Actor((Geo_RenderObj*)&trig);

	Geo_PaneLayout defaultLayout("layout1", &scene, 3, 3);
	Geo_PaneLayout customLayout("layout2", &scene, 12, 1, 0.25f, 0.02f);
}

// Shared functions

namespace Main {
    void init(Platform* platform) {
		platform->createWindow();

		Topl::customLayout.move(&Topl::scene, Eigen::Vector3f(0.75f, 0.0f, 0.0f));
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