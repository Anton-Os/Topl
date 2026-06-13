#include "Molecular.hpp"

static unsigned sculptIndex = 0;

void Molecular_Demo::onAnyKey(keyboard_t key){
    Topl_Light* targetLight;
    if((Topl_Program::shaderMode / 10) % 3 == 0) targetLight = &skyLight;
    else if((Topl_Program::shaderMode / 10) % 3 == 1) targetLight = &flashLight;
    else targetLight = &lampLight;

    if (tolower(key) == 'w' || tolower(key) == 'a' || tolower(key) == 's' || tolower(key) == 'd') 
        for (unsigned m = 0; m < 3; m++)
            for (unsigned c = 0; c < MOLECULAR_CONSTRUCTS; c++)
                if(sculptIndex == c)
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
    if (menu == PROGRAM_Sculpt) {  //for(unsigned c = 0; c < MOLECULAR_CONSTRUCTS - 1; c++) construct_links[c].kVal = PHYS_DEFAULT_K / (paneIndex + 1);
        sculptIndex = paneIndex;
        for (unsigned m = 0; m < 3; m++)
            for (unsigned c = 0; c < MOLECULAR_CONSTRUCTS; c++) constructs[m][c].toggleShow(c == paneIndex);
    }
}

void Molecular_Demo::init(){
    isEnable_background = false;
    Platform::keyControl.addHandler(std::bind(&Molecular_Demo::onAnyKey, this, std::placeholders::_1));

    for(unsigned m = 0; m < 3; m++)
        for(unsigned c = 0; c < MOLECULAR_CONSTRUCTS; c++) {
            constructs[m][c].shift(Vec3f({ 
                ((float)rand() / (float)RAND_MAX - 0.5F) * 2,
                ((float)rand() / (float)RAND_MAX - 0.5F) * 2,
                ((float)rand() / (float)RAND_MAX - 0.5F)
            }));
            constructs[m][c].configure(&scene);
        }

    _renderer->buildScene(&scene);

    _renderer->isMeshUpdate = false;
}

void Molecular_Demo::loop(double frameTime){
    static double totalTime = 0.0;
    unsigned f = _renderer->getFrameCount();
#ifdef TOPL_ENABLE_PHYSICS
    // scene.resolvePhysics();
#endif
    _renderer->updateScene(&scene);
    _renderer->setDrawMode(DRAW_Lines);
    _renderer->drawScene(&scene);

    for(unsigned m = 0; m < 3; m++)
        for (unsigned c = 0; c < MOLECULAR_CONSTRUCTS; c++) { // constructs[m][c].rotate({ 0.0F, 0.0F, (float)frameTime * 0.0000025F });
            constructs[m][c].expand(((f / 60) % 2 == 0 ? -MOLECULAR_PULSE : MOLECULAR_PULSE) * frameTime);
            for (unsigned o = 0; o < constructs[m][c].getActorCount(); o++) {
                Geo_Actor* actor = constructs[m][c].getGeoActor(o);
                if (actor->getName().find("line") == std::string::npos) { // dont rotate the lines
                    float size = 1.0F + sin((float)(totalTime * 0.000002F + o - c)) * 0.5F;
                    actor->setSize({ size, size, size });
                }
                _renderer->update(actor);
                _renderer->draw(actor);
            }
        }
    totalTime += frameTime;
}

MAIN_ENTRY{
    Molecular = new Molecular_Demo(argv[0]);
    Molecular->run();

    delete(Molecular);
    return 0;
}
