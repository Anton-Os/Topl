#include "Molecular.hpp"

void Molecular_Demo::onAnyKey(keyboard_t key){
    Topl_Light* targetLight;
    if((Topl_Program::shaderMode / 10) % 3 == 0) targetLight = &skyLight;
    else if((Topl_Program::shaderMode / 10) % 3 == 1) targetLight = &flashLight;
    else targetLight = &lampLight;

    if (tolower(key) == 'w' || tolower(key) == 'a' || tolower(key) == 's' || tolower(key) == 'd') 
        for (unsigned m = 0; m < 3; m++)
            for (unsigned c = 0; c < MOLECULAR_CONSTRUCTS; c++)
                switch (tolower(key)) {
                    case 'w': constructs[m][c].addForce(Vec3f({ 0.0F, MOLECULAR_FORCE, 0.0F })); break;
                    case 'a': constructs[m][c].addForce(Vec3f({ -MOLECULAR_FORCE, 0.0F, 0.0F })); break;
                    case 's': constructs[m][c].addForce(Vec3f({ 0.0F, -MOLECULAR_FORCE, 0.0F })); break;
                    case 'd': constructs[m][c].addForce(Vec3f({ MOLECULAR_FORCE, 0.0F, 0.0F })); break;
                }
    else if (tolower(key) == 'j') targetLight->pos = targetLight->pos + Vec3f({ 0.1F, 0.0F, 0.0F });
    else if(tolower(key) == 'b') targetLight->pos = targetLight->pos + Vec3f({ -0.1F, 0.0F, 0.0F });
    else if(tolower(key) == 'k') targetLight->pos = targetLight->pos + Vec3f({ 0.0F, 0.1F, 0.0F });
    else if(tolower(key) == 'n') targetLight->pos = targetLight->pos + Vec3f({ 0.0F, -0.1F, 0.0F });
    else if(tolower(key) == 'l') targetLight->pos = targetLight->pos + Vec3f({ 0.0F, 0.0F, 0.1F });
    else if(tolower(key) == 'm') targetLight->pos = targetLight->pos + Vec3f({ 0.0F, 0.0F, -0.1F });

    if((Topl_Program::shaderMode / 10) % 3 == 0) _beamsVShader.setLight(LIGHT_Sky, *targetLight);
    else if((Topl_Program::shaderMode / 10) % 3 == 1) _beamsVShader.setLight(LIGHT_Flash, *targetLight);
    else _beamsVShader.setLight(LIGHT_Lamp, *targetLight);
}

void Molecular_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex) {
    if (menu == PROGRAM_Sculpt) for(unsigned c = 0; c < MOLECULAR_CONSTRUCTS - 1; c++) construct_links[c].kVal = PHYS_DEFAULT_K / (paneIndex + 1);
}

void Molecular_Demo::init(){
    Platform::keyControl.addHandler(std::bind(&Molecular_Demo::onAnyKey, this, std::placeholders::_1));

    for(unsigned m = 0; m < 3; m++)
        for(unsigned c = 0; c < MOLECULAR_CONSTRUCTS; c++) {
            constructs[m][c].shift(Vec3f({ 
                ((float)rand() / (float)RAND_MAX - 0.5F) * MOLECULAR_SIZE,
                ((float)rand() / (float)RAND_MAX - 0.5F) * MOLECULAR_SIZE,
                ((float)rand() / (float)RAND_MAX - 0.5F) * MOLECULAR_SIZE 
            }));
            constructs[m][c].configure(&scene);
            /* if (c > 0) {
                construct_links[c - 1].preset(constructs[m][c - 1].getOrigin(), constructs[m][c].getOrigin());
                construct_links[c].kVal = PHYS_DEFAULT_K / 10;
                scene.addLink(&construct_links[c - 1], constructs[m][c - 1].getPrefix() + "hub",  constructs[m][c].getPrefix() + "hub");
            } */
        }

    _renderer->buildScene(&scene);

    _renderer->isMeshUpdate = false;
}

void Molecular_Demo::loop(double frameTime){
#ifdef TOPL_ENABLE_PHYSICS
    scene.resolvePhysics();
#endif
    for(unsigned m = 0; m < 3; m++)
        for(unsigned c = 0; c < MOLECULAR_CONSTRUCTS; c++) 
            constructs[m][c].rotate({ ((float)rand() / (float)RAND_MAX - 0.5F) / 100.0F, ((float)rand() / (float)RAND_MAX - 0.5F) / 100.0F, 0.0F });

    _renderer->updateScene(&scene);
    _renderer->setDrawMode(DRAW_Lines);
    _renderer->drawScene(&scene);

    for(unsigned m = 0; m < 3; m++)
        for(unsigned c = 0; c < MOLECULAR_CONSTRUCTS; c++)
            for(unsigned o = 0; o < constructs[m][c].getActorCount(); o++)
                _renderer->draw(constructs[m][c].getGeoActor(o));
}

MAIN_ENTRY{
    Molecular = new Molecular_Demo(argv[0]);
    Molecular->run();

    delete(Molecular);
    return 0;
}
