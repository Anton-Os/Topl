#include "Splitscreen.hpp"

// #define APP_BACKEND APP_OpenGL_4
#define APP_BACKEND APP_DirectX_11
// #define APP_BACKEND APP_Vulkan

void callback_w() { 
	viewport1.yOffset -= 1; viewport1.height += 1;
	viewport2.yOffset -= 1; viewport2.height -= 1;
}

void callback_s() {
	viewport1.yOffset += 1; viewport1.height -= 1;
	viewport2.yOffset += 1; viewport2.height += 1;
}

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

	// Events and Callbacks

	Platform::keyControl.addCallback('w', callback_w);
	Platform::keyControl.addCallback('s', callback_s);

	// Geometries and Scene Elements

	scene.addGeometry(&planeActor);

	_renderer->buildScene(&scene);
}

void Splitscreen_App::loop(double frameTime) {
	{
		_renderer->setPipeline(pipeline1);
		_renderer->updateScene(&scene);
		_renderer->setViewport(&viewport1);
		_renderer->renderScene(&scene);
	}

	{
		_renderer->setPipeline(pipeline2);
		_renderer->updateScene(&scene);
		_renderer->setViewport(&viewport2);
		_renderer->renderScene(&scene);
	}
}

int main(int argc, char** argv) {
	Splitscreen_App app = Splitscreen_App(argv[0], APP_BACKEND);

	app.run();
    return 0;
}