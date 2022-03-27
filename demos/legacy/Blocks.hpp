#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "Geo_Tree.hpp"
#include "primitives/Geo_Flat.hpp"
#include "primitives/Geo_Conic.hpp"
#include "primitives/Geo_Extruded.hpp"
#include "trees/Chain.hpp"
#include "trees/Grid.hpp"

#define MOVE_AMOUNT 0.25
#define CAM_INC_ANGLE 3.14159 / 64
#define CAM_MAX_ANGLE 3.14159 / 2.0 // roughly 90 degrees
#define CAM_MIN_ANGLE 3.14159 / -2.0 // roughly -90 degrees

namespace App {
	// Management Objects
	Topl_Scene scene;
	// Topl_Camera camera;
	Topl_Camera camera = Topl_Camera(PROJECTION_Ortho, SpatialBounds3D(1.0f));
	Eigen::Vector2f camRotAngles = Eigen::Vector2f(0.0f, 0.0f);

	// Primitive Geometry Objects
	Geo_FlatSquare rect1 = Geo_FlatSquare(0.1f);
	Geo_ExtrudedHex hex1 = Geo_ExtrudedHex(0.1f, 0.1f);
	Geo_ConicCircle cone1 = Geo_ConicCircle(0.1f, Eigen::Vector3f(0.0f, 0.0f, 0.1f));

	// Complex Geometry Objects
	Geo_Actor chainGeo = Geo_Actor((const Geo_RenderObj*)&cone1);
	Geo_Chain_Properties chainActor = Geo_Chain_Properties(Eigen::Vector3f(0.33f, 0.0, 0.0)); // stretches along X axis only
	Geo_Chain chain("chain", &scene, &chainGeo, &chainActor, 9);
	Geo_Actor gridGeo = Geo_Actor((const Geo_RenderObj*)&hex1);
	Geo_Grid_Properties gridActor = Geo_Grid_Properties(std::make_pair(3, 0.4f));
	Geo_Grid grid("grid", &scene, &gridGeo, &gridActor);

	float allRange[2] = { -1.0f, 1.0f }; // encompass entire screen area
	float posRange[2] = { 0.9f, 1.0f }; // encompass positive-side screen segment
	float negRange[2] = { -0.9f, -1.0f }; // encompass negative-side screen segment
	Input_CursorRange topRange = Input_CursorRange(allRange, posRange);
	Input_CursorRange rightRange = Input_CursorRange(posRange, allRange);
	Input_CursorRange leftRange = Input_CursorRange(negRange, allRange);
	Input_CursorRange botRange = Input_CursorRange(allRange, negRange);
}

static void calcCamera() {
	// TODO: Perform rotation calculation here!
	App::camera.setLookPos(Vec3f({ App::camRotAngles[0], App::camRotAngles[1], 0.0f }));
}
void hoverCallback_top(float x, float y) {
	if (App::camRotAngles[1] < CAM_MAX_ANGLE) App::camRotAngles[1] += CAM_INC_ANGLE;
	calcCamera();
}
void hoverCallback_right(float x, float y) {
	if (App::camRotAngles[0] < CAM_MAX_ANGLE) App::camRotAngles[0] += CAM_INC_ANGLE;
	calcCamera();
}
void hoverCallback_left(float x, float y) { 
	if (App::camRotAngles[0] > CAM_MIN_ANGLE) App::camRotAngles[0] -= CAM_INC_ANGLE;
	calcCamera();
}
void hoverCallback_bot(float x, float y) {
	if (App::camRotAngles[1] > CAM_MIN_ANGLE) App::camRotAngles[1] -= CAM_INC_ANGLE;
	calcCamera();
}

void buttonCallback_w(void) { App::camera.updatePos(Vec3f({ 0.0f, 0.0f, MOVE_AMOUNT })); } // Move forward
void buttonCallback_a(void) { App::camera.updatePos(Vec3f({ -1.0f * MOVE_AMOUNT, 0.0f, 0.0 })); } // Move left
void buttonCallback_s(void) { App::camera.updatePos(Vec3f({ 0.0f, 0.0f, -1.0f * MOVE_AMOUNT })); } // Move backwards
void buttonCallback_d(void) { App::camera.updatePos(Vec3f({ MOVE_AMOUNT, 0.0f, 0.0f })); } // Move right

// Shared functions

namespace Main {
	void init(Platform* platform) {
		platform->createWindow();

		Platform::mouseLogger.addHoverCallback(&App::topRange, hoverCallback_top);
		Platform::mouseLogger.addHoverCallback(&App::rightRange, hoverCallback_right);
		Platform::mouseLogger.addHoverCallback(&App::leftRange, hoverCallback_left);
		Platform::mouseLogger.addHoverCallback(&App::botRange, hoverCallback_bot);
		Platform::keyLogger.addCallback('w', buttonCallback_w);
		Platform::keyLogger.addCallback('a', buttonCallback_a);
		Platform::keyLogger.addCallback('s', buttonCallback_s);
		Platform::keyLogger.addCallback('d', buttonCallback_d);

		// App::camera.setPos(Eigen::Vector3f(0.0f, 0.0f, MOVE_AMOUNT));
		App::chain.move(Vec3f({ 0.0f, 0.8f, 0.0f }));
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer) {
		while (1) {
			// App::scene.resolvePhysics();

			renderer->clearView();
			renderer->updateScene(&App::scene, &App::camera);
			renderer->renderScene(&App::scene);
			renderer->switchFramebuff();

			platform->handleEvents(true); // enables mouse events
		}
	}
}