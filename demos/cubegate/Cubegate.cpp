#include "Cubegate.hpp"

void Cubegate_Demo::onAnyKey(char key) {} // TODO: Add body

void Cubegate_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex) {} // TODO: Add body

void Cubegate_Demo::preloop() {
    Topl_Program::preloop();
    if (_renderer->getFrameCount() % 60 > 300) {
        renderScene(&worldScenes[0], _flatPipeline, Topl_Program::shaderMode);
        frontFaceTex = _renderer->frame();
        renderScene(&worldScenes[1], _beamsPipeline, Topl_Program::shaderMode);
        backFaceTex = _renderer->frame();
        renderScene(&worldScenes[2], _fieldPipeline, Topl_Program::shaderMode);
        topFaceTex = _renderer->frame();
        renderScene(&worldScenes[3], _patternPipeline, Topl_Program::shaderMode);
        bottomFaceTex = _renderer->frame();
        renderScene(&worldScenes[4], _materialPipeline, Topl_Program::shaderMode);
        leftFaceTex = _renderer->frame();
        renderScene(&worldScenes[5], _texPipeline, Topl_Program::shaderMode);
        rightFaceTex = _renderer->frame();
#ifdef TOPL_ENABLE_TEXTURES
        _renderer->texturizeScene(&scene);
#endif
    }
}

void Cubegate_Demo::init() {
    scene.addGeometry("cube", &cubeActor);
    scene.addGeometry("frontFace", &faceActors[CUBEGATE_Front]);
    scene.addGeometry("backFace", &faceActors[CUBEGATE_Back]);
    scene.addGeometry("topFace", &faceActors[CUBEGATE_Top]);
    scene.addGeometry("bottomFace", &faceActors[CUBEGATE_Bottom]);
    scene.addGeometry("leftFace", &faceActors[CUBEGATE_Left]);
    scene.addGeometry("rightFace", &faceActors[CUBEGATE_Right]);

    faceActors[CUBEGATE_Front].setPos({ 0.0F, 0.0F, CUBEGATE_SIZE / 2.0F });
    faceActors[CUBEGATE_Back].setPos({ 0.0F, 0.0F, -CUBEGATE_SIZE / 2.0F });
    faceActors[CUBEGATE_Top].setPos({ 0.0F, CUBEGATE_SIZE / 2.0F, 0.0F });
    faceActors[CUBEGATE_Top].setRot({ 0.0F, MATH_HALF_PI, 0.0F });
    faceActors[CUBEGATE_Bottom].setPos({ 0.0F, -CUBEGATE_SIZE / 2.0F, 0.0F });
    faceActors[CUBEGATE_Bottom].setRot({ 0.0F, MATH_HALF_PI, 0.0F });
    faceActors[CUBEGATE_Left ].setPos({ CUBEGATE_SIZE / 2.0F, 0.0F, 0.0F });
    faceActors[CUBEGATE_Left].setRot({ 0.0F, 0.0F, MATH_HALF_PI });
    faceActors[CUBEGATE_Right].setPos({ CUBEGATE_SIZE / 2.0F, 0.0F, 0.0F });
    faceActors[CUBEGATE_Right].setRot({ 0.0F, 0.0F, MATH_HALF_PI });

    _renderer->buildScene(&scene);
#ifdef TOPL_ENABLE_TEXTURES
    scene.addTexture("frontFace", &frontFaceTex);
    scene.addTexture("backFace", &backFaceTex);
    scene.addTexture("topFace", &topFaceTex);
    scene.addTexture("bottomFace", &bottomFaceTex);
    scene.addTexture("leftFace", &leftFaceTex);
    scene.addTexture("rightFace", &rightFaceTex);
    scene.addVolumeTex("cube", &cubeTex);

    _renderer->texturizeScene(&scene);
#endif
    for (unsigned b = 0; b < CUBEGATE_BALLS; b++) balls[b].setPos(VEC_3F_RAND);
    floor.setPos({ 0.0F, -1.0F, 0.0F });
    ceiling.setPos({ 0.0F, 1.0F, 0.0F });
    walls.setRot({ 0.0F, MATH_HALF_PI, 0.0F });

    for (unsigned s = 0; s < CUBEGATE_SCENES; s++) {
        for (unsigned b = 0; b < CUBEGATE_BALLS; b++)
            worldScenes[s].addGeometry("ball" + std::to_string(b + 1), &balls[b]);
        worldScenes[s].addGeometry("floor", &floor);
        worldScenes[s].addGeometry("ceiling", &ceiling);
        worldScenes[s].addGeometry("walls", &walls);
#ifdef TOPL_ENABLE_TEXTURES
        _renderer->texturizeScene(&worldScenes[s]);
#endif
    }
}

void Cubegate_Demo::loop(double frameTime) {
    cubeActor.updateRot({ 0.0F, 0.0F, (float)frameTime * 0.00000025F });

    _renderer->updateScene(&scene);
    _renderer->drawScene(&scene);
}

MAIN_ENTRY{
    Cubegate = new Cubegate_Demo(argv[0], BACKEND_GL4);
    Cubegate->run();

    delete(Cubegate);
    return 0;
}