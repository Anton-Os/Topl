#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "primitives/Geo_Flat.hpp"
#include "trees/Pane.hpp"

namespace App {
	Topl_Scene scene;
	Topl_Camera camera = Topl_Camera(); // identity matrix
    
	std::string assetsPath = ASSETS_DIR;
	std::string fontsPath = assetsPath + "fonts/";
	std::string font1 = fontsPath + "Tw-Cen-MT.ttf";
	std::string font2 = fontsPath + "MajorMonoDisplay-Regular.ttf";
	std::string font3 = fontsPath + "PoiretOne-Regular.ttf";
	std::string font4 = fontsPath + "NerkoOne-Regular.ttf";
	std::string text = "i";

	Geo_FlatSquare captureSquare = Geo_FlatSquare(0.25f);
	Geo_Actor captureSquareGeo = Geo_Actor((Geo_RenderObj*)&captureSquare); // used for capturing framebuffer
	Geo_FlatCircle pickerCircle = Geo_FlatCircle(0.25f);
	Geo_Actor pickerCircleGeo = Geo_Actor((Geo_RenderObj*)&pickerCircle); // used for picking out cursor color

	Geo_UnitLayout unitLayout("L1");
	Geo_RowLayout rowLayout("L2", 4, 0.3f, 0.03f);
	Geo_BoxedLayout boxedLayout("L3", 3, 0.3f, 0.03f);

	bool isPressPend;
#ifdef RASTERON_H
	Topl_Image pickerImage, captureImage, synthesisImage;
	Topl_Image textImage1, textImage2, textImage3, textImage4;

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
		App::isPressPend = true;
	puts("Key pressed");
}

void upCallback(float x, float y) {
	puts("Key released");
}

// Helper Functions

void setCaptureImage(Topl_Renderer* renderer) {
	App::captureImage.setImage(renderer->frame()); // screen capture code
	App::scene.addTexture("capture", App::captureImage.getImage());
	renderer->texturize(&App::scene);
}

void setPickerImage(unsigned color) {
	App::pickerImage.setColorImage(color);
	App::scene.addTexture("picker", App::pickerImage.getImage());
}

