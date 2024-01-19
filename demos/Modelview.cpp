#include "Modelview.hpp"

void Modelview_Demo::init(){
    Topl_Program::cameraObj.setZoom(0.5F);

    model1.configure(&scene);
    model1.shift({ -1.0F, -0.33F, 0.0F });
    model2.configure(&scene);
    model2.shift({ 0.0F, 0.33F, 0.0F });
    model3.configure(&scene);
    model3.shift({ 1.0F, -0.33F, 0.0F });

    _renderer->buildScene(&scene);
}

void Modelview_Demo::loop(double frameTime){
    Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, beamsPipeline);

    _renderer->updateScene(&scene);
    _renderer->renderScene(&scene);
}

int main(int argc, char** argv) {
    _instance = new Modelview_Demo(argv[0], BACKEND_GL4);
    _instance->run();

    delete(_instance);
    return 0;
}