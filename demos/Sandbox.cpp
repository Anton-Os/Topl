#include "Sandbox.hpp"

unsigned short Sandbox_Demo::mode = 0;
unsigned short Sandbox_Demo::option = 0;

static bool isRebuildReq = false;
static std::vector<Vec3f> sculptPoints;
static bool isRepaintReq = false;
static Img_Canvas canvasImg = Img_Canvas(0xFFFF | 0x22000000);
static bool isTick = false;

void onAnyKey(char key){
    if(isdigit(key)){ Sandbox_Demo::mode = key - '0'; }
}

void onActionPanePress(MOUSE_Event event){
    if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
        Sandbox_Demo::mode = SANDBOX_ACTION;
        Sandbox_Demo::option = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;
    }
}

/* void onObjectPanePress(MOUSE_Event event){
    if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
        Sandbox_Demo::mode = SANDBOX_OBJECT;
        Sandbox_Demo::option = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;
    }
} */

void onSculptPanePress(MOUSE_Event event){
    if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
        Sandbox_Demo::mode = SANDBOX_SCULPT;
        Sandbox_Demo::option = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;
    }
}

void onPaintPanePress(MOUSE_Event event){
    if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
        Sandbox_Demo::mode = SANDBOX_PAINT;
        Sandbox_Demo::option = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;
    }
}

void onTick(){
    std::cout << "Dynamic ticker value is" << std::to_string(floor(Topl_Program::timeline.dynamic_ticker.getAbsSecs())) << ", ";
    isTick = true;
}

void onEditAction(float x, float y){
    switch(Sandbox_Demo::mode){
        case SANDBOX_SCULPT: 
            {
                Vec3f pointVec = Vec3f({ Platform::getCursorX(), Platform::getCursorY(), 0.0F });
                Vec3f nearestVec = Vec3f({ INVALID_CURSOR_POS, INVALID_CURSOR_POS, 0.0F });
                Geo_Actor* nearestNode = nullptr;
                for(unsigned n = 0; n < _DEMO->editorGrid.getActorCount(); n++){
                    Vec3f oldVec = pointVec - nearestVec;
                    Vec3f diffVec = pointVec - *(_DEMO->editorGrid.getGeoActor(n)->getPos());
                    if(diffVec.len() < oldVec.len()){ 
                        nearestVec = *(_DEMO->editorGrid.getGeoActor(n)->getPos()); // calculate nearest point
                        nearestNode = _DEMO->editorGrid.getGeoActor(n);
                    }
                }
                if(nearestNode != nullptr) nearestNode->setSize(Vec3f({1.5F, 1.5F, 1.5F }));
                sculptPoints.push_back(nearestVec);
            }
            break;
        case SANDBOX_PAINT:
            Input_TracerStep lastStep = Platform::mouseControl.getTracerSteps()->at(Platform::mouseControl.getTracerSteps()->size() - 2);
            if(Sandbox_Demo::option == 1) canvasImg.drawPath(0.1F, { lastStep.step.first + 0.5F, 1.0F - (lastStep.step.second + 0.5F) }, { Platform::getCursorX() + 0.5F, 1.0F - (Platform::getCursorY() + 0.5F) }, RAND_COLOR());
            else if(Sandbox_Demo::option == 2) canvasImg.drawBox({ lastStep.step.first + 0.5F, 1.0F - (lastStep.step.second + 0.5F) }, { Platform::getCursorX() + 0.5F, 1.0F - (Platform::getCursorY() + 0.5F) }, RAND_COLOR());
            else canvasImg.drawDot(0.1F, { Platform::getCursorX() + 0.5F, 1.0F - (Platform::getCursorY() + 0.5F) }, RAND_COLOR());
            isRepaintReq = true; // set for immediate updates
            break;
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
            switch(Sandbox_Demo::option){
                case 0:
                    Topl_Program::pickerObj->updatePos(Vec3f({ diffVec[0], diffVec[1], 0.0F }) * 1.25F);
                    // if(_DEMO->positions_map.find(Topl_Program::pickerObj) == _DEMO->positions_map.end()) _DEMO->positions_map.insert({ Topl_Program::pickerObj, *Topl_Program::pickerObj->getPos() });
                    // Topl_Program::timeline.addSequence_vec3f(&_DEMO->positions_map[Topl_Program::pickerObj], std::make_pair(TIMELINE_AT, *Topl_Program::pickerObj->getPos()));
                    break;
                case 1:
                    Topl_Program::pickerObj->updateRot(Vec3f({ diffVec[0], diffVec[1], 0.0F }) * 2.0F);
                    // if(_DEMO->rotations_map.find(Topl_Program::pickerObj) == _DEMO->rotations_map.end()) _DEMO->rotations_map.insert({ Topl_Program::pickerObj, *Topl_Program::pickerObj->getPos() });
                    // Topl_Program::timeline.addSequence_vec3f(&_DEMO->rotations_map[Topl_Program::pickerObj], std::make_pair(TIMELINE_AT, *Topl_Program::pickerObj->getRot()));
                    break;
                case 2:
                    Topl_Program::pickerObj->updateSize(Vec3f({ diffVec[0], diffVec[1], 0.0F }) * 2.0f);
                    // if(_DEMO->scales_map.find(Topl_Program::pickerObj) == _DEMO->scales_map.end()) _DEMO->scales_map.insert({ Topl_Program::pickerObj, *Topl_Program::pickerObj->getPos() });
                    // Topl_Program::timeline.addSequence_vec3f(&_DEMO->scales_map[Topl_Program::pickerObj], std::make_pair(TIMELINE_AT, *Topl_Program::pickerObj->getSize()));
                    break;
            }
        switch(Sandbox_Demo::option){
            case SANDBOX_PANES - 1: Topl_Program::cameraObj.updatePos(Vec3f({ diffVec[0], diffVec[1], 0.0F })); break;
            case SANDBOX_PANES - 2: Topl_Program::cameraObj.updateRot(Vec3f({ diffVec[0], diffVec[1], 0.0F })); break;
            case SANDBOX_PANES - 3: Topl_Program::cameraObj.setZoom(*Topl_Program::cameraObj.getZoom() + diffVec[0]); break;
        }
    }
}