void genImages() {
	initFreeType(&App::freetypeLib);
	/* Topl_Image seedBaseImage = Topl_Image(0xFF111111);
	Topl_Image seededImage = createImgSeedRaw(seedBaseImage.getImage(), SEED_COLOR, 0.03);
	App::synthesisImage.setImage(createCellPatImg8(seededImage.getImage(), synthesisCallback)); */
	Rasteron_Image blankSlate = { "slate", 256, 256, NULL };
	Rasteron_GradientNoise noise1 = { 3, 3, BLACK_COLOR, RED_CHANNEL };
	Topl_Image noiseImage1 = Topl_Image(createGradientNoiseImg(&blankSlate, &noise1));
	Rasteron_GradientNoise noise2 = { 60, 6, BLACK_COLOR, GREEN_CHANNEL };
	Topl_Image noiseImage2 = Topl_Image(createGradientNoiseImg(&blankSlate, &noise2));
	Rasteron_GradientNoise noise3 = { 33, 31, BLACK_COLOR, BLUE_CHANNEL };
	Topl_Image noiseImage3 = Topl_Image(createGradientNoiseImg(&blankSlate, &noise3));
	App::synthesisImage.setImage(createImgFuse(noiseImage3.getImage(), noiseImage2.getImage()));
	// App::synthesisImage.setImage(createImgFuse(App::synthesisImage.getImage(), noiseImage1.getImage()));
	App::unitLayout.getChildPane(0)->selectImage(App::synthesisImage.getImage());

	Rasteron_ColorPointTable colorPointTable = { {}, 0 };
	addColorPoint(&colorPointTable, WHITE_COLOR, 0.5f, 0.5f);
	// colorPointTable.pixelPointCount = 0;
	for (unsigned short p = 0; p < App::boxedLayout.getRowCount() * App::boxedLayout.getColCount(); p++) {
		Geo_Pane* pane = App::boxedLayout.getChildPane(p);
		// Rasteron_Image* frameImg = App::windows.getFrameAt(p);

		Vec2f randVec; randVec.randomize();
		addColorPoint(&colorPointTable, genRandColor(), randVec[0], randVec[1]);
		Topl_Image layoutImage = Topl_Image(createImgProxim(App::windows.getFrameAt(p), &colorPointTable));
		App::windows.addFrame(layoutImage.getImage());
		pane->selectImage(App::windows.getFrameAt(p));
	}

	// TODO: Debug inside of Rasteron and compare!
	App::textObj.fileName = App::font1.c_str();
	App::textImage1.setTextImage(&App::freetypeLib, &App::textObj);
	App::textObj.fileName = App::font2.c_str();
	App::textImage2.setTextImage(&App::freetypeLib, &App::textObj);
	App::textObj.fileName = App::font3.c_str();
	App::textImage3.setTextImage(&App::freetypeLib, &App::textObj);
	App::textObj.fileName = App::font4.c_str();
	App::textImage4.setTextImage(&App::freetypeLib, &App::textObj);
	
	for (unsigned short p = 0; p < App::rowLayout.getRowCount() * App::rowLayout.getColCount(); p++) {
		Geo_Pane* pane = App::rowLayout.getChildPane(p);

		switch (p % 4) {
		case 0: pane->selectImage(App::textImage1.getImage()); break;
		case 1: pane->selectImage(App::textImage2.getImage()); break;
		case 2: pane->selectImage(App::textImage3.getImage()); break;
		case 3: pane->selectImage(App::textImage4.getImage()); break;
		}
	}
}

// Retrieves the pixel where the cursor is positioned
unsigned getPressPixel(Topl_Renderer* renderer) {
	float xPos = Platform::getCursorX();
	float yPos = Platform::getCursorY();

	unsigned pixel = genRandColorVal(); // random color
	if(xPos != BAD_CURSOR_POS && yPos != BAD_CURSOR_POS) // captures pixel at cursor position
		pixel = renderer->getPixelAt(Platform::getCursorX(), Platform::getCursorY());
#ifdef RASTERON_H
	App::rowLayout.interact(xPos, yPos, pixel);
	App::boxedLayout.interact(xPos, yPos, pixel);
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

		App::unitLayout.move(Vec3f({ 0.0f, 0.75f, 0.0f }));
		App::rowLayout.move(Vec3f({ 0.75f, 0.0f, 0.0f }));

		App::pickerCircleGeo.setPos(Vec3f({ 0.0f, -0.75f, 0.0f }));
		App::scene.addGeometry("picker", &App::pickerCircleGeo);
		App::captureSquareGeo.setPos(Vec3f({ -0.75f, 0.0f, 0.0f }));
		App::scene.addGeometry("capture", &App::captureSquareGeo);

		genImages();
		App::unitLayout.configure(&App::scene);
		App::rowLayout.configure(&App::scene);
		App::boxedLayout.configure(&App::scene);
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer) {
		while (1) {
			renderer->clearView();
			renderer->updateScene(&App::scene);
			renderer->renderScene(&App::scene);

			if (App::captureImage.getImage() == nullptr) setCaptureImage(renderer); // set initial value
			if (App::pickerImage.getImage() == nullptr) setPickerImage(genRandColor()); // set initial value

			if (App::isPressPend) {
				unsigned pixel = getPressPixel(renderer);
				// setPickerImage(pixel); // check for selection and random results
				renderer->texturize(&App::scene);
				// setCaptureImage(renderer); // check for framebuffer results
				App::isPressPend = false; // mouse callback has been handled
			}

			renderer->switchFramebuff();
			platform->handleEvents(true);
		}
	}
}