#include "Sandbox.hpp"

unsigned short Sandbox_Demo::mode = 0;
unsigned short Sandbox_Demo::option = 0;

static bool isRebuildReq = false;
static std::vector<Vec3f> sculptPoints;
static bool isRepaintReq = false;
static Img_Canvas canvasImg = Img_Canvas(0xFFFF | 0x22000000);
static bool isTick = false;
static bool isModal = false;

void onAnyKey(char key){
    if(isdigit(key)){ Sandbox_Demo::mode = key - '0'; }
}

void onActionPanePress(MOUSE_Event event){
    if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
        isModal = Sandbox_Demo::mode == SANDBOX_ACTION;
        Sandbox_Demo::mode = SANDBOX_ACTION;
        Sandbox_Demo::option = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;
    }
}

void onSculptPanePress(MOUSE_Event event){
    if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
        isModal = Sandbox_Demo::mode == SANDBOX_SCULPT;
        Sandbox_Demo::mode = SANDBOX_SCULPT;
        Sandbox_Demo::option = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;
    }
}

void onPaintPanePress(MOUSE_Event event){
    if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
        isModal = Sandbox_Demo::mode == SANDBOX_PAINT;
        Sandbox_Demo::mode = SANDBOX_PAINT;
        Sandbox_Demo::option = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;
    }
}

void onTick(){ isTick = true; }

void onEditAction(float x, float y){
    if(Sandbox_Demo::mode == SANDBOX_SCULPT){
        Vec3f pointVec = Vec3f({ Platform::getCursorX(), Platform::getCursorY(), 0.0F });
        Vec3f nearestVec = Vec3f({ INVALID_CURSOR_POS, INVALID_CURSOR_POS, 0.0F });
        Geo_Actor* nearestNode = nullptr;
        for(unsigned n = 0; n < _DEMO->plotGrid.getActorCount(); n++){
            Vec3f oldVec = pointVec - nearestVec;
            Vec3f diffVec = pointVec - *(_DEMO->plotGrid.getGeoActor(n)->getPos());
            if(diffVec.len() < oldVec.len()){ 
                nearestVec = *(_DEMO->plotGrid.getGeoActor(n)->getPos()); // calculate nearest point
                nearestNode = _DEMO->plotGrid.getGeoActor(n);
            }
        }
        if(nearestNode != nullptr) nearestNode->setSize(Vec3f({2.0F, 2.0F, 2.0F }));
        sculptPoints.push_back(nearestVec);
    } else if(Sandbox_Demo::mode == SANDBOX_PAINT){
        Input_TracerStep lastStep = Platform::mouseControl.getTracerSteps()->at(Platform::mouseControl.getTracerSteps()->size() - 2);
        if(Sandbox_Demo::option == 1) canvasImg.drawPath(0.1F, { lastStep.step.first + 0.5F, 1.0F - (lastStep.step.second + 0.5F) }, { Platform::getCursorX() + 0.5F, 1.0F - (Platform::getCursorY() + 0.5F) }, RAND_COLOR());
        else if(Sandbox_Demo::option == 2) canvasImg.drawBox({ lastStep.step.first + 0.5F, 1.0F - (lastStep.step.second + 0.5F) }, { Platform::getCursorX() + 0.5F, 1.0F - (Platform::getCursorY() + 0.5F) }, RAND_COLOR());
        else canvasImg.drawDot(0.1F, { Platform::getCursorX() + 0.5F, 1.0F - (Platform::getCursorY() + 0.5F) }, RAND_COLOR());
        isRepaintReq = true; // set for immediate updates
    }
}

