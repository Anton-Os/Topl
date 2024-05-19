#include "Sandbox.hpp"

int Sandbox_Demo::mode = 0;
int Sandbox_Demo::pipelineIndex = 0;
bool Sandbox_Demo::isShaderVariant = false;
bool Sandbox_Demo::isDetailsShown = false;
unsigned Sandbox_Demo::shaderMode = 0;
Vec3f Sandbox_Demo::texScroll = { 0.0, 0.0, 0.0 };
std::string Sandbox_Demo::fontFilePath = std::string(FONTS_DIR) + "MajorMonoDisplay-Regular.ttf";

static Vec3f cameraPos, cameraRot;
static bool isPickerTab = false;
static unsigned short tabIndex = 0, dropMenuIndex = 0, expandMenuIndex = 0;
static float cameraZoom = 1.0;
static std::string dropMenuLabel = "mode ";
static std::string dropMenuOutputs[6] = { "F", "E", "D", "C", "B", "A" };

static std::map<Geo_Actor*, Vec3f> _positions_map, _rotations_map, _scales_map;

std::string fontPaths[9] = {
    std::string(FONTS_DIR) + "MajorMonoDisplay-Regular.ttf",
    std::string(FONTS_DIR) + "Quattrocento-Regular.ttf",
    std::string(FONTS_DIR) + "MiddleSchooler-Regular.ttf",
    std::string(FONTS_DIR) + "CutiveMono-Regular.ttf",
    std::string(FONTS_DIR) + "Pricedown.ttf",
    std::string(FONTS_DIR) + "Hpunk.ttf",
    std::string(FONTS_DIR) + "NerkoOne-Regular.ttf",
    std::string(FONTS_DIR) + "Trirong-Regular.ttf",
    std::string(FONTS_DIR) + "TW-Cen-MT.ttf",
}; 

#ifdef RASTERON_H
unsigned boxImg_callback(double x, double y){ return (sin(x * 20) > 0.5)? 0xFF0000FF : 0xFFFFFF00; }
unsigned pyramidImg_callback(double x, double y){ return (cos(y * 20) < 0.5)? 0xFFFF0000 : 0xFF00FFFF; }
unsigned sphereImg_callback(double x, double y){ return (tan(x * y) > 0.25 && tan(x * y) < 0.75)? 0xFF8833CC : 0xFF88CC33; }
unsigned hexImg_callback(double x, double y){ return (x > 0.4 && x < 0.6 && y > 0.4 && y < 0.6)? 0xFF3333333 : 0xFFEEEEEE; }
unsigned paramImg_callback(double x, double y){ 
    if(x > 0.5F) return (y > 0.5F)? 0xCC00FFFF : 0xCCFF00FF; 
    else return (y > 0.5F)? 0xCCFFFF00: 0xCCEEEEEE; 
}
#endif

static void onAnyKey(char k){ 
    if(isdigit(k)){ 
        Sandbox_Demo::mode = k - '0'; // conversion from char to decimal
        std::cout << "Key mode: " << std::to_string(Sandbox_Demo::mode) << std::endl;
    }

    if(k == 0x11) {
        Topl_Program::userInput.clear();
        _DEMO->inputInfoActor.isShown = false; // allow user input
        Topl_Program::isInputEnabled = true;
    } else if(k == 0x0D) {
        _DEMO->inputTextObj.text = Topl_Program::userInput.c_str();
        _DEMO->inputInfo_texture.setTextImage(&_DEMO->inputTextObj);
        _DEMO->inputInfoActor.setSize({ 0.14F * (float)Topl_Program::userInput.length(), 1.0F, 1.0F });
        _DEMO->inputInfoActor.isShown = true; // allow user input
        Topl_Program::isInputEnabled = false;
    }

    if(!Topl_Program::isInputEnabled){
        if(k == 0x25) Topl_Program::timeline.dynamic_ticker.updateTime(-1.0); // Sandbox_Demo::texScroll[0] -= 0.05f; // left arrow
        else if(k == 0x27) Topl_Program::timeline.dynamic_ticker.updateTime(1.0); // Sandbox_Demo::texScroll[0] += 0.05f; // right arrow
        else if(k == 0x26) Topl_Program::timeline.dynamic_ticker.isPaused = false; // Sandbox_Demo::texScroll[1] -= 0.05f; // up arrow
        else if(k == 0x28) Topl_Program::timeline.dynamic_ticker.isPaused = true; // Sandbox_Demo::texScroll[1] += 0.05f; // down arrow
        else if(isspace(k)){
            (Sandbox_Demo::pipelineIndex < 2)? Sandbox_Demo::pipelineIndex++ : Sandbox_Demo::pipelineIndex = 0;
            std::string sceneLabel = "Scene" + std::to_string(Sandbox_Demo::pipelineIndex + 1);
            Rasteron_Text sceneTextObj = { fontPaths[1].c_str(), sceneLabel.c_str(), 0xFFEEEEEE, 0xFF333333 };
            _DEMO->sceneInfo_texture.setTextImage(&sceneTextObj);
        } 

        if(isalpha(k)){
            if(tolower(k) == 'x') { Sandbox_Demo::isShaderVariant = !Sandbox_Demo::isShaderVariant; }
            if(tolower(k) == 'w' || tolower(k) == 's' || tolower(k) == 'a' || tolower(k) == 'd'){ // Recording Camera
                cameraPos = *Topl_Program::cameraObj.getPos();
                Topl_Program::timeline.addSequence_vec3f(&cameraPos, std::make_pair(TIMELINE_AT, cameraPos)); 
            } else if(tolower(k) == 'q' || tolower(k) == 'e'){
                cameraRot = *Topl_Program::cameraObj.getRot();
                Topl_Program::timeline.addSequence_vec3f(&cameraRot, std::make_pair(TIMELINE_AT, cameraRot));
            } else if(tolower(k) == 'z' || tolower(k) == 'c'){
                cameraZoom = *Topl_Program::cameraObj.getZoom();
                Topl_Program::timeline.addSequence_float(&cameraZoom, std::make_pair(TIMELINE_AT, cameraZoom));
            }
        }
    }
}

