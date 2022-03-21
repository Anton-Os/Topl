#include "Exec.hpp"

#define APP_BACKEND APP_DirectX_11

void Exec_App::init() {
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
	setActivePipeline(flatPipeline);

	// Configurations, Geometry, and Building

	scene_main.addGeometry(&triangleActor);
	rowLayout.init(&scene_overlay);
	boxedLayout.init(&scene_overlay);

	_renderer->buildScene(&scene_main);
}

void Exec_App::loop(double secs, unsigned long frame) {
	_renderer->updateScene(&scene_main);
	_renderer->renderScene(&scene_main);
}

int main(int argc, char** argv) {
    Exec_App app = Exec_App(argv[0], APP_BACKEND);

	app.run();
    return 0;
}