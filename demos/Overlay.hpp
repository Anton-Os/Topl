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

	bool isPressPend;
#ifdef RASTERON_H
	Rasteron_Image* pickerBk = nullptr;
	Rasteron_Image* captureBk = nullptr;
#endif
}

void downCallback(float x, float y) {
	if(Platform::getCursorX() != BAD_CURSOR_POS && Platform::getCursorY() != BAD_CURSOR_POS)
		Topl::isPressPend = true;
	puts("Key pressed");
}

void upCallback(float x, float y) { puts("Key released"); } // for testing

// Attempts to retrieve the pixel where the cursor is
unsigned getPressPixel(Topl_Renderer* renderer) {
	unsigned pixel;
	if(Platform::getCursorX() != BAD_CURSOR_POS && Platform::getCursorY() != BAD_CURSOR_POS) // captures pixel at cursor position
		pixel = renderer->getPixColor(Platform::getCursorX(), Platform::getCursorY());
#ifdef RASTERON_H
	if (Topl::pickerBk == nullptr) Topl::scene.addTexture("picker", createImgBlank(255, 255, pixel)); // picks out the color
#endif
	return pixel;
}

namespace Main {
    void init(Platform* platform) {
		platform->createWindow();

		Platform::mouseLogger.addCallback(MOUSE_LeftBtn_Down, downCallback);
		Platform::mouseLogger.addCallback(MOUSE_RightBtn_Down, downCallback);
		Platform::mouseLogger.addCallback(MOUSE_LeftBtn_Up, upCallback);
		Platform::mouseLogger.addCallback(MOUSE_RightBtn_Up, upCallback);

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
			renderer->renderAll();

			if (Topl::isPressPend) {
				unsigned pixel = getPressPixel(renderer);
				Topl::isPressPend = false; // mouse callback has been handled
			}

			if (Topl::captureBk == nullptr) {
				Topl::captureBk = renderer->frame(); // attempt to capture screen
				Topl::scene.addTexture("capture", Topl::captureBk);
			}

			renderer->switchFramebuff();
			platform->handleEvents(true);
		}
	}

	void cleanup() {
		if(Topl::pickerBk != nullptr) deleteImg(Topl::pickerBk);
		if(Topl::captureBk != nullptr) deleteImg(Topl::captureBk);
	}
}