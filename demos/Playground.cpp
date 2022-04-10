#include "Playground.hpp"

#define APP_BACKEND APP_OpenGL_4

void Playground_App::init() {
	// Shaders and Pipeline

	Topl_Pipeline pipeline;
	if (APP_BACKEND == APP_OpenGL_4) {
		vertexShader = GL4_Flat_VertexShader();
		fragShader = GL4_Flat_FragmentShader();
		tessCtrlShader = GL4_Advance_TessCtrlShader();
		tessEvalShader = GL4_Advance_TessEvalShader();
		geomShader = GL4_Advance_GeometryShader();
	} else {
		vertexShader = Drx11_Flat_VertexShader();
		fragShader = Drx11_Flat_FragmentShader();
		tessCtrlShader = Drx11_Advance_TessCtrlShader();
		tessEvalShader = Drx11_Advance_TessEvalShader();
		geomShader = Drx11_Advance_GeometryShader();
	}

	flatPipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader, &fragShader);
	// advancePipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader, &fragShader, &tessCtrlShader, &tessEvalShader, &geomShader);

	// Configurations, Geometry, and Building

	squareActor.setPos(Vec3f({ -0.66f, 0.66f, 0.0f }));

	// scene_main.addGeometry(&triangleActor);
	scene_main.addGeometry(&squareActor);
	rowLayout.configure(&scene_overlay);
	boxedLayout.configure(&scene_overlay);

	_renderer->buildScene(&scene_main);
}

void Playground_App::loop(unsigned long frame) {
	_renderer->updateScene(&scene_main);
	_renderer->renderScene(&scene_main);

	if (_renderer->getFrameCount() == 2) {
		unsigned pixel1 = _renderer->getPixelAt(0.95f, 0.95f); // testing
		unsigned pixel2 = _renderer->getPixelAt(0.95f, -0.95f); // testing
		unsigned pixel3 = _renderer->getPixelAt(-0.95f, -0.95f); // testing
		unsigned pixel4 = _renderer->getPixelAt(-0.95f, 0.95f); // testing
	}
}

int main(int argc, char** argv) {
	/* { Playground_App app = Playground_App(argv[0], APP_BACKEND); } // destructor test */

    Playground_App app = Playground_App(argv[0], APP_BACKEND);

	app.run();
    return 0;
}