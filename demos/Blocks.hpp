#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "Geo_Tree.hpp"
#include "primitives/Geo_Flat.hpp"
#include "primitives/Geo_Conic.hpp"
#include "primitives/Geo_Extruded.hpp"
#include "trees/Chain.hpp"
#include "trees/Grid.hpp"

#define MOVE_AMOUNT 0.5

namespace Topl {
	// Management Objects
	Topl_Scene scene;
	Topl_Camera camera;
	// Topl_Camera camera = Topl_Camera(PROJECTION_Ortho, SpatialBounds3D(1.0f));

	// Primitive Geometry Objects
	Geo_FlatSquare rect1 = Geo_FlatSquare(0.1f);
	Geo_ExtrudedHex hex1 = Geo_ExtrudedHex(0.1f, 0.1f);
	Geo_ConicSquare cone1 = Geo_ConicSquare(0.1f, Eigen::Vector3f(0.0f, 0.0f, 0.1f));
	// Complex Geometry Objects
	/* Geo_Actor chainGeo = Geo_Actor((const Geo_RenderObj*)&cone1);
	Geo_Chain_Properties chainActor = Geo_Chain_Properties(Eigen::Vector3f(0.33f, 0.0, 0.0)); // stretches along X axis only
	Geo_Chain chain("chain", &scene, &chainGeo, &chainActor, 4); */
	Geo_Actor gridGeo = Geo_Actor((const Geo_RenderObj*)&hex1);
	Geo_Grid_Properties gridActor = Geo_Grid_Properties(std::make_pair(2, 0.5f));
	Geo_Grid grid("grid", &scene, &gridGeo, &gridActor);
}

void buttonCallback_w(void) { Topl::camera.movePos(Eigen::Vector3f(0.0f, 0.0f, MOVE_AMOUNT)); } // Move forward
void buttonCallback_a(void) { Topl::camera.movePos(Eigen::Vector3f(-1.0f * MOVE_AMOUNT, 0.0f, 0.0)); } // Move left
void buttonCallback_s(void) { Topl::camera.movePos(Eigen::Vector3f(0.0f, 0.0f, -1.0f * MOVE_AMOUNT)); } // Move backwards
void buttonCallback_d(void) { Topl::camera.movePos(Eigen::Vector3f(MOVE_AMOUNT, 0.0f, 0.0f)); } // Move right

// Shared functions

namespace Main {
	void init(Platform* platform) {
		platform->createWindow();

		Platform::keyLogger.addCallback('w', buttonCallback_w);
		Platform::keyLogger.addCallback('a', buttonCallback_a);
		Platform::keyLogger.addCallback('s', buttonCallback_s);
		Platform::keyLogger.addCallback('d', buttonCallback_d);
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer) {
		Timer_Ticker gameTicker;

		while (1) {
			// Topl::chain.rotate(&Topl::scene, Eigen::Vector2f(-0.0001 * gameTicker.getAbsSecs(), 0.0));
			Topl::grid.rotate(&Topl::scene, Eigen::Vector2f(0.0, 0.0001 * gameTicker.getAbsSecs()));
			// Topl::scene.resolvePhysics();

			renderer->clearView();
			renderer->updateScene(&Topl::scene, &Topl::camera);
			renderer->renderScene(DRAW_Triangles);

			platform->handleEvents();
		}
	}
}