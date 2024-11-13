#include "Meshform.hpp"

static void onAnyKey(char key){
    if(tolower(key) == 'u' || tolower(key) == 'i' || tolower(key) == 'o' || tolower(key) == 'p'){
        for(unsigned a = 0; a < 4; a++) _DEMO->orbActors[a].isShown = false;
        switch(tolower(key)){
            case 'u': _DEMO->orbActors[0].isShown = true; break;
            case 'i': _DEMO->orbActors[1].isShown = true; break;
            case 'o': _DEMO->orbActors[2].isShown = true; break;
            case 'p': _DEMO->orbActors[3].isShown = true; break;
        }
    }
}


void Meshform_Demo::init(){
    scene.camera = &Topl_Program::cameraObj;

    Platform::keyControl.addAnyCallback(onAnyKey);

    scene.addGeometry("trigOrb", &orbActors[0]);
    scene.addGeometry("quadOrb", &orbActors[1]);
    scene.addGeometry("hexOrb", &orbActors[2]);
    scene.addGeometry("decOrb", &orbActors[3]);

    for(unsigned a = 0; a < 4; a++) _DEMO->orbActors[a].isShown = false;
    _DEMO->orbActors[0].isShown = true;

    _renderer->buildScene(&scene);
#ifdef RASTERON_H
    scene.addVolumeTex("trigOrb", &volumeImg);
    scene.addVolumeTex("quadOrb", &volumeImg);
    scene.addVolumeTex("hexOrb", &volumeImg);
    scene.addVolumeTex("decOrb", &volumeImg);
    _renderer->texturizeScene(&scene);
#endif
}

void Meshform_Demo::loop(double frameTime){
    Vec3f rotationVec = Vec3f({ (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX });
    rotationVec = rotationVec * (0.001 * frameTime);
    for(unsigned a = 0; a < 4; a++) _DEMO->orbActors[a].updateRot(rotationVec);

    _flatVShader.setMode(-MESHFORM_TESS); // TODO: Change this to volumetric texture
    _effectVShader.setMode(1);
    _renderer->setPipeline(_flatPipeline);
    _renderer->updateScene(&scene);
    _renderer->drawScene(&scene);
}

MAIN_ENTRY {
    _DEMO = new Meshform_Demo(argv[0], BACKEND_DX11);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}
