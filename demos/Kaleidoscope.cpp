#include "Kaleidoscope.hpp"

unsigned short Kaleidoscope_Demo::mode = 2;

static DRAW_Mode drawMode = DRAW_Triangles;

void Kaleidoscope_Demo::onAnyKey(char key){
    switch(tolower(key)){
        case 'u': drawMode = DRAW_Triangles; break;
        case 'i': drawMode = DRAW_Lines; break;
        case 'o': drawMode = DRAW_Points; break;
        case 'p': drawMode = DRAW_Strip; break;
        case 'h': Kaleidoscope_Demo::mode = 0; break;
        case 'j': Kaleidoscope_Demo::mode = 1; break;
        case 'k': Kaleidoscope_Demo::mode = 2; break;
        case 'l': Kaleidoscope_Demo::mode = 3; break;
        case 'v': _DEMO->getConstruct()->rotate({ 0.1F, 0.0F, 0.0F }); break;
        case 'b': _DEMO->getConstruct()->rotate({ -0.1F, 0.0F, 0.0F }); break;
        case 'n': _DEMO->getConstruct()->rotate({ 0.0F, 0.1F, 0.0F }); break;
        case 'm': _DEMO->getConstruct()->rotate({ 0.0F, -0.1F, 0.0F }); break;
        // case 9: _DEMO->getConstruct()->rotate({ 0.0F, 0.0F, 0.1F }); break;
        // case 0: _DEMO->getConstruct()->rotate({ 0.0F, 0.0F, -0.1F }); break;
        // default: drawMode = DRAW_Triangles; break;
    }
    // if(tolower(key) == 'n') _DEMO->getConstruct()->shift({ 0.0F, 0.0F, 0.025F });
    // else if(tolower(key) == 'm') _DEMO->getConstruct()->shift({ 0.0F, 0.0F, -0.025F });
}

void Kaleidoscope_Demo::init(){
    Platform::keyControl.addHandler(std::bind(&Kaleidoscope_Demo::onAnyKey, this, std::placeholders::_1));

    construct1.configure(&scene);
    construct2.configure(&scene);
    construct3.configure(&scene);
    construct4.configure(&scene);

    _renderer->setPipeline(_flatPipeline);
    _renderer->buildScene(&scene);
}

void Kaleidoscope_Demo::loop(double frameTime){
    for(unsigned s = 0; s < construct1.getActorCount(); s++){
        construct1.getGeoActor(s)->updateRot(Vec3f({ construct1.getSpinFactor(s), 0.0F, 0.0F }));
        construct2.getGeoActor(s)->updateRot(Vec3f({ construct2.getSpinFactor(s), 0.0F, 0.0F }));
        construct3.getGeoActor(s)->updateRot(Vec3f({ construct3.getSpinFactor(s), 0.0F, 0.0F }));
        construct4.getGeoActor(s)->updateRot(Vec3f({ construct4.getSpinFactor(s), 0.0F, 0.0F }));
    }

    // _flatVShader.setMode(8);
    // Topl_Factory::switchPipeline(_renderer, _flatPipeline);
    _renderer->setDrawMode(drawMode);
    _renderer->updateScene(&scene);
    if(getConstruct() != nullptr)
        for(unsigned a = 0; a < getConstruct()->getActorCount(); a++)
            _renderer->draw(getConstruct()->getGeoActor(a));
}

MAIN_ENTRY {
    _DEMO = new Kaleidoscope_Demo(argv[0], BACKEND_GL4);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}