void dropMenu_onPick(MOUSE_Event event){
    int c = (char)(Topl_Program::pickerObj->getName().back()) - '0';
    dropMenuIndex = c - 1;
    for(unsigned m = 0; m < 6; m++){
        std::string modeText = dropMenuLabel + dropMenuOutputs[m];
        Rasteron_Text textObj = { fontPaths[1].c_str(), modeText.c_str(), 0xFF111111, 0xFFEEEEEE };
        _DEMO->dropLabelBtns[m].setText(textObj);
        if(_DEMO->checkPicker(_DEMO->dropMenuLayout.getGeoActor(m))) _DEMO->dropMenuLayout.setState(m, event != MOUSE_Hover);
        // _DEMO->expandMenuLayout.toggleShow();
    }
    _DEMO->expandMenuLayout.shift(_DEMO->dropMenuLayout.getOrigin() - _DEMO->expandMenuLayout.getOrigin());
    _DEMO->expandMenuLayout.shift({ (_DEMO->dropMenuLayout.getOrigin()[0] > 0.5F)? -0.225F : 0.225F, 0.0F, 0.0F });
}

void expandMenu_onPick(MOUSE_Event event){
    if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
        int c = (char)(Topl_Program::pickerObj->getName().back()) - '0';
        expandMenuIndex = c - 1;
        for(unsigned m = 0; m < 6; m++){
            std::string optionText = "option " + std::to_string(m + 1);
            Rasteron_Text textObj = {
                fontPaths[8].c_str(), optionText.c_str(), 
                (m != expandMenuIndex)? 0xFFEEEEEE : 0xFF00FF00, (m != expandMenuIndex)? 0xFF111111 : 0xFFEEEEEE
            };
            _DEMO->expandMenuBtns[m].setTextImage(&textObj);
            _DEMO->expandLabelBtns[m].setText(textObj);
            if(_DEMO->checkPicker(_DEMO->expandMenuLayout.getGeoActor(m))) _DEMO->dropMenuLayout.setState(m, event != MOUSE_Hover);
        }
    }
}

void updateScenePanes(MOUSE_Event event){
    if(!isPickerTab){ 
        dropMenuLabel = ""; 
        dropMenuOutputs[0] = "F"; dropMenuOutputs[1] = "E"; dropMenuOutputs[2] = "D";
        dropMenuOutputs[3] = "C"; dropMenuOutputs[4] = "B"; dropMenuOutputs[5] = "A";

        switch(tabIndex){
            case SCENEMODE_ADD: 
                dropMenuLabel = "+ "; 
                dropMenuOutputs[3] = "volume"; dropMenuOutputs[4] = "conic"; dropMenuOutputs[5] = "surface";
                break;
            case SCENEMODE_PAN: 
                dropMenuLabel = "view: ";
                dropMenuOutputs[3] = "frustum"; dropMenuOutputs[4] = "ortho"; dropMenuOutputs[5] = "none";
                break;
            case SCENEMODE_SETTINGS: 
                dropMenuLabel = "pipeline: "; 
                dropMenuOutputs[3] = "tex"; dropMenuOutputs[4] = "lit"; dropMenuOutputs[5] = "flat";
                break;
            default: dropMenuLabel = "obj: ";
        }
        // for(unsigned d = 0; d < 3; d++) _DEMO->dropMenuLayout.getGeoActor(d)->isShown = true;

        if(tabIndex > SCENEMODE_SETTINGS)
            for(unsigned o = 0; o < 6; o++){
                if(o >= _DEMO->getScene()->getActorCount()) _DEMO->dropMenuLayout.getGeoActor(6 - o - 1)->isShown = false;
                else {
                    _DEMO->dropMenuLayout.getGeoActor(o)->isShown = true;
                    dropMenuOutputs[6 - o - 1] =  _DEMO->getScene()->getGeoActor(o)->getName();
                }
            }
        else for(unsigned d = 0; d < 3; d++) _DEMO->dropMenuLayout.getGeoActor(d)->isShown = false;
    }
}

