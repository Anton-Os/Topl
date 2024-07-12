#include "MathArt.hpp"

void MathArt::init(){
    for(unsigned s = 0; s < MATHART_SLICES; s++){
        scene.addGeometry("circle" + std::to_string(s), circleActors[s]);
        // TODO: Add texture?
    }

    _renderer.buildScene(&scene);
}

void MathArt::loop(double frameTime){
    // TODO: Add loop logic
}

int main(int argc, char** argv) {
    _DEMO = new MathArt_Demo(argv[0], BACKEND_GL4);

    _DEMO->run();
    delete(_DEMO);
    return 0;
}