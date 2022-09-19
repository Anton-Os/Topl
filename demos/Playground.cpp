#include "Playground.hpp"

// #define APP_BACKEND APP_OpenGL_4
#define APP_BACKEND APP_DirectX_11
// #defint APP_BACKEND App_Vulkan

#define VIEW_SPACE 2.0f
#define CAMERA_LOOK Vec3f({ 0.0f, 0.0f, 2.0f })

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
		// tessCtrlShader = GL4_Advance_TessCtrlShader();
		// tessEvalShader = GL4_Advance_TessEvalShader();
		// geomShader = GL4_Advance_GeometryShader();
	}
	else if (APP_BACKEND == APP_DirectX_11) {
		vertexShader1 = Drx11_Textured_VertexShader(); fragShader1 = Drx11_Textured_FragmentShader();
		vertexShader2 = Drx11_Flat_VertexShader(FLAT_MODE_ALTERNATE); fragShader2 = Drx11_Flat_FragmentShader();
		vertexShader3 = Drx11_Beams_VertexShader(BEAMS_MODE_DEPTH); fragShader3 = Drx11_Beams_FragmentShader();
		vertexShader4 = Drx11_Layered_VertexShader(); fragShader4 = Drx11_Layered_FragmentShader();
		// tessCtrlShader = Drx11_Advance_TessCtrlShader();
		// tessEvalShader = Drx11_Advance_TessEvalShader();
		// geomShader = Drx11_Advance_GeometryShader();
	}

	// texPipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader1, &fragShader1);
	// litPipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader3, &fragShader3);
	colPipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader2, &fragShader2);
	// advancePipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader1, &fragShader, &tessCtrlShader, &tessEvalShader, &geomShader);

}

void Playground_App::init() {
	genShaderPipeline();

	// Events and Callbacks

	Platform::mouseControl.addCallback(MOUSE_RightBtn_Down, press);
	Platform::mouseControl.addCallback(MOUSE_LeftBtn_Down, press);
	Platform::mouseControl.addCallback(MOUSE_RightBtn_Up, release);
	Platform::mouseControl.addCallback(MOUSE_LeftBtn_Up, release);

	// Geometries and Scene Elements

	// grid.configure(&scene_main);
	scene_main.addGeometry("Sphere", &sphereActor);
	sphereActor.updateRot(Vec2f({ 0.0f, 1.0f }));
	scene_details.addGeometry("Cone1", &coneActor1);
	coneActor1.updatePos(Vec3f({ 0.5f, -0.75f, 0.0f }));
	scene_details.addGeometry("Cone2", &coneActor2);
	coneActor2.updatePos(Vec3f({ -0.5f, 0.75f, 0.0f }));

	// scene_overlay.addGeometry("captureSquare", &captureSquare);
	rowLayout.move(Vec3f({ 0.5f, 0.5f, 0.0f }));
	for (unsigned short p = 0; p < rowLayout.getRowCount(); p++) {
		Rasteron_Image* rowPaneImg = createSolidImg({ PANE_IMAGE_HEIGHT, PANE_IMAGE_WIDTH }, BLACK_COLOR + (0x08 * p));
		rowLayout.getChildPane(p)->selectImage(rowPaneImg);
	}
	rowLayout.configure(&scene_overlay);
	boxedLayout.move(Vec3f({ -0.5f, -0.5f, 0.0f }));
	for (unsigned short p = 0; p < boxedLayout.getRowCount() * boxedLayout.getColCount(); p++) {
		// Rasteron_Image* boxedPaneImg;
		boxedLayout.getChildPane(p)->selectImage(nullptr);
		// deleteImg(boxedPaneImg);
	}
	boxedLayout.configure(&scene_overlay);

	// _renderer->setCamera(&camera1); // ortho projection
	// _renderer->setCamera(&camera2); // perspective projection
	// _renderer->buildScene(&scene_main);
	_renderer->buildScene(&scene_overlay);
	// _renderer->buildScene(&scene_details);

	_renderer->setDrawMode(DRAW_Triangles);
}

void Playground_App::loop(unsigned long frame) {
	if (frame % 30 == 0) preFrame();

	Topl_Factory::switchPipeline(APP_BACKEND, _renderer, colPipeline);

	_renderer->updateScene(&scene_overlay);

	_renderer->renderScene(&scene_overlay);
	// _renderer->renderScene(&scene_main);
	// _renderer->renderScene(&scene_details);
	// _renderer->renderAll();

	if (frame % 30 == 0) postFrame();
}

void Playground_App::preFrame() {
	// TODO: Perform operation before frame
}

void Playground_App::postFrame() {
	// TODO: Perform operation after frame
}

int main(int argc, char** argv) {
	/* { Playground_App app = Playground_App(argv[0], APP_BACKEND); } // destructor test */

    Playground_App app = Playground_App(argv[0], APP_BACKEND);

	app.run();
    return 0;
}