void updatePickerPanes(MOUSE_Event event){
    if(isPickerTab){ 
        switch(tabIndex){
        case PICKMODE_MOVE: 
            dropMenuOutputs[5] = "X: " + std::to_string((*Topl_Program::pickerObj->getPos()).data[0]).substr(0, 5);
            dropMenuOutputs[4] = "Y: " + std::to_string((*Topl_Program::pickerObj->getPos()).data[1]).substr(0, 5);
            dropMenuOutputs[3] = "Z: " + std::to_string((*Topl_Program::pickerObj->getPos()).data[2]).substr(0, 5);
            dropMenuLabel = "pos"; break;
        case PICKMODE_ROTATE: 
            dropMenuOutputs[5] = "X: " + std::to_string((*Topl_Program::pickerObj->getRot()).data[0]).substr(0, 5);
            dropMenuOutputs[4] = "Y: " + std::to_string((*Topl_Program::pickerObj->getRot()).data[1]).substr(0, 5);
            dropMenuOutputs[3] = "Z: " + std::to_string((*Topl_Program::pickerObj->getRot()).data[2]).substr(0, 5);
            dropMenuLabel = "angle"; break;
        case PICKMODE_SCALE: 
            dropMenuOutputs[5] = "X: " + std::to_string((*Topl_Program::pickerObj->getSize()).data[0]).substr(0, 5);
            dropMenuOutputs[4] = "Y: " + std::to_string((*Topl_Program::pickerObj->getSize()).data[1]).substr(0, 5);
            dropMenuOutputs[3] = "Z: " + std::to_string((*Topl_Program::pickerObj->getSize()).data[2]).substr(0, 5);
            dropMenuLabel = "scale"; break;
        // default: dropMenuLabel = ""; break;
        }
        for(unsigned d = 0; d < 3; d++) _DEMO->dropMenuLayout.getGeoActor(d)->isShown = false;
        // dropMenuOutputs[0] = ""; dropMenuOutputs[1] = ""; dropMenuOutputs[2] = "";
    }
}

void panesBtn_onPick(MOUSE_Event event){
    if(Topl_Program::pickerObj){
        if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
            int c = (char)(Topl_Program::pickerObj->getName().back()) - '0';
            _DEMO->dropMenuLayout.shift(*Topl_Program::pickerObj->getPos() - _DEMO->dropMenuLayout.getOrigin());
            _DEMO->dropMenuLayout.shift({ 0.0F, 0.215F, 0.0F});
            for(unsigned a = 0; a < 7; a++) _DEMO->dropMenuLayout.getGeoActor(a)->isShown = true;
            _DEMO->expandMenuLayout.shift(_DEMO->dropMenuLayout.getOrigin() - _DEMO->expandMenuLayout.getOrigin());
            _DEMO->expandMenuLayout.shift({ (_DEMO->dropMenuLayout.getOrigin()[0] > 0.5F)? -0.225F : 0.225F, 0.0F, 0.0F });
        }
        for(unsigned b = 0; b < 6; b++)
            if(_DEMO->checkPicker(_DEMO->scenePropsLayout.getGeoActor(6 - b - 1)) && !_DEMO->checkPicker(_DEMO->scenePropsLayout.getGeoActor(6))){
                isPickerTab = false;
                tabIndex = b;
                _DEMO->scenePropsLayout.setState(tabIndex, event != MOUSE_Hover);
                break;
            }
        updateScenePanes(event);
        for(unsigned b = 0; b < 6; b++)
            if(_DEMO->checkPicker(_DEMO->pickerPropsLayout.getGeoActor(6 - b - 1)) && !_DEMO->checkPicker(_DEMO->pickerPropsLayout.getGeoActor(6))){
                isPickerTab = true;
                tabIndex = b;
                _DEMO->pickerPropsLayout.setState(tabIndex, event != MOUSE_Hover);
                break;
            }
        updatePickerPanes(event);
        dropMenu_onPick(event);
    }
}

void timelineBtn_onPick(MOUSE_Event event){
    for(unsigned b = 0; b < 3; b++) 
        if(_DEMO->checkPicker(_DEMO->timelinePropsLayout.getGeoActor(3 - b - 1)) && !_DEMO->checkPicker(_DEMO->timelinePropsLayout.getGeoActor(3)))
            _DEMO->timelinePropsLayout.setState(b, event != MOUSE_Hover, (double)Topl_Program::pickerCoord.data[0], (double)Topl_Program::pickerCoord.data[1]);
    if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
        int c = (char)(Topl_Program::pickerObj->getName().back()) - '0';
        if(c - 1 == 0){
            Topl_Program::timeline.dynamic_ticker.setTime(Topl_Program::timeline.dynamic_ticker.range.first);
            Topl_Program::timeline.seqCallback(Topl_Program::timeline.dynamic_ticker.getAbsSecs());
        }
        else if(c - 1 == 2){ 
            Topl_Program::timeline.dynamic_ticker.setTime(Topl_Program::timeline.dynamic_ticker.range.second);
            Topl_Program::timeline.seqCallback(Topl_Program::timeline.dynamic_ticker.getAbsSecs());
        }
        else Topl_Program::timeline.dynamic_ticker.isPaused = !Topl_Program::timeline.dynamic_ticker.isPaused;
    }
}

