
#include "Sandbox.hpp"

unsigned short Sandbox_Demo::mode = 0;
unsigned short Sandbox_Demo::option = 0;

static bool isRebuildReq = false;
static std::vector<Geo_Vertex> sculptPoints;
static bool isRepaintReq = false;
static Img_Canvas canvasImg = Img_Canvas((0x00FFFFFF & RAND_COLOR()) | 0x66000000);
static bool isTick = false;
static bool isModal = false;

void Sandbox_Demo::onAnyKey(char key){
    if(isdigit(key)){ Sandbox_Demo::mode = key - '0'; }
    else if(tolower(key) == 'y'){ Topl_Program::camera.setProjMatrix(Projection(PROJECTION_None, 1.0F).genProjMatrix()); }
    else if(tolower(key) == 'u'){ Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Orthographic, 1.0F).genProjMatrix()); }
    else if(tolower(key) == 'i'){ Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Perspective, 1.0F).genProjMatrix()); }
    else if(tolower(key) == 'o'){ Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Experimental, 1.0F).genProjMatrix()); }
    // else if(tolower(key) == 'w' || tolower(key) == 'a' || tolower(key) == 's' || tolower(key) == 'd')
    //     Topl_Program::timeline.addSequence_vec3f(Topl_Program::camera.getPos(), std::make_pair(TIMELINE_AT, *Topl_Program::camera.getPos()));
}

void Sandbox_Demo::onTimePanePress(MOUSE_Event event, Geo_Actor* actor){
    isModal = Sandbox_Demo::mode != SANDBOX_TIME;
    Sandbox_Demo::mode = SANDBOX_TIME;
}

void Sandbox_Demo::onActionPanePress(MOUSE_Event event, Geo_Actor* actor){
    isModal = Sandbox_Demo::mode != SANDBOX_ACTION || Sandbox_Demo::option != std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;
    if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
        Sandbox_Demo::mode = SANDBOX_ACTION;
        Sandbox_Demo::option = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;
    }
}

void Sandbox_Demo::onSculptPanePress(MOUSE_Event event, Geo_Actor* actor){
    isModal = Sandbox_Demo::mode != SANDBOX_SCULPT;
    if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
        Sandbox_Demo::mode = SANDBOX_SCULPT;
        Sandbox_Demo::option = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;
        if(Sandbox_Demo::option == SANDBOX_PANES - 1) Platform::openFileDialog(true);
    }
}

void Sandbox_Demo::onPaintPanePress(MOUSE_Event event, Geo_Actor* actor){
    isModal = Sandbox_Demo::mode != SANDBOX_PAINT;
    if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
        Sandbox_Demo::mode = SANDBOX_PAINT;
        Sandbox_Demo::option = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;
        canvasImg.setBackground((0x00FFFFFF & RAND_COLOR()) | 0x66000000);
        if(Sandbox_Demo::option == SANDBOX_PANES - 1) Platform::openFileDialog(true);
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
        sculptPoints.push_back(Geo_Vertex(nearestVec));
    } else if(Sandbox_Demo::mode == SANDBOX_PAINT){
        Input_TracerStep lastStep = Platform::mouseControl.getTracerSteps()->at(Platform::mouseControl.getTracerSteps()->size() - 2);
        if(Sandbox_Demo::option == 1) canvasImg.drawPath(0.1F, { lastStep.step.first + 0.5F, 0.95F - (lastStep.step.second + 0.5F) }, { Platform::getCursorX() + 0.5F, 0.95F - (Platform::getCursorY() + 0.5F) }, RAND_COLOR());
        else if(Sandbox_Demo::option == 2) canvasImg.drawBox({ lastStep.step.first + 0.5F, 0.95F - (lastStep.step.second + 0.5F) }, { Platform::getCursorX() + 0.5F, 0.95F - (Platform::getCursorY() + 0.5F) }, RAND_COLOR());
        else canvasImg.drawDot(0.1F, { Platform::getCursorX() + 0.5F, 0.95F - (Platform::getCursorY() + 0.5F) }, RAND_COLOR());
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
            case SANDBOX_MOVE: Topl_Program::camera.updatePos(Vec3f({ diffVec[0], diffVec[1], 0.0F })); break;
            case SANDBOX_ROTATE: Topl_Program::camera.updateRot(Vec3f({ diffVec[0], diffVec[1], 0.0F })); break;
            case SANDBOX_SIZE: Topl_Program::camera.setZoom(*Topl_Program::camera.getZoom() + diffVec[0]); break;
        }
    }
}

