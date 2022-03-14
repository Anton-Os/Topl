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
	std::string font2 = fontsPath + "MajorMonoDisplay-Regular.ttf";
	std::string font3 = fontsPath + "PoiretOne-Regular.ttf";
	std::string font4 = fontsPath + "NerkoOne-Regular.ttf";
	std::string text = "o";

	Geo_FlatSquare captureSquare = Geo_FlatSquare(0.25f);
	Geo_Actor captureSquareGeo = Geo_Actor((Geo_RenderObj*)&captureSquare); // used for capturing framebuffer
	Geo_FlatCircle pickerCircle = Geo_FlatCircle(0.25f);
	Geo_Actor pickerCircleGeo = Geo_Actor((Geo_RenderObj*)&pickerCircle); // used for picking out cursor color

	Geo_UnitLayout unitLayout("L1");
	Geo_RowLayout rowLayout("L2", 4, 0.3f, 0.03f);
	Geo_BoxedLayout boxedLayout("L3", 3, 0.3f, 0.03f);

	bool isPressPend;
#ifdef RASTERON_H
	Topl_Image pickerBk, captureBk, synthesisBk;
	Topl_Image textBk1, textBk2, textBk3, textBk4;

	FT_Library freetypeLib;
	Rasteron_FormatText textObj = { "", text.c_str(), WHITE_COLOR, BLACK_COLOR };

	Topl_Frames windows = Topl_Frames("windows", 256, 256, 9); // used for boxedLayout
	Topl_Frames texting = Topl_Frames("texting", 256, 256, 9); // used for unitLayout
#endif
}

// Callbacks

/* unsigned synthesisCallback(unsigned br, unsigned b, unsigned bl, unsigned r, unsigned l, unsigned tr, unsigned t, unsigned tl) {
	if (b == SEED_COLOR) return 0xFFFF0000;
	else if (r == SEED_COLOR || l == SEED_COLOR) return 0xFF00FFF00;
	else if (t == SEED_COLOR) return 0xFF0000FF;
	else return ZERO_COLOR;
} */

void downCallback(float x, float y) {
	if (Platform::getCursorX() != BAD_CURSOR_POS && Platform::getCursorY() != BAD_CURSOR_POS)
		Topl::isPressPend = true;
	puts("Key pressed");
}

void upCallback(float x, float y) {
	puts("Key released");
}

// Helper Functions

void setCaptureBk(Topl_Renderer* renderer) {
	Topl::captureBk.setImage(renderer->frame()); // screen capture code
	Topl::scene.addTexture("capture", Topl::captureBk.getImage());
	renderer->texturize(&Topl::scene);
}

void setPickerBk() {
	Topl::pickerBk.setImage(createImgBlank(255, 255, 0xFFFF00FF));
	Topl::scene.addTexture("picker", Topl::pickerBk.getImage());
}

void genImages() {
	initFreeType(&Topl::freetypeLib);
	/* Topl_Image seedBaseImage = Topl_Image(0xFF111111);
	Topl_Image seededImage = createImgSeedRaw(seedBaseImage.getImage(), SEED_COLOR, 0.03);
	Topl::synthesisBk.setImage(createCellPatImg8(seededImage.getImage(), synthesisCallback)); */
	Rasteron_Image blankSlate = { "slate", 256, 256, NULL };
	Rasteron_GradientNoise noise1 = { 3, 3, BLACK_COLOR, RED_CHANNEL };
	Topl_Image noiseImage1 = Topl_Image(createGradientNoiseImg(&blankSlate, &noise1));
	Rasteron_GradientNoise noise2 = { 60, 6, BLACK_COLOR, GREEN_CHANNEL };
	Topl_Image noiseImage2 = Topl_Image(createGradientNoiseImg(&blankSlate, &noise2));
	Rasteron_GradientNoise noise3 = { 33, 31, BLACK_COLOR, BLUE_CHANNEL };
	Topl_Image noiseImage3 = Topl_Image(createGradientNoiseImg(&blankSlate, &noise3));
	Topl::synthesisBk.setImage(createImgFuse(noiseImage3.getImage(), noiseImage2.getImage()));
	Topl::synthesisBk.setImage(createImgFuse(Topl::synthesisBk.getImage(), noiseImage1.getImage()));
	Topl::unitLayout.getChildPane(0)->selectBk(Topl::synthesisBk.getImage());

	Rasteron_ColorPointTable colorPointTable = { {}, 0 };
	addColorPoint(&colorPointTable, WHITE_COLOR, 0.5f, 0.5f);
	// colorPointTable.pixelPointCount = 0;
	for (unsigned short p = 0; p < Topl::boxedLayout.getRowCount() * Topl::boxedLayout.getColCount(); p++) {
		Geo_Pane* pane = Topl::boxedLayout.getChildPane(p);
		// Rasteron_Image* frameImg = Topl::windows.getFrameAt(p);

		addColorPoint(&colorPointTable, genRandColor(), genRandFloat(), genRandFloat());
		Topl_Image layoutImage = Topl_Image(createImgProxim(Topl::windows.getFrameAt(p), &colorPointTable));
		Topl::windows.addFrame(layoutImage.getImage());
		pane->selectBk(Topl::windows.getFrameAt(p));
	}

	Topl::textObj.fileName = Topl::font1.c_str();
	Topl::textBk1.setTextImage(&Topl::freetypeLib, &Topl::textObj);
	Topl::textObj.fileName = Topl::font2.c_str();
	Topl::textBk2.setTextImage(&Topl::freetypeLib, &Topl::textObj);
	Topl::textObj.fileName = Topl::font3.c_str();
	Topl::textBk3.setTextImage(&Topl::freetypeLib, &Topl::textObj);
	Topl::textObj.fileName = Topl::font4.c_str();
	Topl::textBk4.setTextImage(&Topl::freetypeLib, &Topl::textObj);
	
	for (unsigned short p = 0; p < Topl::rowLayout.getRowCount() * Topl::rowLayout.getColCount(); p++) {
		Geo_Pane* pane = Topl::rowLayout.getChildPane(p);

		switch (p % 4) {
		case 0: pane->selectBk(Topl::textBk1.getImage()); break;
		case 1: pane->selectBk(Topl::textBk2.getImage()); break;
		case 2: pane->selectBk(Topl::textBk3.getImage()); break;
		case 3: pane->selectBk(Topl::textBk4.getImage()); break;
		}
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

// Main Operations

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