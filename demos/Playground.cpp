#include "Playground.hpp"

#define APP_BACKEND APP_OpenGL_4
// #define APP_BACKEND APP_DirectX_11
// #define APP_BACKEND App_Vulkan

#define VIEW_SPACE 2.0f
#define CAMERA_LOOK Vec3f({ 0.0f, 0.0f, 2.0f })

#define PRE_FRAME_FRAC 5
#define POST_FRAME_FRAC 20

Topl_Camera Playground_App::camera1 = Topl_Camera();
Topl_Camera Playground_App::camera2 = Topl_Camera(PROJECTION_Ortho, VIEW_SPACE);
Topl_Camera Playground_App::camera3 = Topl_Camera(PROJECTION_Perspective, VIEW_SPACE);

Topl_Light Playground_App::skyLight = Topl_Light({ 0.0, -1.0f, 0.0 });
Topl_Light Playground_App::flashLight = Topl_Light({ 0.0, 0.0f, 1.0 });
Topl_Light Playground_App::lampLight = Topl_Light({ -1.0, 0.0f, 0.0 });

static unsigned hoverColor = ZERO_COLOR;
static Vec3f displaceVec = { 0.0f, 0.0f, 0.0f };
static Vec3f pawnVec = { 0.0f, 0.0f, 0.0f };

static void callback_press(float x, float y) {
	if (hoverColor == (CLEAR_COLOR_CODE & 0x00FFFFFF)) puts("\nBackground pressed!");
	else puts("\nObject pressed!");
}

static void callback_release(float x, float y) { 
	printf("Displacement is %6.4lf, %6.4lf ", x, y);
	displaceVec = { x, y, 0.0f }; 
}

static void callback_pane(unsigned short num) { printf("pane %d callback_pressed!", num); }

unsigned drawCursor_callback(double xFrac, double yFrac) {
	float cursorX = Platform::getCursorX();
	float cursorY = Platform::getCursorY();

	if (cursorX - 0.5f == xFrac); // cursor and image offset match!
	else if (cursorY - 0.5f == yFrac); // cursor and image offset match!
	return ZERO_COLOR;
}

static void event_character_swap(Topl_Renderer* renderer, Topl_Scene* scene) {
	static unsigned swapCount = 0;

	if (swapCount % 100 == 33) // ghost swap
		for (unsigned p = 0; p < HUMANOID_PARTS_COUNT; p++)
			scene->addTexture(character.getPrefix() + bodyPartsStr[p], ghostImages[p].getImage());
	else if (swapCount % 100 == 66) // angel swap
		for (unsigned p = 0; p < HUMANOID_PARTS_COUNT; p++)
			scene->addTexture(character.getPrefix() + bodyPartsStr[p], angelImages[p].getImage());
	else if (swapCount % 100 == 99) // demon swap
		for (unsigned p = 0; p < HUMANOID_PARTS_COUNT; p++)
			scene->addTexture(character.getPrefix() + bodyPartsStr[p], devilImages[p].getImage());

	if (swapCount % 100 == 33 || swapCount % 100 == 66 || swapCount % 100 == 99)
		renderer->texturize(scene);

	swapCount++;
}

void Playground_App::createPipeline() {
	if (APP_BACKEND == APP_OpenGL_4) {
		vertexShader1 = GL4_Textured_VertexShader(); fragShader1 = GL4_Textured_FragmentShader();
		vertexShader2 = GL4_Flat_VertexShader(FLAT_MODE_ALTERNATE); fragShader2 = GL4_Flat_FragmentShader();
		vertexShader3 = GL4_Beams_VertexShader(); fragShader3 = GL4_Beams_FragmentShader();
	}
	else if (APP_BACKEND == APP_DirectX_11) {
		vertexShader1 = Drx11_Textured_VertexShader(); fragShader1 = Drx11_Textured_FragmentShader();
		vertexShader2 = Drx11_Flat_VertexShader(FLAT_MODE_ALTERNATE); fragShader2 = Drx11_Flat_FragmentShader();
		vertexShader3 = Drx11_Beams_VertexShader(); fragShader3 = Drx11_Beams_FragmentShader();
	}

	texPipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader1, &fragShader1);
	colPipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader2, &fragShader2);
	litPipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader3, &fragShader3);
}

void Playground_App::createScene_Main() {
	for (unsigned p = 0; p < HUMANOID_PARTS_COUNT; p++) {
		ghostImages[p].setFileImage(ghostAssets[p]);
		angelImages[p].setFileImage(angelAssets[p]);
		devilImages[p].setFileImage(devilAssets[p]);
	}

	character.configure(&scene_main);
	for (unsigned p = 0; p < HUMANOID_PARTS_COUNT; p++) // initial texture assignment
		scene_main.addTexture(character.getPrefix() + bodyPartsStr[p], yellowImg.getImage());

	Topl_Factory::switchPipeline(APP_BACKEND, _renderer, texPipeline); // switch to correct pipeline
	_renderer->buildScene(&scene_main); 
}

