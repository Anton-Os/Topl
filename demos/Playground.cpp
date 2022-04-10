#include "Playground.hpp"

#define APP_BACKEND APP_DirectX_11

void Playground_App::init() {
	// Shaders and Pipeline

	Topl_Pipeline pipeline;
	if (APP_BACKEND == APP_OpenGL_4) {
		vertexShader = GL4_Textured_VertexShader();
		fragShader = GL4_Textured_FragmentShader();
		tessCtrlShader = GL4_Advance_TessCtrlShader();
		tessEvalShader = GL4_Advance_TessEvalShader();
		geomShader = GL4_Advance_GeometryShader();
	} else {
		vertexShader = Drx11_Textured_VertexShader();
		fragShader = Drx11_Textured_FragmentShader();
		tessCtrlShader = Drx11_Advance_TessCtrlShader();
		tessEvalShader = Drx11_Advance_TessEvalShader();
		geomShader = Drx11_Advance_GeometryShader();
	}

	flatPipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader, &fragShader);
	// advancePipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader, &fragShader, &tessCtrlShader, &tessEvalShader, &geomShader);

	// Configurations, Geometry, and Building

	// sphereActor.setPos(Vec3f({ -0.5f, 0.5f, 0.0f }));
	scene_main.addGeometry(&sphereActor);

	// scene_overlay.addGeometry("captureSquare", &captureSquare);
	rowLayout.move(Vec3f({ 0.5f, 0.5f, 0.0f }));
	rowLayout.configure(&scene_overlay);
	boxedLayout.move(Vec3f({ -0.5f, 0.5f, 0.0f }));
	boxedLayout.configure(&scene_overlay);

	_renderer->buildScene(&scene_overlay);
}

void Playground_App::loop(unsigned long frame) {
	_renderer->updateScene(&scene_overlay);
	_renderer->renderScene(&scene_overlay);

	if (_renderer->getFrameCount() == 2) {
		unsigned pixel1 = _renderer->getPixelAt(0.5f, 0.5f); // upper right test
		unsigned pixel2 = _renderer->getPixelAt(0.5f, -0.5f); // lower right test
		unsigned pixel3 = _renderer->getPixelAt(-0.5f, -0.5f); // lower left test
		unsigned pixel4 = _renderer->getPixelAt(-0.5f, 0.5f); // upper left test
		unsigned pixel5 = _renderer->getPixelAt(0.0f, 0.0f); // center test
	}
}

int main(int argc, char** argv) {
	/* { Playground_App app = Playground_App(argv[0], APP_BACKEND); } // destructor test */

    Playground_App app = Playground_App(argv[0], APP_BACKEND);

	app.run();
    return 0;
}