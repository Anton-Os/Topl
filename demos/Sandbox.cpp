#include "Sandbox.hpp"

SANDBOX_Action Sandbox_Demo::_action = SANDBOX_Move;

static void onSpace(){
    // logMessage("onSpace() triggered");
}

static void onAnyKey(char k){
    // logMessage("onAnyKey() triggered");
}

static void onDrag(float x, float y){
    // logMessage("onDrag() triggered");
}

static void onScroll(bool isZoom){
    // logMessage("onScroll() triggered");
}

static void timeAdjEvent(double m){
    // logMessage("timeAdjEvent() triggered");
}

static void timeStepEvent(){
    // logMessage("timeStepEvent() triggered");
}

void Sandbox_Demo::onBackdropPick(MOUSE_Event event){
    std::cout << "onBackdropPick() triggered with " << Topl_Program::pickerObj->getName() << " and extension " << Topl_Program::pickerObj->getNameExt() << std::endl;

    if(event == MOUSE_LeftBtn_Drag || event == MOUSE_LeftBtn_Press){
        switch(_action){
            case SANDBOX_Pan: 
                std::cout << "move camera!" << std::endl;
                Topl_Program::cameraObj.updatePos({ 0.1F, 0.0F, 0.0F });
                break;
            case SANDBOX_Pivot: 
                std::cout << "rotate camera!" << std::endl; 
                Topl_Program::cameraObj.updateRot(Topl_Program::getCamCursorPos());
                break;
            case SANDBOX_Zoom: 
                std::cout << "scale camera!" << std::endl; 
                Topl_Program::cameraObj.setZoom(*Topl_Program::cameraObj.getZoom() * 1.05);
                break;
        }
    }
}

void Sandbox_Demo::onObjectPick(MOUSE_Event event){
    std::cout << "onObjectPick() triggered with " << Topl_Program::pickerObj->getName() << " and extension " << Topl_Program::pickerObj->getNameExt() << std::endl;

    if(event == MOUSE_RightBtn_Press){
        // _billboards.back()->toggleShow(true);
        // TODO: Show properties and labels
    } else if(Platform::mouseControl.getIsMouseDown().second){
        std::cout << "_action is " << std::to_string(_action) << std::endl;
        switch(_action){
            case SANDBOX_Move: 
                std::cout << "move object!" << std::endl;
                Topl_Program::pickerObj->setPos(Topl_Program::getCamCursorPos());
                break;
            case SANDBOX_Rotate: 
                std::cout << "rotate object!" << std::endl;
                Topl_Program::pickerObj->updateRot(Topl_Program::getCamCursorPos());
                break;
            case SANDBOX_Scale: 
                std::cout << "scale object!" << std::endl; 
                Topl_Program::pickerObj->updateSize(Topl_Program::getCamCursorPos());
                break;
        }
    }
}

void Sandbox_Demo::onMenuPick(MOUSE_Event event){
    std::cout << "onMenuPick() triggered with " << Topl_Program::pickerObj->getName() << " and extension " << Topl_Program::pickerObj->getNameExt() << std::endl;

    unsigned short ext = std::stoi(Topl_Program::pickerObj->getNameExt());
    if(event == MOUSE_LeftBtn_Press){
        // TODO: Change the current action
        if(Topl_Program::pickerObj->getName().find("actions_board") != std::string::npos){
            _action = (SANDBOX_Action)ext;
            std::cout << "actions_board pressed! _action is " << std::to_string(_action) << std::endl;
            _DEMO->_billboards.at(2)->setState(ext, Platform::mouseControl.getIsMouseDown().second);
        } 
        else if(Topl_Program::pickerObj->getName().find("objs_board") != std::string::npos) std::cout << "objs_board pressed!" << std::endl;
        else if(Topl_Program::pickerObj->getName().find("sculpt_board") != std::string::npos){ 
            _action = SANDBOX_Sculpt;
            std::cout << "sculpt_board pressed!" << std::endl;
        }
        else if(Topl_Program::pickerObj->getName().find("paint_board") != std::string::npos){ 
            _action = SANDBOX_Paint;
            std::cout << "sculpt_board pressed!" << std::endl;
        }
    }
}


