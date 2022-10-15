#include "Playground.hpp"

#define APP_BACKEND APP_OpenGL_4
// #define APP_BACKEND APP_DirectX_11
// #defint APP_BACKEND App_Vulkan

#define VIEW_SPACE 2.0f
#define CAMERA_LOOK Vec3f({ 0.0f, 0.0f, 2.0f })

#define PRE_FRAME_FRAC 3
#define POST_FRAME_FRAC 30

Topl_Camera Playground_App::camera1 = Topl_Camera();
Topl_Camera Playground_App::camera2 = Topl_Camera(PROJECTION_Ortho, VIEW_SPACE);
Topl_Camera Playground_App::camera3 = Topl_Camera(PROJECTION_Perspective, 1.0 + (1.0 / VIEW_SPACE));

static unsigned hoverColor = ZERO_COLOR;
static Vec3f displaceVec = { 0.0f, 0.0f, 0.0f };
static Vec3f pawnVec = { 0.0f, 0.0f, 0.0f };

void callback_press(float x, float y) {
	if (hoverColor == (CLEAR_COLOR_HEX & 0x00FFFFFF)) puts("\nBackground pressed!");
	else puts("\nObject pressed!");
}
void callback_release(float x, float y) { 
	printf("Displacement is %6.4lf, %6.4lf ", x, y);
	displaceVec = { x, y, 0.0f }; 
}

void callback_pane(unsigned short num) { printf("pane %d callback_pressed!", num); }

void Playground_App::genShaderPipeline() {
	if (APP_BACKEND == APP_OpenGL_4) {
		vertexShader0 = GL4_Idle_VertexShader(); fragShader0 = GL4_Idle_FragmentShader();
		vertexShader1 = GL4_Textured_VertexShader(); fragShader1 = GL4_Textured_FragmentShader();
		vertexShader2 = GL4_Flat_VertexShader(FLAT_MODE_ALTERNATE); fragShader2 = GL4_Flat_FragmentShader();
		vertexShader3 = GL4_Beams_VertexShader(BEAMS_MODE_DEPTH); fragShader3 = GL4_Beams_FragmentShader();
		// tessCtrlShader = GL4_Advance_TessCtrlShader(); 
		// tessEvalShader = GL4_Advance_TessEvalShader();
		geomShader = GL4_Advance_GeometryShader();
	}
	else if (APP_BACKEND == APP_DirectX_11) {
		vertexShader0 = Drx11_Idle_VertexShader(); fragShader0 = Drx11_Idle_FragmentShader();
		vertexShader1 = Drx11_Textured_VertexShader(); fragShader1 = Drx11_Textured_FragmentShader();
		vertexShader2 = Drx11_Flat_VertexShader(FLAT_MODE_ALTERNATE); fragShader2 = Drx11_Flat_FragmentShader();
		vertexShader3 = Drx11_Beams_VertexShader(BEAMS_MODE_DEPTH); fragShader3 = Drx11_Beams_FragmentShader();
		// tessCtrlShader = Drx11_Advance_TessCtrlShader(); 
		// tessEvalShader = Drx11_Advance_TessEvalShader();
		geomShader = Drx11_Advance_GeometryShader();
	}

	texPipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader1, &fragShader1);
	colPipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader2, &fragShader2);
	litPipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader3, &fragShader3);
	advPipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader2, &fragShader2, &geomShader, nullptr, nullptr);
	// advPipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader0, &fragShader0, &geomShader, &tessCtrlShader, &tessEvalShader);
}

void Playground_App::createScene_Main() {
	ghost.configure(&scene_main);
	// demon.configure(&scene_main);
	// angel.configure(&scene_main);

	demon.move({ -0.5f, 0.0f, 0.0f });
	angel.move({ 0.5f, 0.0f, 0.0f });

	// scene_main.addGeometry(&heightmapActor); // TODO: Get this to work!

	// Build Scene
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

	// Build Scene
	_renderer->buildScene(&scene_overlay);
}

unsigned callback_gradient(double x, double y) { return blend(BLACK_COLOR, WHITE_COLOR, y); } // black and white

