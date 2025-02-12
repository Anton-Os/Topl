#include "Brush3D.hpp"

unsigned short Brush3D_Demo::mode = 0;

static DRAW_Mode drawMode = DRAW_Triangles;

void Brush3D_Demo::onAnyKey(char key){
    switch(tolower(key)){
        case 'h': drawMode = DRAW_Triangles; break;
        case 'j': drawMode = DRAW_Points; break;
        case 'k': drawMode = DRAW_Lines; break;
        case 'l': drawMode = DRAW_Fan; break;
    }
}

void Brush3D_Demo::init(){
    Platform::keyControl.addHandler(std::bind(&Brush3D_Demo::onAnyKey, this, std::placeholders::_1));

    for(unsigned a = 0; a < 4; a++)
        scene.addGeometry("brush" + std::to_string(a + 1), &brushes3D[a]);
    _renderer->buildScene(&scene);
}

void Brush3D_Demo::loop(double frameTime){
    trigBrushes[0]->drawMax = (_renderer->getFrameCount() / 6) % trigBrushes[0]->getIndexCount();
    quadBrushes[0]->drawMax = (_renderer->getFrameCount() / 6) % quadBrushes[0]->getIndexCount();
    hexBrushes[0]->drawMax = (_renderer->getFrameCount() / 6) % hexBrushes[0]->getIndexCount();
    circleBrushes[0]->drawMax = (_renderer->getFrameCount() / 6) % circleBrushes[0]->getIndexCount();
    
    _renderer->setDrawMode(drawMode);
    _renderer->updateScene(&scene);
    _renderer->drawScene(&scene);
}

MAIN_ENTRY {
    _DEMO = new Brush3D_Demo(argv[0], BACKEND_DX11);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}
