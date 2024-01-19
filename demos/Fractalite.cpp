#include "Fractalite.hpp"

void Fractalite_Demo::init(){
    srand(time(NULL));

    for(unsigned a = 0; a < FRACTALITE_COUNT; a++){
        /* switch(a % 4){
            case 0: actors[a] = Geo_Actor(&triangles[0]); break;
            case 1: actors[a] = Geo_Actor(&quads[0]); break;
            case 2: actors[a] = Geo_Actor(&hexes[0]); break;
            case 3: actors[a] = Geo_Actor(&circles[0]); break;
        } */
        actors[a] = Geo_Actor(&circles[a]);
        actors[a].setName("actor" + std::to_string(a));
        actors[a].setPos({ static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f, static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f, 0.0 });
        actors[a].setRot({ static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f, static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f, 0.0, });
        scene.addGeometry(&actors[a]);
    }
    _renderer->buildScene(&scene);
}

void Fractalite_Demo::loop(double frameTime){
    _renderer->updateScene(&scene);
    _renderer->renderScene(&scene);
}

int main(int argc, char** argv) {
    _instance = new Fractalite_Demo(argv[0], BACKEND_GL4);
    _instance->run();

    delete(_instance);
    return 0;
}