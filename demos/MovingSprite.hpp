
#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "Geo_Construct.hpp"
#include "composites/Humanoid.hpp"


#define MOVE_AMOUNT 16.0

namespace Topl {
	Topl_Scene scene;
	std::string assetsPath = ASSETS_DIR;
	std::string imagesSubPath = "images/";

	// TODO: Make these not device specific, relative file paths only!
	std::pair<std::string, Eigen::Vector3f> humanoidProps[HUMANOID_PARTS_COUNT] = {
		std::make_pair(assetsPath + imagesSubPath + "Head.png", Eigen::Vector3f(0.0f, 0.11f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "LeftArm.png", Eigen::Vector3f(0.0f, -0.1f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "RightArm.png", Eigen::Vector3f(0.12f, -0.14f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "Body.png", Eigen::Vector3f(-0.12f, -0.14f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "LeftLeg.png", Eigen::Vector3f(0.06f, -0.35f, 0.0)),
		std::make_pair(assetsPath + imagesSubPath + "RightLeg.png", Eigen::Vector3f(-0.06f, -0.35f, 0.0))
	};

	// Geo_Humanoid humanoid;
	Geo_Humanoid humanoid("humanoid", &scene, humanoidProps, 0.25f);
}

void buttonCallback_w(void) { Topl::humanoid.move(&Topl::scene, Eigen::Vector3f(0.0f, MOVE_AMOUNT, 0.0f)); } // Move up
void buttonCallback_a(void) { Topl::humanoid.move(&Topl::scene, Eigen::Vector3f(-1 * MOVE_AMOUNT, 0.0f, 0.0f)); } // Move left
void buttonCallback_s(void) { Topl::humanoid.move(&Topl::scene, Eigen::Vector3f(0.0f, -1 * MOVE_AMOUNT, 0.0f)); } // Move down
void buttonCallback_d(void) { Topl::humanoid.move(&Topl::scene, Eigen::Vector3f(MOVE_AMOUNT, 0.0f, 0.0f)); } // Move right
void buttonCallback_r(void) { Topl::humanoid.rotate(&Topl::scene, Eigen::Vector2f(1.0f, 0.0f)); } // Rotate

// Shared functions

namespace Main {
	void init(Platform* platform) {
		platform->createWindow("Moving Sprite");

		Platform::keyLogger.addCallback('w', buttonCallback_w);
		Platform::keyLogger.addCallback('a', buttonCallback_a);
		Platform::keyLogger.addCallback('s', buttonCallback_s);
		Platform::keyLogger.addCallback('d', buttonCallback_d);
		Platform::keyLogger.addCallback('r', buttonCallback_r);
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