void onChangeAction(float x, float y){
    if(Sandbox_Demo::mode == SANDBOX_ACTION){
        Input_TracerPath lastPath = Platform::mouseControl.getTracerPaths()->back();
        Vec2f diffVec = Vec2f({ 0.0F, 0.0F });
        if(lastPath.stepsCount % MAX_PATH_STEPS > 2){ 
            unsigned short steps = lastPath.stepsCount % MAX_PATH_STEPS;
            diffVec = Vec2f({ lastPath.steps[steps - 1].first - lastPath.steps[steps - 2].first, lastPath.steps[steps - 1].second - lastPath.steps[steps - 2].second  });
        }

        if(Topl_Program::pickerObj != nullptr)
            if(Topl_Program::pickerObj->getName().find("board") == std::string::npos) // can properly update
                switch(Sandbox_Demo::option){
                case SANDBOX_MOVE:
                    Topl_Program::pickerObj->updatePos(Vec3f({ diffVec[0], diffVec[1], 0.0F }) * 1.25F);
                    if(_DEMO->positions_map.find(Topl_Program::pickerObj) == _DEMO->positions_map.end()) _DEMO->positions_map.insert({ Topl_Program::pickerObj, *Topl_Program::pickerObj->getPos() });
                    Topl_Program::timeline.addSequence_vec3f(&_DEMO->positions_map[Topl_Program::pickerObj], std::make_pair(TIMELINE_AT, *Topl_Program::pickerObj->getPos()));
                break;
                case SANDBOX_ROTATE:
                    Topl_Program::pickerObj->updateRot(Vec3f({ diffVec[0], diffVec[1], 0.0F }) * 2.0F);
                    if(_DEMO->rotations_map.find(Topl_Program::pickerObj) == _DEMO->rotations_map.end()) _DEMO->rotations_map.insert({ Topl_Program::pickerObj, *Topl_Program::pickerObj->getRot() });
                    Topl_Program::timeline.addSequence_vec3f(&_DEMO->rotations_map[Topl_Program::pickerObj], std::make_pair(TIMELINE_AT, *Topl_Program::pickerObj->getRot()));
                break;
                case SANDBOX_SIZE:
                    Topl_Program::pickerObj->updateSize(Vec3f({ diffVec[0], diffVec[1], 0.0F }) * 2.0f);
                    if(_DEMO->scales_map.find(Topl_Program::pickerObj) == _DEMO->scales_map.end()) _DEMO->scales_map.insert({ Topl_Program::pickerObj, *Topl_Program::pickerObj->getSize() });
                    Topl_Program::timeline.addSequence_vec3f(&_DEMO->scales_map[Topl_Program::pickerObj], std::make_pair(TIMELINE_AT, *Topl_Program::pickerObj->getSize()));
                break;
                }
        switch(SANDBOX_PANES - Sandbox_Demo::option - 1){
            case SANDBOX_MOVE: Topl_Program::cameraObj.updatePos(Vec3f({ diffVec[0], diffVec[1], 0.0F })); break;
            case SANDBOX_ROTATE: Topl_Program::cameraObj.updateRot(Vec3f({ diffVec[0], diffVec[1], 0.0F })); break;
            case SANDBOX_SIZE: Topl_Program::cameraObj.setZoom(*Topl_Program::cameraObj.getZoom() + diffVec[0]); break;
        }
    }
}

void onConfirmAction(float x, float y){
    if(Sandbox_Demo::mode == SANDBOX_SCULPT){
        if(sculptPoints.size() > 2) {
            switch(Sandbox_Demo::option){
                case 0: _DEMO->objectMeshes.push_back(new Geo_Surface(sculptPoints.data(), sculptPoints.size())); break;
                case 1: _DEMO->objectMeshes.push_back(new Geo_Cone(sculptPoints.data(), sculptPoints.size(), Vec3f({ 0.0F, 0.0F, -1.0F }))); break;
                case 2: _DEMO->objectMeshes.push_back(new Geo_Volume(sculptPoints.data(), sculptPoints.size(), 1.0F)); break;
                default: _DEMO->objectMeshes.push_back(new Geo_Mesh(sculptPoints.data(), sculptPoints.size())); 
            }
            _DEMO->objectTextures.push_back(nullptr); // non-textured object
            _DEMO->objectPhysics.push_back(nullptr); // non-dynamic object
            _DEMO->objectActors.push_back(new Geo_Actor(_DEMO->objectMeshes.back()));
            for(unsigned n = 0; n < _DEMO->plotGrid.getActorCount(); n++) _DEMO->plotGrid.getGeoActor(n)->setSize(Vec3f({ 1.0F, 1.0F, 1.0F }));
            sculptPoints.clear();
            isRebuildReq = true;
        }
        else std::cerr << "Not enough sculpt points!" << std::endl;
    } else if(Sandbox_Demo::mode == SANDBOX_PAINT){
        std::cout << "Handle paint confirm action..." << std::endl;
    }
}

