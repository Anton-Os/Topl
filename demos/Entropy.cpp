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

void setScene1(){ Entropy_Demo::mode = 0; } // surface
void setScene2(){ Entropy_Demo::mode = 1; } // conic
void setScene3(){ Entropy_Demo::mode = 2; } // volumetric
void setScene4(){ Entropy_Demo::mode = 3; } // extended surface
void setScene5(){ Entropy_Demo::mode = 4; } // extended conic
void setScene6(){ Entropy_Demo::mode = 5; } // extended volumetric


void Entropy_Demo::init(){
    // Platform::mouseControl.addCallback(MOUSE_RightBtn_Press, spawnPress);
    Platform::keyControl.addAnyCallback(onAnyKey);
    Platform::keyControl.addCallback('g', setScene1);
    Platform::keyControl.addCallback('h', setScene2);
    Platform::keyControl.addCallback('j', setScene3);
    Platform::keyControl.addCallback('b', setScene4);
    Platform::keyControl.addCallback('n', setScene5);
    Platform::keyControl.addCallback('m', setScene6);

    Topl_Program::timeline.persist_ticker.addPeriodicEvent(30000, entropyReset);
    Topl_Program::cameraObj.setZoom(2.0);

    backdropScene.addGeometry("Backdrop", &backdropActor);
    _renderer->buildScene(&backdropScene);

    for(unsigned a = 0; a < ENTROPIC_COUNT; a++){
        scene1.addGeometry("actor_surface" + std::to_string(a), &surface_actors[a]);
#ifdef TOPL_ENABLE_PHYSICS
        physActors[a].damping = 0.995;
        // physActors[a].mass *= surface_actors[a].getSize()->data[0] / ENTROPIC_SIZE;
        scene1.addPhysics("actor_surface" + std::to_string(a), &physActors[a]);
#endif
        scene2.addGeometry("actor_conic" + std::to_string(a), &conic_actors[a]);
        scene3.addGeometry("actor_volume" + std::to_string(a), &volume_actors[a]);
        ext_scene1.addGeometry("actor_surfaceExt" + std::to_string(a), &surfaceExt_actors[a]);
        ext_scene2.addGeometry("actor_conicExt" + std::to_string(a), &conicExt_actors[a]);
        ext_scene3.addGeometry("actor_volumeExt" + std::to_string(a), &volumeExt_actors[a]);
    }
    _renderer->buildScene(&scene1); _renderer->buildScene(&scene2); _renderer->buildScene(&scene3);
    _renderer->buildScene(&ext_scene1); _renderer->buildScene(&ext_scene2); _renderer->buildScene(&ext_scene3);
}

void Entropy_Demo::loop(double frameTime){
    scene1.camera = &Topl_Program::cameraObj; scene2.camera = &Topl_Program::cameraObj; scene3.camera = &Topl_Program::cameraObj;
    ext_scene1.camera = &Topl_Program::cameraObj; ext_scene2.camera = &Topl_Program::cameraObj; ext_scene3.camera = &Topl_Program::cameraObj;

    _beamsVShader.setMode((shaderMode + lightMode) * -1);
    _effectVShader.setMode(-1);
    Topl_Factory::switchPipeline(_renderer, _effectPipeline);
    _renderer->updateScene(&backdropScene);
    _renderer->drawScene(&backdropScene);

    for(unsigned a = 0; a < ENTROPIC_COUNT; a++) {
#ifdef TOPL_ENABLE_PHYSICS
        if((rand() / (float)RAND_MAX) < ENTROPIC_PROB)
            scene1.addForce("actor_surface" + std::to_string(a), {
                ((float)rand() / (float)RAND_MAX - 0.5f) * ENTROPIC_FORCE + (surface_actors[a].getPos()->data[0] * ((isInEntropy)? ENTROPIC_FORCE : -ENTROPIC_FORCE)), 
                ((float)rand() / (float)RAND_MAX - 0.5f) * ENTROPIC_FORCE + (surface_actors[a].getPos()->data[1] * ((isInEntropy)? ENTROPIC_FORCE : -ENTROPIC_FORCE)), 
                0.0F 
            });
        if(physActors[a].actingForceCount > 0) surface_actors[a].updateRot(*(physActors[a].forces));
        // Set size and mass based on distance
        scene1.resolvePhysics();
#else
        surface_actors[a].updatePos({((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F, ((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F, 0.0F });
        surface_actors[a].updatePos({ *surface_actors[a].getPos() * ((isInEntropy)? 0.0035F : -0.0035F) });
        // surface_actors[a].updateSize({((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F, ((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F, ((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F });
        surface_actors[a].updateSize({ ((float)rand() / (float)RAND_MAX) * ((isInEntropy)? 0.00035F : -0.00035F), ((float)rand() / (float)RAND_MAX) * ((isInEntropy)? 0.00035F : -0.00035F), 0.0 });
        surface_actors[a].updateRot({((float)rand() / (float)RAND_MAX) / 10.0F, ((float)rand() / (float)RAND_MAX) / 10.0F, ((float)rand() / (float)RAND_MAX) / 10.0F });
#endif
        conic_actors[a].setPropsTo(surface_actors[a]);
        volume_actors[a].setPropsTo(surface_actors[a]);
        surfaceExt_actors[a].setPropsTo(surface_actors[a]);
        conicExt_actors[a].setPropsTo(surface_actors[a]);
        volumeExt_actors[a].setPropsTo(surface_actors[a]);
    }

    // _flatVShader.setMode(flatMode);
    _beamsVShader.setMode(shaderMode + lightMode);
    _flatVShader.setMode(8);
    Topl_Factory::switchPipeline(_renderer, _flatPipeline);
    _renderer->updateScene(getScene());
    _renderer->drawScene(getScene());
    // _renderer->clear();
}

MAIN_ENTRY {
    _DEMO = new Entropy_Demo(argv[0], BACKEND_GL4);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}
