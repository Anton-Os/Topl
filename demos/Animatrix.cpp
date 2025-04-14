#include "Animatrix.hpp"

void Animatrix_Demo::init(){
    Platform::keyControl.addHandler(std::bind(&Animatrix_Demo::onAnyKey, this, std::placeholders::_1));

    isEnable_background = false;

    for(unsigned p = 0; p < ANIMATRIX_PUPPETS; p++){
        puppets[p].configure(&scene2D);
        puppets[p].shift(Vec3f({ -(2.5F / ANIMATRIX_PUPPETS) + ((2.5F / ANIMATRIX_PUPPETS) * p), 0.0F, 0.0F }));
        //for(unsigned a = 0; a < puppets[p].getActorCount(); a++)
        //    _texVShader.setParams(puppets[p].getGeoActor(a), { (unsigned)FLIP_None, 0.0F, Vec3f({ 0.0F, 0.0F, 0.0F }), Vec3f({ 1.0F, 1.0F, 1.0F }) });
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
        case 'a': for(unsigned p = 0; p < ANIMATRIX_PUPPETS; p++) scene2D.addForce(puppets[p].getGeoActor(PUPPET_Body)->getName(), Vec3f({ -1.0F, 0.0F, 0.0F }) * Topl_Program::speed * 100.0F); break;
        case 's': for(unsigned p = 0; p < ANIMATRIX_PUPPETS; p++) scene2D.addForce(puppets[p].getGeoActor(PUPPET_Body)->getName(), Vec3f({ 0.0F, -1.0F, 0.0F }) * Topl_Program::speed * 100.0F); break;
        case 'd': for(unsigned p = 0; p < ANIMATRIX_PUPPETS; p++) scene2D.addForce(puppets[p].getGeoActor(PUPPET_Body)->getName(), Vec3f({ 1.0F, 0.0F, 0.0F }) * Topl_Program::speed * 100.0F); break;
        case 'w': for(unsigned p = 0; p < ANIMATRIX_PUPPETS; p++) scene2D.addForce(puppets[p].getGeoActor(PUPPET_Body)->getName(), Vec3f({ 0.0F, 1.0F, 0.0F }) * Topl_Program::speed * 100.0F); break;
    }
}

void Animatrix_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex){
    if(menu == PROGRAM_AppBar)
        switch(paneIndex){
            // TODO: Set parameters appropriately
        }
}

MAIN_ENTRY {
    Animatrix = new Animatrix_Demo(argv[0], BACKEND_DX11);
    Animatrix->run();

    delete(Animatrix);
    return 0;
}