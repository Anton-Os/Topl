
#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "Geo_Tree.hpp"
#include "trees/Humanoid.hpp"


#define SHIFT_AMOUNT 0.2
#define MOVE_SCALE 22

namespace Topl {
	Topl_Scene scene;
	Topl_Camera camera = Topl_Camera(PROJECTION_Ortho, SpatialBounds3D(3.0f));
	Timer_Ticker gameTicker;
	std::string assetsPath = ASSETS_DIR;
	std::string imagesSubPath = "images/";

	std::pair<std::string, Eigen::Vector3f> demonActor[HUMANOID_PARTS_COUNT] = {
		std::make_pair(assetsPath + imagesSubPath + "Demon-Head.png", Eigen::Vector3f(0.0f, 0.08f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "Demon-LeftWing.png", Eigen::Vector3f(0.12f, -0.2f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "Demon-RightWing.png", Eigen::Vector3f(-0.12f, -0.2f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "Demon-Torso.png", Eigen::Vector3f(0.0f, -0.05f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "Demon-LeftLeg.png", Eigen::Vector3f(0.06f, -0.19f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "Demon-RightLeg.png", Eigen::Vector3f(-0.06f, -0.19f, 0.0))
	};

	std::pair<std::string, Eigen::Vector3f> angelActor[HUMANOID_PARTS_COUNT] = {
		std::make_pair(assetsPath + imagesSubPath + "Angel-Head.png", Eigen::Vector3f(0.0f, 0.08f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "Angel-LeftWing.png", Eigen::Vector3f(0.12f, -0.17f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "Angel-RightWing.png", Eigen::Vector3f(-0.12f, -0.17f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "Angel-Torso.png", Eigen::Vector3f(0.0f, -0.05f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "Angel-LeftLeg.png", Eigen::Vector3f(0.06f, -0.19f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "Angel-RightLeg.png", Eigen::Vector3f(-0.06f, -0.19f, 0.0))
	};

	std::pair<std::string, Eigen::Vector3f> ghostActor[HUMANOID_PARTS_COUNT] = {
		std::make_pair(assetsPath + imagesSubPath + "Ghost-Head.png", Eigen::Vector3f(0.0f, 0.08f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "Ghost-LeftArm.png", Eigen::Vector3f(0.12f, -0.11f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "Ghost-RightArm.png", Eigen::Vector3f(-0.12f, -0.11f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "Ghost-Torso.png", Eigen::Vector3f(0.0f, -0.05f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "Ghost-LeftLeg.png", Eigen::Vector3f(0.06f, -0.19f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "Ghost-RightLeg.png", Eigen::Vector3f(-0.06f, -0.19f, 0.0))
	};

	Geo_Humanoid demon("demon", &scene, demonActor, 0.25f);
	Geo_Humanoid angel("angel", &scene, angelActor, 0.25f);
	Geo_Humanoid ghost("ghost", &scene, ghostActor, 0.25f);
}

void buttonCallback_w(void) { Topl::demon.move(&Topl::scene, Eigen::Vector3f(0.0f, SHIFT_AMOUNT, 0.0f)); } // Move up
void buttonCallback_a(void) { Topl::demon.move(&Topl::scene, Eigen::Vector3f(-1 * SHIFT_AMOUNT, 0.0f, 0.0f)); } // Move left
void buttonCallback_s(void) { Topl::demon.move(&Topl::scene, Eigen::Vector3f(0.0f, -1 * SHIFT_AMOUNT, 0.0f)); } // Move down
void buttonCallback_d(void) { Topl::demon.move(&Topl::scene, Eigen::Vector3f(SHIFT_AMOUNT, 0.0f, 0.0f)); } // Move right
void buttonCallback_r(void) { Topl::demon.rotate(&Topl::scene, Eigen::Vector2f(1.0f, 0.0f)); } // Rotate

void actionCallback() {
	// adding movements for testing
	Topl::scene.addForce("demon_head", Eigen::Vector3f(0.0f, 1.0f * MOVE_SCALE, 0.0f));
	Topl::scene.addForce("demon_body", Eigen::Vector3f(0.0f, -1.0f * MOVE_SCALE, 0.0f));
	Topl::scene.addForce("demon_rightLeg", Eigen::Vector3f(3.0f * MOVE_SCALE, 0.0f, 0.0f));
	Topl::scene.addForce("demon_leftLeg", Eigen::Vector3f(-3.0f * MOVE_SCALE, 0.0f, 0.0f));

	Topl::scene.addForce("angel_head", Eigen::Vector3f(0.0f, -1.0f * MOVE_SCALE, 0.0f));
	Topl::scene.addForce("angel_body", Eigen::Vector3f(0.0f, -3.0f * MOVE_SCALE, 0.0f));
	Topl::scene.addForce("angel_rightLeg", Eigen::Vector3f(0.0f, 2.0f * MOVE_SCALE, 0.0f));
	Topl::scene.addForce("angel_leftLeg", Eigen::Vector3f(0.0f, 2.0f * MOVE_SCALE, 0.0f));
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

		Topl::demon.move(&Topl::scene, Eigen::Vector3f(-0.3f, 0.0f, 0.0f));
		Topl::angel.move(&Topl::scene, Eigen::Vector3f(0.3f, 0.0f, 0.0f));
		Topl::ghost.move(&Topl::scene, Eigen::Vector3f(0.0, -0.5f, 0.0f));
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