void Sandbox_Demo::init(){
    Platform::keyControl.addAnyCallback(onAnyKey);
    Platform::mouseControl.addCallback(MOUSE_RightBtn_Release, onEditAction);
    Platform::mouseControl.addCallback(MOUSE_LeftBtn_Release, onConfirmAction);
    Platform::mouseControl.addDragCallback(onChangeAction);

    Topl_Program::timeline.persist_ticker.addPeriodicEvent(1000, onTick);

    modeBillboard.configure(&overlayScene);
    actionsBillboard.configure(&overlayScene);
    timeBillboard.configure(&overlayScene);
    // objectsBillboard.configure(&overlayScene);
    sculptBillboard.configure(&overlayScene);
    paintBillboard.configure(&overlayScene);
    propsBillboard.configure(&overlayScene);
    timeTextBillboard.configure(&overlayScene);
    timeCtrlBillboard.configure(&overlayScene);

    std::string fontPath = std::string(FONTS_DIR) + "Tw-Cen-MT.ttf";
    _labels.push_back(new Img_Label(MENU_XL, { fontPath.c_str(), "Blank", 0xFF111111, 0xFFEEEEEE }));
    modeBillboard.overlay(0, _labels.back());
    modeBillboard.toggleShow(false);
    _labels.push_back(new Img_Label(MENU_Large, { fontPath.c_str(), "[ 0:00 ]", 0xFF111111, 0xFFEEEEEE }));
    timeTextBillboard.overlay(0, _labels.back());
    _buttons.push_back(new Img_Button(MENU_XL, "fast_forward"));
    timeCtrlBillboard.overlay(0, _buttons.back());
    _buttons.push_back(new Img_Button(MENU_XL, "play_arrow"));
    timeCtrlBillboard.overlay(1, _buttons.back());
    _buttons.push_back(new Img_Button(MENU_XL, "fast_rewind"));
    timeCtrlBillboard.overlay(2, _buttons.back());
    _sliders.push_back(new Img_Slider(MENU_XL, 60));
    timeBillboard.overlay(0, _sliders.back());
    for(unsigned p = 0; p < actionsBillboard.getParams()->getGridSize(); p++){
        switch(p){
            case SANDBOX_MOVE: _buttons.push_back(new Img_Button(MENU_XL, "control_camera")); break;
            case SANDBOX_ROTATE:_buttons.push_back(new Img_Button(MENU_XL, "flip_camera_android")); break;
            case SANDBOX_SIZE:  _buttons.push_back(new Img_Button(MENU_XL, "filter_center_focus")); break;
            case SANDBOX_PANES - SANDBOX_MOVE - 1: _buttons.push_back(new Img_Button(MENU_XL, "gamepad")); break;
            case SANDBOX_PANES - SANDBOX_ROTATE - 1: _buttons.push_back(new Img_Button(MENU_XL, "crop_rotate")); break;
            case SANDBOX_PANES - SANDBOX_SIZE - 1: _buttons.push_back(new Img_Button(MENU_XL, "aspect_ratio")); break;
            default: _buttons.push_back(new Img_Button(MENU_XL, "sort"));
        }
        actionsBillboard.overlay(p, _buttons.back());
        (p < 3 || p >= SANDBOX_PANES - 3)? actionsBillboard.getGeoActor(p)->pickerFunc = onActionPanePress : actionsBillboard.getGeoActor(p)->isShown = false;
    }
    for(unsigned p = 0; p < sculptBillboard.getParams()->getGridSize(); p++){
        switch(p){
            case SANDBOX_PANES - 1: _buttons.push_back(new Img_Button(MENU_XL, "folder_open")); break;
            default: _buttons.push_back(new Img_Button(MENU_XL, "category"));
        }
        sculptBillboard.overlay(p, _buttons.back());
        (p < 3 || p >= SANDBOX_PANES - 1)? sculptBillboard.getGeoActor(p)->pickerFunc = onSculptPanePress : sculptBillboard.getGeoActor(p)->isShown = false;
    }
    for(unsigned p = 0; p < paintBillboard.getParams()->getGridSize(); p++){
        switch(p){
            case SANDBOX_PANES - 1: _buttons.push_back(new Img_Button(MENU_XL, "folder_open")); break;
            default: _buttons.push_back(new Img_Button(MENU_XL, "brush"));
        }
        paintBillboard.overlay(p, _buttons.back());
        (p < 3 || p >= SANDBOX_PANES - 1)? paintBillboard.getGeoActor(p)->pickerFunc = onPaintPanePress : paintBillboard.getGeoActor(p)->isShown = false;
    }
    for(unsigned p = 0; p < propsBillboard.getParams()->getGridSize(); p++){
        _dials.push_back(new Img_Dial(MENU_XL, 4));
        propsBillboard.overlay(p, _dials.back());
    }

    // plotChain.configure(&editsScene);
    plotGrid.configure(&editsScene);

    Topl_Factory::switchPipeline(_renderer, _texPipeline);
    _renderer->buildScene(&overlayScene);
    _renderer->texturizeScene(&overlayScene);

    canvasScene.addGeometry("canvas", &backdropActor);
    _renderer->buildScene(&canvasScene);

    for(unsigned i = 1; i < 8; i++){ 
        _images.push_back(new Img_Base(checkeredImgOp({ 1024, 1024 }, { i * 2, i * 2, RAND_COLOR(), RAND_COLOR() })));
        canvasScene.addTexture(std::to_string(i), _images.back());
    }
    canvasScene.addTexture("canvas", &canvasImg);

    _renderer->buildScene(&editsScene);
}

