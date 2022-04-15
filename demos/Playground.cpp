#include "Playground.hpp"

// #define APP_BACKEND APP_OpenGL_4
#define APP_BACKEND APP_DirectX_11

#define CAMERA_LOOK Vec3f({ 0.0f, 0.0f, 1.0f })

Topl_Camera Playground_App::camera1 = Topl_Camera(PROJECTION_Ortho);
Topl_Camera Playground_App::camera2 = Topl_Camera(PROJECTION_Perspective);
Phys_Motion Playground_App::inOutMotion = Phys_Motion(MOTION_Linear, Vec3f({ 0.0f, 0.0f, 1.0f }), 4.0);

void inOutEvent(double absSecs) {
	Vec3f motionVec = Playground_App::inOutMotion.getMotion(absSecs);
	Playground_App::camera1.setPos(motionVec);
	// Playground_App::camera1.setLookPos(motionVec + CAMERA_LOOK);
	Playground_App::camera2.setPos(motionVec);
	// Playground_App::camera2.setLookPos(motionVec + CAMERA_LOOK);
}

void Playground_App::init() {
	// Shaders and Pipeline

	Topl_Pipeline pipeline;
	if (APP_BACKEND == APP_OpenGL_4) {
		vertexShader1 = GL4_Textured_VertexShader();
		fragShader1 = GL4_Textured_FragmentShader();
		vertexShader2 = GL4_Flat_VertexShader(FLAT_MODE_ALTERNATE);
		fragShader2 = GL4_Flat_FragmentShader();
		tessCtrlShader = GL4_Advance_TessCtrlShader();
		tessEvalShader = GL4_Advance_TessEvalShader();
		geomShader = GL4_Advance_GeometryShader();
	} else {
		vertexShader1 = Drx11_Textured_VertexShader();
		fragShader1 = Drx11_Textured_FragmentShader();
		vertexShader2 = Drx11_Flat_VertexShader(FLAT_MODE_ALTERNATE);
		fragShader2 = Drx11_Flat_FragmentShader();
		tessCtrlShader = Drx11_Advance_TessCtrlShader();
		tessEvalShader = Drx11_Advance_TessEvalShader();
		geomShader = Drx11_Advance_GeometryShader();
	}

	texPipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader1, &fragShader1);
	colorPipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader2, &fragShader2);
	// advancePipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader1, &fragShader, &tessCtrlShader, &tessEvalShader, &geomShader);

	// Configurations, Geometry, and Scene Creation

	// sphereActor.setPos(Vec3f({ -0.5f, 0.5f, 0.0f }));
	scene_main.addGeometry(&sphereActor);

	// scene_overlay.addGeometry("captureSquare", &captureSquare);
	rowLayout.move(Vec3f({ 0.5f, 0.5f, 0.0f }));
	rowLayout.configure(&scene_overlay);
	boxedLayout.move(Vec3f({ -0.5f, 0.5f, 0.0f }));
	boxedLayout.configure(&scene_overlay);

	_renderer->setCamera(&camera1); // ortho projection
	// _renderer->setCamera(&camera2); // perspective projection
	_renderer->buildScene(&scene_main);
	// _renderer->buildScene(&scene_overlay);
	// _renderer->buildScene(&scene_details);

	_renderer->setDrawMode(DRAW_Points);
}

void Playground_App::loop(unsigned long frame) {
	_renderer->setPipeline(colorPipeline);
	_renderer->updateScene(&scene_main);
	_renderer->renderScene(&scene_main);
	// switch pipelines
	// _renderer->renderScene(&scene_overlay);

	if (frame % 30 == 0) postFrame();
}

void Playground_App::postFrame() {
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