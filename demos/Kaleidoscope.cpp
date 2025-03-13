#include "Kaleidoscope.hpp"

unsigned short Kaleidoscope_Demo::mode = 2;

static DRAW_Mode drawMode = DRAW_Triangles;

void Kaleidoscope_Demo::onAnyKey(char key){
    switch(tolower(key)){
        case 'u': drawMode = DRAW_Triangles; break;
        case 'i': drawMode = DRAW_Lines; break;
        case 'o': drawMode = DRAW_Points; break;
        case 'p': drawMode = DRAW_Strip; break;
        case 'h': setConstruct(0); break;
        case 'j': setConstruct(1); break;
        case 'k': setConstruct(2); break;
        case 'l': setConstruct(3); break;
        case 'v': _DEMO->getConstruct()->rotate({ 0.1F, 0.0F, 0.0F }); break;
        case 'b': _DEMO->getConstruct()->rotate({ -0.1F, 0.0F, 0.0F }); break;
        case 'n': _DEMO->getConstruct()->rotate({ 0.0F, 0.1F, 0.0F }); break;
        case 'm': _DEMO->getConstruct()->rotate({ 0.0F, -0.1F, 0.0F }); break;
    }
}

void Kaleidoscope_Demo::init(){
    Platform::keyControl.addHandler(std::bind(&Kaleidoscope_Demo::onAnyKey, this, std::placeholders::_1));

    construct1.configure(&scene);
    construct2.configure(&scene);
    construct3.configure(&scene);
    construct4.configure(&scene);

    _renderer->setPipeline(_flatPipeline);
    _renderer->buildScene(&scene);

    setConstruct(Kaleidoscope_Demo::mode);
}

void Kaleidoscope_Demo::loop(double frameTime){
    double totalTime = 0.0;

    for(unsigned s = 0; s < _DEMO->getConstruct()->getActorCount(); s++){
        _DEMO->getConstruct()->getGeoActor(s)->updateRot(Vec3f({ _DEMO->getConstruct()->getSpinFactor(s), 0.0F, 0.0F }));
        // float zRot = _DEMO->getConstruct()->getGeoActor(s)->getRot()->data[0];
        // _DEMO->getConstruct()->getGeoActor(s)->updatePos(Vec3f({ 0.0F, 0.0F, sin(zRot) }) * 0.01F);
    }

    if(_renderer->getPipeline() == _patternPipeline){
        for(unsigned p = 0; p < PATTERN_POINTS_MAX; p++)
            _patternVShader.setCtrlPoint(p, Vec3f({ 0.0F, 0.0F, -1.0F + ((2.0F / PATTERN_POINTS_MAX) * p) }));
        for(unsigned s = 0; s < _DEMO->getConstruct()->getActorCount(); s++)
            _patternVShader.setAlpha(_DEMO->getConstruct()->getGeoActor(s), (totalTime * s) - floor(totalTime * s));
    }

    _renderer->setDrawMode(drawMode);
    // _renderer->updateScene(&scene);
    renderScene(&scene, nullptr, -Topl_Program::shaderMode); // _renderer->drawScene(&scene);

    totalTime += frameTime;
}

MAIN_ENTRY {
    _DEMO = new Kaleidoscope_Demo(argv[0], BACKEND_GL4);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}