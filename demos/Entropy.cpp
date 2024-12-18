#include "Entropy.hpp"

unsigned short Entropy_Demo::mode = 0;

static bool isInEntropy = true;
static bool isInMotion = true;
static short lightMode = 10;
static short shaderMode = 2;

void Entropy_Demo::onAnyKey(char key){
    // else if(isdigit(key)) Topl_Program::shaderMode = key - '0';
    if(isspace(key)) isInMotion = !isInMotion;
    else switch(tolower(key)){
        case 'j': Entropy_Demo::mode = 0; break;
        case 'k': Entropy_Demo::mode = 1; break;
        case 'l': Entropy_Demo::mode = 2; break;
        case 'b': Entropy_Demo::mode = 3; break;
        case 'n': Entropy_Demo::mode = 4; break;
        case 'm': Entropy_Demo::mode = 5; break;
    }
}

void entropyReset(){ isInEntropy = !isInEntropy; }

void Entropy_Demo::init(){
    // Platform::mouseControl.addCallback(MOUSE_RightBtn_Press, spawnPress);
    Platform::keyControl.addHandler(std::bind(&Entropy_Demo::onAnyKey, this, std::placeholders::_1));

    Topl_Program::timeline.persist_ticker.addPeriodicEvent(10000, entropyReset);
    Topl_Program::camera.setZoom(2.0);
    Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Orthographic, 2.0F).genProjMatrix());

    scene1.camera = &Topl_Program::camera; scene2.camera = &Topl_Program::camera; scene3.camera = &Topl_Program::camera;
    ext_scene1.camera = &Topl_Program::camera; ext_scene2.camera = &Topl_Program::camera; ext_scene3.camera = &Topl_Program::camera;

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

    _renderer->isMeshUpdate = false;
}

void Entropy_Demo::loop(double frameTime){
    for(unsigned a = 0; a < ENTROPIC_COUNT; a++) {
#ifdef TOPL_ENABLE_PHYSICS
        if((rand() / (float)RAND_MAX) < ENTROPIC_PROB && isInMotion)
            scene1.addForces("actor_surface" + std::to_string(a), {
                ((float)rand() / (float)RAND_MAX - 0.5f) * ENTROPIC_FORCE + (surface_actors[a].getPos()->data[0] * ((isInEntropy)? ENTROPIC_FORCE : -ENTROPIC_FORCE)), 
                ((float)rand() / (float)RAND_MAX - 0.5f) * ENTROPIC_FORCE + (surface_actors[a].getPos()->data[1] * ((isInEntropy)? ENTROPIC_FORCE : -ENTROPIC_FORCE)), 
                0.0F 
            }, VEC_3F_ZERO, VEC_3F_ZERO);
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

    _renderer->setDrawMode(DRAW_Triangles);
    _renderer->updateScene(getScene());
    _renderer->drawScene(getScene());
    _renderer->setDrawMode(DRAW_Points);
    _renderer->drawScene(getScene());
}

MAIN_ENTRY {
    _DEMO = new Entropy_Demo(argv[0], BACKEND_GL4);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}
