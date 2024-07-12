#include "Sandbox.hpp"

SANDBOX_Action Sandbox_Demo::_action = SANDBOX_Move;
unsigned short Sandbox_Demo::_mode = 0;

static Vec3f pointsSet[4] = { Vec3f({ 0.0F, 0.0F, 0.0F }), Vec3f({ 0.0F, 0.0F, 0.0F }), Vec3f({ 0.0F, 0.0F, 0.0F }), Vec3f({ 0.0F, 0.0F, 0.0F }) };
static Img_Base paintImg = Img_Base(checkerImgOp({ 1024, 1024 }, { 2, 2, 0xFF0000FF, 0xFFFF0000 }));

static void onSpace(){
    // logMessage("onSpace() triggered");
}

static void onAnyKey(char k){
    // logMessage("onAnyKey() triggered");
}

static void onPress(float x, float y){
    static unsigned short pressCount = 0;
    pointsSet[pressCount % 4] = Vec3f({ Platform::getCursorX(), Platform::getCursorY(), 0.0F });
    pressCount++;
}

static void onDrag(float x, float y){
    // logMessage("onDrag() triggered");
}

static void onScroll(bool isZoom){
    if(Topl_Program::pickerObj != nullptr){
        switch(Sandbox_Demo::_action){
            case SANDBOX_Move: Topl_Program::pickerObj->updatePos({ 0.0F, 0.0F, (isZoom)? 0.05F : -0.05F }); break;
            case SANDBOX_Rotate: Topl_Program::pickerObj->updateRot({ 0.0F, 0.0F, (isZoom)? 0.05F : -0.05F }); break;
            case SANDBOX_Scale: Topl_Program::pickerObj->updateSize({ 0.0F, 0.0F, (isZoom)? 0.05F : -0.05F }); break;
        }
    }
}

static void timeAdjEvent(double m){
    // logMessage("timeAdjEvent() triggered");
}

static void timeStepEvent(){
    // logMessage("timeStepEvent() triggered");
}

void Sandbox_Demo::onBackdropPick(MOUSE_Event event){
    if(event == MOUSE_LeftBtn_Drag || event == MOUSE_LeftBtn_Press){
        switch(_action){
            case SANDBOX_Pan:
                Topl_Program::cameraObj.updatePos({ (Topl_Program::lastPickerCoord[0] - Topl_Program::pickerCoord[0] - 0.25F) / 20.0F, (Topl_Program::lastPickerCoord[1] - Topl_Program::pickerCoord[1] - 0.25F) / 20.0F, 0.0F });
                break;
            case SANDBOX_Pivot:
                Topl_Program::cameraObj.updateRot({ Topl_Program::lastPickerCoord[0] - Topl_Program::pickerCoord[0], Topl_Program::lastPickerCoord[1] - Topl_Program::pickerCoord[1], 0.0F });
                break;
            case SANDBOX_Zoom:
                Topl_Program::cameraObj.setZoom(*Topl_Program::cameraObj.getZoom() + (Topl_Program::lastPickerCoord[0] - Topl_Program::pickerCoord[0]) + ( Topl_Program::lastPickerCoord[1] - Topl_Program::pickerCoord[1]));
                break;
            case SANDBOX_Sculpt:
                // _DEMO->_meshes.push_back(new Geo_Mesh({ pointsSet[0], pointsSet[1], pointsSet[2] }));
                switch(Sandbox_Demo::_mode){
                    case 0: _DEMO->_meshes.push_back(new Geo_Surface({ pointsSet[0], pointsSet[1], pointsSet[2] })); break;
                    case 1: _DEMO->_meshes.push_back(new Geo_Cone({ pointsSet[0], pointsSet[1], pointsSet[2] }, Vec3f({ 0.0, 0.0, 1.0F }) )); break;
                    case 2: _DEMO->_meshes.push_back(new Geo_Volume({ pointsSet[0], pointsSet[1], pointsSet[2] }, 1.0F )); break;
                    default: _DEMO->_meshes.push_back(new Geo_Mesh({ pointsSet[0], pointsSet[1], pointsSet[2] }));

                }
                _DEMO->_actors.push_back(new Geo_Actor(_DEMO->_meshes.back()));
                _DEMO->_actors.back()->setPos({ Platform::getCursorX(), Platform::getCursorY(), 0.0F });
                // _DEMO->mainScene.addGeometry(_DEMO->_actors.back());
                // TODO: Rebuild scene to dislay
                break;
        }
    }
}