static void slider_onPick(MOUSE_Event event){ 
    if(event == MOUSE_RightBtn_Press || event == MOUSE_LeftBtn_Press){
        _DEMO->timelineLayout.setState(0, event != MOUSE_Hover, (double)Topl_Program::pickerCoord.data[0], (double)Topl_Program::pickerCoord.data[1]); // _DEMO->timelineSlider.setState(Topl_Program::pickerCoord[0]); 
        Topl_Program::timeline.dynamic_ticker.isPaused = true;
        Topl_Program::timeline.dynamic_ticker.setTime(Topl_Program::pickerCoord[0] * 60.0);
        Topl_Program::timeline.seqCallback(Topl_Program::timeline.dynamic_ticker.getAbsSecs());
    }
}

static void onScroll(bool positive){
    if(Topl_Program::pickerObj != NO_PICKER_OBJ){
        if(_DEMO->checkPicker(&_DEMO->boxActor) || _DEMO->checkPicker(&_DEMO->pyramidActor) || _DEMO->checkPicker(&_DEMO->sphereActor) || _DEMO->checkPicker(&_DEMO->hexActor)){
            auto size = std::find_if(_scales_map.begin(), _scales_map.end(), [](const std::pair<Geo_Actor*, Vec3f>& s){ return s.first == Topl_Program::pickerObj; });
            if(size == _scales_map.end()) _scales_map.insert({ Topl_Program::pickerObj, Vec3f({ 1.0F, 1.0F, 1.0F }) });

            _scales_map[Topl_Program::pickerObj] = _scales_map[Topl_Program::pickerObj] + ((positive)? Vec3f({ 0.15F, 0.15F, 0.15F }) : Vec3f({ -0.15F, -0.15F, -0.15F }));
            Topl_Program::timeline.addSequence_vec3f(&_scales_map[Topl_Program::pickerObj], std::make_pair(TIMELINE_AT, _scales_map[Topl_Program::pickerObj]));
        }
        // else if(_DEMO->checkPicker(_DEMO->dropMenuLayout.getGeoActor(6)) || _DEMO->checkPicker(_DEMO->expandMenuLayout.getGeoActor(6)))
        //    std::cout << "Scroll on menu triggered!" << std::endl;

        tabIndex = PICKMODE_SCALE;
        updatePickerPanes(MOUSE_RightBtn_Press);
        dropMenu_onPick(MOUSE_RightBtn_Press);
    }
    else Topl_Program::cameraObj.setZoom((positive)? *Topl_Program::cameraObj.getZoom() * 1.1 : *Topl_Program::cameraObj.getZoom() * 0.9); 
}

static void onDrag(float x, float y){ 
    static Vec3f savedColorVec = Vec3f{0.0, 0.0, 0.0};
    static const Geo_Mesh* targetMesh = nullptr;

    if(Platform::mouseControl.getIsMouseDown().second) {
        Topl_Program::cursorPos = { x, y, 0.0F }; 
        if(Topl_Program::pickerObj != NO_PICKER_OBJ){
            std::string pickerText = Topl_Program::pickerObj->getName();
            Rasteron_Text pickerTextObj = { fontPaths[1].c_str(), pickerText.c_str(), 0xFFEEEEEE, 0xFF333333 };
            _DEMO->pickerInfo_texture.setTextImage(&pickerTextObj);
            _DEMO->pickerInfoActor.setSize({ 0.185F * (float)pickerText.length(), 1.0F, 1.0F });
            _DEMO->pickerInfoActor.isShown = true;
            // _DEMO->sceneInfoActor.isShown = false;

            if(_DEMO->checkPicker(&_DEMO->boxActor) || _DEMO->checkPicker(&_DEMO->pyramidActor) || _DEMO->checkPicker(&_DEMO->sphereActor) || _DEMO->checkPicker(&_DEMO->hexActor)){
                if(Platform::mouseControl.getIsMouseDown().first == MOUSE_LeftBtn_Press){
                    auto pos = std::find_if(_positions_map.begin(), _positions_map.end(), [](const std::pair<Geo_Actor*, Vec3f>& p){ return p.first == Topl_Program::pickerObj; });
                    if(pos == _positions_map.end()) _positions_map.insert({ Topl_Program::pickerObj, *Topl_Program::pickerObj->getPos() });

                    tabIndex = PICKMODE_MOVE;
                    _positions_map[Topl_Program::pickerObj] = Topl_Program::getCamCursorPos();
                    Topl_Program::timeline.addSequence_vec3f(&_positions_map[Topl_Program::pickerObj], std::make_pair(TIMELINE_AT, _positions_map[Topl_Program::pickerObj]));
                } else if(Platform::mouseControl.getIsMouseDown().first == MOUSE_RightBtn_Press){
                    auto rot = std::find_if(_rotations_map.begin(), _rotations_map.end(), [](const std::pair<Geo_Actor*, Vec3f>& r){ return r.first == Topl_Program::pickerObj; });
                    if(rot == _rotations_map.end()) _rotations_map.insert({ Topl_Program::pickerObj, *Topl_Program::pickerObj->getRot() });

                    tabIndex = PICKMODE_ROTATE;
                    _rotations_map[Topl_Program::pickerObj] = _rotations_map[Topl_Program::pickerObj] + Vec3f({ 
                        (savedColorVec[0] - Topl_Program::pickerCoord[0]) * 10.0F, (savedColorVec[1] - Topl_Program::pickerCoord[1]) * 10.0F, 0.0F 
                    });
                    Topl_Program::timeline.addSequence_vec3f(&_rotations_map[Topl_Program::pickerObj], std::make_pair(TIMELINE_AT, _rotations_map[Topl_Program::pickerObj]));
                }

                /* Sandbox_Demo::isDetailsShown = true;
                targetMesh = Topl_Program::pickerObj->getMesh();
                for(unsigned d = 0; d < DETAIL_COUNT * 3; d++){
                    _DEMO->detailActors[d].isShown = false;
                    if(d > targetMesh->getVertexCount()) break;
                    else {
                        _DEMO->detailActors[d].isShown = true;
                        Vec3f pos = (targetMesh->getVertices() + d)->position;
                        _DEMO->detailActors[d].setPos({ (*Topl_Program::pickerObj->getPos()).data[0] - pos.data[0], (*Topl_Program::pickerObj->getPos()).data[1] - pos.data[1], 0.0F});
                    }
                } */
            }

            updatePickerPanes(Platform::mouseControl.getIsMouseDown().first);
            dropMenu_onPick(Platform::mouseControl.getIsMouseDown().first);
        } else for(unsigned a = 0; a < 6 + 1; a++) _DEMO->expandMenuLayout.getGeoActor(a)->isShown = false;
    } else Topl_Program::pickerObj = NO_PICKER_OBJ; // TODO: Handle Camera Events

    savedColorVec = Topl_Program::pickerCoord;
}

