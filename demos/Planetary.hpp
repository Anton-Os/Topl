#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "primitives/Geo_Sphere.hpp"

#define MOVE_AMOUNT 0.5

namespace Topl {
	Topl_Scene scene;

	// NGon3D ngon = { 0.3, 6, 12 };
	NGon3D ngon = { 0.1, 7, 7 };
	Geo_SphereUV sphere(ngon);
	Geo_Component sphereGeo((const Geo_RenderObj*)&sphere);
}

void buttonCallback_w(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(0.0f, 0.0f, MOVE_AMOUNT)); } // Move forward
void buttonCallback_a(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(-1.0f * MOVE_AMOUNT, 0.0f, 0.0)); } // Move left
void buttonCallback_s(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(0.0f, 0.0f, -1.0f * MOVE_AMOUNT)); } // Move backwards
void buttonCallback_d(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(MOVE_AMOUNT, 0.0f, 0.0f)); } // Move right

// Shared functions

namespace Main {
	void init(Platform* platform){
		platform->createWindow("Planetary");

		Platform::keyLogger.addCallback('w', buttonCallback_w);
		Platform::keyLogger.addCallback('a', buttonCallback_a);
		Platform::keyLogger.addCallback('s', buttonCallback_s);
		Platform::keyLogger.addCallback('d', buttonCallback_d);
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer){
		Timer_Ticker gameTicker;

		while (renderer->renderScene(DRAW_Triangles)) {
			Topl::sphereGeo.updateRot(Eigen::Vector2f(0.01f, 0.0f) * gameTicker.getAbsSecs());
			renderer->updateScene(&Topl::scene);

			platform->handleEvents();
		}
	}
}