void Playground_App::createScene_Overlay() {
	rowLayout.move(Vec3f({ 0.7f, 0.7f, 0.0f }));
	for (unsigned short p = 0; p < rowLayout.getRowCount(); p++) {
		Rasteron_Image* rowPaneImg = createSolidImg({ PANE_IMAGE_HEIGHT, PANE_IMAGE_WIDTH }, BLACK_COLOR + (0x08 * p));
		rowLayout.getChildPane(p)->selectImage(rowPaneImg);
	}
	rowLayout.configure(&scene_overlay);

	boxedLayout.move(Vec3f({ -0.7f, -0.7f, 0.0f }));
	for (unsigned short p = 0; p < boxedLayout.getRowCount() * boxedLayout.getColCount(); p++)
		if (p < PLAYGROUND_PANE_COUNT) {
			std::string text = std::to_string(p + 1); // count should begin at 1
			boxedTextObjs[p] = { fontStr.c_str(), text.c_str(), WHITE_COLOR, 0xFF333333 };
			Rasteron_Image* image = bakeTextI(&boxedTextObjs[p]);
			boxedLayout.getChildPane(p)->selectImage(image);
		}
	boxedLayout.configure(&scene_overlay);

	Topl_Factory::switchPipeline(APP_BACKEND, _renderer, colPipeline); // switch to correct pipeline
	_renderer->buildScene(&scene_overlay); // build operation
}

void Playground_App::createScene_Details() {
	prismActor.updatePos(Vec3f({ 0.7f, -0.7f, 0.0f }));
	scene_details.addGeometry("Cone1", &prismActor);
	coneActor.updatePos(Vec3f({ -0.7f, 0.7f, 0.0f }));
	scene_details.addGeometry("Cone2", &coneActor);

	Topl_Factory::switchPipeline(APP_BACKEND, _renderer, litPipeline);
	_renderer->buildScene(&scene_details); // build operation
}

void Playground_App::init() {
	createPipeline();

	// Events and Callbacks

	Platform::mouseControl.addCallback(MOUSE_RightBtn_Down, callback_press);
	Platform::mouseControl.addCallback(MOUSE_LeftBtn_Down, callback_press);
	Platform::mouseControl.addCallback(MOUSE_RightBtn_Up, callback_release);
	Platform::mouseControl.addCallback(MOUSE_LeftBtn_Up, callback_release);

	// Geometries and Scene Elements
	
	createScene_Main();
	createScene_Overlay();
	createScene_Details();

	_renderer->setCamera(&camera1); // no projection
	_renderer->setCamera(&camera2); // ortho projection
	// camera3.setPos({ 0, 0, -VIEW_SPACE - 0.1f });
	// _renderer->setCamera(&camera3); // perspective projection

	_renderer->setDrawMode(DRAW_Triangles);
}

void Playground_App::loop(unsigned long frame) {
	if(frame % PRE_FRAME_FRAC == 0) preFrame();

	vertexShader1.setTime(_ticker.getAbsMillisecs());
	Topl_Factory::switchPipeline(APP_BACKEND, _renderer, texPipeline);

	event_character_swap(_renderer, &scene_main); // character swap
	_renderer->updateScene(&scene_main);
	_renderer->renderScene(&scene_main);

	Topl_Factory::switchPipeline(APP_BACKEND, _renderer, colPipeline);

	_renderer->updateScene(&scene_overlay);
	_renderer->renderScene(&scene_overlay);

	Topl_Factory::switchPipeline(APP_BACKEND, _renderer, litPipeline);

	_renderer->updateScene(&scene_details);
	_renderer->renderScene(&scene_details);

	if(frame % POST_FRAME_FRAC == 0) postFrame();
}

void Playground_App::preFrame() {
	prismActor.updateRot({ 0.0f, -0.05f, 0.0f });
	coneActor.updateRot({ 0.0f, 0.0f, 0.05f });
	
	character.move((displaceVec - character.getOrigin()) * 0.33);

	scene_main.resolvePhysics();
}

void Playground_App::postFrame() {
	hoverColor = _renderer->getPixelAt(Platform::getCursorX(), Platform::getCursorY());

	frameImage = _renderer->frame();
	writeFileImageRaw("Frame.bmp", IMG_Bmp, frameImage->height, frameImage->width, frameImage->data); // draw to output file
	deleteImg(frameImage);
}

int main(int argc, char** argv) {
    Playground_App app = Playground_App(argv[0], APP_BACKEND);

	app.run();
    return 0;
}