static void cameraUpdate(double m){ Topl_Program::cameraObj.setPos(cameraPos); Topl_Program::cameraObj.setRot(cameraRot); Topl_Program::cameraObj.setZoom(cameraZoom); }

static void shaderModeUpdate(){ (Sandbox_Demo::shaderMode < 8)? Sandbox_Demo::shaderMode++ : Sandbox_Demo::shaderMode = 0; }

static void timerTextUpdate(){
    unsigned secs = Topl_Program::timeline.dynamic_ticker.getAbsSecs(); // make accessible inside of program timer
    unsigned splitsecs = Topl_Program::timeline.dynamic_ticker.getAbsMillisecs() / 10;
    if(secs > Topl_Program::timeline.dynamic_ticker.range.second) secs = Topl_Program::timeline.dynamic_ticker.range.second; // clamps to maximum value

    std::string timeText = ((secs < 60)? "" : (secs % 60 < 10)? std::to_string(secs / 60) + ":0" : std::to_string(secs / 60) + ":") + std::to_string(secs % 60) + ":00"; //+ std::to_string(splitsecs);
    Rasteron_Text textObj = { Sandbox_Demo::fontFilePath.c_str(), timeText.c_str(), 0xFF222222, 0xFFEEEEEE };
    _DEMO->timerInfo_texture.setTextImage(&textObj);

    _DEMO->timelineSlider.setState(secs / 60.0);
}

Sandbox_Demo::~Sandbox_Demo(){
#ifdef RASTERON_H
    RASTERON_QUEUE_DEALLOC(words_queue);
    RASTERON_DEALLOC(boxImg); RASTERON_DEALLOC(pyramidImg); RASTERON_DEALLOC(sphereImg); RASTERON_DEALLOC(hexImg); RASTERON_DEALLOC(paramImg);
#endif
}

void Sandbox_Demo::addSceneGeometry(Geo_Actor* actor){
    details.addGeometry(actor);
    _renderer->buildScene(&details);
    _renderer->texturizeScene(&details);
}