void Sandbox_Demo::init(){
    Platform::keyControl.addCallback(' ', onSpace);
    Platform::keyControl.addAnyCallback(onAnyKey);
    Platform::mouseControl.addDragCallback(onDrag);
    Platform::mouseControl.setOnScroll(onScroll);

    Topl_Program::timeline.dynamic_ticker.addRecurringEvent(timeAdjEvent);
    Topl_Program::timeline.persist_ticker.addPeriodicEvent(1000, timeStepEvent);

    Topl_Factory::switchPipeline(_renderer, _texPipeline);

    backdropScene.addGeometry("Backdrop", &backdropActor);
    backdropActor.pickerFunc = onBackdropPick;
    _renderer->buildScene(&backdropScene);
    _renderer->texturizeScene(&backdropScene);

    for(unsigned short s = 1; s < SANDBOX_MESH_COUNT - 1; s++){
        surfaceActors[s].setPos({ rand() / (float)RAND_MAX - 0.5F, rand() / (float)RAND_MAX - 0.5F, 0.0F });
        surfaceActors[s].pickerFunc = &Sandbox_Demo::onObjectPick;
        mainScene.addGeometry("surfaceActor" + std::to_string(s + 1), &surfaceActors[s]);
        coneActors[s].setPos({ rand() / (float)RAND_MAX - 0.5F, rand() / (float)RAND_MAX - 0.5F, 0.0F });
        coneActors[s].pickerFunc = &Sandbox_Demo::onObjectPick;
        mainScene.addGeometry("coneActor" + std::to_string(s + 1), &coneActors[s]);
        volumeActors[s].setPos({ rand() / (float)RAND_MAX - 0.5F, rand() / (float)RAND_MAX - 0.5F, 0.0F });
        volumeActors[s].pickerFunc = &Sandbox_Demo::onObjectPick;
        mainScene.addGeometry("volumeActor" + std::to_string(s + 1), &volumeActors[s]);
#ifdef RASTERON_H
        mainScene.addTexture("surfaceActor" + std::to_string(s + 1), _images[0]);
        mainScene.addTexture("coneActor" + std::to_string(s + 1), _images[1]);
        mainScene.addTexture("volumeActor" + std::to_string(s + 1), _images[2]);
#endif
    }
    _renderer->buildScene(&mainScene);
    _renderer->texturizeScene(&mainScene);

    for(auto b = _billboards.begin(); b != _billboards.end(); b++){ 
        (*b)->configure(&overlayScene);
        Rasteron_Image* gradientImg = gradientImgOp({ 1024, 1024 }, SIDE_Bottom, 0xFF333333, 0xFFEEEEEE);
        for(unsigned short paneIndex = 0; paneIndex < (*b)->getParams()->getGridSize(); paneIndex++){
            (*b)->getGeoActor(paneIndex)->pickerFunc = &Sandbox_Demo::onMenuPick;
            (*b)->getImgAt(paneIndex)->setImage(gradientImg);

            if((*b)->getPrefix() == "actions_board_"){
               if(paneIndex > 2 && paneIndex < SANDBOX_PANE_COUNT - 3) (*b)->getGeoActor(paneIndex)->isShown = false;
               /* else if(paneIndex == 0) (*b)->getImgAt(paneIndex)->setFileImage(std::string(IMAGES_DIR) + "/containers/Camera-Zoom.png");
               else if(paneIndex == 1) (*b)->getImgAt(paneIndex)->setFileImage(std::string(IMAGES_DIR) + "/containers/Camera-Pivot.png");
               else if(paneIndex == 2) (*b)->getImgAt(paneIndex)->setFileImage(std::string(IMAGES_DIR) + "/containers/Camera-Pan.png");
               else if(paneIndex == SANDBOX_PANE_COUNT - 3) (*b)->getImgAt(paneIndex)->setFileImage(std::string(IMAGES_DIR) + "/containers/Actor-Size.png");
               else if(paneIndex == SANDBOX_PANE_COUNT - 2) (*b)->getImgAt(paneIndex)->setFileImage(std::string(IMAGES_DIR) + "/containers/Actor-Rot.png");
               else if(paneIndex == SANDBOX_PANE_COUNT - 1) (*b)->getImgAt(paneIndex)->setFileImage(std::string(IMAGES_DIR) + "/containers/Actor-Pos.png"); */
               else (*b)->overlay(paneIndex, _buttons[paneIndex]);
            }
            else if((*b)->getPrefix() == "sculpt_board_"){
                if(paneIndex > 2 && paneIndex < SANDBOX_PANE_COUNT - 1) (*b)->getGeoActor(paneIndex)->isShown = false;
                else (*b)->overlay(paneIndex, _buttons[paneIndex]);
            }
            else if((*b)->getPrefix() == "paint_board_"){ 
                if(paneIndex > 2 && paneIndex < SANDBOX_PANE_COUNT - 1) (*b)->getGeoActor(paneIndex)->isShown = false;
                else (*b)->overlay(paneIndex, _buttons[paneIndex]);
            }
            else if((*b)->getPrefix() == "objs_board_"){
                if((paneIndex == 0 || paneIndex == SANDBOX_PANE_COUNT - 1)) (*b)->getGeoActor(paneIndex)->isShown = false;
                else (*b)->overlay(paneIndex, _buttons[paneIndex]);
            }
            else (*b)->overlay(paneIndex, _buttons[paneIndex]);
        }
        RASTERON_DEALLOC(gradientImg);
    }
    /* _billboards[0]->getImgAt(0)->setFileImage(std::string(IMAGES_DIR) + "/containers/Container-ORG-Cut.png");
    for(unsigned short p = 0; p < _billboards[1]->getParams()->getGridSize(); p++) _billboards[1]->getImgAt(p)->setFileImage(std::string(IMAGES_DIR) + "/containers/Container-ORG-Inset.png");
    for(unsigned short p = 0; p < _billboards[2]->getParams()->getGridSize(); p++) _billboards[2]->getImgAt(p)->setFileImage(std::string(IMAGES_DIR) + "/containers/Container-GRN-Cut.png");
    for(unsigned short p = 0; p < _billboards[3]->getParams()->getGridSize(); p++) _billboards[3]->getImgAt(p)->setFileImage(std::string(IMAGES_DIR) + "/containers/Container-BLU-Cut.png"); */

    _renderer->buildScene(&overlayScene);
    _renderer->texturizeScene(&overlayScene);
}

