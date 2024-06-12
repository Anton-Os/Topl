#include "Sandbox.hpp"

static void onSpace(){
    logMessage("onSpace() triggered");
}

static void onAnyKey(char k){
    logMessage("onAnyKey() triggered");
}

static void onDrag(float x, float y){
    logMessage("onDrag() triggered");
}

static void onScroll(bool isZoom){
    logMessage("onScroll() triggered");
}

static void timeAdjEvent(double m){
    logMessage("timeAdjEvent() triggered");
}

static void timeStepEvent(){
    logMessage("timeStepEvent() triggered");
}

void Sandbox_Demo::init(){
    Platform::keyControl.addCallback(' ', onSpace);
    Platform::keyControl.addAnyCallback(onAnyKey);
    Platform::mouseControl.addDragCallback(onDrag);
    Platform::mouseControl.setOnScroll(onScroll);

    Topl_Program::timeline.dynamic_ticker.addRecurringEvent(timeAdjEvent);
    Topl_Program::timeline.persist_ticker.addPeriodicEvent(1000, timeStepEvent);

    Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, _texPipeline);

    backdropScene.addGeometry("Backdrop", &backdropActor);
    _renderer->buildScene(&backdropScene);
    _renderer->texturizeScene(&backdropScene);

    for(unsigned short s = 1; s < SANDBOX_MESH_COUNT; s++){
        surfaceActors[s].setPos({ rand() / (float)RAND_MAX - 0.5F, rand() / (float)RAND_MAX - 0.5F, 0.0F }); 
        mainScene.addGeometry("surfaceActor" + std::to_string(s + 1), &surfaceActors[s]);
#ifdef RASTERON_H
        mainScene.addTexture("surfaceActor" + std::to_string(s + 1), _images[s]);
#endif
    }
    _renderer->buildScene(&mainScene);

    for(auto b = _billboards.begin(); b != _billboards.end(); b++) (*b)->configure(&overlayScene);
    /* _billboards[0]->getImgAt(0)->setFileImage(std::string(IMAGES_DIR) + "/containers/Container-ORG-Cut.png");
    for(unsigned short p = 0; p < _billboards[1]->getParams()->getGridSize(); p++) _billboards[1]->getImgAt(p)->setFileImage(std::string(IMAGES_DIR) + "/containers/Container-ORG-Inset.png");
    for(unsigned short p = 0; p < _billboards[2]->getParams()->getGridSize(); p++) _billboards[2]->getImgAt(p)->setFileImage(std::string(IMAGES_DIR) + "/containers/Container-GRN-Cut.png");
    for(unsigned short p = 0; p < _billboards[3]->getParams()->getGridSize(); p++) _billboards[3]->getImgAt(p)->setFileImage(std::string(IMAGES_DIR) + "/containers/Container-BLU-Cut.png"); */

    _renderer->buildScene(&overlayScene);
    _renderer->texturizeScene(&overlayScene);
}

void Sandbox_Demo::loop(double frameTime){
    // check overlay for match
    unsigned pick = colorPicker(&overlayScene);
    Vec3f coord = coordPicker(&overlayScene);

    // check main scene for match
    if(Topl_Program::pickerObj == nullptr) {
        pick = colorPicker(&mainScene);
        Vec3f coord = coordPicker(&mainScene);
    }

    _renderer->setDrawMode(DRAW_Triangles);

    Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, _effectPipeline);
    // _renderer->updateScene(&backdropScene);
    // _renderer->drawScene(&backdropScene);

    Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, _texPipeline);
    _renderer->setCamera(&camera);
    _renderer->updateScene(&mainScene);
    _renderer->drawScene(&mainScene);

    // _flatVShader.setMode(-2);
    // Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, _flatPipeline);
    _renderer->setCamera(&fixedCamera);
    _renderer->updateScene(&overlayScene);
    _renderer->drawScene(&overlayScene);
}

int main(int argc, char** argv) {
    _DEMO = new Sandbox_Demo(argv[0], BACKEND_DX11);

    _DEMO->run();
    delete(_DEMO);
    return 0;
}