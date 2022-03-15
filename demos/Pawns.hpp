
#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "Geo_Tree.hpp"
#include "trees/Humanoid.hpp"


#define SHIFT_AMOUNT 0.2
#define ROTATE_AMOUNT 0.3
#define PARTS_SCALE 3
#define MOVE_SCALE 22

namespace App {
	Topl_Scene scene;
	Topl_Camera camera = Topl_Camera(PROJECTION_Ortho, SpatialBounds3D(3.0f));
	Timer_Ticker gameTicker;

	std::string assetsPath = ASSETS_DIR;
	std::string imagePath = assetsPath + "images/";

	std::pair<Eigen::Vector3f, Eigen::Vector2f> orientations[HUMANOID_PARTS_COUNT] = { // shared orientations
		std::make_pair(Eigen::Vector3f(0.0f, 0.08f * PARTS_SCALE, 0.0), Eigen::Vector2f(-1.0 * TOPL_HALF_PI, 0.0)),
		std::make_pair(Eigen::Vector3f(0.12f * PARTS_SCALE, -0.2f * PARTS_SCALE, 0.0), Eigen::Vector2f(0.0, 0.0)),
		std::make_pair(Eigen::Vector3f(-0.12f * PARTS_SCALE, -0.2f * PARTS_SCALE, 0.0), Eigen::Vector2f(-1.0 * TOPL_PI, 0.0)),
		std::make_pair(Eigen::Vector3f(0.0f, -0.05f * PARTS_SCALE, 0.0), Eigen::Vector2f(0.0, 0.0)),
		std::make_pair(Eigen::Vector3f(0.06f * PARTS_SCALE, -0.19f * PARTS_SCALE, 0.0), Eigen::Vector2f(0.0, 0.0)),
		std::make_pair(Eigen::Vector3f(-0.06f * PARTS_SCALE, -0.19f * PARTS_SCALE, 0.0), Eigen::Vector2f(-1.0 * TOPL_PI, 0.0))
	};

	std::string demonAssets[HUMANOID_PARTS_COUNT] = { 
		imagePath + "Demon-Head.png", 
		imagePath + "Demon-LeftWing.png", 
		imagePath + "Demon-RightWing.png", 
		imagePath + "Demon-Torso.png", 
		imagePath + "Demon-LeftLeg.png", 
		imagePath + "Demon-RightLeg.png" 
	};

	std::string angelAssets[HUMANOID_PARTS_COUNT] = {
		imagePath + "Angel-Head.png",
		imagePath + "Angel-LeftWing.png",
		imagePath + "Angel-RightWing.png",
		imagePath + "Angel-Torso.png",
		imagePath + "Angel-LeftLeg.png",
		imagePath + "Angel-RightLeg.png"
	};

	std::string ghostAssets[HUMANOID_PARTS_COUNT] = {
		imagePath + "Ghost-Head.png",
		imagePath + "Ghost-LeftArm.png",
		imagePath + "Ghost-RightArm.png",
		imagePath + "Ghost-Torso.png",
		imagePath + "Ghost-LeftLeg.png",
		imagePath + "Ghost-RightLeg.png"
	};

	// Geo_Humanoid2D demon("demon", &scene, orientations, 0.25f);
	Geo_Humanoid2D demon("demon", &scene, demonAssets, 0.25f);
	Geo_Humanoid2D angel("angel", &scene, angelAssets, 0.25f);
	Geo_Humanoid2D ghost("ghost", &scene, ghostAssets, 0.25f);

	Phys_Connector demonAnchor, angelAnchor;
	Eigen::Vector3f demonOffset = Eigen::Vector3f(-1.5f, 0.0f, 0.0f);
	Eigen::Vector3f angelOffset = Eigen::Vector3f(1.5f, 0.0f, 0.0f);
	Eigen::Vector3f ghostOffset = Eigen::Vector3f(0.0, -0.5f, 0.0f);
}

void buttonCallback_w(void) { App::demon.move(Eigen::Vector3f(0.0f, SHIFT_AMOUNT, 0.0f)); } // Move up
void buttonCallback_a(void) { App::demon.move(Eigen::Vector3f(-1 * SHIFT_AMOUNT, 0.0f, 0.0f)); } // Move left
void buttonCallback_s(void) { App::demon.move(Eigen::Vector3f(0.0f, -1 * SHIFT_AMOUNT, 0.0f)); } // Move down
void buttonCallback_d(void) { App::demon.move(Eigen::Vector3f(SHIFT_AMOUNT, 0.0f, 0.0f)); } // Move right
void buttonCallback_r(void) { 
	App::demon.rotateAll(Eigen::Vector2f(ROTATE_AMOUNT, ROTATE_AMOUNT));
	App::ghost.rotate(Eigen::Vector2f(ROTATE_AMOUNT, 0.0f));
}

void actionCallback() {
	// adding movements for testing
	/* App::scene.addForce("demon_head", Eigen::Vector3f(0.0f, 3.0f * MOVE_SCALE, 0.0f));
	App::scene.addForce("demon_body", Eigen::Vector3f(0.0f, -3.0f * MOVE_SCALE, 0.0f));
	App::scene.addForce("demon_rightLeg", Eigen::Vector3f(MOVE_SCALE, 0.0f, 0.0f));
	App::scene.addForce("demon_leftLeg", Eigen::Vector3f(-1.0f * MOVE_SCALE, 0.0f, 0.0f)); */

	/* App::scene.addForce("angel_head", Eigen::Vector3f(0.0f, -1.0f * MOVE_SCALE, 0.0f));
	App::scene.addForce("angel_body", Eigen::Vector3f(0.0f, MOVE_SCALE, 0.0f));
	App::scene.addForce("angel_rightLeg", Eigen::Vector3f(-2.0f, 4.0f * MOVE_SCALE, 0.0f));
	App::scene.addForce("angel_leftLeg", Eigen::Vector3f(2.0f, -4.0f * MOVE_SCALE, 0.0f)); */
}

// Shared functions

namespace Main {
	void init(Platform* platform) {
		platform->createWindow();

		App::gameTicker.addPeriodicEvent(1000, actionCallback);

		Platform::keyLogger.addCallback('w', buttonCallback_w);
		Platform::keyLogger.addCallback('a', buttonCallback_a);
		Platform::keyLogger.addCallback('s', buttonCallback_s);
		Platform::keyLogger.addCallback('d', buttonCallback_d);
		Platform::keyLogger.addCallback('r', buttonCallback_r);

		// Initial Positions
		App::demon.move(App::demonOffset);
		App::demon.orientAll(App::orientations);
		App::angel.move(App::angelOffset);
		App::angel.orientAll(App::orientations);
		App::ghost.move(App::ghostOffset);
		App::ghost.orientAll(App::orientations);

		App::scene.addAnchor(&App::demonAnchor, "demon_body", &App::demonOffset);
		App::scene.addAnchor(&App::angelAnchor, "angel_head", &App::angelOffset);
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer) {
		while (1) {
			App::gameTicker.updateTimer();
			App::scene.resolvePhysics();

			renderer->clearView();
			renderer->updateScene(&App::scene);
			renderer->renderScene(&App::scene);
			renderer->switchFramebuff();

			platform->handleEvents(false);
		}
	}
}