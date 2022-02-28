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

	Geo_UnitLayout unitLayout("L1");
	Geo_RowLayout rowLayout("L2", 8, 0.25f, 0.03f);
	Geo_BoxedLayout boxedLayout("L3", 8, 0.3f, 0.1f);

	// ValueGen valueGen = ValueGen(); // seeds random number generation
	bool isPressPend;
#ifdef RASTERON_H
	Rasteron_Image* pickerBk = nullptr;
	Rasteron_Image* captureBk = nullptr;
	Rasteron_Image* textDisplayBk = nullptr;

	Rasteron_FormatText textObj = { font1.c_str(), text.c_str(), 0xFF000000, 0xFFFFFFFF };
	FT_Library freetypeLib; // required for loading glyphs

	Topl_Frames symbols = Topl_Frames("symbols", 256, 256, 9); // used for boxedLayout
	Topl_Frames sequence = Topl_Frames("sequence", 256, 256, 9); // used for unitLayout
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

void genImages() {
	initFreeType(&Topl::freetypeLib);
	Topl::textDisplayBk = bakeImgText(&Topl::textObj, &Topl::freetypeLib, 20);

	for (unsigned short p = 0; p < Topl::boxedLayout.getRowCount() * Topl::boxedLayout.getColCount(); p++) {
		Geo_Pane* pane = Topl::boxedLayout.getChildPane(p);
		// Rasteron_Image* frameImg = Topl::symbols.getFrameAt(p);

		// pane->selectBk(frameImg);
	}
	
	for (unsigned short p = 0; p < Topl::rowLayout.getRowCount() * Topl::rowLayout.getColCount(); p++) {
		Geo_Pane* pane = Topl::rowLayout.getChildPane(p);

		// pane->selectBk(Topl::textDisplayBk);
	}
}

// Retrieves the pixel where the cursor is positioned
unsigned getPressPixel(Topl_Renderer* renderer) {
	unsigned pixel = genRandColorVal(); // yellow color by default
	if(Platform::getCursorX() != BAD_CURSOR_POS && Platform::getCursorY() != BAD_CURSOR_POS) // captures pixel at cursor position
		pixel = renderer->getPixColor(Platform::getCursorX(), Platform::getCursorY());
#ifdef RASTERON_H
	if (Topl::pickerBk != nullptr) deleteImg(Topl::pickerBk); // deletes previous image
	Topl::pickerBk = createImgBlank(255, 255, pixel);
	// Topl::scene.addTexture("picker", Topl::pickerBk);
#endif
	return pixel;
}

namespace Main {
    void init(Platform* platform) {
		srand(time(NULL)); // seed random number generator
		platform->createWindow();

		Platform::mouseLogger.addCallback(MOUSE_LeftBtn_Down, downCallback);
		Platform::mouseLogger.addCallback(MOUSE_RightBtn_Down, downCallback);
		Platform::mouseLogger.addCallback(MOUSE_LeftBtn_Up, upCallback);
		Platform::mouseLogger.addCallback(MOUSE_RightBtn_Up, upCallback);

		Topl::unitLayout.move(Eigen::Vector3f(0.0f, 0.75f, 0.0f));
		Topl::rowLayout.move(Eigen::Vector3f(0.75f, 0.0f, 0.0f));

		Topl::captureSquareGeo.setPos(Eigen::Vector3f(-0.75f, 0.0f, 0.0f));
		Topl::scene.addGeometry("capture", &Topl::captureSquareGeo);
		Topl::pickerCircleGeo.setPos(Eigen::Vector3f(0.0f, -0.75f, 0.0f));
		Topl::scene.addGeometry("picker", &Topl::pickerCircleGeo);

		genImages();
		Topl::unitLayout.init(&Topl::scene);
		Topl::rowLayout.init(&Topl::scene);
		Topl::boxedLayout.init(&Topl::scene);
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer) {
		while (1) {
			renderer->clearView();
			renderer->updateScene(&Topl::scene);
			renderer->renderScene(&Topl::scene);

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