#include "Playground.hpp"

// #define APP_BACKEND APP_OpenGL_4
#define APP_BACKEND APP_DirectX_11
// #defint APP_BACKEND App_Vulkan

#define VIEW_SPACE 2.0f
#define CAMERA_LOOK Vec3f({ 0.0f, 0.0f, 2.0f })

Topl_Camera Playground_App::camera1 = Topl_Camera(PROJECTION_Ortho, VIEW_SPACE);
Topl_Camera Playground_App::camera2 = Topl_Camera(PROJECTION_Perspective, 1.0 + (1.0 / VIEW_SPACE));

void press(float x, float y) { puts("Mouse Press"); }
void release(float x, float y) { puts("Mounse Release"); }

void Playground_App::init() {
	// Shaders and Pipeline
	genShaders();

	// pipeline order creation should be independent!
	texPipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader1, &fragShader1);
	litPipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader3, &fragShader3);
	colorPipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader2, &fragShader2);
	// advancePipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader1, &fragShader, &tessCtrlShader, &tessEvalShader, &geomShader);

	// Configurations, Geometry, and Events

	Platform::mouseControl.addCallback(MOUSE_RightBtn_Down, press);
	Platform::mouseControl.addCallback(MOUSE_LeftBtn_Down, press);
	Platform::mouseControl.addCallback(MOUSE_RightBtn_Up, release);
	Platform::mouseControl.addCallback(MOUSE_LeftBtn_Up, release);

	// grid.configure(&scene_main);
	scene_main.addGeometry(&sphereActor);
	sphereActor.updateRot(Vec2f({ 0.0f, 1.0f }));

	// scene_overlay.addGeometry("captureSquare", &captureSquare);
	rowLayout.move(Vec3f({ 0.5f, 0.5f, 0.0f }));
	rowLayout.configure(&scene_overlay);
	boxedLayout.move(Vec3f({ -0.5f, -0.5f, 0.0f }));
	boxedLayout.configure(&scene_overlay);

	_renderer->setCamera(&camera1); // ortho projection
	// _renderer->setCamera(&camera2); // perspective projection
	// _renderer->buildScene(&scene_main);
	_renderer->buildScene(&scene_overlay);
	// _renderer->buildScene(&scene_details);

	_renderer->setDrawMode(DRAW_Triangles);
}

void Playground_App::loop(unsigned long frame) {
	_renderer->setPipeline(colorPipeline);
	// _renderer->updateScene(&scene_main);
	// _renderer->renderScene(&scene_main);
	// switch pipelines
	// _renderer->setPipeline(texPipeline);
	_renderer->renderScene(&scene_overlay);

	if (frame % 30 == 0) postFrame();
}

void Playground_App::genShaders() {
	if (APP_BACKEND == APP_OpenGL_4) {
		vertexShader1 = GL4_Textured_VertexShader();
		fragShader1 = GL4_Textured_FragmentShader();
		vertexShader2 = GL4_Flat_VertexShader(FLAT_MODE_ALTERNATE);
		fragShader2 = GL4_Flat_FragmentShader();
		vertexShader3 = GL4_Beams_VertexShader(BEAMS_MODE_DEPTH);
		fragShader3 = GL4_Beams_FragmentShader();
		vertexShader4 = GL4_Layered_VertexShader();
		fragShader4 = GL4_Layered_FragmentShader();
		tessCtrlShader = GL4_Advance_TessCtrlShader();
		tessEvalShader = GL4_Advance_TessEvalShader();
		geomShader = GL4_Advance_GeometryShader();
	}
	else if(APP_BACKEND == APP_DirectX_11) {
		vertexShader1 = Drx11_Textured_VertexShader();
		fragShader1 = Drx11_Textured_FragmentShader();
		vertexShader2 = Drx11_Flat_VertexShader(FLAT_MODE_ALTERNATE);
		fragShader2 = Drx11_Flat_FragmentShader();
		vertexShader3 = Drx11_Beams_VertexShader(BEAMS_MODE_DEPTH);
		fragShader3 = Drx11_Beams_FragmentShader();
		vertexShader4 = Drx11_Layered_VertexShader();
		fragShader4 = Drx11_Layered_FragmentShader();
		tessCtrlShader = Drx11_Advance_TessCtrlShader();
		tessEvalShader = Drx11_Advance_TessEvalShader();
		geomShader = Drx11_Advance_GeometryShader();
	}
}

void Playground_App::postFrame() {
	// checking pixel colors
	unsigned pixel1 = _renderer->getPixelAt(0.5f, 0.5f); // upper right test
	unsigned pixel2 = _renderer->getPixelAt(0.5f, -0.5f); // lower right test
	unsigned pixel3 = _renderer->getPixelAt(-0.5f, -0.5f); // lower left test
	unsigned pixel4 = _renderer->getPixelAt(-0.5f, 0.5f); // upper left test
	unsigned pixel5 = _renderer->getPixelAt(0.0f, 0.0f); // center test
}

int main(int argc, char** argv) {
	/* { Playground_App app = Playground_App(argv[0], APP_BACKEND); } // destructor test */

    Playground_App app = Playground_App(argv[0], APP_BACKEND);

	app.run();
    return 0;
}