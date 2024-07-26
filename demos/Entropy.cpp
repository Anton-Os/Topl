#include "Entropy.hpp"

unsigned short Entropy_Demo::mode = 0;

static bool isInEntropy = true;
static short lightMode = 10;
static short shaderMode = BEAMS_MODE_LIGHT;

static void onAnyKey(char key){
    if(tolower(key) == 'v') lightMode = 30; // 0;
    else if(tolower(key) == 'b') lightMode = 40; // 10;
    else if(tolower(key) == 'n') lightMode = 50; // 20;
    else if(tolower(key) == 'm') lightMode = 60;
    else if(isdigit(key)) shaderMode = key - '0';
}

void entropyReset(){ isInEntropy = !isInEntropy;  }

void setScene1(){ Entropy_Demo::mode = 0; }
void setScene2(){ Entropy_Demo::mode = 1; }
void setScene3(){ Entropy_Demo::mode = 2; }

void Entropy_Demo::init(){
    // Platform::mouseControl.addCallback(MOUSE_RightBtn_Press, spawnPress);
    Platform::keyControl.addAnyCallback(onAnyKey);
    Platform::keyControl.addCallback('g', setScene1);
    Platform::keyControl.addCallback('h', setScene2);
    Platform::keyControl.addCallback('j', setScene3);

    Topl_Program::timeline.persist_ticker.addPeriodicEvent(30000, entropyReset);
    Topl_Program::cameraObj.setZoom(2.0);

    backdropScene.addGeometry("Backdrop", &backdropActor);
    _renderer->buildScene(&backdropScene);

    for(unsigned a = 0; a < ENTROPIC_COUNT; a++){
        scene1.addGeometry("actor" + std::to_string(a), &actors[a]);
#ifdef TOPL_ENABLE_PHYSICS
        physActors[a].damping = 0.995;
        physActors[a].mass *= actors[a].getSize()->data[0] / ENTROPIC_SIZE;
        scene1.addPhysics("actor" + std::to_string(a), &physActors[a]);
#endif
        // scene2.addGeometry("tessActor" + std::to_string(a), &tessActors[a]);
        // scene3.addGeometry("duplexActor" + std::to_string(a), &duplexActors[a]);
    }
    _renderer->buildScene(&scene1);
    // _renderer->buildScene(&scene2);
    // _renderer->buildScene(&scene3);
}

void Entropy_Demo::loop(double frameTime){
    _beamsVShader.setMode(shaderMode);
    Topl_Factory::switchPipeline(_renderer, _beamsPipeline);
    _renderer->updateScene(&backdropScene);
    _renderer->drawScene(&backdropScene);

    for(unsigned a = 0; a < ENTROPIC_COUNT; a++) {
#ifdef TOPL_ENABLE_PHYSICS
        if((rand() / (float)RAND_MAX) < ENTROPIC_PROB)
            scene1.addForce("actor" + std::to_string(a), {
                ((float)rand() / (float)RAND_MAX - 0.5f) * ENTROPIC_FORCE + (actors[a].getPos()->data[0] * ((isInEntropy)? ENTROPIC_FORCE : -ENTROPIC_FORCE)), 
                ((float)rand() / (float)RAND_MAX - 0.5f) * ENTROPIC_FORCE + (actors[a].getPos()->data[1] * ((isInEntropy)? ENTROPIC_FORCE : -ENTROPIC_FORCE)), 
                0.0F 
            });
        if(physActors[a].actingForceCount > 0) actors[a].updateRot(*(physActors[a].forces));
        // Set size and mass based on distance
        scene1.resolvePhysics();
#else
        actors[a].updatePos({((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F, ((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F, 0.0F });
        actors[a].updatePos({ *actors[a].getPos() * ((isInEntropy)? 0.0035F : -0.0035F) });
        // actors[a].updateSize({((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F, ((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F, ((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F });
        actors[a].updateSize({ ((float)rand() / (float)RAND_MAX) * ((isInEntropy)? 0.00035F : -0.00035F), ((float)rand() / (float)RAND_MAX) * ((isInEntropy)? 0.00035F : -0.00035F), 0.0 });
        actors[a].updateRot({((float)rand() / (float)RAND_MAX) / 10.0F, ((float)rand() / (float)RAND_MAX) / 10.0F, ((float)rand() / (float)RAND_MAX) / 10.0F });
#endif
        tessActors[a].setPos(*actors[a].getPos());
        tessActors[a].setRot(*actors[a].getRot());
        tessActors[a].setSize(*actors[a].getSize());
        duplexActors[a].setPos(*actors[a].getPos());
        duplexActors[a].setRot(*actors[a].getRot());
        duplexActors[a].setSize(*actors[a].getSize());
    }

    // _flatVShader.setMode(flatMode);
    _beamsVShader.setMode(shaderMode + lightMode);
    _flatVShader.setMode(-shaderMode);
    Topl_Factory::switchPipeline(_renderer, _beamsPipeline);
    _renderer->updateScene(getScene());
    _renderer->drawScene(getScene());
    // _renderer->clear();
}

int main(int argc, char** argv) {
    _DEMO = new Entropy_Demo(argv[0], BACKEND_DX11);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}