#include "Kaleidoscope.hpp"

unsigned short Kaleidoscope_Demo::mode = 2;

static DRAW_Mode drawMode = DRAW_Triangles;

static void onAnyKey(char key){
    if(isdigit(key)){
        switch(key - '0'){
            case 2: drawMode = DRAW_Lines; break;
            case 3: drawMode = DRAW_Points; break;
            case 4: drawMode = DRAW_Strip; break;
            case 5: _DEMO->getConstruct()->rotate({ 0.1F, 0.0F, 0.0F }); break;
            case 6: _DEMO->getConstruct()->rotate({ -0.1F, 0.0F, 0.0F }); break;
            case 7: _DEMO->getConstruct()->rotate({ 0.0F, 0.1F, 0.0F }); break;
            case 8: _DEMO->getConstruct()->rotate({ 0.0F, -0.1F, 0.0F }); break;
            case 9: _DEMO->getConstruct()->rotate({ 0.0F, 0.0F, 0.1F }); break;
            case 0: _DEMO->getConstruct()->rotate({ 0.0F, 0.0F, -0.1F }); break;
            default: drawMode = DRAW_Triangles; break;
        }
    }
}

void setConstruct1(){ Kaleidoscope_Demo::mode = 0; }
void setConstruct2(){ Kaleidoscope_Demo::mode = 1; }
void setConstruct3(){ Kaleidoscope_Demo::mode = 2; }
void setConstruct4(){ Kaleidoscope_Demo::mode = 4; }

void Kaleidoscope_Demo::init(){
    Platform::keyControl.addAnyCallback(onAnyKey);
    Platform::keyControl.addCallback('g', setConstruct1);
    Platform::keyControl.addCallback('h', setConstruct2);
    Platform::keyControl.addCallback('j', setConstruct3);
    Platform::keyControl.addCallback('k', setConstruct4);

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

    _renderer->setDrawMode(drawMode);
    _flatVShader.setMode(4);// _effectVShader.setMode(EFFECT_MODE_FRACTALS);
    Topl_Factory::switchPipeline(_renderer, _flatPipeline);
    
    _renderer->updateScene(&scene);
    _renderer->drawScene(&scene);
    _renderer->clear();
    if(getConstruct() != nullptr)
        for(unsigned a = 0; a < getConstruct()->getActorCount(); a++)
            _renderer->draw(getConstruct()->getGeoActor(a));
}

int main(int argc, char** argv) {
    _DEMO = new Kaleidoscope_Demo(argv[0], BACKEND_GL4);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}