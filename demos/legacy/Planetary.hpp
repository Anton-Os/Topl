#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "primitives/Geo_Sphere.hpp"

#define MOVE_AMOUNT 0.5f
#define LIGHT_DISTANCE 2.0f
#define SMALL_RADIUS 0.1f
#define LARGE_RADIUS 0.2f

namespace App {
	Topl_Scene scene;
	Topl_Camera camera = Topl_Camera(PROJECTION_Ortho, SpatialBounds3D(3.0f));
	Timer_Ticker gameTicker;
	Topl_Light skyLight = Topl_Light(Vec3f({ 0.0f, LIGHT_DISTANCE, 0.0f }));
	Topl_Light flashLight = Topl_Light(Vec3f({ 0.0f, 0.0f, -1.0f * LIGHT_DISTANCE }), Vec3f({ 1.0f, 0.0, 0.0 }));
	Topl_Light lampLight = Topl_Light(Vec3f({ 0.0f, -1.0 * LIGHT_DISTANCE, 0.0f })); // this light will be moving

	NGon3D ngon = { LARGE_RADIUS, 1024, 1024 };
	NGon3D ngon2 = { SMALL_RADIUS, 4, 4 };
	Geo_SphereUV sphere(ngon);
	Geo_SphereUV sphere2(ngon2);
	Geo_Actor sphereGeo((const Geo_RenderObj*)&sphere);
	Geo_Actor sphereGeo2((const Geo_RenderObj*)&sphere2);

	Phys_Motion pongMotion = Phys_Motion(MOTION_Linear, Vec3f({ 1.0f, 0.0f, 0.0 }), 2.0);
	Phys_Motion orbitMotion = Phys_Motion(MOTION_Orbit, Vec3f({ 1.0f, 0.0f, 0.0 }), 2.0);
}

void buttonCallback_w(void) { App::camera.updatePos(Vec3f({ 0.0f, 0.0f, MOVE_AMOUNT })); } // Move forward
void buttonCallback_a(void) { App::camera.updatePos(Vec3f({ -1.0f * MOVE_AMOUNT, 0.0f, 0.0f })); } // Move left
void buttonCallback_s(void) { App::camera.updatePos(Vec3f({ 0.0f, 0.0f, -1.0f * MOVE_AMOUNT })); } // Move backwards
void buttonCallback_d(void) { App::camera.updatePos(Vec3f({ MOVE_AMOUNT, 0.0f, 0.0f })); } // Move right

void moveUpEvent() {
	Vec3f updatePos = App::sphereGeo2.getPosition() + Vec3f({0.0f, 0.1f, 0.0f});
	App::sphereGeo2.setPos(updatePos);
}

void pongEvent(double absSecs) {
	Vec3f motionVec = App::pongMotion.getMotion(absSecs);
	App::sphereGeo.setPos(Vec3f({ motionVec[0] - 0.5f, motionVec[1] - 0.5f, 0.0f }));
}

void orbitEvent(double absSecs){
	Vec3f motionVec = App::orbitMotion.getMotion(absSecs);
	App::lampLight.pos = Vec3f({ motionVec[0], motionVec[1], 0.0 });
}


namespace Main {
	void init(Platform* platform){
		platform->createWindow();

		Platform::keyLogger.addCallback('w', buttonCallback_w);
		Platform::keyLogger.addCallback('a', buttonCallback_a);
		Platform::keyLogger.addCallback('s', buttonCallback_s);
		Platform::keyLogger.addCallback('d', buttonCallback_d);

		App::scene.addLight(&App::skyLight);
		App::scene.addLight(&App::flashLight);
		App::scene.addLight(&App::lampLight);
		App::scene.addGeometry("sphere", &App::sphereGeo);
		App::scene.addGeometry("sphere2", &App::sphereGeo2);

		App::gameTicker.addRecurringEvent(&pongEvent);
		// App::gameTicker.addRecurringEvent(&orbitEvent);
		App::gameTicker.addPeriodicEvent(2000, moveUpEvent);
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer){
		renderer->setDrawMode(DRAW_Triangles);

		while (1) {
			App::gameTicker.updateTimer();
			renderer->clearView();
			renderer->updateScene(&App::scene);
			renderer->renderScene(&App::scene);
			renderer->switchFramebuff();

			platform->handleEvents(false);
		}
	}
}