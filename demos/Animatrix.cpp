#include "Animatrix.hpp"

void Animatrix_Demo::init(){
    Platform::keyControl.addHandler(std::bind(&Animatrix_Demo::onAnyKey, this, std::placeholders::_1));

    // Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Perspective, 5.0F).genProjMatrix());

    for(unsigned p = 0; p < ANIMATRIX_PUPPETS; p++){
        puppets[p].configure(&scene2D);
        puppets[p].shift(Vec3f({ -(2.5F / ANIMATRIX_PUPPETS) + ((2.5F / ANIMATRIX_PUPPETS) * p), (p % 2 == 1)? -0.5F : 0.5F, 0.0F }));
        // anchors.push_back(Phys_Connector(Vec3f(0.0F, 0.0F, 0.0F), *puppets[p].getGeoActor(PUPPET_Body)->getPos()));
        // scene2D.addAnchor(&anchors.back(), puppets[p].getGeoActor(PUPPET_Body)->getName(), nullptr);
    }

    _renderer->buildScene(&scene2D);
    // _renderer->buildScene(&scene3D);

    _renderer->texturizeScene(&scene2D);
    // _renderer->texturizeScene(&scene3D);

    shaderMode = 0;
}

void Animatrix_Demo::loop(double frameTime){
    // if(_renderer->getFrameCount() > 300 && _renderer->getFrameCount() % 120 == 0)
    //    for(unsigned p = 0; p < ANIMATRIX_PUPPETS; p++)
    //        scene2D.addForce(puppets[p].getGeoActor(PUPPET_Body)->getName(), (VEC_3F_RAND - Vec3f({ 0.5F, 0.5F, 0.5F})) * 50.0F);

    scene2D.resolvePhysics(FORCE_Directional);

    renderScene(&scene2D, nullptr, Topl_Program::shaderMode); // _texPipeline, TEX_BASE); 
    // renderScene(&scene3D, _materialPipeline, Topl_Program::mode); 
}

void Animatrix_Demo::onAnyKey(char key){
    switch(tolower(key)){
        case 'd': for(unsigned p = 0; p < ANIMATRIX_PUPPETS; p++) scene2D.addForce(puppets[p].getGeoActor(PUPPET_Body)->getName(), Vec3f({ -1.0F, 0.0F, 0.0F }) * Topl_Program::speed * 1000.0F); break;
        case 'w': for(unsigned p = 0; p < ANIMATRIX_PUPPETS; p++) scene2D.addForce(puppets[p].getGeoActor(PUPPET_Body)->getName(), Vec3f({ 0.0F, -1.0F, 0.0F }) * Topl_Program::speed * 1000.0F); break;
        case 'a': for(unsigned p = 0; p < ANIMATRIX_PUPPETS; p++) scene2D.addForce(puppets[p].getGeoActor(PUPPET_Body)->getName(), Vec3f({ 1.0F, 0.0F, 0.0F }) * Topl_Program::speed * 1000.0F); break;
        case 's': for(unsigned p = 0; p < ANIMATRIX_PUPPETS; p++) scene2D.addForce(puppets[p].getGeoActor(PUPPET_Body)->getName(), Vec3f({ 0.0F, 1.0F, 0.0F }) * Topl_Program::speed * 1000.0F); break;
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