#include "Portalcube.hpp"

void Portalcube_Demo::onAnyKey(char key){ } // TODO: Add body

void Portalcube_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex){ } // TODO: Add body

void Portalcube_Demo::init(){ 
    scene.addGeometry("cube", &cubeActor);

    _renderer->buildScene(&scene);
#ifdef RASTERON_H
    scene.addTexture("1", &frontFaceTex);
    scene.addTexture("2", &backFaceTex);
    scene.addTexture("3", &topFaceTex);
    scene.addTexture("4", &bottomFaceTex);
    scene.addTexture("5", &leftFaceTex);
    scene.addTexture("6", &rightFaceTex);
    scene.addVolumeTex("cube", &cubeTex);

    _renderer->texturizeScene(&scene);
#endif
}

void Portalcube_Demo::loop(double frameTime){
    _renderer->updateScene(&scene);
    _renderer->drawScene(&scene);
}

MAIN_ENTRY {
    Portalcube = new Portalcube_Demo(argv[0], BACKEND_GL4);
    Portalcube->run();

    delete(Portalcube);
    return 0;
}