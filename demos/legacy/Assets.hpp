
#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "Geo_Tree.hpp"
#include "trees/Model.hpp"

#define VIEW_SPACE 3.0
#define MOVE_AMOUNT 0.5

namespace App {
	Topl_Scene scene;
	Topl_Camera camera = Topl_Camera();
	// Topl_Camera camera = Topl_Camera(PROJECTION_Ortho, SpatialBounds3D(VIEW_SPACE));
	// Topl_Camera camera = Topl_Camera(PROJECTION_Perspective, SpatialBounds3D(VIEW_SPACE));
	// Topl_Camera camera = Topl_Camera(PROJECTION_Stereographic, SpatialBounds3D(VIEW_SPACE));
	// Topl_Camera camera = Topl_Camera(PROJECTION_Gnomonic, SpatialBounds3D(VIEW_SPACE));
	std::string assetsPath = ASSETS_DIR;
	std::string modelsPath = assetsPath + "models/";

	Geo_Model model("model", modelsPath + "UrkwinAlien.obj", &scene);
}

void buttonCallback_w(void) { // Move up
	App::camera.updatePos(Vec3f({ 0.0f, MOVE_AMOUNT, 0.0f }));
	App::camera.updateLookPos(Vec3f({ 0.0f, 0.0f, 1.0f }));
}
void buttonCallback_a(void) { // Move left
	App::camera.updatePos(Vec3f({ -1.0f * MOVE_AMOUNT, 0.0f, 0.0 }));
	App::camera.updateLookPos(Vec3f({ 0.0f, 0.0f, 1.0f }));
}
void buttonCallback_s(void) { // Move down
	App::camera.updatePos(Vec3f({ 0.0f, -1.0f * MOVE_AMOUNT, 0.0f }));
	App::camera.updateLookPos(Vec3f({ 0.0f, 0.0f, 1.0f }));
}
void buttonCallback_d(void) { // Move right
	App::camera.updatePos(Vec3f({ MOVE_AMOUNT, 0.0f, 0.0f }));
	App::camera.updateLookPos(Vec3f({ 0.0f, 0.0f, 1.0f }));
}

// Shared functions

namespace Main {
	void init(Platform* platform) {
		platform->createWindow();

		Platform::keyLogger.addCallback('w', buttonCallback_w);
		Platform::keyLogger.addCallback('a', buttonCallback_a);
		Platform::keyLogger.addCallback('s', buttonCallback_s);
		Platform::keyLogger.addCallback('d', buttonCallback_d);

		// App::camera.setLookPos(Eigen::Vector3f(0.0, 1.0f, -1.0f));
		// App::model.rotate(Eigen::Vector2f(0.0f, TOPL_PI));
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