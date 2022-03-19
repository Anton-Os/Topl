#include "Exec.hpp"

void Exec_App::init() {
	return;
}

void Exec_App::loop(double secs, unsigned long frame) {
	return;
}

int main(int argc, char** argv) {
	{
		Exec_App testApp = Exec_App(argv[0], APP_DirectX_11);
	}

    Exec_App app = Exec_App(argv[0], APP_OpenGL_4);
	app.run();
    return 0;
}