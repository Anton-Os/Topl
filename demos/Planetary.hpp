#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "primitives/Geo_Sphere.hpp"

#define MOVE_AMOUNT 0.5

namespace Topl {
	Topl_Scene scene;
	Timer_Ticker gameTicker;

	NGon3D ngon = { 0.1, 12, 10 };
	NGon3D ngon2 = { 0.3, 105, 8 };
	Geo_SphereUV sphere(ngon);
	Geo_SphereUV sphere2(ngon2);
	Geo_Component sphereGeo((const Geo_RenderObj*)&sphere);
	Geo_Component sphereGeo2((const Geo_RenderObj*)&sphere2);

	Phys_Motion bounceMotion = Phys_Motion(MOTION_Linear, Eigen::Vector3f(0.0f, 0.3f, 0.0), 2.0 );
	Phys_Motion swivelMotion = Phys_Motion(MOTION_Pivot, Eigen::Vector3f(0.2f, 0.3f, 0.0), 2.5);
}

void buttonCallback_w(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(0.0f, 0.0f, MOVE_AMOUNT)); } // Move forward
void buttonCallback_a(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(-1.0f * MOVE_AMOUNT, 0.0f, 0.0)); } // Move left
void buttonCallback_s(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(0.0f, 0.0f, -1.0f * MOVE_AMOUNT)); } // Move backwards
void buttonCallback_d(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(MOVE_AMOUNT, 0.0f, 0.0f)); } // Move right

void bounceCallback(double absSecs) {
	Topl::sphereGeo2.setPos(Topl::bounceMotion.getMotion(absSecs));
}

void swivelCallback(double absSecs) {
	Eigen::Vector3f motionVec = 0.01 * Topl::swivelMotion.getMotion(absSecs);
	Topl::sphereGeo2.updateRot(Eigen::Vector2f(motionVec.x(), motionVec.y()));
}

// Shared functions

namespace Main {
	void init(Platform* platform){
		platform->createWindow("Planetary");

		Platform::keyLogger.addCallback('w', buttonCallback_w);
		Platform::keyLogger.addCallback('a', buttonCallback_a);
		Platform::keyLogger.addCallback('s', buttonCallback_s);
		Platform::keyLogger.addCallback('d', buttonCallback_d);

		Topl::scene.setCamera(PROJECTION_Ortho, SpatialBounds3D(3.0f));
		// Topl::scene.addGeometry("sphere", &Topl::sphereGeo);
		Topl::scene.addGeometry("sphere2", &Topl::sphereGeo2);

		// Topl::gameTicker.addPeriodicEvent(60, &bounceCallback);
		Topl::gameTicker.addRecurringEvent(&swivelCallback);
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer){
		while (1) {
			// Topl::sphereGeo2.updateRot(Eigen::Vector2f(0.0003f, 0.0f) * Topl::gameTicker.getAbsMillisecs());

			renderer->clearView();
			renderer->updateScene(&Topl::scene);
			renderer->renderScene(DRAW_Triangles);

			platform->handleEvents();
			Topl::gameTicker.updateTimer();
		}
	}
}