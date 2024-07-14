#include "MathArt.hpp"


static DRAW_Mode drawMode = DRAW_Triangles;

static void onAnyKey(char key){
    if(isdigit(key)){
        switch(key - '0'){
            case 2: drawMode = DRAW_Lines; break;
            case 3: drawMode = DRAW_Points; break;
            case 4: drawMode = DRAW_Fan; break;
            case 5: drawMode = DRAW_Strip; break;
            default: drawMode = DRAW_Triangles; break;
        }
    }
}

void MathArt_Demo::init(){
    Platform::keyControl.addAnyCallback(onAnyKey);

    for(unsigned s = 0; s < MATHART_SLICES; s++){
        scene.addGeometry("circle" + std::to_string(s), &circleActors[s]);
        // TODO: Add texture?
    }

    _renderer->setPipeline(_flatPipeline);
    // _renderer->setCamera(&Topl_Program::cameraObj);
    _renderer->buildScene(&scene);
}

void MathArt_Demo::loop(double frameTime){
    for(unsigned s = 0; s < MATHART_SLICES; s++) circleActors[s].updateRot(Vec3f({ spinFactors[s], 0.0F, 0.0F }));

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