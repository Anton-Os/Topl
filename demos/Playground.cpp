#include "Playground.hpp"

#define APP_BACKEND APP_OpenGL_4
// #define APP_BACKEND APP_DirectX_11
// #defint APP_BACKEND App_Vulkan

#define VIEW_SPACE 2.0f
#define CAMERA_LOOK Vec3f({ 0.0f, 0.0f, 2.0f })

#define PRE_FRAME_FRAC 2
#define POST_FRAME_FRAC 30

Topl_Camera Playground_App::camera1 = Topl_Camera(PROJECTION_Ortho, VIEW_SPACE);
Topl_Camera Playground_App::camera2 = Topl_Camera(PROJECTION_Perspective, 1.0 + (1.0 / VIEW_SPACE));

void press(float x, float y) { puts("Mouse Press"); }
void release(float x, float y) { puts("Mouse Release"); }

void Playground_App::genShaderPipeline() {
	if (APP_BACKEND == APP_OpenGL_4) {
		vertexShader1 = GL4_Textured_VertexShader(); fragShader1 = GL4_Textured_FragmentShader();
		vertexShader2 = GL4_Flat_VertexShader(FLAT_MODE_ALTERNATE); fragShader2 = GL4_Flat_FragmentShader();
		vertexShader3 = GL4_Beams_VertexShader(BEAMS_MODE_DEPTH); fragShader3 = GL4_Beams_FragmentShader();
		vertexShader4 = GL4_Layered_VertexShader(); fragShader4 = GL4_Layered_FragmentShader();
		// tessCtrlShader = GL4_Advance_TessCtrlShader(); // tessEvalShader = GL4_Advance_TessEvalShader();
		// geomShader = GL4_Advance_GeometryShader();
	}
	else if (APP_BACKEND == APP_DirectX_11) {
		vertexShader1 = Drx11_Textured_VertexShader(); fragShader1 = Drx11_Textured_FragmentShader();
		vertexShader2 = Drx11_Flat_VertexShader(FLAT_MODE_ALTERNATE); fragShader2 = Drx11_Flat_FragmentShader();
		vertexShader3 = Drx11_Beams_VertexShader(BEAMS_MODE_DEPTH); fragShader3 = Drx11_Beams_FragmentShader();
		vertexShader4 = Drx11_Layered_VertexShader(); fragShader4 = Drx11_Layered_FragmentShader();
		// tessCtrlShader = Drx11_Advance_TessCtrlShader(); // tessEvalShader = Drx11_Advance_TessEvalShader();
		// geomShader = Drx11_Advance_GeometryShader();
	}

	texPipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader1, &fragShader1);
	litPipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader3, &fragShader3);
	colPipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader2, &fragShader2);
	// advancePipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader1, &fragShader1, &tessCtrlShader, &tessEvalShader, &geomShader);
}

void Playground_App::createScene_Main() {
	// platformActor.setRot({ 0.0f, TOPL_PI / 2 });
	// scene_main.addGeometry("platform", &platformActor);

	std::fill_n(sphereActors, PLAYGROUND_SPHERES_COUNT, ((Geo_RenderObj*)&sphere));
	for (unsigned short s = 0; s < PLAYGROUND_SPHERES_COUNT; s++) {
		sphereActors[s].setPos({ (s * 0.2f) - 0.5f, (s % 2) * 0.05f, s * 0.15f });
		// scene_main.addTexture("sphere" + std::to_string(s), createSolidImg({ 1024, 1024 }, genRandColor()));
		scene_main.addGeometry("sphere" + std::to_string(s), &sphereActors[s]);
	}

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
		if (p < PLAYGROUND_TEXTOBJ_COUNT) {
			std::string text = std::to_string(p + 1); // count should begin at 1
			boxedTextObjs[p] = { fontStr.c_str(), text.c_str(), WHITE_COLOR, 0xFF333333 };
			// Topl_Image image = Topl_Image(&_freetypeLib, textObjects[p]);
			Rasteron_Image* image = bakeTextI(&boxedTextObjs[p]);
			boxedLayout.getChildPane(p)->selectImage(image);
		}
	boxedLayout.configure(&scene_overlay);

	// Build Scene
	_renderer->buildScene(&scene_overlay);
}

void Playground_App::createScene_Details() {
	coneActor1.updatePos(Vec3f({ 0.7f, -0.7f, 0.0f }));
	scene_details.addGeometry("Cone1", &coneActor1);
	coneActor2.updatePos(Vec3f({ -0.7f, 0.7f, 0.0f }));
	scene_details.addGeometry("Cone2", &coneActor2);

	// Build Scene
	_renderer->buildScene(&scene_details);
}

void Playground_App::init() {
	genShaderPipeline();

	// Events and Callbacks

	Platform::mouseControl.addCallback(MOUSE_RightBtn_Down, press);
	Platform::mouseControl.addCallback(MOUSE_LeftBtn_Down, press);
	Platform::mouseControl.addCallback(MOUSE_RightBtn_Up, release);
	Platform::mouseControl.addCallback(MOUSE_LeftBtn_Up, release);

	// Geometries and Scene Elements

	Topl_Factory::switchPipeline(APP_BACKEND, _renderer, texPipeline);

	createScene_Overlay();
	createScene_Main();
	createScene_Details();

	// _renderer->setCamera(&camera1); // ortho projection
	// _renderer->setCamera(&camera2); // perspective projection

	_renderer->setDrawMode(DRAW_Triangles);
}

void Playground_App::loop(unsigned long frame) {
	if (frame % PRE_FRAME_FRAC == 0) preFrame();

	_renderer->updateScene(&scene_main);
	_renderer->updateScene(&scene_details);
	_renderer->updateScene(&scene_overlay);

	Topl_Factory::switchPipeline(APP_BACKEND, _renderer, texPipeline);
	_renderer->renderScene(&scene_main);
	_renderer->renderScene(&scene_details);
	_renderer->renderScene(&scene_overlay);

	if (frame % POST_FRAME_FRAC == 0) postFrame();
}

void Playground_App::preFrame() {
	coneActor1.updateRot({ 0.003f, 0.0f });
	coneActor2.updateRot({ -0.003f, 0.0f });

	for (unsigned short s = 0; s < PLAYGROUND_SPHERES_COUNT; s++) {
		// sphereActors[s].setPos({ (s * 0.2f) - 0.5f, (s % 2) * 0.05f, s * 0.15f });

		if (rand() % 2 == 0) // random x update
			if (rand() % 2 == 0) sphereActors[s].updatePos({ 0.001f * (rand() % 5), 0.0f, 0.0f });
			else sphereActors[s].updatePos({ -0.001f * (rand() % 5), 0.0f, 0.0f });

		if (rand() % 2 == 0) // random y update
			if (rand() % 2 == 0) sphereActors[s].updatePos({ 0.0f, 0.001f * (rand() % 5), 0.0f });
			else sphereActors[s].updatePos({ 0.0f, -0.001f * (rand() % 5), 0.0f });
	}
}

void Playground_App::postFrame() {
	// Draws Output to File
	frameImage = _renderer->frame();
	writeFileImageRaw("Frame.bmp", IMG_Bmp, frameImage->height, frameImage->width, frameImage->data);
	deleteImg(frameImage);
}

int main(int argc, char** argv) {
	/* { Playground_App app = Playground_App(argv[0], APP_BACKEND); } // destructor test */

    Playground_App app = Playground_App(argv[0], APP_BACKEND);

	app.run();
    return 0;
}