void Sandbox_Demo::loop(double frameTime){
    static unsigned colorPick;
    static Vec3f coordPick;

    if(Platform::mouseControl.getIsMouseDown().second){
        // check overlay for match first
        if(Topl_Program::pickerObj == nullptr) {
            colorPick = colorPicker(&overlayScene);
            coordPick = coordPicker(&overlayScene);
        }

        // check main scene for match
        if(Topl_Program::pickerObj == nullptr) {
            colorPick = colorPicker(&mainScene);
            coordPick = coordPicker(&mainScene);
        }
        // _renderer->texturizeScene(&mainScene);

        // check against backdrop for match?
        if(Topl_Program::pickerObj == nullptr){
            colorPick = colorPicker(&backdropScene);
            coordPick = coordPicker(&backdropScene);
        }
    } else Topl_Program::pickerObj = nullptr; // resets

    _renderer->setDrawMode(DRAW_Triangles);

    // Topl_Factory::switchPipeline( _renderer, _effectPipeline);
    // _renderer->updateScene(&backdropScene);
    // _renderer->drawScene(&backdropScene);

    // _flatVShader.setMode(FLAT_MODE_COORD);
    Topl_Factory::switchPipeline(_renderer, _texPipeline);
    _renderer->setCamera(&Topl_Program::cameraObj);
    _renderer->updateScene(&mainScene);
    _renderer->drawScene(&mainScene);

    // _flatVShader.setMode(FLAT_MODE_SOLID);
    // Topl_Factory::switchPipeline(_renderer, _flatPipeline);
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