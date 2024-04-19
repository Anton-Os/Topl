#include "MathArt.hpp"

void MathArt_Demo::init(){
    scene.addGeometry(&triangleActor);
    scene.addGeometry(&boxActor);
    scene.addGeometry(&circleActor);

    _renderer->buildScene(&scene);
    // _renderer->texturizeScene(&scene);
}

void MathArt_Demo::loop(double frameTime){
    float rotFactor = 0.01;

    triangleActor.updateRot({ (rand() / (float)RAND_MAX) * rotFactor, 0.0F, 0.0F });
    boxActor.updateRot({ 0.0F, (rand() / (float)RAND_MAX) * rotFactor, 0.0F });
    circleActor.updateRot({ 0.0F, 0.0F, (rand() / (float)RAND_MAX) * rotFactor });

    // _renderer->setDrawMode((_renderer->getFrameCount() % 180 > 120)? DRAW_Strip : (_renderer->getFrameCount() % 180 > 60)? DRAW_Lines : DRAW_Points);
    _renderer->setDrawMode(DRAW_Lines);
    _renderer->updateScene(&scene);
    _renderer->drawScene(&scene);
}

int main(int argc, char** argv) {
    _instance = new MathArt_Demo(argv[0], BACKEND_DX11);
    _instance->run();

    delete(_instance);
    return 0;
}