#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "Geo_Construct.hpp"
#include "primitives/Geo_Flat.hpp"
#include "primitives/Geo_Conic.hpp"
#include "primitives/Geo_Extruded.hpp"
#include "constructs/Chain.hpp"
#include "constructs/Grid.hpp"

#define MOVE_AMOUNT 0.5

namespace Topl {
	// Management Objects
	Topl_Scene scene;

	// Primitive Geometry Objects
	Geo_FlatHex hex1 = Geo_FlatHex(0.1f);
	Geo_ExtrudedSquare rect1 = Geo_ExtrudedSquare(0.1f, 0.4f);
	Geo_ConicSquare cone1 = Geo_ConicSquare(0.1f, Eigen::Vector3f(0.0f, 0.0f, 0.2f));
	// Complex Geometry Objects
	Geo_Component chainGeo = Geo_Component((const Geo_RenderObj*)&cone1);
	Geo_Chain_Properties chainProps = Geo_Chain_Properties(Eigen::Vector3f(0.33f, 0.0, 0.0)); // stretches along X axis only
	Geo_Chain chain("chain", &scene, &chainGeo, &chainProps, 4);
	Geo_Component gridGeo = Geo_Component((const Geo_RenderObj*)&rect1);
	Geo_Grid_Properties gridProps = Geo_Grid_Properties(std::make_pair(1, 0.5f));
	Geo_Grid grid("grid", &scene, &gridGeo, &gridProps);
}

void buttonCallback_w(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(0.0f, 0.0f, MOVE_AMOUNT)); } // Move forward
void buttonCallback_a(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(-1.0f * MOVE_AMOUNT, 0.0f, 0.0)); } // Move left
void buttonCallback_s(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(0.0f, 0.0f, -1.0f * MOVE_AMOUNT)); } // Move backwards
void buttonCallback_d(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(MOVE_AMOUNT, 0.0f, 0.0f)); } // Move right

// Shared functions

namespace Main {
	void init(Platform* platform) {
		platform->createWindow("Simple Shapes");

		Platform::keyLogger.addCallback('w', buttonCallback_w);
		Platform::keyLogger.addCallback('a', buttonCallback_a);
		Platform::keyLogger.addCallback('s', buttonCallback_s);
		Platform::keyLogger.addCallback('d', buttonCallback_d);

		Topl::scene.setCamera(PROJECTION_Ortho, SpatialBounds3D(3.0f));
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer) {
		Timer_Ticker gameTicker;

		while (1) {
			Topl::chain.rotate(&Topl::scene, Eigen::Vector2f(-0.0001 * gameTicker.getAbsSecs(), 0.0));
			Topl::grid.rotate(&Topl::scene, Eigen::Vector2f(0.0, 0.0001 * gameTicker.getAbsSecs()));
			// Topl::scene.resolvePhysics();

			renderer->clearView();
			renderer->updateScene(&Topl::scene);
			renderer->renderScene(DRAW_Triangles);

			platform->handleEvents();
		}
	}
}