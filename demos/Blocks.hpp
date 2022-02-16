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

namespace Topl {
	// Management Objects
	Topl_Scene scene;
	// Topl_Camera camera;
	Topl_Camera camera = Topl_Camera(PROJECTION_Ortho, SpatialBounds3D(1.0f));

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
	// Geo_Grid grid("grid", &scene, &gridGeo, &gridActor);

	float allRange[2] = { -1.0f, 1.0f }; // encompass entire screen area
	float posRange[2] = { 0.9f, 1.0f }; // encompass positive-side screen segment
	float negRange[2] = { -0.9f, -1.0f }; // encompass negative-side screen segment
	Input_CursorRange topRange = Input_CursorRange(allRange, posRange);
	Input_CursorRange rightRange = Input_CursorRange(posRange, allRange);
	Input_CursorRange leftRange = Input_CursorRange(negRange, allRange);
	Input_CursorRange botRange = Input_CursorRange(allRange, negRange);
}

// Modify camera look position inside of hover callback
void hoverCallback_top(float x, float y) { puts("Inside Top Zone!"); }
void hoverCallback_right(float x, float y) { puts("Inside Right Zone!"); }
void hoverCallback_left(float x, float y) { puts("Inside Left Zone!"); }
void hoverCallback_bot(float x, float y) { puts("Inside Bottom Zone!"); }

void buttonCallback_w(void) { Topl::camera.movePos(Eigen::Vector3f(0.0f, 0.0f, MOVE_AMOUNT)); } // Move forward
void buttonCallback_a(void) { Topl::camera.movePos(Eigen::Vector3f(-1.0f * MOVE_AMOUNT, 0.0f, 0.0)); } // Move left
void buttonCallback_s(void) { Topl::camera.movePos(Eigen::Vector3f(0.0f, 0.0f, -1.0f * MOVE_AMOUNT)); } // Move backwards
void buttonCallback_d(void) { Topl::camera.movePos(Eigen::Vector3f(MOVE_AMOUNT, 0.0f, 0.0f)); } // Move right

// Shared functions

namespace Main {
	void init(Platform* platform) {
		platform->createWindow();

		Platform::mouseLogger.addHoverCallback(&Topl::topRange, hoverCallback_top);
		Platform::mouseLogger.addHoverCallback(&Topl::rightRange, hoverCallback_right);
		Platform::mouseLogger.addHoverCallback(&Topl::leftRange, hoverCallback_left);
		Platform::mouseLogger.addHoverCallback(&Topl::botRange, hoverCallback_bot);
		Platform::keyLogger.addCallback('w', buttonCallback_w);
		Platform::keyLogger.addCallback('a', buttonCallback_a);
		Platform::keyLogger.addCallback('s', buttonCallback_s);
		Platform::keyLogger.addCallback('d', buttonCallback_d);

		// Topl::camera.setPos(Eigen::Vector3f(0.0f, 0.0f, MOVE_AMOUNT));
		Topl::chain.move(Eigen::Vector3f(0.0f, 0.8f, 0.0f));
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer) {
		Timer_Ticker gameTicker;

		while (1) {
			// Topl::scene.resolvePhysics();

			renderer->clearView();
			renderer->updateScene(&Topl::scene, &Topl::camera);
			renderer->renderAll();
			renderer->switchFramebuff();

			platform->handleEvents(true); // enables mouse events
		}
	}
}