void onConfirmAction(float x, float y){
    if(Sandbox_Demo::mode == SANDBOX_SCULPT){
        if(sculptPoints.size() > 2) {
            switch(Sandbox_Demo::option){
                case 0: _DEMO->objectMeshes.push_back(new Geo_Surface((vertex_cptr_t)sculptPoints.data(), sculptPoints.size())); break;
                case 1: _DEMO->objectMeshes.push_back(new Geo_Cone((vertex_cptr_t)sculptPoints.data(), sculptPoints.size(), Vec3f({ 0.0F, 0.0F, -1.0F }))); break;
                case 2: _DEMO->objectMeshes.push_back(new Geo_Volume((vertex_cptr_t)sculptPoints.data(), sculptPoints.size(), 1.0F)); break;
                default: _DEMO->objectMeshes.push_back(new Geo_Mesh((vertex_cptr_t)sculptPoints.data(), sculptPoints.size())); 
            }
            _DEMO->objectTextures.push_back(new Img_Base(RAND_COLOR())); // non-textured object
            _DEMO->objectPhysics.push_back(nullptr); // non-dynamic object
            _DEMO->objectActors.push_back(new Geo_Actor(_DEMO->objectMeshes.back()));
            for(unsigned n = 0; n < _DEMO->plotGrid.getActorCount(); n++) _DEMO->plotGrid.getGeoActor(n)->setSize(Vec3f({ 1.0F, 1.0F, 1.0F }));
            sculptPoints.clear();
            isRebuildReq = true;
        }
        else std::cerr << "Not enough sculpt points!" << std::endl;
    } else if(Sandbox_Demo::mode == SANDBOX_PAINT){
        isRepaintReq = true;
        if(!_DEMO->objectTextures.empty()) _DEMO->objectTextures.back() = new Img_Base(RAND_COLOR());
    }
}

