#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "primitives/Geo_Flat.hpp"
#include "trees/Pane.hpp"

namespace Topl {
	Topl_Scene scene;
	Topl_Camera camera = Topl_Camera(); // identity matrix
    
	std::string assetsPath = ASSETS_DIR;
	std::string fontsPath = assetsPath + "fonts/";
	std::string font1 = fontsPath + "CutiveMono-Regular.ttf";
	std::string font2 = fontsPath + "NerkoOne-Regular.ttf";
	std::string font3 = fontsPath + "PoiretOne-Regular.ttf";
	std::string text = "Hello World";

	Geo_FlatSquare captureSquare = Geo_FlatSquare(0.25f);
	Geo_Actor captureSquareGeo = Geo_Actor((Geo_RenderObj*)&captureSquare); // used for capturing framebuffer
	Geo_FlatCircle pickerCircle = Geo_FlatCircle(0.25f);
	Geo_Actor pickerCircleGeo = Geo_Actor((Geo_RenderObj*)&pickerCircle); // used for picking out cursor color

	Geo_PaneLayout boxedLayout("layout1", &scene, 3, 3);
	Geo_PaneLayout scanLayout("layout2", &scene, 12, 1, 0.25f, 0.02f);

	ValueGen valueGen = ValueGen(); // seeds random number generation
	bool isPressPend;
#ifdef RASTERON_H
	Rasteron_Image* pickerBk = nullptr;
	Rasteron_Image* captureBk = nullptr;
	Rasteron_Image* textDisplayBk = nullptr;

	Rasteron_FormatText textObj = { font1.c_str(), text.c_str(), 0xFF000000, 0xFFFFFFFF };
	FT_Library freetypeLib; // required for loading glyphs

	Topl_Frames symbols = Topl_Frames("symbols", 256, 256, 9); // used as textures for boxedLayout
#endif
}

void downCallback(float x, float y) {
	if(Platform::getCursorX() != BAD_CURSOR_POS && Platform::getCursorY() != BAD_CURSOR_POS)
		Topl::isPressPend = true;
	puts("Key pressed");
}

void upCallback(float x, float y) { puts("Key released"); } // for testing

void captureBk(Topl_Renderer* renderer) {
	Topl::captureBk = renderer->frame(); // attempt to capture screen
	Topl::scene.addTexture("capture", Topl::captureBk);
	renderer->texturize(&Topl::scene);
}

void setupPanes() {
	initFreeType(&Topl::freetypeLib);
	Topl::textDisplayBk = bakeImgText(&Topl::textObj, &Topl::freetypeLib, 20);

	for (unsigned short p = 0; p < Topl::boxedLayout.getRowCount() * Topl::boxedLayout.getColCount(); p++) {
		Geo_Pane* pane = Topl::boxedLayout.getChildPane(p);
		Rasteron_Image* frameImg = Topl::symbols.getFrameAt(p);

		// Create Image Here
	}
	
	for (unsigned short p = 0; p < Topl::scanLayout.getRowCount() * Topl::scanLayout.getColCount(); p++) {
		Geo_Pane* pane = Topl::scanLayout.getChildPane(p);

		// Create Image Here
	}

}

// Retrieves the pixel where the cursor is positioned
unsigned getPressPixel(Topl_Renderer* renderer) {
	unsigned pixel = genRandColorVal(); // yellow color by default
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

		Topl::scanLayout.move(Eigen::Vector3f(0.75f, 0.0f, 0.0f));
		Topl::captureSquareGeo.setPos(Eigen::Vector3f(-0.75f, 0.0f, 0.0f));
		Topl::scene.addGeometry("capture", &Topl::captureSquareGeo);
		Topl::pickerCircleGeo.setPos(Eigen::Vector3f(0.0f, -0.75f, 0.0f));
		Topl::scene.addGeometry("picker", &Topl::pickerCircleGeo);

		setupPanes();
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer) {
		while (1) {
			renderer->clearView();
			renderer->updateScene(&Topl::scene);
			renderer->renderAll();

			if (Topl::isPressPend) {
				unsigned pixel = getPressPixel(renderer);
				renderer->texturize(&Topl::scene);
				Topl::isPressPend = false; // mouse callback has been handled
			}

			if (Topl::captureBk == nullptr) 
				captureBk(renderer);

			renderer->switchFramebuff();
			platform->handleEvents(true);
		}
	}

	void cleanup() {
		if(Topl::pickerBk != nullptr) deleteImg(Topl::pickerBk);
		if(Topl::captureBk != nullptr) deleteImg(Topl::captureBk);
		if(Topl::textDisplayBk != nullptr) deleteImg(Topl::textDisplayBk);
		cleanupFreeType(&Topl::freetypeLib);
	}
}