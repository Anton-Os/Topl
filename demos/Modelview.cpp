#include "Modelview.hpp"

void Modelview_Demo::init(){
    Topl_Program::cameraObj.setZoom(0.5F);

    model1.configure(&scene);
    model1.shift({ -1.0F, -0.33F, 0.0F });
    model1.rotateAll({ 0.0F, 0.1F, 0.0F });
    model2.configure(&scene);
    model2.shift({ 0.0F, 0.33F, 0.0F });
    model3.configure(&scene);
    model3.shift({ 1.0F, -0.33F, 0.0F });
    model3.rotateAll({ 0.0F, -0.1F, 0.0F });

    _renderer->buildScene(&scene);
}

void Modelview_Demo::loop(double frameTime){
    Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, (_renderer->getFrameCount() % 60 < 30)? flatPipeline : beamsPipeline);
    flatVShader.setMode((_renderer->getFrameCount() % 30 < 15)? 0 : 1);
    beamsVShader.setMode((_renderer->getFrameCount() % 30 < 15)? 0 : 3);

    // puppet.configure(&_instance->scene);

    model1.rotateAll({ 0.0, 0.0F, (float)frameTime / -500.0F });
    model2.rotateAll({ 0.0, 0.0F, (_renderer->getFrameCount() % 120 < 60)? (float)frameTime / 1000.0F : (float)frameTime / -1000.0F });
    model3.rotateAll({ 0.0, 0.0F, (float)frameTime / 500.0F });

    _renderer->setDrawMode((_renderer->getFrameCount() % 180 > 120)? DRAW_Strip : (_renderer->getFrameCount() % 180 > 60)? DRAW_Lines : DRAW_Points);

    _renderer->updateScene(&scene);
    _renderer->drawScene(&scene);
}

int main(int argc, char** argv) {
    _instance = new Modelview_Demo(argv[0], BACKEND_GL4);
    _instance->run();

    delete(_instance);
    return 0;
}