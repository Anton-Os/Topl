#include "Splitscreen.hpp"

#define APP_BACKEND APP_OpenGL_4
// #define APP_BACKEND APP_DirectX_11
// #defint APP_BACKEND App_Vulkan

void Splitscreen_App::init() {
	// Shaders and Pipeline

	Topl_Pipeline pipeline;
	if (APP_BACKEND == APP_OpenGL_4) {
		vertShader = GL4_Flat_VertexShader(FLAT_MODE_SOLID);
		fragShader = GL4_Flat_FragmentShader();
	} else {
		vertShader = Drx11_Flat_VertexShader(FLAT_MODE_SOLID);
		fragShader = Drx11_Flat_FragmentShader();
	}

	// pipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertShader, &fragShader);
}

void Splitscreen_App::loop(unsigned long frame) {
	// _renderer->updateScene(&scene);
	// _renderer->renderScene(&scene);
}

int main(int argc, char** argv) {
	Topl_Viewport viewports[] = {
		Topl_Viewport(0, 0, TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT / 2), // top-half pane
		Topl_Viewport(0, TOPL_WIN_HEIGHT / 2, TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT / 2), // bottom-half pane
	};

	Splitscreen_App app = Splitscreen_App(argv[0], APP_BACKEND, { viewports[0], viewports[1] });

	app.run();
    return 0;
}