void Sandbox_Demo::init(){
    Platform::keyControl.addHandler(std::bind(&Sandbox_Demo::onAnyKey, this, std::placeholders::_1));
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
    _labels.push_back(new Img_Label(MENU_XL, { fontPath.c_str(), "[ New Project ]", 0xFF111111, 0xFFEEEEEE }));
    modeBillboard.overlay(0, _labels.back());
    // modeBillboard.toggleShow(false);
    _labels.push_back(new Img_Label(MENU_Large, { fontPath.c_str(), "[ 0:00 ]", 0xFF111111, 0xFFEEEEEE }));
    timeTextBillboard.overlay(0, _labels.back());
    _buttons.push_back(new Img_Button(MENU_XL, "fast_forward"));
    timeCtrlBillboard.overlay(0, _buttons.back());
    _buttons.push_back(new Img_Button(MENU_XL, "play_arrow"));
    timeCtrlBillboard.overlay(1, _buttons.back());
    _buttons.push_back(new Img_Button(MENU_XL, "fast_rewind"));
    timeCtrlBillboard.overlay(2, _buttons.back());
    _sliders.push_back(new Img_Slider(MENU_XL, (unsigned)SANDBOX_SEQUENCE));
    timeBillboard.overlay(0, _sliders.back());
    timeBillboard.getGeoActor(0)->pickFunc = std::bind(&Sandbox_Demo::onTimePanePress, this, std::placeholders::_1, std::placeholders::_2);
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
        if(p < 3 || p >= SANDBOX_PANES - 3) actionsBillboard.getGeoActor(p)->pickFunc = std::bind(&Sandbox_Demo::onActionPanePress, this, std::placeholders::_1, std::placeholders::_2);
        else actionsBillboard.getGeoActor(p)->isShown = false;
    }
    for(unsigned p = 0; p < sculptBillboard.getParams()->getGridSize(); p++){
        switch(p){
            case SANDBOX_PANES - 1: _buttons.push_back(new Img_Button(MENU_XL, "folder_open")); break;
            default: _buttons.push_back(new Img_Button(MENU_XL, "category"));
        }
        sculptBillboard.overlay(p, _buttons.back());
        if(p < 3 || p >= SANDBOX_PANES - 1) sculptBillboard.getGeoActor(p)->pickFunc = std::bind(&Sandbox_Demo::onSculptPanePress, this, std::placeholders::_1, std::placeholders::_2);
        else sculptBillboard.getGeoActor(p)->isShown = false;
    }
    for(unsigned p = 0; p < paintBillboard.getParams()->getGridSize(); p++){
        switch(p){
            case SANDBOX_PANES - 1: _buttons.push_back(new Img_Button(MENU_XL, "folder_open")); break;
            default: _buttons.push_back(new Img_Button(MENU_XL, "brush"));
        }
        paintBillboard.overlay(p, _buttons.back());
        if(p < 3 || p >= SANDBOX_PANES - 1) paintBillboard.getGeoActor(p)->pickFunc = std::bind(&Sandbox_Demo::onPaintPanePress, this, std::placeholders::_1, std::placeholders::_2);
        else paintBillboard.getGeoActor(p)->isShown = false;
    }
    for(unsigned p = 0; p < propsBillboard.getParams()->getGridSize(); p++){
        _dials.push_back(new Img_Dial(MENU_XL, 4));
        propsBillboard.overlay(p, _dials.back());
    }

    mainScene.camera = &Topl_Program::camera;

    editsScene.camera = &fixedCamera;
    plotGrid.configure(&editsScene);
    _renderer->buildScene(&editsScene);

    Topl_Factory::switchPipeline(_renderer, _texPipeline);
    _renderer->buildScene(&overlayScene);
    _renderer->texturizeScene(&overlayScene);

    canvasScene.camera = &fixedCamera;
    canvasScene.addGeometry("canvas", &backdropActor);
    _renderer->buildScene(&canvasScene);

    for(unsigned i = 1; i < 8; i++){ 
        _images.push_back(new Img_Base(checkeredImgOp({ 1024, 1024 }, { i * 2, i * 2, RAND_COLOR(), RAND_COLOR() })));
        canvasScene.addTexture(std::to_string(i), _images.back());
    }
    canvasScene.addTexture("canvas", &canvasImg);
    sequence_map.insert({ &backdropActor, Img_Sequence((unsigned)SANDBOX_SEQUENCE) });

    _renderer->texturizeScene(&canvasScene);
}

