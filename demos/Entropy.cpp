#include "Entropy.hpp"

static bool isInEntropy = true;
static bool isInMotion = true;
static short lightMode = 10;
static short shaderMode = 2;
static Vec3f* points = nullptr;

void entropyReset() { isInEntropy = !isInEntropy; }

void Entropy_Demo::onAnyKey(char key){ if(isspace(key)) isInMotion = !isInMotion; }

void Entropy_Demo::setInstances(Geo_Mesh* mesh) {
    float e = ENTROPIC_RECURSHAPE;
    mesh->setInstances({
        Mat4x4::translation({ e, 0.0F, 0.0F}), Mat4x4::translation({ 0.0F, e, 0.0F}), Mat4x4::translation({ 0.0F, 0.0F, e }),
        Mat4x4::translation({ -e, 0.0F, 0.0F}), Mat4x4::translation({ 0.0F, -e, 0.0F}), Mat4x4::translation({ 0.0F, 0.0F, -e }),
        Mat4x4::translation({ e, e, 0.0F}), Mat4x4::translation({ 0.0F, e, e}), Mat4x4::translation({ e, 0.0F, e }),
        Mat4x4::translation({ -e, -e, 0.0F}), Mat4x4::translation({ 0.0F, -e, -e}), Mat4x4::translation({ -e, 0.0F, -e }),
        Mat4x4::translation({ e, -e, 0.0F}), Mat4x4::translation({ 0.0F, e, -e}), Mat4x4::translation({ -e, 0.0F, e }),
        Mat4x4::translation({ -e, e, 0.0F}), Mat4x4::translation({ 0.0F, -e, e}), Mat4x4::translation({ e, 0.0F, -e }),
        Mat4x4::translation({ -e, e, e }), Mat4x4::translation({ e, -e, e}), Mat4x4::translation({ e, e, -e }),
        Mat4x4::translation({ -e, -e, e }), Mat4x4::translation({ -e, e, -e}), Mat4x4::translation({ e, -e, -e }),
        Mat4x4::translation({ -e, -e, -e}), Mat4x4::translation({ e, e, e })
    });
}

void Entropy_Demo::init(){
    // Platform::mouseControl.addCallback(MOUSE_RightBtn_Press, spawnPress);
    Platform::keyControl.addHandler(std::bind(&Entropy_Demo::onAnyKey, this, std::placeholders::_1));

    Topl_Program::timeline.persist_ticker.addPeriodicEvent(10000, entropyReset);
 
    for(unsigned a = 0; a < ENTROPIC_COUNT; a++){
        scenes[0].addGeometry("surface" + std::to_string(a), &surface_actors[a]);
#ifdef TOPL_ENABLE_PHYSICS
        scenes[0].addPhysics("surface" + std::to_string(a), &physActors[a]);
#endif
        scenes[1].addGeometry("conic" + std::to_string(a), &conic_actors[a]);
        scenes[2].addGeometry("volume" + std::to_string(a), &volume_actors[a]);
        scenes[3].addGeometry("surfaceExt" + std::to_string(a), &surfaceExt_actors[a]);
        scenes[4].addGeometry("conicExt" + std::to_string(a), &conicExt_actors[a]);
        scenes[5].addGeometry("volumeExt" + std::to_string(a), &volumeExt_actors[a]);
        scenes[6].addGeometry("surfacePt" + std::to_string(a), &surfacePt_actors[a]);
        scenes[7].addGeometry("conicPt" + std::to_string(a), &conicPt_actors[a]);
        scenes[8].addGeometry("volumePt" + std::to_string(a), &volumePt_actors[a]);
    }
    for(unsigned s = 0; s < ENTROPIC_SCENES; s++) _renderer->buildScene(&scenes[s]);

    free(points);
    _renderer->isMeshUpdate = true;
}

