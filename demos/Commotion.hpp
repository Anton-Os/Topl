#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "Geo_Tree.hpp"
#include "primitives/Geo_Flat.hpp"

#define BIG_RADIUS 1.8

namespace App {
	// Management Objects
	Topl_Scene scene;
	Topl_Camera camera = Topl_Camera(PROJECTION_Ortho, SpatialBounds3D(3.0f));

	Geo_FlatCircle circle1 = Geo_FlatCircle(BIG_RADIUS, 0.0);
	Geo_FlatCircle circle2 = Geo_FlatCircle(BIG_RADIUS / 2, 0.0 + 0.01);
	Geo_FlatCircle circle3 = Geo_FlatCircle(BIG_RADIUS / 4, 0.0 + 0.02);
	Geo_FlatCircle circle4 = Geo_FlatCircle(BIG_RADIUS / 8, 0.0 + 0.03);
	Geo_FlatCircle circle5 = Geo_FlatCircle(BIG_RADIUS / 16, 0.0 + 0.04);
	Geo_FlatCircle circle6 = Geo_FlatCircle(BIG_RADIUS / 32, 0.0 + 0.05);

	Geo_Actor actors[6] = {
		Geo_Actor((const Geo_RenderObj*)&circle1),
		Geo_Actor((const Geo_RenderObj*)&circle2),
		Geo_Actor((const Geo_RenderObj*)&circle3),
		Geo_Actor((const Geo_RenderObj*)&circle4),
		Geo_Actor((const Geo_RenderObj*)&circle5),
		Geo_Actor((const Geo_RenderObj*)&circle6)
	};
}

// Shared functions

namespace Main {
	void init(Platform* platform) {
		platform->createWindow();
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer) {
		Timer_Ticker gameTicker;

		while (1) {
			renderer->clearView();
			renderer->updateScene(&App::scene);
			renderer->renderScene(&App::scene);

			platform->handleEvents();
		}
	}
}