void Sandbox_Demo::loop(double frameTime){
    updateOverlay();
    
    if(Sandbox_Demo::mode == SANDBOX_PAINT){
        _renderer->texturizeScene(&canvasScene);
        _renderer->setDrawMode(DRAW_Triangles);
        backdropActor.updatePos({ 0.0F, 0.0F, 0.01F });
        // _canvasVShader.setMode(10 * (Sandbox_Demo::option + 1) + Sandbox_Demo::option + 1);
        _effectVShader.setMode(10 * Sandbox_Demo::option);
        Topl_Factory::switchPipeline(_renderer, _canvasPipeline);
        _renderer->updateScene(&canvasScene);
        _renderer->drawScene(&canvasScene);
        backdropActor.updatePos({ 0.0F, 0.0F, -0.01F });
    }
    else if(Sandbox_Demo::mode == SANDBOX_SCULPT){
        _flatVShader.setMode(5);
        Topl_Factory::switchPipeline(_renderer, _forgePipeline);
        _renderer->updateScene(&editsScene);
        _renderer->drawScene(&editsScene);
        _effectVShader.setMode(20);
    } else {
        if(isRebuildReq){
            for(unsigned short g = mainScene.getActorCount(); g < objectActors.size(); g++)
                mainScene.addGeometry("object" + std::to_string(g), objectActors[g]);
            if(mainScene.getActorCount() > 0) _renderer->buildScene(&mainScene);
            isRebuildReq = false;
        }

        if(isRepaintReq){
            if(!objectTextures.empty()){ 
                objectTextures.back()->setImage(canvasImg.getImage());
                mainScene.addTexture("object" + std::to_string(objectTextures.size() - 1), &canvasImg); // objectTextures.back());
            }
            isRepaintReq = false;
        }

        if(mainScene.getActorCount() > 0 && objectActors.size() > 0){
            if(Platform::mouseControl.getIsMouseDown().second && Topl_Program::pickerObj == nullptr){ 
                colorPicker(&mainScene);
                if(Topl_Program::pickerObj != nullptr) coordPicker(&mainScene);
            }
            _texVShader.setMode(100);
            _renderer->setDrawMode(DRAW_Triangles);

            Topl_Factory::switchPipeline(_renderer, _texPipeline);
            _renderer->updateScene(&mainScene);
            _renderer->texturizeScene(&mainScene);
            _renderer->drawScene(&mainScene);
        }
    }

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
        timeBillboard.setState(0, secs / SANDBOX_SEQUENCE, 0.0F);
        _renderer->texturizeScene(&overlayScene);
        isTick = false;
    }

    if(isModal){
        std::string fontPath = std::string(FONTS_DIR) + "Tw-Cen-MT.ttf";
        std::string modeTxt = "[ New Project ]";
        switch(Sandbox_Demo::mode){
            case SANDBOX_TIME: modeTxt = "[ Animate ]"; fontPath = std::string(FONTS_DIR) + "Raleway-Regular.ttf"; break;
            case SANDBOX_ACTION: // modeTxt = (Sandbox_Demo::option < SANDBOX_PANES / 2)? "[ " + Topl_Program::pickerObj->getName() + " ]" : "[ Scene ]"; break;
                if(Sandbox_Demo::option == SANDBOX_MOVE) modeTxt = "[ Move Object ]";
                else if(Sandbox_Demo::option == SANDBOX_ROTATE) modeTxt = "[ Rotate Object ]";
                else if(Sandbox_Demo::option == SANDBOX_SIZE) modeTxt = "[ Scale Object ]";
                else if(Sandbox_Demo::option == SANDBOX_PANES - SANDBOX_MOVE - 1) modeTxt = "[ Move Scene ]";
                else if(Sandbox_Demo::option == SANDBOX_PANES - SANDBOX_ROTATE - 1) modeTxt = "[ Rotate Scene ]";
                else if(Sandbox_Demo::option == SANDBOX_PANES - SANDBOX_SIZE - 1) modeTxt = "[ Scale Scene ]";
            break;
            case SANDBOX_SCULPT: modeTxt = "[ Sculpt ]"; fontPath = std::string(FONTS_DIR) + "MajorMonoDisplay-Regular.ttf"; break;
            case SANDBOX_PAINT: modeTxt = "[ Paint ]"; fontPath = std::string(FONTS_DIR) + "Pricedown.ttf"; break;
        }
        _labels[0]->setText({ fontPath.c_str(), modeTxt.c_str(), 0xFF111111, 0xFFEEEEEE });
        modeBillboard.setState(0, true);
        isModal = false;
    }

    if(Topl_Program::pickerObj != nullptr)
        if(Topl_Program::pickerObj->getName().find("board") != std::string::npos){
            unsigned short index = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;

            if(Topl_Program::pickerObj->getName().find("time_board") != std::string::npos && Platform::mouseControl.getIsMouseDown().second){
                timeBillboard.setState(0, coordColor[0], coordColor[1]);
                Topl_Program::timeline.dynamic_ticker.setTime(coordColor[0] * SANDBOX_SEQUENCE); // convert from coordinates to secs
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

MAIN_ENTRY {
    _DEMO = new Sandbox_Demo(argv[0], BACKEND_DX11);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}