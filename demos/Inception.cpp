#include "Inception.hpp"

void Inception_App::init() {
	return;
}

void Inception_App::loop(double secs, unsigned long frame) {
	return;
}

int main(int argc, char** argv) {
    Inception_App app = Inception_App(argv[0], APP_DirectX_11);
	app.run();
    return 0;
}