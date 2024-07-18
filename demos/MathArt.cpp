#include "MathArt.hpp"


static DRAW_Mode drawMode = DRAW_Triangles;

static void onAnyKey(char key){
    if(isdigit(key)){
        switch(key - '0'){
            case 2: drawMode = DRAW_Lines; break;
            case 3: drawMode = DRAW_Points; break;
            case 4: drawMode = DRAW_Strip; break;
            case 5: _DEMO->construct1.rotate({ 0.1F, 0.0F, 0.0F }); break;
            case 6: _DEMO->construct1.rotate({ -0.1F, 0.0F, 0.0F }); break;
            case 7: _DEMO->construct1.rotate({ 0.0F, 0.1F, 0.0F }); break;
            case 8: _DEMO->construct1.rotate({ 0.0F, -0.1F, 0.0F }); break;
            case 9: _DEMO->construct1.rotate({ 0.0F, 0.0F, 0.1F }); break;
            case 0: _DEMO->construct1.rotate({ 0.0F, 0.0F, -0.1F }); break;
            default: drawMode = DRAW_Triangles; break;
        }
    }
}

void MathArt_Demo::init(){
    Platform::keyControl.addAnyCallback(onAnyKey);

    construct1.configure(&scene);
    // construct2.configure(&scene);
    // construct3.configure(&scene);

    _renderer->setPipeline(_flatPipeline);
    _renderer->buildScene(&scene);
}

void MathArt_Demo::loop(double frameTime){
    for(unsigned s = 0; s < construct1.getActorCount(); s++){
        construct1.getGeoActor(s)->updateRot(Vec3f({ construct1.getSpinFactor(s), 0.0F, 0.0F }));
        construct2.getGeoActor(s)->updateRot(Vec3f({ construct2.getSpinFactor(s), 0.0F, 0.0F }));
        construct3.getGeoActor(s)->updateRot(Vec3f({ construct3.getSpinFactor(s), 0.0F, 0.0F }));
    }

    _renderer->setDrawMode(drawMode);
    _flatVShader.setMode(4);// _effectVShader.setMode(EFFECT_MODE_FRACTALS);
    Topl_Factory::switchPipeline(_renderer, _flatPipeline);
    
    _renderer->updateScene(&scene);
    _renderer->drawScene(&scene);
}

int main(int argc, char** argv) {
    _DEMO = new MathArt_Demo(argv[0], BACKEND_DX11);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}