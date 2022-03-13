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
	std::string text = "O";

	Geo_FlatSquare captureSquare = Geo_FlatSquare(0.25f);
	Geo_Actor captureSquareGeo = Geo_Actor((Geo_RenderObj*)&captureSquare); // used for capturing framebuffer
	Geo_FlatCircle pickerCircle = Geo_FlatCircle(0.25f);
	Geo_Actor pickerCircleGeo = Geo_Actor((Geo_RenderObj*)&pickerCircle); // used for picking out cursor color

	Geo_UnitLayout unitLayout("L1");
	Geo_RowLayout rowLayout("L2", 4, 0.25f, 0.03f);
	Geo_BoxedLayout boxedLayout("L3", 3, 0.3f, 0.1f);

	bool isPressPend;
#ifdef RASTERON_H
	Topl_Image pickerBk;
	Topl_Image captureBk;
	Topl_Image textDisplayBk;

	Rasteron_FormatText textObj = { font1.c_str(), text.c_str(), 0xFF000000, 0xFFFFFFFF };

	Topl_Frames windows = Topl_Frames("windows", 256, 256, 9); // used for boxedLayout
	Topl_Frames texting = Topl_Frames("texting", 256, 256, 9); // used for unitLayout
#endif
}

void downCallback(float x, float y) {
	if(Platform::getCursorX() != BAD_CURSOR_POS && Platform::getCursorY() != BAD_CURSOR_POS)
		Topl::isPressPend = true;
	puts("Key pressed");
}

void upCallback(float x, float y) { puts("Key released"); } // for testing

void setCaptureBk(Topl_Renderer* renderer) {
	Topl::captureBk.setImage(renderer->frame()); // attempt to capture screen
	Topl::scene.addTexture("capture", Topl::captureBk.getImage());
	renderer->texturize(&Topl::scene);
}

void setPickerBk() {
	Topl::pickerBk.setImage(createImgBlank(255, 255, 0xFFFF00FF));
	Topl::scene.addTexture("picker", Topl::pickerBk.getImage());
}

void genImages() {
	Topl::textDisplayBk.setTextImage(&Topl::textObj, 100);

	Rasteron_ColorPointTable colorPointTable = { {}, 0 };
	addColorPoint(&colorPointTable, 0xFFFFFF00, 0.5f, 0.5f); // origin point
	// colorPointTable.pixelPointCount = 0;
	for (unsigned short p = 0; p < Topl::boxedLayout.getRowCount() * Topl::boxedLayout.getColCount(); p++) {
		Geo_Pane* pane = Topl::boxedLayout.getChildPane(p);
		// Rasteron_Image* frameImg = Topl::windows.getFrameAt(p);

		addColorPoint(&colorPointTable, genRandColor(), genRandFloat(), genRandFloat());
		// Rasteron_Image* tempImage = genImgProxim(Topl::windows.getFrameAt(p), &colorPointTable);
		// Topl::windows.addFrame(tempImage);
		pane->selectBk(Topl::windows.getFrameAt(p));
		// deleteImg(tempImage);
	}
	
	for (unsigned short p = 0; p < Topl::rowLayout.getRowCount() * Topl::rowLayout.getColCount(); p++) {
		Geo_Pane* pane = Topl::rowLayout.getChildPane(p);

		pane->selectBk(Topl::textDisplayBk.getImage());
	}
}

// Retrieves the pixel where the cursor is positioned
unsigned getPressPixel(Topl_Renderer* renderer) {
	unsigned pixel = genRandColorVal(); // random color
	if(Platform::getCursorX() != BAD_CURSOR_POS && Platform::getCursorY() != BAD_CURSOR_POS) // captures pixel at cursor position
		pixel = renderer->getPixColor(Platform::getCursorX(), Platform::getCursorY());
#ifdef RASTERON_H
	// if (Topl::pickerBk != nullptr) deleteImg(Topl::pickerBk); // deletes previous image
	// Topl::pickerBk = createImgBlank(255, 255, pixel);
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

		Topl::pickerCircleGeo.setPos(Eigen::Vector3f(0.0f, -0.75f, 0.0f));
		Topl::scene.addGeometry("picker", &Topl::pickerCircleGeo);
		Topl::captureSquareGeo.setPos(Eigen::Vector3f(-0.75f, 0.0f, 0.0f));
		Topl::scene.addGeometry("capture", &Topl::captureSquareGeo);

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

			if (Topl::pickerBk.getImage() == nullptr)
				setPickerBk();
			if (Topl::captureBk.getImage() == nullptr) 
				setCaptureBk(renderer);

			if (Topl::isPressPend) {
				unsigned pixel = getPressPixel(renderer);
				renderer->texturize(&Topl::scene);
				Topl::isPressPend = false; // mouse callback has been handled
			}

			renderer->switchFramebuff();
			platform->handleEvents(true);
		}
	}
}