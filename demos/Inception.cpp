#include "Inception.hpp"

void Inception_App::init() {
	return;
}

void Inception_App::loop(double secs, unsigned long frame) {
	return;
}

int main(int argc, char** argv) {
	{
		Inception_App testApp = Inception_App(argv[0], APP_DirectX_11);
	}

    Inception_App app = Inception_App(argv[0], APP_OpenGL_4);
	app.run();
    return 0;
}