void Playground_App::createScene_Details() {
	coneActor1.updatePos(Vec3f({ 0.7f, -0.7f, 0.0f }));
	scene_details.addGeometry("Cone1", &coneActor1);
	coneActor2.updatePos(Vec3f({ -0.7f, 0.7f, 0.0f }));
	scene_details.addGeometry("Cone2", &coneActor2);

	Rasteron_Image* image = createMappedImg({ 1024, 1024 }, callback_gradient);
	scene_details.addTexture("Cone1", image);
	scene_details.addTexture("Cone2", image);

	std::fill_n(sphereActors, PLAYGROUND_SPHERES_COUNT, ((Geo_RenderObj*)&sphere));
	for (unsigned short s = 0; s < PLAYGROUND_SPHERES_COUNT; s++)
		sphereActors[s].setPos({ (s * 0.2f) - 0.5f, (s % 2) * 0.05f, s * 0.15f });

	// Build Scene
	_renderer->buildScene(&scene_details);
}

void Playground_App::init() {
	genShaderPipeline();

	// Events and Callbacks

	Platform::mouseControl.addCallback(MOUSE_RightBtn_Down, callback_press);
	Platform::mouseControl.addCallback(MOUSE_LeftBtn_Down, callback_press);
	Platform::mouseControl.addCallback(MOUSE_RightBtn_Up, callback_release);
	Platform::mouseControl.addCallback(MOUSE_LeftBtn_Up, callback_release);

	// Geometries and Scene Elements

	Topl_Factory::switchPipeline(APP_BACKEND, _renderer, texPipeline); // TODO: remove this line DirectX testing!

	createScene_Overlay();
	createScene_Main();
	createScene_Details();

	// camera1.setPos({ 0.3f, 0.4f, 0.0f });
	_renderer->setCamera(&camera1); // no projection
	// _renderer->setCamera(&camera2); // ortho projection
	// _renderer->setCamera(&camera3); // perspective projection

	_renderer->setDrawMode(DRAW_Triangles);
}

void Playground_App::loop(unsigned long frame) {
	if (frame % PRE_FRAME_FRAC == 0) preFrame();

	_renderer->updateScene(&scene_main);
	_renderer->updateScene(&scene_details);
	_renderer->updateScene(&scene_overlay);

	Topl_Factory::switchPipeline(APP_BACKEND, _renderer, texPipeline);
	// Topl_Factory::switchPipeline(APP_BACKEND, _renderer, colPipeline);
	// Topl_Factory::switchPipeline(APP_BACKEND, _renderer, litPipeline);
	_renderer->renderScene(&scene_main);
	_renderer->renderScene(&scene_details);
	_renderer->renderScene(&scene_overlay);

	if (frame % POST_FRAME_FRAC == 0) postFrame();
}

void Playground_App::preFrame() {
	coneActor1.updateRot({ 0.01f, -0.01f });
	coneActor2.updateRot({ -0.01f, 0.01f });
	
	pawnVec = ghost.getOrigin();
	scene_main.addForce("ghost_body", (displaceVec - pawnVec) * 20);
	scene_main.addForce("ghost_head", (displaceVec - pawnVec) * 10);
	scene_main.addForce("ghost_leftArm", displaceVec - pawnVec);
	scene_main.addForce("ghost_rightArm", displaceVec - pawnVec);
	scene_main.addForce("ghost_leftLeg", displaceVec - pawnVec);
	scene_main.addForce("ghost_rightLeg", displaceVec - pawnVec);
	scene_main.resolvePhysics();
}

void Playground_App::postFrame() {
	hoverColor = _renderer->getPixelAt(Platform::getCursorX(), Platform::getCursorY());

	frameImage = _renderer->frame();
	writeFileImageRaw("Frame.bmp", IMG_Bmp, frameImage->height, frameImage->width, frameImage->data); // draw to output file
	deleteImg(frameImage);
}

int main(int argc, char** argv) {
	/* { Playground_App app = Playground_App(argv[0], APP_BACKEND); } // destructor test */

    Playground_App app = Playground_App(argv[0], APP_BACKEND);

	app.run();
    return 0;
}