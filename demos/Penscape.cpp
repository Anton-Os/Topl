#include "Penscape.hpp"

static DRAW_Mode drawMode = DRAW_Triangles;

void Penscape_Demo::onAnyKey(char key){
    // TODO: Add changes based on key events
}

void Penscape_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex){
    if(menu == PROGRAM_AppBar){ 
        for(unsigned a = 0; a < 3; a++)
            for(unsigned s = 0; s < 3; s++)
                brushes3D[a][s].isShown = Penscape_Demo::mode == (a * 3) + (s % 3);
    }
}

void Penscape_Demo::init(){
    Platform::keyControl.addHandler(std::bind(&Penscape_Demo::onAnyKey, this, std::placeholders::_1));

    for(unsigned a = 0; a < 3; a++){
        brushes3D[a][0] = Geo_Actor(trigMeshes[a]);
        brushes3D[a][1] = Geo_Actor(quadMeshes[a]);
        // brushes3D[a][2] = Geo_Actor(hexMeshes[a]);
        brushes3D[a][2] = Geo_Actor(circleMeshes[a]);
    }

    for(unsigned a = 0; a < 3; a++)
        for(unsigned s = 0; s < 3; s++){
            scene.addGeometry("brush" + std::to_string(a + 1) + "_" + std::to_string(s + 1), &brushes3D[a][s]);
            brushes3D[a][s].isShown = Penscape_Demo::mode == (a * 3) + (s % 3);
        }
    _renderer->buildScene(&scene);
}

void Penscape_Demo::loop(double frameTime){
    /* trigMeshes[mode]->drawMax = (_renderer->getFrameCount() / 36) % trigMeshes[mode]->getIndexCount();
    quadMeshes[mode]->drawMax = (_renderer->getFrameCount() / 24) % quadMeshes[mode]->getIndexCount();
    hexMeshes[mode]->drawMax = (_renderer->getFrameCount() / 12) % hexMeshes[mode]->getIndexCount();
    circleMeshes[mode]->drawMax = _renderer->getFrameCount() % circleMeshes[mode]->getIndexCount(); */
    
    _renderer->setDrawMode(drawMode);
    _renderer->updateScene(&scene);
    _renderer->drawScene(&scene);
}

MAIN_ENTRY {
    _DEMO = new Penscape_Demo(argv[0], BACKEND_GL4);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}