void Sandbox_Demo::loop(double frameTime){
    updateOverlay();
    
    if(Sandbox_Demo::mode == SANDBOX_PAINT || Sandbox_Demo::mode == SANDBOX_TIME){
        /* if(isRepaintReq){
            for(unsigned short i = 0; i < objectTextures.size(); i++)
                if(objectTextures[i] != nullptr) mainScene.addTexture("object" + std::to_string(i), objectTextures[i]);

            if(mainScene.getIsTextured()) _renderer->texturizeScene(&mainScene);
            if(canvasScene.getIsTextured()) _renderer->texturizeScene(&canvasScene);
            isRepaintReq = false;
        } */
        
        _renderer->setCamera(&fixedCamera);
        _renderer->texturizeScene(&canvasScene);
        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(_renderer, _texPipeline);
        _renderer->updateScene(&canvasScene);
        _renderer->drawScene(&canvasScene);
        backdropActor.updatePos({ 0.0F, 0.0F, 0.01F });
        _effectVShader.setMode(2);
        Topl_Factory::switchPipeline(_renderer, _effectPipeline);
        _renderer->updateScene(&canvasScene);
        _renderer->drawScene(&canvasScene);
        backdropActor.updatePos({ 0.0F, 0.0F, -0.01F });
    }
    else if(Sandbox_Demo::mode == SANDBOX_SCULPT){
        _flatVShader.setMode(5);
        Topl_Factory::switchPipeline(_renderer, _flatPipeline);
        _renderer->updateScene(&editsScene);
        _renderer->drawScene(&editsScene);
        _effectVShader.setMode(0);
        Topl_Factory::switchPipeline(_renderer, _effectPipeline);
        _renderer->updateScene(&editsScene);
        _renderer->drawScene(&editsScene);
    } else {
        if(isRebuildReq){
            for(unsigned short g = mainScene.getActorCount(); g < objectActors.size(); g++)
                mainScene.addGeometry("object" + std::to_string(g), objectActors[g]);

            if(mainScene.getActorCount() > 0) _renderer->buildScene(&mainScene);
            isRebuildReq = false;
        }

        if(mainScene.getActorCount() > 0 && objectActors.size() > 0){
            if(Platform::mouseControl.getIsMouseDown().second && Topl_Program::pickerObj == nullptr){ 
                colorPicker(&mainScene);
                if(Topl_Program::pickerObj != nullptr) coordPicker(&mainScene);
            }
            _texVShader.setMode(0);
            _renderer->setDrawMode(DRAW_Triangles);

            Topl_Factory::switchPipeline(_renderer, _beamsPipeline);
            _renderer->setCamera(&Topl_Program::cameraObj);
            _renderer->updateScene(&mainScene);
            _renderer->drawScene(&mainScene);
        }
    }

    _renderer->setCamera(&fixedCamera);
    _renderer->setDrawMode(DRAW_Triangles);
    _texVShader.setMode(0);
    Topl_Factory::switchPipeline(_renderer, _texPipeline);
    _renderer->updateScene(&overlayScene);
    _renderer->drawScene(&overlayScene);
}

