#include "MathArt.hpp"

void MathArt_Demo::init(){
    scene.addGeometry(&triangleActor);
    scene.addGeometry(&boxActor);
    scene.addGeometry(&circleActor);

    for(unsigned s = 0; s < MATHART_SLICES; s++){
        // Rasteron_Image* sliceImg = solidImgOp({ MATHART_SLICES, MATHART_SLICES }, 0xFFEEEEEE & (unsigned)(0x11000000 * (unsigned)(16.0F / ((1.0F / MATHART_SLICES) * s))));
        Rasteron_Image* sliceImg = solidImgOp({ MATHART_SLICES, MATHART_SLICES }, 0xFFFFFF00);
        volumeImg.addSlice(sliceImg, s);
        RASTERON_DEALLOC(sliceImg);
    }
    scene.addVolumeTex(triangleActor.getName(), &volumeImg);
    scene.addVolumeTex(boxActor.getName(), &volumeImg);
    scene.addVolumeTex(circleActor.getName(), &volumeImg);

    // _renderer->setPipeline(texPipeline);
    _renderer->buildScene(&scene);
    _renderer->texturizeScene(&scene);
}

void MathArt_Demo::loop(double frameTime){
    float rotFactor = 0.01;

    triangleActor.updateRot({ (rand() / (float)RAND_MAX) * rotFactor, 0.0F, 0.0F });
    boxActor.updateRot({ 0.0F, (rand() / (float)RAND_MAX) * rotFactor, 0.0F });
    circleActor.updateRot({ 0.0F, 0.0F, (rand() / (float)RAND_MAX) * rotFactor });

    // texVShader.setMode(TEX_MODE_VOLUME);
    // _renderer->setPipeline(texPipeline);
    _renderer->setDrawMode(DRAW_Lines);
    _renderer->updateScene(&scene);
    _renderer->drawScene(&scene);
}

int main(int argc, char** argv) {
    _DEMO = new MathArt_Demo(argv[0], BACKEND_DX11);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}