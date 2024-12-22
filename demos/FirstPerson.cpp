#include "FirstPerson.hpp"

#define TOPL_ENABLE_PHYSICS

static Vec3f puppet1_forceVec = VEC_3F_ZERO, puppet2_forceVec = VEC_3F_ZERO, puppet3_forceVec = VEC_3F_ZERO;

void FirstPerson_Demo::onAnyKey(char key){
    switch(tolower(key)){
        case 'a': for(unsigned a = 0; a < 3; a++) _DEMO->anchorOffs[a].data[0] -= Topl_Program::speed * 2; break;
        case 's': for(unsigned a = 0; a < 3; a++) _DEMO->anchorOffs[a].data[1] -= Topl_Program::speed * 2; break;
        case 'd': for(unsigned a = 0; a < 3; a++) _DEMO->anchorOffs[a].data[0] += Topl_Program::speed * 2; break;
        case 'w': for(unsigned a = 0; a < 3; a++) _DEMO->anchorOffs[a].data[1] += Topl_Program::speed * 2; break;
    }
}

void FirstPerson_Demo::init(){
    Platform::keyControl.addHandler(std::bind(&FirstPerson_Demo::onAnyKey, this, std::placeholders::_1));

    Topl_Program::camera.setZoom(0.5F);
    Topl_Program::camera.setPos({ 0.0F, -0.5F, 10.0F });
    // Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Orthographic, 1.0, 1.0, 1.0, 1.0, 10.0, 10.0).genProjMatrix());
    // Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Perspective, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0).genProjMatrix());
    Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Perspective, 1.0F).genProjMatrix());

    _renderer->setPipeline(_texPipeline);

    scene3D.addGeometry("floor", &floor);
    scene3D.addTexture("floor", &floorTex);
    scene3D.addGeometry("roof", &roof);
    scene3D.addTexture("roof", &roofTex);
    for(unsigned short p = 0; p < 4; p++){ 
        scene3D.addGeometry("pillar" + std::to_string(p), &pillars[p]);
        scene3D.addTexture("pillar" + std::to_string(p), &pillarTex[p]);
    }
#ifdef TOPL_ENABLE_MODELS
    for(unsigned m = 0; m < 5; m++){ 
        models[m].configure(&scene3D);

        Rasteron_Image* image;
        switch(m){
            case 0: image = noiseImgOp_scratch({ 1024, 1024 }, { 16, 16, 0xFF00EE55, 0xFF555533 }); break;
            case 1: image = noiseImgOp_tiled({ 1024, 1024 }, { 16, 16, 0xFF44AACC, 0xFFAA44AA }); break;
            case 2: image = noiseImgOp_octave({ 1024, 1024 }, { 8, 8, 0xFF5500EE, 0xFF22AAAA }, 4); break;
            case 3: image = noiseImgOp_diff({ 1024, 1024 }, { 8, 8, 0xFF00FF00, 0xFF880088 }, 4); break;
            default: image = noiseImgOp_value({ 1024, 1024 }, { 8, 8, 0xFFD9A233, 0xFFEE7392 }); break;
        }
        modelTexs[m].setImage(image);
        for(unsigned n = 0; n < models[m].getActorCount(); n++) models[m].getImgAt(n)->setImage(image);
        scene3D.addTexture(std::to_string(m + 1), &modelTexs[m]);
        RASTERON_DEALLOC(image);
    }
#endif

    _renderer->buildScene(&scene3D);
    _renderer->texturizeScene(&scene3D);

    puppets[0].configure(&_DEMO->scene2D);
    puppets[1].configure(&_DEMO->scene2D);
    puppets[2].configure(&_DEMO->scene2D);

#ifdef TOPL_ENABLE_PHYSICS
    anchors[0].kVal *= 0.33;
    scene2D.addAnchor(&anchors[0], puppets[0].getGeoActor(PUPPET_Head)->getName(), &anchorOffs[0]);
    anchors[1].kVal *= 1.0;
    scene2D.addAnchor(&anchors[1], puppets[1].getGeoActor(PUPPET_Body)->getName(), &anchorOffs[1]);
    anchors[2].kVal *= 3.0;
    scene2D.addAnchor(&anchors[2], puppets[2].getGeoActor(PUPPET_LeftArm)->getName(), &anchorOffs[2]);
#endif

    Topl_Program::timeline.addSequence_vec3f(&puppet1_forceVec, std::make_pair(60.0, Vec3f({ 0.0F, 50.0F, 0.0F })));
    Topl_Program::timeline.addSequence_vec3f(&puppet2_forceVec, std::make_pair(60.0, Vec3f({ -50.0F, 0.0F, 0.0F })));
    Topl_Program::timeline.addSequence_vec3f(&puppet3_forceVec, std::make_pair(60.0, Vec3f({ 50.0F, 0.0F, 0.0F })));

    _renderer->buildScene(&scene2D);
    _renderer->texturizeScene(&scene2D);
}

void FirstPerson_Demo::loop(double frameTime){
    {
#ifdef TOPL_ENABLE_PHYSICS
        if(_renderer->getFrameCount() > 60) scene2D.resolvePhysics();
#endif
        _texVShader.setMode(0);
        Topl_Factory::switchPipeline(_renderer, _texPipeline);
        _renderer->setDrawMode(DRAW_Triangles);

        _renderer->updateScene(&scene2D);
        _renderer->drawScene(&scene2D);
    }


    {
        _flatVShader.setMode(FLAT_COORD);
        Topl_Factory::switchPipeline(_renderer, _texPipeline);
        // _beamsVShader.setMode((_renderer->getFrameCount() % 120 < 40)? 0 : (_renderer->getFrameCount() % 120 < 80)? 4 : 3);
#ifdef TOPL_ENABLE_MODELS
        models[0].rotateAll({ 0.0F, 0.0F, (_renderer->getFrameCount() % 120 < 60)? (float)frameTime / 1000.0F : (float)frameTime / -1000.0F });
        models[1].rotateAll({ 0.0F, 0.0F, (float)frameTime / -500.0F });
        models[2].rotateAll({ 0.0F, 0.0F, (float)frameTime / 500.0F });
        models[3].rotateAll({ 0.0F, 0.0F, (float)frameTime / -500.0F });
        models[4].rotateAll({ 0.0F, 0.0F, (float)frameTime / 500.0F });
        // for(unsigned m = 0; m < 5; m++)
        //    models[m].shift({ sin((float)Topl_Program::timeline.persist_ticker.getAbsSecs()) / 10.0F, 0.0F, cos((float)Topl_Program::timeline.persist_ticker.getAbsSecs()) / 10.0F });
#endif
        // _renderer->setDrawMode((_renderer->getFrameCount() % 180 > 120)? DRAW_Strip : (_renderer->getFrameCount() % 180 > 60)? DRAW_Lines : DRAW_Points);
        _texVShader.setMode(0);
        _renderer->setDrawMode(DRAW_Triangles);

        _texVShader.setTexScroll({ 0.0F, _renderer->getFrameCount() * 0.01F, 0.0F });
        _renderer->updateScene(&scene3D);
        _renderer->drawScene(&scene3D);

        _texVShader.setTexScroll({ 0.0F, 0.0F, 0.0F });
    }
}

MAIN_ENTRY {
    _DEMO = new FirstPerson_Demo(argv[0], BACKEND_GL4);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}
