#include "Entropy.hpp"

#define TOPL_ENABLE_PHYSICS

static bool isInEntropy = true;
static bool isAllShown = true;
static unsigned short spawnIdx = 0;

static void spawnPress(float x, float y){
    if(spawnIdx < ENTROPIC_SPAWN){
        _instance->spawnActors[spawnIdx] = Geo_Actor(&_instance->spheres[spawnIdx]);
        _instance->spawnActors[spawnIdx].setPos(Topl_Program::getCamCursorPos());
        _instance->spawnActors[spawnIdx].setRot(VEC_3F_ZERO);
        _instance->spawnActors[spawnIdx].setSize({ 0.025F, 0.025F, 0.025F });
        _instance->spawnScenes[spawnIdx].addGeometry(&_instance->spawnActors[spawnIdx]);
        spawnIdx++;
    }
}

void entropyReset(){ isInEntropy = !isInEntropy;  }
void showAll(){ isAllShown = !isAllShown; }

void Entropy_Demo::init(){
    srand(time(NULL));

    Platform::mouseControl.addCallback(MOUSE_RightBtn_Down, spawnPress);
    Platform::keyControl.addCallback(' ', showAll);
    Topl_Program::timeline.persist_ticker.addPeriodicEvent(30000, entropyReset);
    Topl_Program::cameraObj.setZoom(2.0);

    for(unsigned a = 0; a < ENTROPIC_COUNT; a++){
        switch(a % 4){
            case 0: actors[a] = Geo_Actor(&triangles[a / 4]); break;
            case 1: actors[a] = Geo_Actor(&quads[a / 4]); break;
            case 2: actors[a] = Geo_Actor(&hexes[a / 4]); break;
            case 3: actors[a] = Geo_Actor(&circles[a / 4]); break;
        }
        actors[a].setName("actor" + std::to_string(a));
        actors[a].setPos({ static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f, static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f, 0.0 });
        actors[a].setRot({ static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX), static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX), static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX), });
        actors[a].setSize({ (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * 0.025F, (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * 0.025F, (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * 0.025F });
        scene.addGeometry("actor" + std::to_string(a), &actors[a]);
#ifdef TOPL_ENABLE_PHYSICS
        // physActors[a] = Phys_Actor(Vec3f({ 0.0F, -0.1F, 0.0F }));
        scene.addPhysics("actor" + std::to_string(a), &physActors[a]);
#endif
    }
    _renderer->buildScene(&scene);
}

void Entropy_Demo::loop(double frameTime){
    if(_instance->spawnIndex < spawnIdx){
        _renderer->buildScene(&_instance->spawnScenes[_instance->spawnIndex]);
        _instance->spawnIndex++;
    }

    for(unsigned a = 0; a < ENTROPIC_COUNT; a++) {
/* #ifdef TOPL_ENABLE_PHYSICS
        // physActors[a].addForce({(static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f), (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f), 0.0F });
        scene.addForce("actor" + std::to_string(a), {(static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f) * 1000.0F, (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f) * 1000.0F, 0.0F });
        scene.resolvePhysics();
#else */
        actors[a].updatePos({(static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f) / 100.0F, (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f) / 100.0F, 0.0F });
        actors[a].updatePos({ *actors[a].getPos() * ((isInEntropy)? 0.0035F : -0.0035F) });
        actors[a].updateRot({(static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) / 10.0F, (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) / 10.0F, (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) / 10.0F });
        // actors[a].updateSize({(static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f) / 100.0F, (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f) / 100.0F, (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f) / 100.0F });
        actors[a].updateSize({ (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * ((isInEntropy)? 0.00035F : -0.00035F), (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * ((isInEntropy)? 0.00035F : -0.00035F), 0.0 });
// #endif
    }

    flatVShader.setMode(FLAT_MODE_SOLID);
    _renderer->updateScene(&scene);
    _renderer->drawScene(&scene);
    _renderer->clear();

    if(isAllShown) for(unsigned a = 0; a < ENTROPIC_COUNT; a++){
        _renderer->setDrawMode((a % 3 == 0)? DRAW_Strip : (a % 3 == 1)? DRAW_Lines : DRAW_Points);
        _renderer->draw(&actors[a]);
    }

    for(unsigned s = 0; s < _instance->spawnIndex; s++){
        spawnActors[s].updatePos({ *spawnActors[s].getPos() * ((isInEntropy)? 0.0035F : -0.0035F) });

        _renderer->updateScene(&_instance->spawnScenes[s]);
        _renderer->setDrawMode((s % 3 == 0)? DRAW_Strip : (s % 3 == 1)? DRAW_Lines : DRAW_Points);
        _renderer->drawScene(&_instance->spawnScenes[s]);
    }
}

int main(int argc, char** argv) {
    _instance = new Entropy_Demo(argv[0], BACKEND_DX11);
    _instance->run();

    delete(_instance);
    return 0;
}