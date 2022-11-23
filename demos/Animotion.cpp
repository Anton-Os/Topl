#include "Animotion.hpp"

#define APP_BACKEND APP_OpenGL_4
// #define APP_BACKEND APP_DirectX_11
// #define APP_BACKEND App_Vulkan

Topl_Camera Animotion_App::camera = Topl_Camera();

#define MOVE_AMOUNT 0.1

static void callback_w() { Animotion_App::camera.updatePos({ 0.0, MOVE_AMOUNT, 0.0 }); }
static void callback_a() { Animotion_App::camera.updatePos({ -MOVE_AMOUNT, 0.0, 0.0 }); }
static void callback_s() { Animotion_App::camera.updatePos({ 0.0, -MOVE_AMOUNT, 0.0 }); }
static void callback_d() { Animotion_App::camera.updatePos({ MOVE_AMOUNT, 0.0, 0.0 }); }

#define LOOK_AMOUNT 0.01

static void callback_up(float x, float y) { Animotion_App::camera.updateLookPos({ 0.0, LOOK_AMOUNT, 0.0 }); }
static void callback_left (float x, float y) { Animotion_App::camera.updateLookPos({ -LOOK_AMOUNT, 0.0, 0.0 }); }
static void callback_down(float x, float y) { Animotion_App::camera.updateLookPos({ 0.0, -LOOK_AMOUNT, 0.0 }); }
static void callback_right(float x, float y) { Animotion_App::camera.updateLookPos({ LOOK_AMOUNT, 0.0, 0.0 });}

void Animotion_App::init() {
	srand(time(NULL));

	// Shaders and Pipeline

	if (APP_BACKEND == APP_OpenGL_4) {
		vertShader = GL4_Flat_VertexShader(FLAT_MODE_SOLID);
		fragShader = GL4_Flat_FragmentShader();
	} else {
		vertShader = Drx11_Flat_VertexShader(FLAT_MODE_SOLID);
		fragShader = Drx11_Flat_FragmentShader();
	}

	Topl_Pipeline* pipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertShader, &fragShader);

	// Events and Callbacks

	Platform::keyControl.addCallback('w', callback_w);
	Platform::keyControl.addCallback('a', callback_a);
	Platform::keyControl.addCallback('s', callback_s);
	Platform::keyControl.addCallback('d', callback_d);

	Platform::mouseControl.addHoverCallback(&upRange, callback_up);
	Platform::mouseControl.addHoverCallback(&leftRange, callback_left);
	Platform::mouseControl.addHoverCallback(&downRange, callback_down);
	Platform::mouseControl.addHoverCallback(&rightRange, callback_right);

	// Geometries and Scene Elements

	model1.configure(&scene);
	model1.move({ 0.0f, -0.25f, 0.0f });

	model2.configure(&scene);
	model2.move({ 0.5f, -0.25f, 0.0f });

	model3.configure(&scene);
	model3.move({ -0.5f, -0.25f, 0.0f });

	_renderer->buildScene(&scene);
	_renderer->setCamera(&camera);
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