void Sandbox_Demo::init(){
    std::replace(Sandbox_Demo::fontFilePath.begin(), Sandbox_Demo::fontFilePath.end(), '/', '\\'); // make sure font file parsed correctly

    Platform::keyControl.addAnyCallback(onAnyKey);
    Platform::mouseControl.setOnScroll(onScroll);
    Platform::mouseControl.addDragCallback(onDrag);

    Topl_Program::timeline.dynamic_ticker.addRecurringEvent(cameraUpdate);
    Topl_Program::timeline.persist_ticker.addPeriodicEvent(2500, shaderModeUpdate);
    Topl_Program::timeline.persist_ticker.addPeriodicEvent(1000, timerTextUpdate);
    Topl_Program::cameraObj.setProjMatrix(Projection(PROJECTION_Ortho, 1.0, 1.0, 1.0, 1.0, 10.0, 10.0).genProjMatrix());

    canvasActor.setPos({ 0.0f, 0.0f, -1.0F});
    canvas.addGeometry("Backdrop", &canvasActor);
    // canvasTex.setFileImage("F:\\Design\\Motivation-Build.png"); // placeholder image
    // Rasteron_Image* backdropImg = noiseImgOp_tiled({ 1024, 2000 }, { 20, 20, 0x1F0000FF, 0x1FEEEEEE });
    // Rasteron_Image* sizedBackgropImg = resizeImgOp({ 1024, 1024 }, backdropImg);
    Rasteron_Image* backdropImg = gradientImgOp({ 1024, 1024 }, SIDE_Radial, 0x330000FF, 0x33EEEEEE);
    canvasTex.setImage(backdropImg);
    RASTERON_DEALLOC(backdropImg); // RASTERON_DEALLOC(sizedBackgropImg);
    canvas.addTexture("Backdrop", &canvasTex);
    Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, _flatPipeline);
    _renderer->buildScene(&canvas);
    _renderer->texturizeScene(&canvas);

    boxMesh.scale({ 0.25f, 0.25f, 0.25f});
    boxActor.setPos({ 0.5f, 0.5f, 0.0f });
    scene.addGeometry("Box", &boxActor);
    pyramidMesh.scale({ 0.25f, 0.25f, 0.25f});
    pyramidActor.setPos({ -0.5f, 0.5f, 0.0f });
    scene.addGeometry("Pyramid", &pyramidActor);
    sphereMesh.scale({ 0.25f, 0.25f, 0.25f});
    sphereActor.setPos({ -0.5f, -0.5f, 0.0f });
    scene.addGeometry("Orb", &sphereActor);
    hexMesh.scale({ 0.25f, 0.25f, 0.25f});
    hexActor.setPos({ 0.5f, -0.5f, 0.0f });
    scene.addGeometry("Hex", &hexActor);
    scene.addGeometry("Param", &paramActor);
    tessActor.setPos({ 0.0F, 0.5F, 0.0F });
    scene.addGeometry("Tess", &tessActor);
    duplexActor.setPos({ 0.0F, -0.5F, 0.0F });
    scene.addGeometry("Duplex", &duplexActor);
#ifdef RASTERON_H // Adding textures for scene
    boxImg = mapImgOp({1024, 1024}, boxImg_callback);
    boxTex.setImage(boxImg);
    scene.addTexture("Box", &boxTex);
    pyramidImg = mapImgOp({1024, 1024}, pyramidImg_callback);
    pyramidTex.setImage(pyramidImg);
    scene.addTexture("Pyramid", &pyramidTex);
    sphereImg = mapImgOp({1024, 1024}, sphereImg_callback);
    sphereTex.setImage(sphereImg);
    scene.addTexture("Orb", &sphereTex);
    hexImg = mapImgOp({1024, 1024}, hexImg_callback);
    hexTex.setImage(hexImg);
    scene.addTexture("Hex", &hexTex);
    paramImg = mapImgOp({1024, 1024}, paramImg_callback);
    paramTex.setImage(paramImg);
    scene.addTexture("Param", &paramTex);
