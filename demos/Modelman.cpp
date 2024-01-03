#include "Modelman.hpp"

void Modelman_Demo::init(){
    model.configure(&scene);
    _renderer->buildScene(&scene);
}

void Modelman_Demo::loop(double frameTime){
    _renderer->updateScene(&scene);
    _renderer->renderScene(&scene);
}

int main(int argc, char** argv) {
    _instance = new Modelman_Demo(argv[0], BACKEND_GL4);
    _instance->run();

    delete(_instance);
    return 0;
}