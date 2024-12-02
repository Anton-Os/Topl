#include "Meshform.hpp"

static void onAnyKey(char key){
    if(tolower(key) == 'u' || tolower(key) == 'i' || tolower(key) == 'o'){
        /* for(unsigned a = 0; a < 4; a++) _DEMO->orbActors[a].isShown = false;
        switch(tolower(key)){
            case 'u': _DEMO->orbActors[0].isShown = true; break;
            case 'i': _DEMO->orbActors[1].isShown = true; break;
            case 'o': _DEMO->orbActors[2].isShown = true; break;
            case 'p': _DEMO->orbActors[3].isShown = true; break;
        } */
    }
}


void Meshform_Demo::init(){
    scene.camera = &Topl_Program::camera;

    Platform::keyControl.addAnyCallback(onAnyKey);

    orbActors[MESHFORM_INDEX][0].setPos({ 0.5F, 0.5F, 0.0F });
    scene.addGeometry("trigOrb", &orbActors[MESHFORM_INDEX][0]);
    orbActors[MESHFORM_INDEX][1].setPos({ -0.5F, -0.5F, 0.0F });
    scene.addGeometry("quadOrb", &orbActors[MESHFORM_INDEX][1]);
    orbActors[MESHFORM_INDEX][2].setPos({ 0.5F, -0.5F, 0.0F });
    scene.addGeometry("hexOrb", &orbActors[MESHFORM_INDEX][2]);
    orbActors[MESHFORM_INDEX][3].setPos({ -0.5F, 0.5F, 0.0F });
    scene.addGeometry("decOrb", &orbActors[MESHFORM_INDEX][3]);

    _renderer->buildScene(&scene);
#ifdef RASTERON_H
    /* for(unsigned d = 0; d < volumeImg.getDepth(); d++){
        Rasteron_Image* gradientImg = gradientImgOp({ DEFAULT_IMG_HEIGHT, DEFAULT_IMG_WIDTH }, SIDE_Radial,  color_level(0xFFFFFFFF, (1.0 / 256) * d), 0xFFFF0000 + (0x1 * d) - (0x10000 * d));
        volumeImg.addSlice(gradientImg, d);
        RASTERON_DEALLOC(gradientImg);
    } */
    scene.addVolumeTex("trigOrb", &volumeImg);
    scene.addVolumeTex("quadOrb", &volumeImg);
    scene.addVolumeTex("hexOrb", &volumeImg);
    scene.addVolumeTex("decOrb", &volumeImg);
    _renderer->texturizeScene(&scene);
#endif
}

void Meshform_Demo::loop(double frameTime){
    for(unsigned a = 0; a < 4; a++){ 
        Vec3f rotationVec = Vec3f({ (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX });
        rotationVec = rotationVec * (0.001 * frameTime);
        for(unsigned o = 0; o < 3; o++) _DEMO->orbActors[o][a].updateRot(rotationVec);
    }

    _texVShader.setMode(8);
    _flatVShader.setMode(MESHFORM_TESS); // TODO: Change this to volumetric texture
    _effectVShader.setMode(-23);
    Topl_Factory::switchPipeline(_renderer, _texPipeline);
    _renderer->updateScene(&scene);
    _renderer->setDrawMode(DRAW_Triangles);
    _renderer->drawScene(&scene);
    // _renderer->setDrawMode(DRAW_Lines);
    // _renderer->drawScene(&scene);
}

MAIN_ENTRY {
    _DEMO = new Meshform_Demo(argv[0], BACKEND_DX11);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}