/* void Sandbox_Demo::updateAnim(){
    // _positions_map[Topl_Program::pickerObj] = Topl_Program::pickerObj->getPos();
    // Topl_Program::timeline.addSequence_vec3f(&_positions_map[Topl_Program::pickerObj], std::make_pair(TIMELINE_AT, _positions_map[Topl_Program::pickerObj]));
    // _rotations_map[Topl_Program::pickerObj] = Topl_Program::pickerObj->getRot();
    // Topl_Program::timeline.addSequence_vec3f(&_positions_map[Topl_Program::pickerObj], std::make_pair(TIMELINE_AT, _rotations_map[Topl_Program::pickerObj]));
    // _scales_map[Topl_Program::pickerObj] = Topl_Program::pickerObj->getSize();
    // Topl_Program::timeline.addSequence_vec3f(&_scales_map[Topl_Program::pickerObj], std::make_pair(TIMELINE_AT, _scales_map[Topl_Program::pickerObj]));
} */


void Sandbox_Demo::onObjectPick(MOUSE_Event event){
    if(Platform::mouseControl.getIsMouseDown().second){
        if(Platform::mouseControl.getIsMouseDown().first == MOUSE_LeftBtn_Press || Platform::mouseControl.getIsMouseDown().first == MOUSE_LeftBtn_Drag){
            switch(_action){
                case SANDBOX_Move: Topl_Program::pickerObj->setPos(Topl_Program::getCamRelPos(nullptr)); break;
                case SANDBOX_Rotate:
                    // Topl_Program::pickerObj->updateRot({ Topl_Program::lastPickerCoord[0] - Topl_Program::pickerCoord[0], Topl_Program::lastPickerCoord[1] - Topl_Program::pickerCoord[1], 0.0F });
                    Topl_Program::pickerObj->updateRot({ Topl_Program::lastPickerCoord[0] - Topl_Program::pickerCoord[0], Topl_Program::lastPickerCoord[1] - Topl_Program::pickerCoord[1], 0.0F });
                    break;
                case SANDBOX_Scale:
                    Topl_Program::pickerObj->updateSize({ Topl_Program::lastPickerCoord[0] - Topl_Program::pickerCoord[0], Topl_Program::lastPickerCoord[1] - Topl_Program::pickerCoord[1], 0.0F });
                    break;
                case SANDBOX_Paint:
                    std::cout << "special paint triggered!" << std::endl;
                    Rasteron_Image* stageImage = copyImgOp(paintImg.getImage());
                    switch(Sandbox_Demo::_mode){ }// TODO: Implement painting algorithm
                    paintImg.setImage(stageImage);
                    // TODO: Update cooresponding picker object and texturize scene
                    RASTERON_DEALLOC(stageImage);
                    break;
            }
            _DEMO->_billboards.back()->toggleShow(false);
        }
        else if(Platform::mouseControl.getIsMouseDown().first == MOUSE_RightBtn_Press || Platform::mouseControl.getIsMouseDown().first == MOUSE_RightBtn_Drag){
            _DEMO->_billboards.back()->shift(*Topl_Program::pickerObj->getPos() - _DEMO->_billboards.back()->getOrigin() + Vec3f({ 0.0F, 0.1F, 0.0F }));
            _DEMO->_billboards.back()->toggleShow(true);
        }
    }
}

void Sandbox_Demo::onMenuPick(MOUSE_Event event){
    _mode = std::stoi(Topl_Program::pickerObj->getNameExt());

    if(Topl_Program::pickerObj->getName().find("actions_board") != std::string::npos){
        _action = (SANDBOX_Action)_mode;
        _DEMO->_billboards.at(SANDBOX_ACTION)->resetState();
        _DEMO->_billboards.at(SANDBOX_ACTION)->setState(_mode - 1, Platform::mouseControl.getIsMouseDown().second);
    } 
    else if(Topl_Program::pickerObj->getName().find("objs_board") != std::string::npos)
        _DEMO->_billboards.at(SANDBOX_PICKER)->setState(_mode - 1, Platform::mouseControl.getIsMouseDown().second);
    else if(Topl_Program::pickerObj->getName().find("sculpt_board") != std::string::npos){ 
        _action = SANDBOX_Sculpt;
        _DEMO->_billboards.at(SANDBOX_ACTION)->resetState();
        _DEMO->_billboards.at(SANDBOX_SCULPT)->setState(_mode - 1, Platform::mouseControl.getIsMouseDown().second);
    }
    else if(Topl_Program::pickerObj->getName().find("paint_board") != std::string::npos){ 
        _action = SANDBOX_Paint;
        _DEMO->_billboards.at(SANDBOX_ACTION)->resetState();
        _DEMO->_billboards.at(SANDBOX_PAINT)->setState(_mode - 1, Platform::mouseControl.getIsMouseDown().second);
    }
    // }
}