void Sandbox_Demo::updateOverlay(){
    colorPicker(&overlayScene);
    Vec3f coordColor = coordPicker(&overlayScene);
    _renderer->clear();

    if(isTick){
        std::string fontPath = std::string(FONTS_DIR) + "Tw-Cen-MT.ttf";
        unsigned secs = (unsigned)floor(Topl_Program::timeline.dynamic_ticker.getAbsSecs());
        std::string timeText = "[ " + ((secs >= 60)? std::to_string(secs / 60) + ":" + ((secs % 10 == 0)? "0" : "") : "") + std::to_string(secs % 60) + ":00 ]";
        _labels[1]->setText({ fontPath.c_str(), timeText.c_str(), 0xFF111111, 0xFFEEEEEE });
        timeTextBillboard.setState(0, !Topl_Program::timeline.dynamic_ticker.isPaused);
        timeBillboard.setState(0, secs / 60.0, 0.0F);
        _renderer->texturizeScene(&overlayScene);
        isTick = false;
    }

    if(Topl_Program::pickerObj != nullptr)
        if(Topl_Program::pickerObj->getName().find("board") != std::string::npos){
            unsigned short index = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;

            if(Topl_Program::pickerObj->getName().find("time_board") != std::string::npos && Platform::mouseControl.getIsMouseDown().second){
                timeBillboard.setState(0, coordColor[0], coordColor[1]);
                Topl_Program::timeline.dynamic_ticker.setTime(coordColor[0] * 60.0); // convert from coordinates to secs
            }
            else if(Topl_Program::pickerObj->getName().find("actions_board") != std::string::npos)
                actionsBillboard.setState(index, Platform::mouseControl.getIsMouseDown().second);
            else if(Topl_Program::pickerObj->getName().find("sculpt_board") != std::string::npos)
                sculptBillboard.setState(index, Platform::mouseControl.getIsMouseDown().second);
            else if(Topl_Program::pickerObj->getName().find("paint_board") != std::string::npos)
                paintBillboard.setState(index, Platform::mouseControl.getIsMouseDown().second);
            else if(Topl_Program::pickerObj->getName().find("time_ctrl_board") != std::string::npos){
                if(Platform::mouseControl.getIsMouseDown().second){
                    switch(index){
                        case 2: Topl_Program::timeline.dynamic_ticker.reset(); break;
                        case 1: Topl_Program::timeline.dynamic_ticker.isPaused = !Topl_Program::timeline.dynamic_ticker.isPaused; break; // toggle pause
                        case 0: Topl_Program::timeline.dynamic_ticker.setTime(Topl_Program::timeline.dynamic_ticker.range.second);
                    }
                }
                timeCtrlBillboard.setState(index, Platform::mouseControl.getIsMouseDown().second);
            }

            /* if(isModal){
                std::string fontPath = std::string(FONTS_DIR) + "Gupis.ttf";
                std::string contentTxt = "Default";
                switch(Sandbox_Demo::mode){
                    case SANDBOX_TIME: contentTxt = "Time";
                    case SANDBOX_ACTION: contentTxt = (Sandbox_Demo::option < SANDBOX_PANES / 2)? Topl_Program::pickerObj->getName() : "Camera"; break;
                    case SANDBOX_SCULPT: contentTxt = "Sculpt"; break;
                    case SANDBOX_PAINT: contentTxt = "Paint"; break;
                }
                _labels[0]->setText({ fontPath.c_str(), contentTxt.c_str(), 0xFF0000FF, 0xFF885588 });
                modeBillboard.setState(0, true);
                isModal = false;
            } */

            _renderer->texturizeScene(&overlayScene);
            // _renderer->texturizeScene(&canvasScene);
        }
    else {
        modeBillboard.resetState();
        actionsBillboard.resetState();
        sculptBillboard.resetState(); 
        paintBillboard.resetState();
    }
}

int main(int argc, char** argv) {
    _DEMO = new Sandbox_Demo(argv[0], BACKEND_GL4);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}