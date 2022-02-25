#include "Inception.hpp"

int main(int argc, char** argv) {
    Inception_App app = Inception_App(argv[0], APP_OpenGL_4);
	app.run();
    return 0;
}