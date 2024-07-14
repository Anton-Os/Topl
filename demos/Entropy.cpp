#include "Entropy.hpp"

static bool isInEntropy = true;
static bool isAllShown = true;
static unsigned short spawnIdx = 0;

static void onAnyKey(char key){
    // TODO: Switch the mode
}

static void spawnPress(float x, float y){
    if(spawnIdx < ENTROPIC_SPAWN){
        _DEMO->spawnActors[spawnIdx].isShown = true;
        _DEMO->spawnActors[spawnIdx].setPos(Topl_Program::getCamRelPos(nullptr));
        _DEMO->spawnActors[spawnIdx].setRot(VEC_3F_ZERO);
        _DEMO->spawnActors[spawnIdx].setSize({ 0.025F, 0.025F, 0.025F });
        spawnIdx++;
    }
}

void entropyReset(){ isInEntropy = !isInEntropy;  }
void showAll(){ isAllShown = !isAllShown; }

void Entropy_Demo::init(){
    Platform::mouseControl.addCallback(MOUSE_RightBtn_Press, spawnPress);
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
        actors[a].setPos({ (float)rand() / (float)RAND_MAX - 0.5f, (float)rand() / (float)RAND_MAX - 0.5f, 0.0 });
        actors[a].setRot({ (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, });
        actors[a].setSize({ ((float)rand() / (float)RAND_MAX) * 0.05F, ((float)rand() / (float)RAND_MAX) * 0.05F, ((float)rand() / (float)RAND_MAX) * 0.05F });
        scene.addGeometry("actor" + std::to_string(a), &actors[a]);
#ifdef TOPL_ENABLE_PHYSICS
        // physActors[a].damping = 0.995;
        // physActors[a] = Phys_Actor(Vec3f({ 0.0F, -0.1F, 0.0F }));
        scene.addPhysics("actor" + std::to_string(a), &physActors[a]);
#endif
    }
    _renderer->buildScene(&scene);

    for(unsigned s = 0; s < ENTROPIC_SPAWN; s++){
        spawnActors[s] = Geo_Actor(&spheres[s]);
        spawnActors[s].isShown = false;

        spawnScene.addGeometry("Spawn" + std::to_string(s), &spawnActors[s]);
    }

    _renderer->buildScene(&spawnScene);
}

void Entropy_Demo::loop(double frameTime){
    for(unsigned a = 0; a < ENTROPIC_COUNT; a++) {
#ifdef TOPL_ENABLE_PHYSICS
        // physActors[a].addForce({((float)rand() / (float)RAND_MAX - 0.5f), ((float)rand() / (float)RAND_MAX - 0.5f), 0.0F });
        
        if((rand() / (float)RAND_MAX) < 0.05)
            scene.addForce("actor" + std::to_string(a), {
                ((float)rand() / (float)RAND_MAX - 0.5f) * 300000.0F, 
                ((float)rand() / (float)RAND_MAX - 0.5f) * 300000.0F, 
                0.0F 
            });
        scene.resolvePhysics();
#else
        actors[a].updatePos({((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F, ((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F, 0.0F });
        actors[a].updatePos({ *actors[a].getPos() * ((isInEntropy)? 0.0035F : -0.0035F) });
        // actors[a].updateSize({((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F, ((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F, ((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F });
        actors[a].updateSize({ ((float)rand() / (float)RAND_MAX) * ((isInEntropy)? 0.00035F : -0.00035F), ((float)rand() / (float)RAND_MAX) * ((isInEntropy)? 0.00035F : -0.00035F), 0.0 });
        actors[a].updateRot({((float)rand() / (float)RAND_MAX) / 10.0F, ((float)rand() / (float)RAND_MAX) / 10.0F, ((float)rand() / (float)RAND_MAX) / 10.0F });
#endif
    }

    _flatVShader.setMode(-1);
    _renderer->updateScene(&scene);
    _renderer->drawScene(&scene);
    _renderer->clear();

    if(isAllShown) for(unsigned a = 0; a < ENTROPIC_COUNT; a++){
        _renderer->setDrawMode((a % 3 == 0)? DRAW_Strip : (a % 3 == 1)? DRAW_Lines : DRAW_Points);
        _renderer->draw(&actors[a]);
    }

    for(unsigned s = 0; s < spawnIdx; s++) spawnActors[s].updatePos({ *spawnActors[s].getPos() * ((isInEntropy)? 0.0035F : -0.0035F) });
    if(spawnIdx > 0){
        _renderer->setDrawMode(DRAW_Strip);
        _renderer->updateScene(&spawnScene);
        _renderer->drawScene(&spawnScene);
    }
}

int main(int argc, char** argv) {
    _DEMO = new Entropy_Demo(argv[0], BACKEND_DX11);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}