#endif

    Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, _texPipeline);
    _renderer->buildScene(&scene);
    _renderer->texturizeScene(&scene);
    
    timerInfoMesh.scale({ 1.5f * 0.85F, 0.3F * 0.85F, 1.0f });
    timerInfoActor.setPos({ 0.0f, -0.883f, 0.0f }); // inputInfoActor.setPos({ 0.0f, -0.9f, 0.0f }); 
    overlay.addGeometry("timerInfo", &timerInfoActor);
    pickerInfoMesh.scale({ 1.0, 0.2, 1.0f });
    pickerInfoActor.setPos({ 0.673f, -0.8415f, 0.0f });
    pickerInfoActor.isShown = false;
    overlay.addGeometry("pickerInfo", &pickerInfoActor);
    sceneInfoMesh.scale({ 1.0, 0.2, 1.0f });
    sceneInfoActor.setPos({ -0.7f, -0.8415f, 0.0f });
    overlay.addGeometry("sceneInfo", &sceneInfoActor);
    inputInfoMesh.scale({ 2.5, 0.25, 1.0f });
    inputInfoActor.setPos({ 0.0, 0.875f, 0.0f });
    inputInfoActor.isShown = false;
    overlay.addGeometry("inputInfo", &inputInfoActor);
    overlay.addTexture("inputInfo", &inputInfo_texture);
    pickerPropsLayout.configure(&overlay);
    pickerPropsLayout.scale({ 1.175F, 0.1F, 1.0F });
    pickerPropsLayout.shift({ 0.673F, -0.9215f, 0.0f });
    overlay.addTexture("pickerPropsLayout_root", &propsRoot);
    pickerPropsLayout.getGeoActor(6)->pickerFunc = panesBtn_onPick;
    for(unsigned p = 0; p < 6; p++) {
        pickerPropsLayout.getGeoActor(p)->pickerFunc = panesBtn_onPick;
        pickerPropsLayout.getGeoActor(p)->updateSize({ -0.5F, 0.0F, 0.0F });
        if(p <= 2) pickerPropsLayout.getGeoActor(p)->isShown = false;
        else pickerPropsLayout.getGeoActor(p)->updatePos({ 0.111F, 0.0F, 0.0F });
        pickerPropsLayout.overlay(p, &pickerPropBtns[p]);
        overlay.addTexture(pickerPropsLayout.getGeoActor(p)->getName(), &pickerPropBtns[6 - p - 1].stateImg);
    }
    scenePropsLayout.configure(&overlay);
    scenePropsLayout.scale({ 1.175F, 0.1F, 1.0F });
    scenePropsLayout.shift({ -0.7F, -0.9215f, 0.0f });
    overlay.addTexture("scenePropsLayout_root", &propsRoot);
    scenePropsLayout.getGeoActor(6)->pickerFunc = panesBtn_onPick;
    for(unsigned p = 0; p < 6; p++) {
        scenePropsLayout.getGeoActor(p)->pickerFunc = panesBtn_onPick;
        scenePropsLayout.getGeoActor(p)->updateSize({ -0.5F, 0.0F, 0.0F });
        scenePropsLayout.overlay(p, &scenePropBtns[p]);
        overlay.addTexture(scenePropsLayout.getGeoActor(p)->getName(), &scenePropBtns[6 - p - 1].stateImg);
    }
    timelinePropsLayout.configure(&overlay);
    timelinePropsLayout.scale({ 0.4, 0.125, 1.0F });
    timelinePropsLayout.shift({ 0.0, -0.825f, 0.0f });
    for(unsigned p = 0; p < 3; p++){ 
        timelinePropsLayout.getGeoActor(p)->pickerFunc = timelineBtn_onPick;
        timelinePropsLayout.overlay(p, &timelineBtns[p]);
        overlay.addTexture(timelinePropsLayout.getGeoActor(p)->getName(), &timelineBtns[3 - p - 1].stateImg);
    }
    timelineLayout.configure(&overlay);
    timelineLayout.scale({ 1.5F, 0.25F, 1.0F });
    timelineLayout.getGeoActor(1)->updateSize({ 2.25F, 0.0F, 1.0F });
    timelineLayout.shift({ -0.015f, -0.92f, 0.0f });
    dropMenuLayout.configure(&overlay);
    dropMenuLayout.shift({ 0.865F, -0.885F, 0.0F });
    dropMenuLayout.scale({ 0.4F, 0.4F, 1.0F });
    for(unsigned a = 0; a < 7; a++){ 
        dropMenuLayout.getGeoActor(a)->isShown = false;
        if(a < 6) dropMenuLayout.overlay(a, &dropLabelBtns[a]);
    }
    expandMenuLayout.configure(&overlay);
    expandMenuLayout.shift({ 0.865F, -0.885F, 0.0F });
    expandMenuLayout.scale({ 0.4F, 0.4F, 1.0F });
    for(unsigned a = 0; a < 7; a++){ 
        expandMenuLayout.getGeoActor(a)->isShown = false;
        if(a < 6) expandMenuLayout.overlay(a, &expandLabelBtns[a]);
    }
#ifdef RASTERON_H // adding textures fro overlay
    overlay.addTexture("timerInfo", &timerInfo_texture);
    overlay.addTexture("pickerInfo", &pickerInfo_texture);
    overlay.addTexture("sceneInfo", &sceneInfo_texture);
    Rasteron_Text sceneTextObj = { fontPaths[1].c_str(), "Scene1", 0xFFEEEEEE, 0xFF333333 };
    _DEMO->sceneInfo_texture.setTextImage(&sceneTextObj);
    overlay.addTexture("timelineLayout_cell1", &timelineSlider.stateImg);
    timelineLayout.getGeoActor(0)->pickerFunc = slider_onPick;
    timelineLayout.overlay(0, &timelineSlider);
    for(unsigned m = 0; m < 6; m++){
        dropMenuLayout.getGeoActor(m)->pickerFunc = dropMenu_onPick;
        overlay.addTexture("dropMenuLayout_cell" + std::to_string(m + 1), &_DEMO->dropLabelBtns[m].stateImg);
    }
    for(unsigned m = 0; m < 6; m++){
        expandMenuLayout.getGeoActor(m)->pickerFunc = expandMenu_onPick;
        overlay.addTexture("expandMenuLayout_cell" + std::to_string(m + 1), &_DEMO->expandLabelBtns[m].stateImg);
    }