void sculptFinish(){
    if(sculptPoints.size() > 2) {
        for(unsigned s = 0; s < sculptPoints.size(); s++) std::cout << "Point is (" << std::to_string(sculptPoints[s][0]) << ", " << std::to_string(sculptPoints[s][1]) << ", " << std::to_string(sculptPoints[s][2]) << ")" << std::endl;
        switch(Sandbox_Demo::option){
            case 0: _DEMO->objectMeshes.push_back(new Geo_Surface(sculptPoints.data(), sculptPoints.size())); break;
            case 1: _DEMO->objectMeshes.push_back(new Geo_Cone(sculptPoints.data(), sculptPoints.size(), Vec3f({ 0.0F, 0.0F, -1.0F }))); break;
            case 2: _DEMO->objectMeshes.push_back(new Geo_Volume(sculptPoints.data(), sculptPoints.size(), 1.0F)); break;
            default: _DEMO->objectMeshes.push_back(new Geo_Mesh(sculptPoints.data(), sculptPoints.size())); 
        }
        _DEMO->objectTextures.push_back(nullptr); // non-textured object
        _DEMO->objectPhysics.push_back(nullptr); // non-dynamic object
        _DEMO->objectActors.push_back(new Geo_Actor(_DEMO->objectMeshes.back()));
        for(unsigned n = 0; n < _DEMO->editorGrid.getActorCount(); n++) _DEMO->editorGrid.getGeoActor(n)->setSize(Vec3f({ 1.0F, 1.0F, 1.0F }));
        sculptPoints.clear();
        isRebuildReq = true;
    }
    else std::cerr << "Sculpt points is empty!" << std::endl;
}

