#include "Molecular.hpp"

static short lightMode = 1;

static void onAnyKey(char key){
    if(tolower(key) == 'v') lightMode = 0;
    else if(tolower(key) == 'b') lightMode = 1;
    else if(tolower(key) == 'n') lightMode = 2;
    else if(tolower(key) == 'm') lightMode = 3;
}

void Molecular_Demo::init(){
    Platform::keyControl.addAnyCallback(onAnyKey);

    for(unsigned m = 0; m < 3; m++)
        for(unsigned c = 0; c < MOLECULAR_CONSTRUCTS; c++) {
            constructs[m][c].shift(Vec3f({ (float)rand() / (float)RAND_MAX - 0.5F, (float)rand() / (float)RAND_MAX - 0.5F - (m * 0.5F + 1.0F), 0.0F }));
            constructs[m][c].configure(&scene);
            if(c < MOLECULAR_CONSTRUCTS - 1){
                construct_links[c].preset(constructs[m][c].getOrigin(), constructs[m][c + 1].getOrigin());
                scene.addLink(&construct_links[c], constructs[m][c].getPrefix() + "hub",  constructs[m][c + 1].getPrefix() + "hub");
            }
        }

    _renderer->buildScene(&scene);

    _renderer->isMeshUpdate = false;
}

void Molecular_Demo::loop(double frameTime){
    scene.camera = &Topl_Program::cameraObj;

    for(unsigned m = 0; m < 3; m++)
        for(unsigned c = 0; c < MOLECULAR_CONSTRUCTS; c++) 
            constructs[m][c].rotate({ ((float)rand() / (float)RAND_MAX - 0.5F) / 100.0F, ((float)rand() / (float)RAND_MAX - 0.5F) / 100.0F, 0.0F });

    _beamsVShader.setMode(lightMode * 10 + 7);
    Topl_Factory::switchPipeline(_renderer, _beamsPipeline);
    _renderer->updateScene(&scene);
    _renderer->setDrawMode(DRAW_Lines);
    _renderer->drawScene(&scene);

    for(unsigned m = 0; m < 3; m++)
        for(unsigned c = 0; c < MOLECULAR_CONSTRUCTS; c++)
            for(unsigned o = 0; o < constructs[m][c].getActorCount(); o++)
                _renderer->draw(constructs[m][c].getGeoActor(o));
}

MAIN_ENTRY {
    _DEMO = new Molecular_Demo(argv[0], BACKEND_DX11);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}