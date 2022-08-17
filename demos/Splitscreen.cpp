#include "Splitscreen.hpp"

#define APP_BACKEND APP_OpenGL_4
// #define APP_BACKEND APP_DirectX_11
// #defint APP_BACKEND App_Vulkan

void Splitscreen_App::init() {
	// Shaders and Pipeline

	Topl_Pipeline pipeline;
	if (APP_BACKEND == APP_OpenGL_4) {
		vertexShader1 = GL4_Effect_VertexShader();
		fragShader = GL4_Effect_FragmentShader();
	} else {
		vertexShader1 = Drx11_Effect_VertexShader();
		fragShader = Drx11_Effect_FragmentShader();
	}

	// pipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader1, &fragShader);
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