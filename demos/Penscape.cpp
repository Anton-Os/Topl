#include "Penscape.hpp"

unsigned short Penscape_Demo::mode = 0;

static DRAW_Mode drawMode = DRAW_Triangles;

void Penscape_Demo::onAnyKey(char key){
    switch(tolower(key)){
        /* case 'u': drawMode = DRAW_Triangles; break;
        case 'i': drawMode = DRAW_Points; break;
        case 'o': drawMode = DRAW_Lines; break;
        case 'p': drawMode = DRAW_Fan; break; */
        case 'j': Penscape_Demo::mode = 0; break;
        case 'k': Penscape_Demo::mode = 1; break;
        case 'l': Penscape_Demo::mode = 2; break;
    }

    if(tolower(key) == 'j' || tolower(key) == 'k' || tolower(key) == 'l')
        for(unsigned a = 0; a < 3; a++)
            for(unsigned s = 0; s < 4; s++)
                brushes3D[a][s].isShown = Penscape_Demo::mode == a;
}

void Penscape_Demo::init(){
    Platform::keyControl.addHandler(std::bind(&Penscape_Demo::onAnyKey, this, std::placeholders::_1));

    for(unsigned a = 0; a < 3; a++)
        for(unsigned s = 0; s < 4; s++){
            scene.addGeometry("brush" + std::to_string(a + 1) + "_" + std::to_string(s + 1), &brushes3D[a][s]);
            brushes3D[a][s].isShown = Penscape_Demo::mode == a;
        }
    _renderer->buildScene(&scene);
}

void Penscape_Demo::loop(double frameTime){
    trigMeshes[mode]->drawMax = (_renderer->getFrameCount() / 36) % trigMeshes[mode]->getIndexCount();
    quadMeshes[mode]->drawMax = (_renderer->getFrameCount() / 24) % quadMeshes[mode]->getIndexCount();
    hexMeshes[mode]->drawMax = (_renderer->getFrameCount() / 12) % hexMeshes[mode]->getIndexCount();
    circleMeshes[mode]->drawMax = _renderer->getFrameCount() % circleMeshes[mode]->getIndexCount();
    
    _renderer->setDrawMode(drawMode);
    _renderer->updateScene(&scene);
    _renderer->drawScene(&scene);
}

MAIN_ENTRY {
    _DEMO = new Penscape_Demo(argv[0], BACKEND_DX11);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}
