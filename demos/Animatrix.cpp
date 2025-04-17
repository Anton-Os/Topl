#include "Animatrix.hpp"

void Animatrix_Demo::init(){
    Platform::keyControl.addHandler(std::bind(&Animatrix_Demo::onAnyKey, this, std::placeholders::_1));

    Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Perspective, 4.0F).genProjMatrix());

    for(unsigned p = 0; p < ANIMATRIX_PUPPETS; p++){
        puppets[p].configure(&scene2D);
        puppets[p].shift(Vec3f({ -(2.5F / ANIMATRIX_PUPPETS) + ((2.5F / ANIMATRIX_PUPPETS) * p), (p % 2 == 1)? -0.5F : 0.5F, 0.0F }));
        // anchors.push_back(Phys_Connector(Vec3f(0.0F, 0.0F, 0.0F), *puppets[p].getGeoActor(PUPPET_Body)->getPos()));
        // scene2D.addAnchor(&anchors.back(), puppets[p].getGeoActor(PUPPET_Body)->getName(), nullptr);
    }

    floor.setPos({ 0.0F, -1.5F, 0.0F });
    floor.updateSize({ ANIMATRIX_FLOOR_SIZE, 0.0F, 0.0F });
    for(unsigned m = 0; m < ANIMATRIX_MODELS; m++) models[m].shift(Vec3f({ 
        ((m % 2 == 0)? -ANIMATRIX_FLOOR_SIZE : ANIMATRIX_FLOOR_SIZE) * 0.25F, 0.0F, (((m / 2) % 2 == 0)? -ANIMATRIX_FLOOR_SIZE : ANIMATRIX_FLOOR_SIZE) * 0.25F
    }));

    scene3D.addGeometry(&floor);
    for(unsigned m = 0; m < ANIMATRIX_MODELS; m++) models[m].configure(&scene3D);

    _renderer->buildScene(&scene2D);
    _renderer->buildScene(&scene3D);

    _renderer->texturizeScene(&scene2D);
    _renderer->texturizeScene(&scene3D);

    shaderMode = 0;
}

void Animatrix_Demo::loop(double frameTime){
    scene2D.resolvePhysics(FORCE_Directional);
    for(unsigned m = 0; m < ANIMATRIX_MODELS; m++) models[m].rotateAll({ 0.0F, 0.0F, (float)frameTime / 1000.0F });

    renderScene(&scene2D, _texPipeline, TEX_BASE); 
    renderScene(&scene3D, _beamsPipeline, Topl_Program::shaderMode); 
}

void Animatrix_Demo::onAnyKey(char key){
    switch(tolower(key)){
        case 'd': for(unsigned p = 0; p < ANIMATRIX_PUPPETS; p++) scene2D.addForce(puppets[p].getGeoActor(PUPPET_Body)->getName(), Vec3f({ 1.0F, 0.0F, 0.0F }) * Topl_Program::speed * 5000.0F); break;
        case 'w': for(unsigned p = 0; p < ANIMATRIX_PUPPETS; p++) scene2D.addForce(puppets[p].getGeoActor(PUPPET_Body)->getName(), Vec3f({ 0.0F, 1.0F, 0.0F }) * Topl_Program::speed * 5000.0F); break;
        case 'a': for(unsigned p = 0; p < ANIMATRIX_PUPPETS; p++) scene2D.addForce(puppets[p].getGeoActor(PUPPET_Body)->getName(), Vec3f({ -1.0F, 0.0F, 0.0F }) * Topl_Program::speed * 5000.0F); break;
        case 's': for(unsigned p = 0; p < ANIMATRIX_PUPPETS; p++) scene2D.addForce(puppets[p].getGeoActor(PUPPET_Body)->getName(), Vec3f({ 0.0F, -1.0F, 0.0F }) * Topl_Program::speed * 5000.0F); break;
    }
}

void Animatrix_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex){
    for(unsigned p = 0; p < ANIMATRIX_PUPPETS; p++)
        if(menu == PROGRAM_AppBar)
            for(unsigned b = 0; b < PUPPET_PARTS; b++) puppets[p].getPhysActor(b)->mass = (PHYS_DEFAULT_MASS * (paneIndex + 1)) * 0.33F; // 1.0 + (paneIndex * 3.0);
        else if(menu == PROGRAM_Sculpt)
            for(unsigned l = 0; l < PUPPET_LINKS; l++) puppets[p].getPhysLink(l)->kVal = (PHYS_DEFAULT_K / 5) * (paneIndex + 1);
}

MAIN_ENTRY {
    Animatrix = new Animatrix_Demo(argv[0], BACKEND_GL4);
    Animatrix->run();

    delete(Animatrix);
    return 0;
}