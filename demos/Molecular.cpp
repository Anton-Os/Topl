#include "Molecular.hpp"

void Molecular_Demo::init(){
    for(unsigned c = 0; c < MOLECULAR_CONSTRUCTS; c++) {
        constructs[c].shift(Vec3f({ (float)rand() / (float)RAND_MAX - 0.5F, (float)rand() / (float)RAND_MAX - 0.5F, 0.0F }));
        constructs[c].configure(&scene);
    }

    _renderer->buildScene(&scene);
}

void Molecular_Demo::loop(double frameTime){
    scene.resolvePhysics();
    _renderer->updateScene(&scene);
    // _renderer->drawScene(&scene);
}

int main(int argc, char** argv) {
    _DEMO = new Molecular_Demo(argv[0], BACKEND_GL4);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}