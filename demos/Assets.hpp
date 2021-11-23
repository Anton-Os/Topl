
#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "Geo_Tree.hpp"
#include "trees/Model.hpp"

namespace Topl {
	Topl_Scene scene;
	Topl_Camera camera = Topl_Camera(PROJECTION_Ortho, SpatialBounds3D(3.0f));
	std::string assetsPath = ASSETS_DIR;
	std::string modelsSubPath = "models/";

	Geo_Model model("model", assetsPath + modelsSubPath + "UrkwinAlien.fbx", &scene);
}

// Shared functions

namespace Main {
	void init(Platform* platform) {
		platform->createWindow();
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer) {

		while (1) {
			Topl::scene.resolvePhysics();

			renderer->clearView();
			renderer->updateScene(&Topl::scene);
			renderer->renderScene(DRAW_Triangles);

			platform->handleEvents();
		}
	}
}