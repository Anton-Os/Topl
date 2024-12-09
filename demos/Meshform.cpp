#include "Meshform.hpp"

void Meshform_Demo::onAnyKey(char key){
    switch(tolower(key)){
        case 'i': for(unsigned m = 0; m < 3; m++) for(unsigned a = 0; a < 4; a++) orbActors[m][a].isShown = m == 0; break;
        case 'o': for(unsigned m = 0; m < 3; m++) for(unsigned a = 0; a < 4; a++) orbActors[m][a].isShown = m == 1; break;
        case 'p': for(unsigned m = 0; m < 3; m++) for(unsigned a = 0; a < 4; a++) orbActors[m][a].isShown = m == 2; break;
    }
}

void Meshform_Demo::init(){
    Platform::keyControl.addHandler(std::bind(&Meshform_Demo::onAnyKey, this, std::placeholders::_1));

    for(unsigned m = 0; m < 3; m++){
        orbActors[m][0].setPos({ 0.5F, 0.5F, 0.0F });
        scene.addGeometry("trigOrb" + std::to_string(m), &orbActors[m][0]);
        orbActors[m][1].setPos({ -0.5F, -0.5F, 0.0F });
        scene.addGeometry("quadOrb" + std::to_string(m), &orbActors[m][1]);
        orbActors[m][2].setPos({ 0.5F, -0.5F, 0.0F });
        scene.addGeometry("hexOrb" + std::to_string(m), &orbActors[m][2]);
        orbActors[m][3].setPos({ -0.5F, 0.5F, 0.0F });
        scene.addGeometry("decOrb" + std::to_string(m), &orbActors[m][3]);
        for(unsigned a = 0; a < 4; a++) orbActors[m][a].isShown = m == MESHFORM_INDEX;

        scene.addVolumeTex("trigOrb" + std::to_string(m), &volumeImg);
        scene.addVolumeTex("quadOrb" + std::to_string(m), &volumeImg);
        scene.addVolumeTex("hexOrb" + std::to_string(m), &volumeImg);
        scene.addVolumeTex("decOrb" + std::to_string(m), &volumeImg);
    }

    _renderer->buildScene(&scene);
#ifdef RASTERON_H
    /* for(unsigned d = 0; d < volumeImg.getDepth(); d++){
        Rasteron_Image* gradientImg = gradientImgOp({ DEFAULT_IMG_HEIGHT, DEFAULT_IMG_WIDTH }, SIDE_Radial,  color_level(0xFFFFFFFF, (1.0 / 256) * d), 0xFFFF0000 + (0x1 * d) - (0x10000 * d));
        volumeImg.addSlice(gradientImg, d);
        RASTERON_DEALLOC(gradientImg);
    } */
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
    _DEMO = new Meshform_Demo(argv[0], BACKEND_GL4);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}
