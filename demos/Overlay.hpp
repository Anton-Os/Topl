#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "primitives/Geo_Flat.hpp"
#include "trees/Pane.hpp"

namespace Topl {
	Topl_Scene scene;
	Topl_Camera camera = Topl_Camera(); // identity matrix
    std::string assetsPath = ASSETS_DIR;
	std::string imagesSubPath = "images/";
	ValueGen valueGen = ValueGen(); // seeds random number generation

	Geo_FlatSquare captureSquare = Geo_FlatSquare(0.25f);
	Geo_Actor captureSquareGeo = Geo_Actor((Geo_RenderObj*)&captureSquare); // used for capturing framebuffer
	Geo_FlatCircle pickerCircle = Geo_FlatCircle(0.25f);
	Geo_Actor pickerCircleGeo = Geo_Actor((Geo_RenderObj*)&pickerCircle); // used for picking out cursor color

	Geo_PaneLayout defaultLayout("layout1", &scene, 3, 3);
	Geo_PaneLayout customLayout("layout2", &scene, 12, 1, 0.25f, 0.02f);
}

// Shared functions

namespace Main {
    void init(Platform* platform) {
		platform->createWindow();

		Topl::customLayout.move(&Topl::scene, Eigen::Vector3f(0.75f, 0.0f, 0.0f));
		Topl::captureSquareGeo.setPos(Eigen::Vector3f(-0.75f, 0.0f, 0.0f));
		Topl::scene.addGeometry("capture", &Topl::captureSquareGeo);
		Topl::pickerCircleGeo.setPos(Eigen::Vector3f(0.0f, -0.75f, 0.0f));
		Topl::scene.addGeometry("picker", &Topl::pickerCircleGeo);
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer) {

		while (1) {
			renderer->clearView();
			renderer->updateScene(&Topl::scene);
			renderer->renderScene(DRAW_Triangles);
			// if(Platform::getIsCursorInClient())
			float x = Platform::getCursorX(); float y = Platform::getCursorY();
			if(x != BAD_CURSOR_POS && y != BAD_CURSOR_POS)
				unsigned pixel = renderer->getPixColor(Platform::getCursorX(), Platform::getCursorY()); // for testing
			renderer->switchFramebuff();

			platform->handleEvents(true);
		}
	}
}