void paintFinish(){
    if(!_DEMO->objectTextures.empty()){
        if(_DEMO->objectTextures.back() == nullptr) _DEMO->objectTextures.back() = new Img_Base(RAND_COLOR());
        _DEMO->objectTextures.back()->setImage(canvasImg.getImage());
        canvasImg.setBackground(0x0000FFFF);
        isRepaintReq = true;
    } else std::cerr << "Texture is empty!" << std::endl;
}

void onConfirmAction(float x, float y){
    switch(Sandbox_Demo::mode){
        case SANDBOX_ACTION: std::cout << "Action confirmation!" << std::endl; break;
        case SANDBOX_SCULPT: sculptFinish(); break;
        case SANDBOX_PAINT: paintFinish(); break;
        default: std::cout << "Unknown operation" << std::endl;
    }
}

void Sandbox_Demo::init(){
    Platform::keyControl.addAnyCallback(onAnyKey);
    Platform::mouseControl.addCallback(MOUSE_RightBtn_Release, onEditAction);
    Platform::mouseControl.addCallback(MOUSE_LeftBtn_Release, onConfirmAction);
    Platform::mouseControl.addDragCallback(onChangeAction);

    Topl_Program::timeline.dynamic_ticker.addPeriodicEvent(1000, onTick);

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
    _labels.push_back(new Img_Label(MENU_Medium, { fontPath.c_str(), "Default", 0xFF111111, 0xFFEEEEEE }));
    modeBillboard.overlay(0, _labels.back());
    modeBillboard.toggleShow(false);
    _labels.push_back(new Img_Label(MENU_Large, { fontPath.c_str(), "[ 0:00 ]", 0xFF111111, 0xFFEEEEEE }));
    timeTextBillboard.overlay(0, _labels.back());
    _buttons.push_back(new Img_Button(MENU_Medium, "arrow_right"));
    timeCtrlBillboard.overlay(0, _buttons.back());
    _buttons.push_back(new Img_Button(MENU_Medium, "play_arrow"));
    timeCtrlBillboard.overlay(1, _buttons.back());
    _buttons.push_back(new Img_Button(MENU_Medium, "arrow_left"));
    timeCtrlBillboard.overlay(2, _buttons.back());
    _sliders.push_back(new Img_Slider(MENU_Medium, 2));
    timeBillboard.overlay(0, _sliders.back());
    for(unsigned p = 0; p < actionsBillboard.getParams()->getGridSize(); p++){
        _buttons.push_back(new Img_Button(MENU_Medium, "sort"));
        actionsBillboard.overlay(p, _buttons.back());
        actionsBillboard.getGeoActor(p)->pickerFunc = onActionPanePress;
    }
    for(unsigned p = 0; p < sculptBillboard.getParams()->getGridSize(); p++){
        _buttons.push_back(new Img_Button(MENU_Medium, "sort"));
        sculptBillboard.overlay(p, _buttons.back());
        sculptBillboard.getGeoActor(p)->pickerFunc = onSculptPanePress;
    }
    for(unsigned p = 0; p < paintBillboard.getParams()->getGridSize(); p++){
        _buttons.push_back(new Img_Button(MENU_Medium, "sort"));
        paintBillboard.overlay(p, _buttons.back());
        paintBillboard.getGeoActor(p)->pickerFunc = onPaintPanePress;
    }
    for(unsigned p = 0; p < propsBillboard.getParams()->getGridSize(); p++){
        _dials.push_back(new Img_Dial(MENU_Medium, 4));
        propsBillboard.overlay(p, _dials.back());
    }

    // editorChain.configure(&editsScene);
    editorGrid.configure(&editsScene);

    Topl_Factory::switchPipeline(_renderer, _texPipeline);
    _renderer->buildScene(&overlayScene);
    _renderer->texturizeScene(&overlayScene);

    canvasScene.addGeometry("canvas", &backdropActor);
    _renderer->buildScene(&canvasScene);

    for(unsigned i = 1; i < 8; i++){ 
        _images.push_back(new Img_Base(checkeredImgOp({ 1024, 1024 }, { i * 2, i * 2, RAND_COLOR(), RAND_COLOR() })));
        canvasScene.addTexture(std::to_string(i), &canvasImg); // _images.back());
    }
    canvasScene.addTexture("canvas", &canvasImg);

    _renderer->buildScene(&editsScene);
}

