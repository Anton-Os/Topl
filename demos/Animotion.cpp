#include "Animotion.hpp"

#define APP_BACKEND APP_OpenGL_4
// #define APP_BACKEND APP_DirectX_11
// #define APP_BACKEND App_Vulkan

Topl_Camera Animotion_App::camera = Topl_Camera();
// Topl_Camera Animotion_App::camera = Topl_Camera(PROJECTION_Ortho, 1.0);
// Topl_Camera Animotion_App::camera = Topl_Camera(PROJECTION_Perspective, 1.0);

#define MOVE_AMOUNT 0.25

static void callback_r() {
	Animotion_App::camera.setPos({ 0.0, 0.0, 0.0 });
	Animotion_App::camera.setRotation({ 0.0, 0.0, 0.0 });
}
static void callback_w() { Animotion_App::camera.updatePos({ 0.0, MOVE_AMOUNT, 0.0 }); }
static void callback_a() { Animotion_App::camera.updatePos({ -MOVE_AMOUNT, 0.0, 0.0 }); }
static void callback_s() { Animotion_App::camera.updatePos({ 0.0, -MOVE_AMOUNT, 0.0 }); }
static void callback_d() { Animotion_App::camera.updatePos({ MOVE_AMOUNT, 0.0, 0.0 }); }
static void callback_q() { Animotion_App::camera.updatePos({ 0.0, 0.0, -MOVE_AMOUNT }); }
static void callback_e() { Animotion_App::camera.updatePos({ 0.0, 0.0, MOVE_AMOUNT }); }

#define LOOK_AMOUNT 0.001

Input_CursorRange upRange = Input_CursorRange({ -1.0f, 1.0f }, { 0.8f, 1.0f });
Input_CursorRange leftRange = Input_CursorRange({ -0.8f, -1.0f }, { -1.0f, 1.0f });
Input_CursorRange downRange = Input_CursorRange({ -1.0f, 1.0f }, { -0.8f, -1.0f });
Input_CursorRange rightRange = Input_CursorRange({ 0.8f, 1.0f }, { -1.0f, 1.0f });

static void callback_up(float x, float y) { Animotion_App::camera.updateRotation({ 0.0, LOOK_AMOUNT, 0.0 }); }
static void callback_left (float x, float y) { Animotion_App::camera.updateRotation({ -LOOK_AMOUNT, 0.0, 0.0 }); }
static void callback_down(float x, float y) { Animotion_App::camera.updateRotation({ 0.0, -LOOK_AMOUNT, 0.0 }); }
static void callback_right(float x, float y) { Animotion_App::camera.updateRotation({ LOOK_AMOUNT, 0.0, 0.0 });}

void Animotion_App::init() {
	srand(time(NULL));

	// Shaders and Pipeline

	if (APP_BACKEND == APP_OpenGL_4) {
		vertShader = GL4_Flat_VertexShader(FLAT_MODE_DIRECTION);
		fragShader = GL4_Flat_FragmentShader();
	} else {
		vertShader = Drx11_Flat_VertexShader(FLAT_MODE_DIRECTION);
		fragShader = Drx11_Flat_FragmentShader();
	}

	Topl_Pipeline* pipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertShader, &fragShader);

	// Events and Callbacks

	Platform::keyControl.addCallback('r', callback_r); // reset
	Platform::keyControl.addCallback('w', callback_w);
	Platform::keyControl.addCallback('a', callback_a);
	Platform::keyControl.addCallback('s', callback_s);
	Platform::keyControl.addCallback('d', callback_d);
	Platform::keyControl.addCallback('q', callback_q);
	Platform::keyControl.addCallback('e', callback_e);

	Platform::mouseControl.addHoverCallback(&upRange, callback_up);
	Platform::mouseControl.addHoverCallback(&leftRange, callback_left);
	Platform::mouseControl.addHoverCallback(&downRange, callback_down);
	Platform::mouseControl.addHoverCallback(&rightRange, callback_right);

	// Geometries and Scene Elements

	camera.setPos({ 0.0, 0.0, -0.0 });

	sphereActor.setPos({ 0.0, 0.0, -1.0f });
	scene.addGeometry("Sphere", &sphereActor);

	model1.configure(&scene);
	model1.move({ 0.0f, -0.25f, 0.0f });

	model2.configure(&scene);
	model2.move({ 0.5f, -0.25f, 1.0f });

	model3.configure(&scene);
	model3.move({ -0.5f, -0.25f, 1.0f });

	_renderer->buildScene(&scene);
	_renderer->setCamera(&camera);
}

void Animotion_App::loop(double frameTime) {
	_renderer->updateScene(&scene);
	_renderer->renderScene(&scene);
}

int main(int argc, char** argv) {
	Animotion_App app = Animotion_App(argv[0], APP_BACKEND );

	app.run();
    return 0;
}