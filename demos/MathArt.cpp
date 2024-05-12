#include "MathArt.hpp"

static bool isRebuildReq = false;

static std::vector<Vec3f> pointsSet;

static void onPress(float x, float y){
    static unsigned pointIndex = 0;

    x = Platform::getCursorX();
    y = Platform::getCursorY();

    std::cout << "Add point to mesh: (" << std::to_string(x) << ", " << std::to_string(y) << ") at index " << std::to_string(pointIndex % 4) << std::endl;
    pointsSet.at(pointIndex % 4) = Vec3f({ x, y, 0.0F });

    pointIndex++;
}

static void surfaceMeshCallback(){
    if(_DEMO->surfaceMesh != nullptr) delete _DEMO->surfaceMesh;
    _DEMO->surfaceMesh = new Geo_Surface({ pointsSet[0], pointsSet[1], pointsSet[2], pointsSet[3],});
    _DEMO->targetActor = Geo_Actor(_DEMO->surfaceMesh);
    isRebuildReq = true;
}

static void coneMeshCallback(){
    if(_DEMO->coneMesh != nullptr) delete _DEMO->coneMesh;
    _DEMO->coneMesh = new Geo_Cone({ pointsSet[0], pointsSet[1], pointsSet[2], pointsSet[3], }, Vec3f({ 0.0, 0.0, 1.0F }));
    _DEMO->targetActor = Geo_Actor(_DEMO->coneMesh);
    isRebuildReq = true;
}

static void volumeMeshCallback(){
    if(_DEMO->volumeMesh != nullptr) delete _DEMO->volumeMesh;
    _DEMO->volumeMesh = new Geo_Volume({ pointsSet[0], pointsSet[1], pointsSet[2], pointsSet[3], }, 1.0F);
    _DEMO->targetActor = Geo_Actor(_DEMO->volumeMesh);
    isRebuildReq = true;
}

void MathArt_Demo::init(){
    pointsSet.resize(4);

    Platform::keyControl.addCallback('i', surfaceMeshCallback);
    Platform::keyControl.addCallback('o', coneMeshCallback);
    Platform::keyControl.addCallback('p', volumeMeshCallback);
    Platform::mouseControl.addCallback(MOUSE_RightBtn_Press, onPress);

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

    flatVShader.setMode(FLAT_MODE_COORD);

    // _renderer->setPipeline(texPipeline);
    _renderer->buildScene(&scene);
    _renderer->texturizeScene(&scene);
}

void MathArt_Demo::loop(double frameTime){
    float rotFactor = 0.01;

    triangleActor.updateRot({ (rand() / (float)RAND_MAX) * rotFactor, 0.0F, 0.0F });
    boxActor.updateRot({ 0.0F, (rand() / (float)RAND_MAX) * rotFactor, 0.0F });
    circleActor.updateRot({ 0.0F, 0.0F, (rand() / (float)RAND_MAX) * rotFactor });

    targetActor.updateRot({ (rand() / (float)RAND_MAX) * rotFactor, (rand() / (float)RAND_MAX) * rotFactor, (rand() / (float)RAND_MAX) * rotFactor });

    if(isRebuildReq){
        targetScene.removeActor("target");
        targetScene.addGeometry("target", &targetActor);
        _renderer->buildScene(&targetScene);
        isRebuildReq = false;
    }

    // texVShader.setMode(TEX_MODE_VOLUME);
    // _renderer->setPipeline(texPipeline);

    if(targetScene.getActorCount() > 0){
        _renderer->setDrawMode(DRAW_Strip);
        _renderer->updateScene(&targetScene);
        _renderer->drawScene(&targetScene);
    } else {
        _renderer->setDrawMode(DRAW_Lines);
        _renderer->updateScene(&scene);
        _renderer->drawScene(&scene);
    }
}

int main(int argc, char** argv) {
    _DEMO = new MathArt_Demo(argv[0], BACKEND_DX11);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}