void Sandbox_Demo::loop(double frameTime){
    updateOverlay();

    if(Sandbox_Demo::mode == SANDBOX_PAINT || Sandbox_Demo::mode == SANDBOX_TIME){
        if(isRepaintReq){
            for(unsigned short i = 0; i < objectTextures.size(); i++)
                if(objectTextures[i] != nullptr) mainScene.addTexture("object" + std::to_string(i), objectTextures[i]);

            if(mainScene.getIsTextured()) _renderer->texturizeScene(&mainScene);
            if(canvasScene.getIsTextured()) _renderer->texturizeScene(&canvasScene);
            isRepaintReq = false;
        }
        
        _renderer->setCamera(&fixedCamera);
        _texVShader.setMode(0); // (Sandbox_Demo::mode + 1);
        _effectVShader.setMode(Sandbox_Demo::mode);
        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(_renderer, _texPipeline);
        _renderer->updateScene(&canvasScene);
        _renderer->drawScene(&canvasScene);
        backdropActor.updatePos({ 0.0F, 0.0F, 0.01F });
        // TODO: Set effect mode depending on draw option
        Topl_Factory::switchPipeline(_renderer, _effectPipeline);
        _renderer->updateScene(&canvasScene);
        _renderer->drawScene(&canvasScene);
        backdropActor.updatePos({ 0.0F, 0.0F, -0.01F });
    }
    else if(Sandbox_Demo::mode == SANDBOX_SCULPT){
        _effectVShader.setMode(2);
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
            // _flatVShader.setMode((_renderer->getFrameCount() / 300) % 8);
            _flatVShader.setMode(-8);

            Topl_Factory::switchPipeline(_renderer, _flatPipeline);
            _renderer->setCamera(&Topl_Program::cameraObj);
            _renderer->updateScene(&mainScene);
            _renderer->drawScene(&mainScene);
            /* if(Topl_Program::pickerObj != nullptr){
                Topl_Program::pickerObj->updateSize({ 0.1F, 0.1F, 0.1F });
                _flatVShader.setMode(0);
                // _renderer->setDrawMode(DRAW_Lines);
                Topl_Factory::switchPipeline(_renderer, _flatPipeline);
                _renderer->updateScene(&mainScene);
                _renderer->drawScene(&mainScene);
                Topl_Program::pickerObj->updateSize({ -0.1F, -0.1F, -0.1F });
            } */
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
    static bool texturizeReq = false;

    colorPicker(&overlayScene);
    Vec3f coordColor = coordPicker(&overlayScene);

    if(isTick){
        std::string fontPath = std::string(FONTS_DIR) + "Tw-Cen-MT.ttf";
        std::string timeText = "[ " + std::to_string((unsigned)floor(Topl_Program::timeline.dynamic_ticker.getAbsSecs())) + ":00 ]";
        _labels[1]->setText({ fontPath.c_str(), timeText.c_str(), 0xFF111111, 0xFFEEEEEE });
        timeTextBillboard.setState(0, Topl_Program::timeline.dynamic_ticker.isPaused);
        _renderer->texturizeScene(&overlayScene);
        isTick = false;
    }

    if(Topl_Program::pickerObj != nullptr){
        unsigned short index = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;

        if(Topl_Program::pickerObj->getName().find("time_board") != std::string::npos && Platform::mouseControl.getIsMouseDown().second)
            timeBillboard.setState(0, coordColor[0], coordColor[1]);
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

        _renderer->texturizeScene(&overlayScene);
        _renderer->texturizeScene(&canvasScene);
    } else {
        actionsBillboard.resetState(); 
        // objectsBillboard.resetState();
        sculptBillboard.resetState(); 
        paintBillboard.resetState();
    }
}

int main(int argc, char** argv) {
    _DEMO = new Sandbox_Demo(argv[0], BACKEND_DX11);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}