void Sandbox_Demo::init(){
    Platform::keyControl.addCallback(' ', onSpace);
    Platform::keyControl.addAnyCallback(onAnyKey);
    Platform::mouseControl.addCallback(MOUSE_RightBtn_Press, onPress);
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
        mainScene.addTexture("surfaceActor" + std::to_string(s + 1), _images[0]); mainScene.addVolumeTex("surfaceActor" + std::to_string(s + 1), _imgVolumes[0]);
        mainScene.addTexture("coneActor" + std::to_string(s + 1), _images[1]); mainScene.addVolumeTex("coneActor" + std::to_string(s + 1), _imgVolumes[1]);
        mainScene.addTexture("volumeActor" + std::to_string(s + 1), _images[2]); mainScene.addVolumeTex("volumeActor" + std::to_string(s + 1), _imgVolumes[2]);
#endif
    }
    _renderer->buildScene(&mainScene);
    _renderer->texturizeScene(&mainScene);

    for(auto b = _billboards.begin(); b != _billboards.end(); b++){ 
        (*b)->configure(&overlayScene);
        Rasteron_Image* gradientImg = gradientImgOp({ 1024, 1024 }, SIDE_Bottom, 0xFF333333, 0xFF00EE00);
        for(unsigned short paneIndex = 0; paneIndex < (*b)->getParams()->getGridSize(); paneIndex++){
            (*b)->getGeoActor(paneIndex)->pickerFunc = &Sandbox_Demo::onMenuPick;
            (*b)->getImgAt(paneIndex)->setImage(gradientImg);

            if((*b)->getPrefix() == "actions_board_"){
               if(paneIndex > 2 && paneIndex < SANDBOX_PANE_COUNT - 3) (*b)->getGeoActor(paneIndex)->isShown = false;
               else if(paneIndex == 0) (*b)->overlay(paneIndex, _buttons[_buttons.size() - 6]);
               else if(paneIndex == 1) (*b)->overlay(paneIndex, _buttons[_buttons.size() - 5]);
               else if(paneIndex == 2) (*b)->overlay(paneIndex, _buttons[_buttons.size() - 4]);
               else if(paneIndex == SANDBOX_PANE_COUNT - 3) (*b)->overlay(paneIndex, _buttons[_buttons.size() - 3]);
               else if(paneIndex == SANDBOX_PANE_COUNT - 2) (*b)->overlay(paneIndex, _buttons[_buttons.size() - 2]);
               else if(paneIndex == SANDBOX_PANE_COUNT - 1) (*b)->overlay(paneIndex, _buttons[_buttons.size() - 1]);
               // else (*b)->overlay(paneIndex, _buttons[paneIndex]);
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
                else (*b)->overlay(paneIndex, _labels[paneIndex]);
                // else (*b)->overlay(paneIndex, _buttons[paneIndex]);
            }
            else if((*b)->getPrefix() == "props_board_"){ 
                (*b)->toggleShow(false);
                (*b)->overlay(paneIndex, _labels[_labels.size() - 1 - paneIndex]);
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
            _renderer->setCamera(&fixedCamera);
            colorPick = colorPicker(&overlayScene);
            coordPick = coordPicker(&overlayScene);
            _renderer->texturizeScene(&overlayScene); // Optimize this!
        }

        // check main scene for match
        // if(Topl_Program::pickerObj == nullptr){
            _renderer->setCamera(&Topl_Program::cameraObj);
            colorPick = colorPicker(&mainScene);
            coordPick = coordPicker(&mainScene);
            _renderer->texturizeScene(&mainScene); // Optimize this!
        // }
        // _renderer->texturizeScene(&mainScene);

        // check against backdrop for match?
        if(Topl_Program::pickerObj == nullptr){
            colorPick = colorPicker(&backdropScene);
            coordPick = coordPicker(&backdropScene);
        }
    } else Topl_Program::pickerObj = nullptr; // resets

    /* if(_renderer->getFrameCount() % 60 == 0){ // TODO: Move to separate thread
        for(unsigned short i = 0; i < SANDBOX_MESH_COUNT; i++) _images[i]->setColorImage(RAND_COLOR());
        _renderer->texturizeScene(&mainScene);
    } */

    _renderer->setDrawMode(DRAW_Triangles);

    // Topl_Factory::switchPipeline( _renderer, _effectPipeline);
    // _renderer->updateScene(&backdropScene);
    // _renderer->drawScene(&backdropScene);

    // _flatVShader.setMode(FLAT_MODE_COORD);
    Topl_Factory::switchPipeline(_renderer, _effectPipeline);
    _effectVShader.setMode(EFFECT_MODE_CURSORY);
    _texVShader.setMode((_renderer->getFrameCount() / 80) % 8);
    _renderer->setCamera(&Topl_Program::cameraObj);
    _renderer->updateScene(&mainScene);
    _renderer->drawScene(&mainScene);

    // _flatVShader.setMode(FLAT_MODE_SOLID);
    Topl_Factory::switchPipeline(_renderer, _texPipeline);
    _texVShader.setMode(0);
    _renderer->setCamera(&fixedCamera);
    _renderer->updateScene(&overlayScene);
    _renderer->drawScene(&overlayScene);
}

int main(int argc, char** argv) {
    _DEMO = new Sandbox_Demo(argv[0], BACKEND_GL4);

    // std::thread texturizeThread; // update textures
    _DEMO->run();
    // texturizeThread.join();
    delete(_DEMO);
    return 0;
}