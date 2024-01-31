#include "Fractalite.hpp"

void Fractalite_Demo::init(){
    srand(time(NULL));

    Topl_Program::cameraObj.setZoom(2.0);

    for(unsigned a = 0; a < FRACTALITE_COUNT; a++){
        switch(a % 4){
            case 0: actors[a] = Geo_Actor(&triangles[a / 4]); break;
            case 1: actors[a] = Geo_Actor(&quads[a / 4]); break;
            case 2: actors[a] = Geo_Actor(&hexes[a / 4]); break;
            case 3: actors[a] = Geo_Actor(&circles[a / 4]); break;
        }
        actors[a].setName("actor" + std::to_string(a));
        actors[a].setPos({ static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f, static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f, 0.0 });
        actors[a].setRot({ static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX), static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX), 0.0, });
        actors[a].setSize({ (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * 0.05F, (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * 0.05F, 1.0F });
        scene.addGeometry(&actors[a]);
#ifdef TOPL_ENABLE_PHYSICS
        scene.addPhysics("actor" + std::to_string(a), &physActors[a]);
#endif
    }
    _renderer->buildScene(&scene);
}

void Fractalite_Demo::loop(double frameTime){
    for(unsigned a = 0; a < FRACTALITE_COUNT; a++) {
#ifdef TOPL_ENABLE_PHYSICS
        // physActors[a].addForce({(static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f), (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f), 0.0F });
        scene.addForce("actor" + std::to_string(a), {(static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f), (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f), 0.0F });
        scene.resolvePhysics();
#else
        actors[a].updatePos({(static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f) / 100.0F, (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f) / 100.0F, 0.0F });
        actors[a].updateRot({(static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) / 10.0F, (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) / 10.0F, (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) / 10.0F });
#endif
    }
    _renderer->updateScene(&scene);
    _renderer->drawScene(&scene);
}

int main(int argc, char** argv) {
    _instance = new Fractalite_Demo(argv[0], BACKEND_GL4);
    _instance->run();

    delete(_instance);
    return 0;
}