#endif
    _renderer->buildScene(&overlay);
    _renderer->texturizeScene(&overlay);

    /* for(unsigned d = 0; d < 300; d++){
        if(d % 3 == 0){
            detailActors[d] = Geo_Actor((Geo_Mesh*)&labels[d / 3]);
            // detailActors[d].setPos({ -0.05F, -1.0F + ((1.0F / 150.0F) * d), 0.01F });// TODO: Set details over properties
            labelsTex[d / 3].setColorImage(0xDD0000FF);
        }
        else if(d % 3 == 1){
            detailActors[d] = Geo_Actor((Geo_Mesh*)&dots[d / 3]);
            // detailActors[d].setPos({ 0.05F, -1.0F + ((1.0F / 150.0F) * d), 0.01F });// TODO: Set details over timeline
            dotsTex[d / 3].setColorImage(0xDDFF0000);
        }
        else {
            detailActors[d] = Geo_Actor((Geo_Mesh*)&arrows[d / 3]);
            // detailActors[d].setPos({ 0.0F, -1.0F + ((1.0F / 150.0F) * d), 0.01F });// TODO: Set details over elements
            arrowsTex[d / 3].setColorImage(0xDD00FF00);
        }

        detailActors[d].setSize({ 0.015F, 0.015F, 0.015F });
        details.addGeometry("detail " + std::to_string(d), &detailActors[d]);
        details.addTexture("detail " + std::to_string(d), (d % 3 == 0)? &labelsTex[d / 3] : (d % 3 == 1)? &dotsTex[d / 3] : &arrowsTex[d / 3]);
    }

    _renderer->buildScene(&details);
    _renderer->texturizeScene(&details); */
}

void Sandbox_Demo::loop(double frameTime){
    // Object Updates

    for(auto p = _positions_map.begin(); p != _positions_map.end(); p++) p->first->setPos(p->second);
    for(auto r = _rotations_map.begin(); r != _rotations_map.end(); r++) r->first->setRot(r->second);
    for(auto s = _scales_map.begin(); s != _scales_map.end(); s++) s->first->setSize(s->second);
#ifdef RASTERON_H
    _renderer->texturizeScene(&overlay);
#endif

    // Capture Renders
    {
        _flatVShader.setMode(FLAT_MODE_SOLID); 

        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, _flatPipeline);
        _renderer->setCamera(&Topl_Program::cameraObj);
        _renderer->updateScene(&scene);
        _renderer->drawScene(&scene);

        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, _flatPipeline);
        _renderer->setCamera(&fixedCamera);
        _renderer->updateScene(&overlay);
        _renderer->drawScene(&overlay);
    #ifdef RASTERON_H
        colorPicker(&scene);
        colorPicker(&overlay);
    #endif
        _renderer->clear();
    }

    if(Platform::mouseControl.getIsMouseDown().second) {
        _flatVShader.setMode(FLAT_MODE_DIRECTION); // _effectVShader.setMode(1);
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, _flatPipeline);
        _renderer->setCamera(&Topl_Program::cameraObj);
        _renderer->updateScene(&canvas);
        _renderer->drawScene(&canvas);

        _flatVShader.setMode(FLAT_MODE_COORD); 
        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, _flatPipeline);
        _renderer->setCamera(&Topl_Program::cameraObj);
        _renderer->updateScene(&scene);
        _renderer->drawScene(&scene);

        _flatVShader.setMode(FLAT_MODE_COORD);
        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, _flatPipeline);
        _renderer->setCamera(&fixedCamera);
        _renderer->updateScene(&overlay);
        _renderer->drawScene(&overlay);
#ifdef RASTERON_H
        Vec3f coordPickVec = coordPicker(&scene);
        if(coordPickVec[0] == 0.0 && coordPickVec[1] == 0.0)  coordPickVec = coordPicker(&overlay);
        if(coordPickVec[0] == 0.0 && coordPickVec[1] == 0.0) coordPickVec = coordPicker(&canvas);
#endif
        _renderer->clear();
    }

    // Display Renders

    {
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, _texPipeline);
        _renderer->updateScene(&canvas);
        _renderer->drawScene(&canvas);

        _flatVShader.setMode((Sandbox_Demo::isShaderVariant)? Sandbox_Demo::shaderMode % 3 : 0); 
        _beamsVShader.setMode((Sandbox_Demo::isShaderVariant)? Sandbox_Demo::shaderMode / 2 : 0); 
        _texVShader.setTexScroll(Sandbox_Demo::texScroll);

        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, (Sandbox_Demo::pipelineIndex == 0)? _texPipeline : (Sandbox_Demo::pipelineIndex == 1)? _beamsPipeline : _flatPipeline);

        _renderer->setDrawMode(DRAW_Lines);
        _renderer->setCamera(&Topl_Program::cameraObj);
        _renderer->updateScene(&scene);
        _renderer->draw(&_DEMO->boxActor);
        _renderer->draw(&_DEMO->pyramidActor);
        _renderer->draw(&_DEMO->sphereActor);
        _renderer->draw(&_DEMO->hexActor);
        _renderer->draw(&_DEMO->paramActor);
        _renderer->draw(&_DEMO->tessActor);
        _renderer->draw(&_DEMO->duplexActor);

        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, _texPipeline);

        _renderer->setDrawMode(DRAW_Triangles);
        _renderer->setCamera(&fixedCamera);
        _renderer->updateScene(&overlay);
        _renderer->drawScene(&overlay);

        _renderer->setDrawMode(DRAW_Triangles);
        if(Sandbox_Demo::isDetailsShown){ 
            _renderer->updateScene(&details);
            _renderer->drawScene(&details);
        }
    }
}

int main(int argc, char** argv) {
    _DEMO = new Sandbox_Demo(argv[0], BACKEND_GL4);

    _DEMO->run();
    delete(_DEMO);
    return 0;
}