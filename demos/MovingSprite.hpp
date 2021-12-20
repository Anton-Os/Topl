
#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "Geo_Tree.hpp"
#include "trees/Humanoid.hpp"


#define SHIFT_AMOUNT 0.2
#define MOVE_SCALE 10

namespace Topl {
	Topl_Scene scene;
	Topl_Camera camera = Topl_Camera(PROJECTION_Ortho, SpatialBounds3D(3.0f));
	Timer_Ticker gameTicker;
	std::string assetsPath = ASSETS_DIR;
	std::string imagesSubPath = "images/";

	std::pair<std::string, Eigen::Vector3f> humanoidActor[HUMANOID_PARTS_COUNT] = {
		std::make_pair(assetsPath + imagesSubPath + "Head.png", Eigen::Vector3f(0.0f, 0.11f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "LeftArm.png", Eigen::Vector3f(0.0f, -0.1f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "RightArm.png", Eigen::Vector3f(0.12f, -0.14f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "Body.png", Eigen::Vector3f(-0.12f, -0.14f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "LeftLeg.png", Eigen::Vector3f(0.06f, -0.35f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "RightLeg.png", Eigen::Vector3f(-0.06f, -0.35f, 0.0))
	};

	// Geo_Humanoid humanoid;
	Geo_Humanoid humanoid("humanoid", &scene, humanoidActor, 0.25f);
}

void buttonCallback_w(void) { Topl::humanoid.move(&Topl::scene, Eigen::Vector3f(0.0f, SHIFT_AMOUNT, 0.0f)); } // Move up
void buttonCallback_a(void) { Topl::humanoid.move(&Topl::scene, Eigen::Vector3f(-1 * SHIFT_AMOUNT, 0.0f, 0.0f)); } // Move left
void buttonCallback_s(void) { Topl::humanoid.move(&Topl::scene, Eigen::Vector3f(0.0f, -1 * SHIFT_AMOUNT, 0.0f)); } // Move down
void buttonCallback_d(void) { Topl::humanoid.move(&Topl::scene, Eigen::Vector3f(SHIFT_AMOUNT, 0.0f, 0.0f)); } // Move right
void buttonCallback_r(void) { Topl::humanoid.rotate(&Topl::scene, Eigen::Vector2f(1.0f, 0.0f)); } // Rotate

void actionCallback() {
	Topl::scene.addForce("humanoid_head", Eigen::Vector3f(0.0f, 1.0f * MOVE_SCALE, 0.0f)); // test head movement
	Topl::scene.addForce("humanoid_body", Eigen::Vector3f(0.0f, -1.0f * MOVE_SCALE, 0.0f)); // test body movement
	Topl::scene.addForce("humanoid_rightLeg", Eigen::Vector3f(3.0f * MOVE_SCALE, 0.0f, 0.0f)); // test leg movement
	Topl::scene.addForce("humanoid_leftLeg", Eigen::Vector3f(-3.0f * MOVE_SCALE, 0.0f, 0.0f)); // test leg movement
}

// Shared functions

namespace Main {
	void init(Platform* platform) {
		platform->createWindow();

		Platform::keyLogger.addCallback('w', buttonCallback_w);
		Platform::keyLogger.addCallback('a', buttonCallback_a);
		Platform::keyLogger.addCallback('s', buttonCallback_s);
		Platform::keyLogger.addCallback('d', buttonCallback_d);
		Platform::keyLogger.addCallback('r', buttonCallback_r);

		Topl::gameTicker.addPeriodicEvent(1000, actionCallback);
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer) {

		while (1) {
			Topl::scene.resolvePhysics();

			renderer->clearView();
			renderer->updateScene(&Topl::scene);
			renderer->renderScene(DRAW_Triangles);

			platform->handleEvents();
			Topl::gameTicker.updateTimer();
		}
	}
}