#include "Splitscreen.hpp"

// #define APP_BACKEND APP_OpenGL_4
#define APP_BACKEND APP_DirectX_11
// #defint APP_BACKEND App_Vulkan

void Splitscreen_App::init() {
	// Shaders and Pipeline

	if (APP_BACKEND == APP_OpenGL_4) {
		vertexShader1 = GL4_Effect_VertexShader(EFFECT_MODE_CURSOR);
		vertexShader2 = GL4_Effect_VertexShader(EFFECT_MODE_FRACTAL);
		fragShader = GL4_Effect_FragmentShader();
	} else {
		vertexShader1 = Drx11_Effect_VertexShader(EFFECT_MODE_CURSOR);
		vertexShader2 = Drx11_Effect_VertexShader(EFFECT_MODE_FRACTAL);
		fragShader = Drx11_Effect_FragmentShader();
	}

	pipeline1 = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader1, &fragShader);
	pipeline2 = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader2, &fragShader);

	// Geometries and Scene Elements

	scene.addGeometry(&planeActor);

	_renderer->buildScene(&scene);
	_renderer->setDrawMode(DRAW_Triangles);
}

void Splitscreen_App::loop(unsigned long frame) {
	_renderer->setPipeline(pipeline2);
	_renderer->updateScene(&scene);
	_renderer->setViewport(&viewports[0]);
	_renderer->renderScene(&scene);
	
	_renderer->setPipeline(pipeline1);
	_renderer->updateScene(&scene);
	_renderer->setViewport(&viewports[1]);
	_renderer->renderScene(&scene);
}

int main(int argc, char** argv) {
	Splitscreen_App app = Splitscreen_App(argv[0], APP_BACKEND);

	app.run();
    return 0;
}