void Entropy_Demo::createGeometry(unsigned i) {
    /* setInstances(&trigs[i]); setInstances(&trigsCones[i]); setInstances(&trigs3D[i]);
    setInstances(&quads[i]); setInstances(&quadsCones[i]); setInstances(&quads3D[i]);
    setInstances(&hexes[i]); setInstances(&hexesCones[i]); setInstances(&hexes3D[i]);
    setInstances(&circles[i]); setInstances(&circlesCones[i]); setInstances(&circles3D[i]); */

    switch (i % 4) {
        case ENTROPIC_TRIG: surface_actors[i] = Geo_Actor(&trigs[i / 4]); conic_actors[i] = Geo_Actor(&trigsCones[i / 4]); volume_actors[i] = Geo_Actor(&trigs3D[i / 4]); break;
        case ENTROPIC_QUAD: surface_actors[i] = Geo_Actor(&quads[i / 4]); conic_actors[i] = Geo_Actor(&quadsCones[i / 4]); volume_actors[i] = Geo_Actor(&quads3D[i / 4]); break;
        case ENTROPIC_HEX: surface_actors[i] = Geo_Actor(&hexes[i / 4]); conic_actors[i] = Geo_Actor(&hexesCones[i / 4]); volume_actors[i] = Geo_Actor(&hexes3D[i / 4]); break;
        case ENTROPIC_CIRCLE: surface_actors[i] = Geo_Actor(&circles[i / 4]); conic_actors[i] = Geo_Actor(&circlesCones[i / 4]); volume_actors[i] = Geo_Actor(&circles3D[i / 4]); break;
    }

    float size = (float)rand() / (float)RAND_MAX * ENTROPIC_SIZE;
    surface_actors[i].setName("surface" + std::to_string(i));
    surface_actors[i].setPos({ (float)rand() / (float)RAND_MAX - 0.5f, (float)rand() / (float)RAND_MAX - 0.5f, 0.0 });
    surface_actors[i].setRot({ (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, });
    surface_actors[i].setSize({ (size > 0.01F) ? size : 0.01F, (size > 0.01F) ? size : 0.01F, (size > 0.01F) ? size : 0.01F });
}

void Entropy_Demo::createExtGeometry(unsigned i) {
    switch (i % 4) {
    case ENTROPIC_TRIG:
        surfaceMeshes.push_back(new Geo_Ext2D({ trigs[i / 4].getRadius(), trigs[i / 4].getSegments() }, DEFAULT_Z, ENTROPIC_ITERS));
        coneMeshes.push_back(new Geo_ExtCone({ trigs[i / 4].getRadius(), trigs[i / 4].getSegments() }, trigsCones[i / 4].getApex(), ENTROPIC_ITERS));
        volumeMeshes.push_back(new Geo_Ext3D({ trigs[i / 4].getRadius(), trigs[i / 4].getSegments() }, trigs3D[i / 4].getDepth(), ENTROPIC_ITERS));
        break;
    case ENTROPIC_QUAD:
        surfaceMeshes.push_back(new Geo_Ext2D({ quads[i / 4].getRadius(), quads[i / 4].getSegments() }, DEFAULT_Z, ENTROPIC_ITERS));
        coneMeshes.push_back(new Geo_ExtCone({ quads[i / 4].getRadius(), quads[i / 4].getSegments() }, quadsCones[i / 4].getApex(), ENTROPIC_ITERS));
        volumeMeshes.push_back(new Geo_Ext3D({ quads[i / 4].getRadius(), quads[i / 4].getSegments() }, quads3D[i / 4].getDepth(), ENTROPIC_ITERS));
        break;
    case ENTROPIC_HEX:
        surfaceMeshes.push_back(new Geo_Ext2D({ hexes[i / 4].getRadius(), hexes[i / 4].getSegments() }, DEFAULT_Z, ENTROPIC_ITERS));
        coneMeshes.push_back(new Geo_ExtCone({ hexes[i / 4].getRadius(), hexes[i / 4].getSegments() }, hexesCones[i / 4].getApex(), ENTROPIC_ITERS));
        volumeMeshes.push_back(new Geo_Ext3D({ hexes[i / 4].getRadius(), hexes[i / 4].getSegments() }, hexes3D[i / 4].getDepth(), ENTROPIC_ITERS));
        break;
    case ENTROPIC_CIRCLE:
        surfaceMeshes.push_back(new Geo_Ext2D({ circles[i / 4].getRadius(), circles[i / 4].getSegments() }, DEFAULT_Z, ENTROPIC_ITERS));
        coneMeshes.push_back(new Geo_ExtCone({ circles[i / 4].getRadius(), circles[i / 4].getSegments() }, circlesCones[i / 4].getApex(), ENTROPIC_ITERS));
        volumeMeshes.push_back(new Geo_Ext3D({ circles[i / 4].getRadius(), circles[i / 4].getSegments() }, circles3D[i / 4].getDepth(), ENTROPIC_ITERS));
        break;
    }

    surfaceExt_actors[i] = Geo_Actor(surfaceMeshes.back());
    conicExt_actors[i] = Geo_Actor(coneMeshes.back());
    volumeExt_actors[i] = Geo_Actor(volumeMeshes.back());
}

void Entropy_Demo::createPtGeometry(unsigned i) {
    if (points == nullptr) {
        points = (Vec3f*)malloc(ENTROPIC_POINTS * sizeof(Vec3f));
        for (unsigned p = 0; p < ENTROPIC_POINTS; p++)
            *(points + p) = (VEC_3F_RAND - Vec3f({ 0.5F, 0.5F, 0.5F })) * ENTROPIC_SPREAD;
    }

    surfacePtMeshes.push_back(new Geo_Surface((vertex_cptr_t)points, (unsigned short)ENTROPIC_POINTS, DEFAULT_Z));
    conePtMeshes.push_back(new Geo_Cone((vertex_cptr_t)points, (unsigned short)ENTROPIC_POINTS, Vec3f({ 0.0F, 0.0F, 0.5F })));
    volumePtMeshes.push_back(new Geo_Volume((vertex_cptr_t)points, (unsigned short)ENTROPIC_POINTS, DEFAULT_Z));

    surfacePt_actors[i] = Geo_Actor(surfacePtMeshes.back());
    conicPt_actors[i] = Geo_Actor(conePtMeshes.back());
    volumePt_actors[i] = Geo_Actor(volumePtMeshes.back());
}

void Entropy_Demo::loop(double frameTime){
    for(unsigned a = 0; a < ENTROPIC_COUNT; a++) {
#ifdef TOPL_ENABLE_PHYSICS
        Vec3f force = {
            ((float)rand() / (float)RAND_MAX - 0.5f) * ENTROPIC_FORCE + (surface_actors[a].getPos()->data[0] * ((isInEntropy)? ENTROPIC_FORCE : -ENTROPIC_FORCE)),
            ((float)rand() / (float)RAND_MAX - 0.5f) * ENTROPIC_FORCE + (surface_actors[a].getPos()->data[1] * ((isInEntropy)? ENTROPIC_FORCE : -ENTROPIC_FORCE)),
            0.0F
        };
        if((rand() / (float)RAND_MAX) < ENTROPIC_PROB && isInMotion) scene1.addForce("surface" + std::to_string(a), force);
        // if((rand() / (float)RAND_MAX) < ENTROPIC_PROB && isInMotion) scene1.addForces("surface" + std::to_string(a), force, force, VEC_3F_ZERO);
        if((rand() / (float)RAND_MAX) < ENTROPIC_PROB && isInMotion) scene1.addForces("surface" + std::to_string(a), force, force, force);
        // if(physActors[a].actingForceCount > 0) surface_actors[a].updateRot(*(physActors[a].forces));
        scene1.resolvePhysics(FORCE_Directional);
#else
        surface_actors[a].updatePos({((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F, ((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F, 0.0F });
        surface_actors[a].updatePos({ *surface_actors[a].getPos() * ((isInEntropy)? 0.0035F : -0.0035F) });
        // surface_actors[a].updateSize({((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F, ((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F, ((float)rand() / (float)RAND_MAX - 0.5f) / 100.0F });
        surface_actors[a].updateSize({ ((float)rand() / (float)RAND_MAX) * ((isInEntropy)? 0.00035F : -0.00035F), ((float)rand() / (float)RAND_MAX) * ((isInEntropy)? 0.00035F : -0.00035F), 0.0 });
        surface_actors[a].updateRot({((float)rand() / (float)RAND_MAX) / 10.0F, ((float)rand() / (float)RAND_MAX) / 10.0F, ((float)rand() / (float)RAND_MAX) / 10.0F });
#endif
        conic_actors[a].copyParams(surface_actors[a]);
        volume_actors[a].copyParams(surface_actors[a]);
        surfaceExt_actors[a].copyParams(surface_actors[a]);
        conicExt_actors[a].copyParams(surface_actors[a]);
        volumeExt_actors[a].copyParams(surface_actors[a]);
        surfacePt_actors[a].copyParams(surface_actors[a]);
        conicPt_actors[a].copyParams(surface_actors[a]);
        volumePt_actors[a].copyParams(surface_actors[a]);
    }

    _renderer->setDrawMode(DRAW_Triangles);
    _renderer->updateScene(getScene());
    _renderer->drawScene(getScene());
    _renderer->setDrawMode(DRAW_Points);
    _renderer->drawScene(getScene());
}

MAIN_ENTRY {
    Meshform = new Entropy_Demo(argv[0], BACKEND_DX11);
    Meshform->run();

    delete(Meshform);
    return 0;
}
