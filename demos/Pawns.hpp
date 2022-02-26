
#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "Geo_Tree.hpp"
#include "trees/Humanoid.hpp"


#define SHIFT_AMOUNT 0.2
#define ROTATE_AMOUNT 0.3
#define PARTS_SCALE 3
#define MOVE_SCALE 22

namespace Topl {
	Topl_Scene scene;
	Topl_Camera camera = Topl_Camera(PROJECTION_Ortho, SpatialBounds3D(3.0f));
	Timer_Ticker gameTicker;

	std::string assetsPath = ASSETS_DIR;
	std::string imagePath = assetsPath + "images/";

	std::pair<std::string, Eigen::Vector3f> demonActor[HUMANOID_PARTS_COUNT] = {
		std::make_pair(imagePath + "Demon-Head.png", Eigen::Vector3f(0.0f, 0.08f * PARTS_SCALE, 0.0)),
		std::make_pair(imagePath + "Demon-LeftWing.png", Eigen::Vector3f(0.12f * PARTS_SCALE, -0.2f * PARTS_SCALE, 0.0)),
		std::make_pair(imagePath + "Demon-RightWing.png", Eigen::Vector3f(-0.12f * PARTS_SCALE, -0.2f * PARTS_SCALE, 0.0)),
		std::make_pair(imagePath + "Demon-Torso.png", Eigen::Vector3f(0.0f, -0.05f * PARTS_SCALE, 0.0)),
		std::make_pair(imagePath + "Demon-LeftLeg.png", Eigen::Vector3f(0.06f * PARTS_SCALE, -0.19f * PARTS_SCALE, 0.0)),
		std::make_pair(imagePath + "Demon-RightLeg.png", Eigen::Vector3f(-0.06f * PARTS_SCALE, -0.19f * PARTS_SCALE, 0.0))
	};

	std::pair<std::string, Eigen::Vector3f> angelActor[HUMANOID_PARTS_COUNT] = {
		std::make_pair(imagePath + "Angel-Head.png", Eigen::Vector3f(0.0f, 0.08f * PARTS_SCALE, 0.0)),
		std::make_pair(imagePath + "Angel-LeftWing.png", Eigen::Vector3f(0.12f * PARTS_SCALE, -0.17f * PARTS_SCALE, 0.0)),
		std::make_pair(imagePath + "Angel-RightWing.png", Eigen::Vector3f(-0.12f * PARTS_SCALE, -0.17f * PARTS_SCALE, 0.0)),
		std::make_pair(imagePath + "Angel-Torso.png", Eigen::Vector3f(0.0f, -0.05f * PARTS_SCALE, 0.0)),
		std::make_pair(imagePath + "Angel-LeftLeg.png", Eigen::Vector3f(0.06f, -0.19f * PARTS_SCALE, 0.0)),
		std::make_pair(imagePath + "Angel-RightLeg.png", Eigen::Vector3f(-0.06f * PARTS_SCALE, -0.19f * PARTS_SCALE, 0.0))
	};

	std::pair<std::string, Eigen::Vector3f> ghostActor[HUMANOID_PARTS_COUNT] = {
		std::make_pair(imagePath + "Ghost-Head.png", Eigen::Vector3f(0.0f, 0.08f * PARTS_SCALE, 0.0)),
		std::make_pair(imagePath + "Ghost-LeftArm.png", Eigen::Vector3f(0.12f * PARTS_SCALE, -0.11f * PARTS_SCALE, 0.0)),
		std::make_pair(imagePath + "Ghost-RightArm.png", Eigen::Vector3f(-0.12f * PARTS_SCALE, -0.11f * PARTS_SCALE, 0.0)),
		std::make_pair(imagePath + "Ghost-Torso.png", Eigen::Vector3f(0.0f, -0.05f * PARTS_SCALE, 0.0)),
		std::make_pair(imagePath + "Ghost-LeftLeg.png", Eigen::Vector3f(0.06f * PARTS_SCALE, -0.19f * PARTS_SCALE, 0.0)),
		std::make_pair(imagePath + "Ghost-RightLeg.png", Eigen::Vector3f(-0.06f * PARTS_SCALE, -0.19f * PARTS_SCALE, 0.0))
	};

	Geo_Humanoid demon("demon", &scene, demonActor, 0.25f);
	Geo_Humanoid angel("angel", &scene, angelActor, 0.25f);
	Geo_Humanoid ghost("ghost", &scene, ghostActor, 0.25f);
}

void buttonCallback_w(void) { Topl::demon.move(Eigen::Vector3f(0.0f, SHIFT_AMOUNT, 0.0f)); } // Move up
void buttonCallback_a(void) { Topl::demon.move(Eigen::Vector3f(-1 * SHIFT_AMOUNT, 0.0f, 0.0f)); } // Move left
void buttonCallback_s(void) { Topl::demon.move(Eigen::Vector3f(0.0f, -1 * SHIFT_AMOUNT, 0.0f)); } // Move down
void buttonCallback_d(void) { Topl::demon.move(Eigen::Vector3f(SHIFT_AMOUNT, 0.0f, 0.0f)); } // Move right
void buttonCallback_r(void) { 
	Topl::demon.rotateAll(Eigen::Vector2f(ROTATE_AMOUNT, ROTATE_AMOUNT));
	Topl::ghost.rotate(Eigen::Vector2f(ROTATE_AMOUNT, 0.0f));

	/* for(unsigned l = 0; l < Topl::scene.getLinkedItemsCount(); l++) */
	Topl::scene.modConnector("angel_body", Eigen::Vector2f(ROTATE_AMOUNT, 0.0f), 1.0f);

} // rotateAll

void actionCallback() {
	// adding movements for testing
	Topl::scene.addForce("demon_head", Eigen::Vector3f(0.0f, 3.0f * MOVE_SCALE, 0.0f));
	Topl::scene.addForce("demon_body", Eigen::Vector3f(0.0f, -3.0f * MOVE_SCALE, 0.0f));
	Topl::scene.addForce("demon_rightLeg", Eigen::Vector3f(MOVE_SCALE, 0.0f, 0.0f));
	Topl::scene.addForce("demon_leftLeg", Eigen::Vector3f(-1.0f * MOVE_SCALE, 0.0f, 0.0f));

	Topl::scene.addForce("angel_head", Eigen::Vector3f(0.0f, -1.0f * MOVE_SCALE, 0.0f));
	Topl::scene.addForce("angel_body", Eigen::Vector3f(0.0f, MOVE_SCALE, 0.0f));
	Topl::scene.addForce("angel_rightLeg", Eigen::Vector3f(-2.0f, 4.0f * MOVE_SCALE, 0.0f));
	Topl::scene.addForce("angel_leftLeg", Eigen::Vector3f(2.0f, -4.0f * MOVE_SCALE, 0.0f));
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

		Topl::demon.move(Eigen::Vector3f(-1.5f, 0.0f, 0.0f));
		Topl::angel.move(Eigen::Vector3f(1.5f, 0.0f, 0.0f));
		Topl::ghost.move(Eigen::Vector3f(0.0, -0.5f, 0.0f));
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer) {
		while (1) {
			Topl::gameTicker.updateTimer();
			Topl::scene.resolvePhysics();

			renderer->clearView();
			renderer->updateScene(&Topl::scene);
			renderer->renderAll();
			renderer->switchFramebuff();

			platform->handleEvents(false);
		}
	}
}