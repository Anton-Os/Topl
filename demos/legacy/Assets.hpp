
#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "Geo_Tree.hpp"
#include "trees/Model.hpp"

#define VIEW_SPACE 3.0
#define MOVE_AMOUNT 1.5

namespace App {
	Topl_Scene scene;
	Topl_Camera camera = Topl_Camera(PROJECTION_Ortho, SpatialBounds3D(30.0f));
	// Topl_Camera camera = Topl_Camera(PROJECTION_Perspective, SpatialBounds3D(VIEW_SPACE));
	// Topl_Camera camera = Topl_Camera(PROJECTION_Stereographic, SpatialBounds3D(VIEW_SPACE));
	// Topl_Camera camera = Topl_Camera(PROJECTION_Gnomonic, SpatialBounds3D(VIEW_SPACE));
	std::string assetsPath = ASSETS_DIR;
	std::string modelsPath = assetsPath + "models/";

	Geo_Model model("model", modelsPath + "UrkwinAlien.obj", &scene);
}

void buttonCallback_w(void) { App::camera.movePos(Eigen::Vector3f(0.0f, MOVE_AMOUNT, 0.0f)); } // Move up
void buttonCallback_a(void) { App::camera.movePos(Eigen::Vector3f(-1.0f * MOVE_AMOUNT, 0.0f, 0.0)); } // Move left
void buttonCallback_s(void) { App::camera.movePos(Eigen::Vector3f(0.0f, -1.0f * MOVE_AMOUNT, 0.0f)); } // Move down
void buttonCallback_d(void) { App::camera.movePos(Eigen::Vector3f(MOVE_AMOUNT, 0.0f, 0.0f)); } // Move right

// Shared functions

namespace Main {
	void init(Platform* platform) {
		platform->createWindow();

		Platform::keyLogger.addCallback('w', buttonCallback_w);
		Platform::keyLogger.addCallback('a', buttonCallback_a);
		Platform::keyLogger.addCallback('s', buttonCallback_s);
		Platform::keyLogger.addCallback('d', buttonCallback_d);

		App::camera.setLookPos(Eigen::Vector3f(0.0, 10.0f, 0.0f));
		App::model.rotate(Eigen::Vector2f(0.0f, TOPL_HALF_PI));
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer) {

		while (1) {
			renderer->clearView();
			renderer->updateScene(&App::scene);
			renderer->renderScene(&App::scene);
			renderer->switchFramebuff();

			platform->handleEvents(false);
		}
	}
}