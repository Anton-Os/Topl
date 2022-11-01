#include "Animotion.hpp"

#define APP_BACKEND APP_OpenGL_4
// #define APP_BACKEND APP_DirectX_11
// #defint APP_BACKEND App_Vulkan

void Animotion_App::init() {
	// Shaders and Pipeline

	if (APP_BACKEND == APP_OpenGL_4) {
		vertShader = GL4_Flat_VertexShader(FLAT_MODE_SOLID);
		fragShader = GL4_Flat_FragmentShader();
	} else {
		vertShader = Drx11_Flat_VertexShader(FLAT_MODE_SOLID);
		fragShader = Drx11_Flat_FragmentShader();
	}

	Topl_Pipeline* pipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertShader, &fragShader);

	// Geometries and Scene Elements

	model.configure(&scene);
	// model.move({ 0.0f, -0.5f, 0.0f });

	_renderer->buildScene(&scene);
	_renderer->setDrawMode(DRAW_Triangles);
}

void Animotion_App::loop(unsigned long frame) {
	_renderer->updateScene(&scene);
	_renderer->renderScene(&scene);
}

int main(int argc, char** argv) {
	Animotion_App app = Animotion_App(argv[0], APP_BACKEND );

	app.run();
    return 0;
}