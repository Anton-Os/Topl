#include "Cubegate.hpp"

void Cubegate_Demo::onAnyKey(keyboard_t key) {
    switch (key) {
        case 'a': worldCamera.updatePos({ -Topl_Program::speed, 0.0F, 0.0F }); break;
        case 's': worldCamera.updatePos({ 0.0F, -Topl_Program::speed, 0.0F }); break;
        case 'd': worldCamera.updatePos({ Topl_Program::speed, 0.0F, 0.0F }); break;
        case 'w': worldCamera.updatePos({ 0.0F, Topl_Program::speed, 0.0F }); break;
    }
}

void Cubegate_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex){ 
    if (menu == PROGRAM_Sculpt)
        for (unsigned c = 0; c < 9; c++)
            cubeGates[c].toggleShow(c == paneIndex);
} 

void Cubegate_Demo::preloop() {
    Topl_Program::preloop();
    if (_renderer->getFrameCount() / 60 > 300) {
#ifdef TOPL_ENABLE_TEXTURES
        renderScene(&worldScenes[0], _coloredPipeline, Topl_Program::shaderMode);
        frontFaceTex = _renderer->frame();
        renderScene(&worldScenes[1], _beamsPipeline, Topl_Program::shaderMode);
        backFaceTex = _renderer->frame();
        renderScene(&worldScenes[2], _fieldPipeline, Topl_Program::shaderMode);
        topFaceTex = _renderer->frame();
        renderScene(&worldScenes[3], _patternPipeline, Topl_Program::shaderMode);
        bottomFaceTex = _renderer->frame();
        renderScene(&worldScenes[4], _materialPipeline, Topl_Program::shaderMode);
        leftFaceTex = _renderer->frame();
        renderScene(&worldScenes[5], _effectPipeline, Topl_Program::shaderMode);
        rightFaceTex = _renderer->frame();

        Sampler_2D* cubeGateTextures[6] = { &frontFaceTex, &backFaceTex, &topFaceTex, &bottomFaceTex, &leftFaceTex, &rightFaceTex };
        // cubeGate.setTextures(&scene, cubeGateTextures);
        _renderer->texturizeScene(&scene);
#endif
    }
}

void Cubegate_Demo::init() {
    Platform::keyControl.addHandler(std::bind(&Cubegate_Demo::onAnyKey, this, std::placeholders::_1));

    // scene.addGeometry("cube", &cubeActor);
 
    // cubeGate.configure(&scene);
    for (unsigned c = 0; c < 9; c++) cubeGates[c].configure(&scene);

    _renderer->buildScene(&scene);
#ifdef TOPL_ENABLE_TEXTURES
    for (unsigned c = 0; c < 9; c++) {
        scene.addTexture(cubeGates[c].getGeoActor(CUBEGATE_Front)->getName(), &frontFaceTex);
        scene.addTexture(cubeGates[c].getGeoActor(CUBEGATE_Back)->getName(), &backFaceTex);
        scene.addTexture(cubeGates[c].getGeoActor(CUBEGATE_Top)->getName(), &topFaceTex);
        scene.addTexture(cubeGates[c].getGeoActor(CUBEGATE_Bottom)->getName(), &bottomFaceTex);
        scene.addTexture(cubeGates[c].getGeoActor(CUBEGATE_Left)->getName(), &leftFaceTex);
        scene.addTexture(cubeGates[c].getGeoActor(CUBEGATE_Right)->getName(), &rightFaceTex);
        for(unsigned f = 0; f < 6; f++) scene.addVolumeTex(cubeGates[c].getGeoActor(f)->getName(), &cubeTex);
    }

    _renderer->texturizeScene(&scene);
#endif
    for (unsigned s = 0; s < CUBEGATE_SCENES; s++) {
        for (unsigned b = 0; b < CUBEGATE_BALLS; b++) {
            worldScenes[s].addGeometry("ball" + std::to_string(b + 1), &balls[b]);
            balls[b].setPos((VEC_3F_RAND - VEC_3F_ONES * 2));
        }

        _renderer->buildScene(&worldScenes[s]);
#ifdef TOPL_ENABLE_TEXTURES
        _renderer->texturizeScene(&worldScenes[s]);
#endif
    }
}

void Cubegate_Demo::loop(double frameTime) {
    float f = (float)frameTime * -0.00000001F;

    for (unsigned c = 0; c < 9; c++) //cubeGates[c].rotate({ (c % 3 == 2)? f : 0.0f , (c % 3 == 1)? f : 0.0f , (c % 3 == 0)? f : 0.0f });
        cubeGates[c].rotate({ 0.0f, 0.0f, f });

    /* renderScene(&worldScenes[0], _coloredPipeline, Topl_Program::shaderMode);
    renderScene(&worldScenes[1], _beamsPipeline, Topl_Program::shaderMode);
    renderScene(&worldScenes[2], _fieldPipeline, Topl_Program::shaderMode);
    renderScene(&worldScenes[3], _patternPipeline, Topl_Program::shaderMode);
    renderScene(&worldScenes[4], _materialPipeline, Topl_Program::shaderMode);
    renderScene(&worldScenes[5], _effectPipeline, Topl_Program::shaderMode); */

    renderScene(&scene);
}

MAIN_ENTRY{
    Cubegate = new Cubegate_Demo(argv[0], BACKEND_GL4);
    Cubegate->run();

    delete(Cubegate);
    return 0;
}
