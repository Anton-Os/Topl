#include "Entropy.hpp"

static bool isInEntropy = true;
static bool isAllShown = true;
// static short flatMode = -1;
static short beamMode = BEAMS_MODE_LIGHT;
static unsigned short spawnIdx = 0;

static void onAnyKey(char key){
    // if(isdigit(key)) flatMode = -(key - '0');
    if(isdigit(key)) beamMode = key - '0';
}

void entropyReset(){ isInEntropy = !isInEntropy;  }
void showAll(){ isAllShown = !isAllShown; }

void Entropy_Demo::init(){
    // Platform::mouseControl.addCallback(MOUSE_RightBtn_Press, spawnPress);
    Platform::keyControl.addAnyCallback(onAnyKey);
    Platform::keyControl.addCallback(' ', showAll);
    Topl_Program::timeline.persist_ticker.addPeriodicEvent(30000, entropyReset);
    Topl_Program::cameraObj.setZoom(2.0);

    backdropScene.addGeometry("Backdrop", &backdropActor);
    _renderer->buildScene(&backdropScene);

    for(unsigned a = 0; a < ENTROPIC_COUNT; a++){
        scene.addGeometry("actor" + std::to_string(a), &actors[a]);
#ifdef TOPL_ENABLE_PHYSICS
        // physActors[a].damping = 0.995;
        // physActors[a] = Phys_Actor(Vec3f({ 0.0F, -0.1F, 0.0F }));
        scene.addPhysics("actor" + std::to_string(a), &physActors[a]);
#endif
    }
    _renderer->buildScene(&scene);
}

void Entropy_Demo::loop(double frameTime){
    _effectVShader.setMode(EFFECT_MODE_CURSORY);
    Topl_Factory::switchPipeline(_renderer, _effectPipeline);
    _renderer->updateScene(&backdropScene);
    _renderer->drawScene(&backdropScene);

    for(unsigned a = 0; a < ENTROPIC_COUNT; a++) {
#ifdef TOPL_ENABLE_PHYSICS
        if((rand() / (float)RAND_MAX) < ENTROPIC_PROB && isInEntropy)
            scene.addForce("actor" + std::to_string(a), {
                ((float)rand() / (float)RAND_MAX - 0.5f) * ENTROPIC_FORCE, 
                ((float)rand() / (float)RAND_MAX - 0.5f) * ENTROPIC_FORCE, 
                0.0F 
            });
        else if((rand() / (float)RAND_MAX) < ENTROPIC_PROB && !isInEntropy)
            actors[a].updateRot({((float)rand() / (float)RAND_MAX), ((float)rand() / (float)RAND_MAX), ((float)rand() / (float)RAND_MAX) });
        // else if((rand() / (float)RAND_MAX) < ENTROPIC_PROB && !isInEntropy)
        //    actors[a].updateRot({(float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX), (float)rand() / (float)RAND_MAX });
        scene.resolvePhysics();
#else
        actors[a].updatePos({((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F, ((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F, 0.0F });
        actors[a].updatePos({ *actors[a].getPos() * ((isInEntropy)? 0.0035F : -0.0035F) });
        // actors[a].updateSize({((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F, ((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F, ((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F });
        actors[a].updateSize({ ((float)rand() / (float)RAND_MAX) * ((isInEntropy)? 0.00035F : -0.00035F), ((float)rand() / (float)RAND_MAX) * ((isInEntropy)? 0.00035F : -0.00035F), 0.0 });
        actors[a].updateRot({((float)rand() / (float)RAND_MAX) / 10.0F, ((float)rand() / (float)RAND_MAX) / 10.0F, ((float)rand() / (float)RAND_MAX) / 10.0F });
#endif
    }

    // _flatVShader.setMode(flatMode);
    _beamsVShader.setMode(beamMode);
    Topl_Factory::switchPipeline(_renderer, _beamsPipeline);
    _renderer->updateScene(&scene);
    _renderer->drawScene(&scene);
    // _renderer->clear();

    /* if(isAllShown) for(unsigned a = 0; a < ENTROPIC_COUNT; a++){
        _renderer->setDrawMode((a % 3 == 0)? DRAW_Strip : (a % 3 == 1)? DRAW_Lines : DRAW_Points);
        _renderer->draw(&actors[a]);
    } */
}

int main(int argc, char** argv) {
    _DEMO = new Entropy_Demo(argv[0], BACKEND_DX11);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}