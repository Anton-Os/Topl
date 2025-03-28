#include "Kaleidoscope.hpp"

static DRAW_Mode drawMode = DRAW_Triangles;

void Kaleidoscope_Demo::onAnyKey(char key){
    switch(tolower(key)){
        case 'u': drawMode = DRAW_Triangles; break;
        case 'i': drawMode = DRAW_Lines; break;
        case 'o': drawMode = DRAW_Points; break;
        case 'p': drawMode = DRAW_Strip; break;
        case 'v': constructs[mode].rotate({ 0.1F, 0.0F, 0.0F }); break;
        case 'b': constructs[mode].rotate({ -0.1F, 0.0F, 0.0F }); break;
        case 'n': constructs[mode].rotate({ 0.0F, 0.1F, 0.0F }); break;
        case 'm': constructs[mode].rotate({ 0.0F, -0.1F, 0.0F }); break;
    }
}

void Kaleidoscope_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex){
    if(menu == PROGRAM_AppBar) setConstruct(Topl_Program::mode);
}

void Kaleidoscope_Demo::init(){
    Platform::keyControl.addHandler(std::bind(&Kaleidoscope_Demo::onAnyKey, this, std::placeholders::_1));

    for(unsigned c = 0; c < 9; c++) constructs[c].configure(&scene);

    _renderer->setPipeline(_flatPipeline);
    _renderer->buildScene(&scene);

    setConstruct(Topl_Program::mode);
}

void Kaleidoscope_Demo::loop(double frameTime){
    for(unsigned s = 0; s < constructs[mode].getActorCount(); s++){
        constructs[mode].getGeoActor(s)->updateRot(Vec3f({ constructs[mode].getSpinFactor(s), 0.0F, 0.0F }));
        // float zRot = constructs[mode].getGeoActor(s)->getRot()->data[0];
        // constructs[mode].getGeoActor(s)->updatePos(Vec3f({ 0.0F, 0.0F, sin(zRot) }) * 0.01F);
    }

    if(_renderer->getPipeline() == _patternPipeline){
        for(unsigned p = 0; p < PATTERN_POINTS_MAX; p++)
            _patternVShader.setCtrlPoint(p, Vec3f({ 0.0F, 0.0F, -1.0F + ((2.0F / PATTERN_POINTS_MAX) * p) }));
    }

    _renderer->setDrawMode(drawMode);
    // _renderer->updateScene(&scene);
    renderScene(&scene, nullptr, -Topl_Program::shaderMode); // _renderer->drawScene(&scene);
}

MAIN_ENTRY {
    _DEMO = new Kaleidoscope_Demo(argv[0], BACKEND_GL4);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}