#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "primitives/Geo_Sphere.hpp"

#define MOVE_AMOUNT 0.5

namespace Topl {
	Topl_Scene scene;
	Topl_Camera camera = Topl_Camera(PROJECTION_Ortho, SpatialBounds3D(3.0f));
	Timer_Ticker gameTicker;
	Topl_LightSource whiteLight = Topl_LightSource(Eigen::Vector3f(0.1f, 0.0f, 10.0f));
	Topl_LightSource redLight = Topl_LightSource(Eigen::Vector3f(3.0f, 0.0f, 0.0f), Eigen::Vector3f(1.0f, 0.0, 0.0), 10.0f);

	NGon3D ngon = { 0.2, 12, 12 };
	NGon3D ngon2 = { 0.1, 6, 30 };
	Geo_SphereUV sphere(ngon);
	Geo_SphereUV sphere2(ngon2);
	Geo_Actor sphereGeo((const Geo_RenderObj*)&sphere);
	Geo_Actor sphereGeo2((const Geo_RenderObj*)&sphere2);

	Phys_Motion motion = Phys_Motion(MOTION_Linear, Eigen::Vector3f(1.0f, 0.0f, 0.0), 4.0);
}

void buttonCallback_w(void) { Topl::camera.movePos(Eigen::Vector3f(0.0f, 0.0f, MOVE_AMOUNT)); } // Move forward
void buttonCallback_a(void) { Topl::camera.movePos(Eigen::Vector3f(-1.0f * MOVE_AMOUNT, 0.0f, 0.0)); } // Move left
void buttonCallback_s(void) { Topl::camera.movePos(Eigen::Vector3f(0.0f, 0.0f, -1.0f * MOVE_AMOUNT)); } // Move backwards
void buttonCallback_d(void) { Topl::camera.movePos(Eigen::Vector3f(MOVE_AMOUNT, 0.0f, 0.0f)); } // Move right

/* void orbitCallback(double absSecs){
	Eigen::Vector3f motionVec = 10.0 * Topl::motion.getMotion(absSecs);
	Topl::sphereGeo.setPos(Eigen::Vector3f(std::sin(motionVec.x()), std::cos(motionVec.y()), 0.0));
} */

void pongCallback(double absSecs) {
	Eigen::Vector3f motionVec = Topl::motion.getMotion(absSecs);
	// Topl::sphereGeo.setRot(Eigen::Vector2f(motionVec.x(), motionVec.y()));
	Topl::sphereGeo.setPos(Eigen::Vector3f(motionVec.x() - 0.5f, motionVec.y() - 0.5f, 0.0f));
}

void moveUpCallback() {
	Eigen::Vector3f updatePos = Eigen::Vector3f(*Topl::sphereGeo2.getPos()) + Eigen::Vector3f(0.0f, 0.1f, 0.0f);
	Topl::sphereGeo2.setPos(updatePos);
}

// Shared functions

namespace Main {
	void init(Platform* platform){
		platform->createWindow();

		Platform::keyLogger.addCallback('w', buttonCallback_w);
		Platform::keyLogger.addCallback('a', buttonCallback_a);
		Platform::keyLogger.addCallback('s', buttonCallback_s);
		Platform::keyLogger.addCallback('d', buttonCallback_d);

		// Topl::scene.addLightSource(Topl_LightSource(Eigen::Vector3f(0.2f, 0.0f, 0.0f)));
		Topl::scene.addLightSource(&Topl::whiteLight);
		Topl::scene.addLightSource(&Topl::redLight);
		Topl::scene.addGeometry("sphere", &Topl::sphereGeo);
		Topl::scene.addGeometry("sphere2", &Topl::sphereGeo2);

		Topl::gameTicker.addRecurringEvent(&pongCallback);
		Topl::gameTicker.addPeriodicEvent(2000, moveUpCallback);
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer){
		while (1) {
			Topl::gameTicker.updateTimer();
			renderer->clearView();
			renderer->updateScene(&Topl::scene);
			renderer->renderAll();
			renderer->switchFramebuff();

			platform->handleEvents(false);
		}
	}
}