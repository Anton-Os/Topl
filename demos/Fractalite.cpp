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
    }
    _renderer->buildScene(&scene);
}

void Fractalite_Demo::loop(double frameTime){
    static Timer_Persist ticker = Timer_Persist();
    std::cout << "Frame time is: " << std::to_string(frameTime) << " ms" << std::endl;

    double t1 = ticker.getRelMillisecs();
    for(unsigned a = 0; a < FRACTALITE_COUNT; a++) {
        actors[a].updatePos({(static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f) / 100.0F, (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f) / 100.0F, 0.0F });
        actors[a].updateRot({(static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) / 10.0F, (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) / 10.0F, (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) / 10.0F });
    }

    double t2 = ticker.getRelMillisecs();
    _renderer->updateScene(&scene);
    double t3 = ticker.getRelMillisecs();
    _renderer->drawScene(&scene);
    double t4 = ticker.getRelMillisecs();

    std::cout << std::to_string(t1) << " ms on start, " << std::to_string(t2) << " ms on updatePos(), " << std::to_string(t3) << " ms on scene update, " << std::to_string(t4) << " ms on scene render" << std::endl;
}

int main(int argc, char** argv) {
    _instance = new Fractalite_Demo(argv[0], BACKEND_GL4);
    _instance->run();

    delete(_instance);
    return 0;
}