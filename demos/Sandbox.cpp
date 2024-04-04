#include "Sandbox.hpp"

int Sandbox_Demo::mode = 0;
int Sandbox_Demo::pipelineIndex = 0;
bool Sandbox_Demo::isShaderVariant = false;
unsigned Sandbox_Demo::shaderMode = 0;
Vec3f Sandbox_Demo::texScroll = { 0.0, 0.0, 0.0 };
std::string Sandbox_Demo::fontFilePath = std::string(FONTS_DIR) + "MajorMonoDisplay-Regular.ttf";

Vec3f cameraPos, cameraRot;
float cameraZoom = 1.0;

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


static Vec3f boxRot, boxScale = { 1.0f, 1.0f, 1.0f }, boxPos = { 0.5f, 0.5f, 0.0f };
static Vec3f pyramidRot, pyramidScale = { 1.0f, 1.0f, 1.0f }, pyramidPos = { -0.5f, 0.5f, 0.0f };
static Vec3f sphereRot, sphereScale = { 1.0f, 1.0f, 1.0f }, spherePos = { -0.5f, -0.5f, 0.0f };
static Vec3f hexRot, hexScale = { 1.0f, 1.0f, 1.0f }, hexPos = { 0.5f, -0.5f, 0.0f };


static void onAnyKey(char k){ 
    if(isdigit(k)){ 
        Sandbox_Demo::mode = k - '0'; // conversion from char to decimal
        std::cout << "Key mode: " << std::to_string(Sandbox_Demo::mode) << std::endl;
    } 
    else if(k == 0x25) Topl_Program::timeline.dynamic_ticker.updateTime(-1.0); // Sandbox_Demo::texScroll[0] -= 0.05f; // left arrow
    else if(k == 0x27) Topl_Program::timeline.dynamic_ticker.updateTime(1.0); // Sandbox_Demo::texScroll[0] += 0.05f; // right arrow
    else if(k == 0x26) Topl_Program::timeline.dynamic_ticker.isPaused = false; // Sandbox_Demo::texScroll[1] -= 0.05f; // up arrow
    else if(k == 0x28) Topl_Program::timeline.dynamic_ticker.isPaused = true; // Sandbox_Demo::texScroll[1] += 0.05f; // down arrow
    else if(toupper(k) == 'X') { Sandbox_Demo::isShaderVariant = !Sandbox_Demo::isShaderVariant; }
    else if(isspace(k)) (Sandbox_Demo::pipelineIndex < 2)? Sandbox_Demo::pipelineIndex++ : Sandbox_Demo::pipelineIndex = 0;
    
    if(toupper(k) == 'W' || toupper(k) == 'S' || toupper(k) == 'A' || toupper(k) == 'D'){
        cameraPos = *Topl_Program::cameraObj.getPos();
        Topl_Program::timeline.addSequence_vec3f(&cameraPos, std::make_pair(TIMELINE_AT, cameraPos));
    } else if(toupper(k) == 'Q' || toupper(k) == 'E'){
        cameraRot = *Topl_Program::cameraObj.getRot();
        Topl_Program::timeline.addSequence_vec3f(&cameraRot, std::make_pair(TIMELINE_AT, cameraRot));
    } else if(toupper(k) == 'Z' || toupper(k) == 'C'){
        cameraZoom = *Topl_Program::cameraObj.getZoom();
        Topl_Program::timeline.addSequence_float(&cameraZoom, std::make_pair(TIMELINE_AT, cameraZoom));
    }
}

static void onScroll(bool positive){
    (positive)? logMessage("Scroll in positive")  : logMessage("Scroll in negative");

    if(Topl_Program::pickerObj != NO_PICKER_OBJ){
        if(Topl_Program::pickerObj->getId() == _instance->boxActor.getId() 
          || Topl_Program::pickerObj->getId() == _instance->pyramidActor.getId()
          || Topl_Program::pickerObj->getId() == _instance->hexActor.getId()
          || Topl_Program::pickerObj->getId() == _instance->sphereActor.getId()
          || Topl_Program::pickerObj->getId() == _instance->paramActors[2].getId()){
            if(positive) Topl_Program::pickerObj->updateSize({ 0.05F, 0.05F, 0.05F });
            else Topl_Program::pickerObj->updateSize({ -0.05F, -0.05F, -0.05F });
        }

        /* if(Topl_Program::pickerObj->getId() == _instance->boxActor.getId()){
            boxScale = (positive)? Vec3f({ boxScale[0] * 1.1f, boxScale[1] * 1.1f, boxScale[2] * 1.1f }) : boxScale = Vec3f({ boxScale[0] * 0.9f, boxScale[1] * 0.9f, boxScale[2] * 0.9f });
            Topl_Program::timeline.addSequence_vec3f(&boxScale, std::make_pair(TIMELINE_AT, boxScale));
        } else if(Topl_Program::pickerObj->getId() == _instance->pyramidActor.getId()){
            pyramidScale = (positive)? Vec3f({ pyramidScale[0] * 1.1f, pyramidScale[1] * 1.1f, pyramidScale[2] * 1.1f }) : pyramidScale = Vec3f({ pyramidScale[0] * 0.9f, pyramidScale[1] * 0.9f, pyramidScale[2] * 0.9f });
            Topl_Program::timeline.addSequence_vec3f(&pyramidScale, std::make_pair(TIMELINE_AT, pyramidScale));
        }
        else if(Topl_Program::pickerObj->getId() == _instance->sphereActor.getId()){
            sphereScale = (positive)? Vec3f({ sphereScale[0] * 1.1f, sphereScale[1] * 1.1f, sphereScale[2] * 1.1f }) : sphereScale = Vec3f({ sphereScale[0] * 0.9f, sphereScale[1] * 0.9f, sphereScale[2] * 0.9f });
            Topl_Program::timeline.addSequence_vec3f(&sphereScale, std::make_pair(TIMELINE_AT, sphereScale));
        }
        else if(Topl_Program::pickerObj->getId() == _instance->hexActor.getId()){
            hexScale = (positive)? Vec3f({ hexScale[0] * 1.1f, hexScale[1] * 1.1f, hexScale[2] * 1.1f }) : hexScale = Vec3f({ hexScale[0] * 0.9f, hexScale[1] * 0.9f, hexScale[2] * 0.9f });
            Topl_Program::timeline.addSequence_vec3f(&hexScale, std::make_pair(TIMELINE_AT, hexScale));
        } */
    }
    else Topl_Program::cameraObj.setZoom((positive)? *Topl_Program::cameraObj.getZoom() * 1.1 : *Topl_Program::cameraObj.getZoom() * 0.9); 
}

static void onDrag(float x, float y){ 
    static Vec3f savedColorVec = Vec3f{0.0, 0.0, 0.0};

    if(Platform::mouseControl.getIsMouseDown().second) {
        Topl_Program::cursorPos = { x, y, 0.0F }; 
        if(Topl_Program::pickerObj != NO_PICKER_OBJ){
            std::string pickerText = Topl_Program::pickerObj->getName();
            Rasteron_Text pickerTextObj = { fontPaths[3].c_str(), pickerText.c_str(), 0xFFEEEEEE, 0xFF333333 };
            _instance->pickerObj_texture.setTextImage(&pickerTextObj);
            _instance->pickerInfoActor.setSize({ 0.25F * (float)pickerText.length(), 1.0F, 1.0F });
            _instance->pickerInfoActor.isShown = true;

            if(Topl_Program::pickerObj->getId() == _instance->boxActor.getId() 
              || Topl_Program::pickerObj->getId() == _instance->pyramidActor.getId()
              || Topl_Program::pickerObj->getId() == _instance->hexActor.getId()
              || Topl_Program::pickerObj->getId() == _instance->sphereActor.getId()
              || Topl_Program::pickerObj->getId() == _instance->paramActors[2].getId()){
                (Platform::mouseControl.getIsMouseDown().first == MOUSE_LeftBtn_Down)
                  ? Topl_Program::pickerObj->setPos(Topl_Program::getCamCursorPos())
                  : Topl_Program::pickerObj->updateRot({ 
                        (savedColorVec[0] - Topl_Program::pickerCoord[0]) * 10, 
                        (savedColorVec[1] - Topl_Program::pickerCoord[1]) * 10, 
                        0.0 
                    });
            }

            /* if(Topl_Program::pickerObj->getId() == _instance->boxActor.getId())
                if(Platform::mouseControl.getIsMouseDown().first == MOUSE_LeftBtn_Down)
                    Topl_Program::timeline.addSequence_vec3f(&boxPos, std::make_pair(TIMELINE_AT, Topl_Program::getCamCursorPos()));
                else { 
                    boxRot = boxRot + Vec3f({ 0.0, (savedColorVec[1] - Topl_Program::pickerCoord[1]) * 5, (savedColorVec[0] - Topl_Program::pickerCoord[0]) * 5 });
                    Topl_Program::timeline.addSequence_vec3f(&boxRot, std::make_pair(TIMELINE_AT, boxRot));
                }
            else if(Topl_Program::pickerObj->getId() == _instance->pyramidActor.getId())
                if(Platform::mouseControl.getIsMouseDown().first == MOUSE_LeftBtn_Down)
                    Topl_Program::timeline.addSequence_vec3f(&pyramidPos, std::make_pair(TIMELINE_AT, Topl_Program::getCamCursorPos()));
                else { 
                    pyramidRot = pyramidRot + Vec3f({ 0.0, (savedColorVec[1] - Topl_Program::pickerCoord[1]) * 5, (savedColorVec[0] - Topl_Program::pickerCoord[0]) * 5 });
                    Topl_Program::timeline.addSequence_vec3f(&pyramidRot, std::make_pair(TIMELINE_AT, pyramidRot));
                }
            else if(Topl_Program::pickerObj->getId() == _instance->sphereActor.getId())
                if(Platform::mouseControl.getIsMouseDown().first == MOUSE_LeftBtn_Down)
                    Topl_Program::timeline.addSequence_vec3f(&spherePos, std::make_pair(TIMELINE_AT, Topl_Program::getCamCursorPos()));
                else { 
                    sphereRot = sphereRot + Vec3f({ 0.0, (savedColorVec[1] - Topl_Program::pickerCoord[1]) * 5, (savedColorVec[0] - Topl_Program::pickerCoord[0]) * 5 });
                    Topl_Program::timeline.addSequence_vec3f(&sphereRot, std::make_pair(TIMELINE_AT, sphereRot));
                }
            else if(Topl_Program::pickerObj->getId() == _instance->hexActor.getId())
                if(Platform::mouseControl.getIsMouseDown().first == MOUSE_LeftBtn_Down)
                    Topl_Program::timeline.addSequence_vec3f(&hexPos, std::make_pair(TIMELINE_AT, Topl_Program::getCamCursorPos()));
                else {
                    hexRot = hexRot + Vec3f({ 0.0, (savedColorVec[1] - Topl_Program::pickerCoord[1]) * 5, (savedColorVec[0] - Topl_Program::pickerCoord[0]) * 5 });
                    Topl_Program::timeline.addSequence_vec3f(&hexRot, std::make_pair(TIMELINE_AT, hexRot));
                } */
                // else _instance->hexActor.updateRot({ 0.0, (savedColorVec[1] - Topl_Program::pickerCoord[1]) * 5, (savedColorVec[0] - Topl_Program::pickerCoord[0]) * 5 });
        } else _instance->pickerInfoActor.isShown = false;
    } else Topl_Program::pickerObj = NO_PICKER_OBJ; // TODO: Handle Camera Events

    savedColorVec = Topl_Program::pickerCoord;
}

static void cameraUpdate(double m){ Topl_Program::cameraObj.setPos(cameraPos); Topl_Program::cameraObj.setRot(cameraRot); Topl_Program::cameraObj.setZoom(cameraZoom); }
static void shaderModeUpdate(){ (Sandbox_Demo::shaderMode < 8)? Sandbox_Demo::shaderMode++ : Sandbox_Demo::shaderMode = 0; }

static void timerTextUpdate(){
    unsigned secs = Topl_Program::timeline.dynamic_ticker.getAbsSecs(); // make accessible inside of program timer
    unsigned splitsecs = Topl_Program::timeline.dynamic_ticker.getAbsMillisecs() / 10;

    std::string timeText = ((secs < 60)? "" : (secs % 60 < 10)? std::to_string(secs / 60) + ":0" : std::to_string(secs / 60) + ":") + std::to_string(secs % 60) + ":00"; //+ std::to_string(splitsecs);
    Rasteron_Text timerTextObj = { Sandbox_Demo::fontFilePath.c_str(), timeText.c_str(), 0xFF333333, 0xFFEEEEEE };
    _instance->timerCount_texture.setTextImage(&timerTextObj);
}

void actions_onPick(Geo_Actor* actor){
    int c = (char)(actor->getName().back()) - '0';
    for(unsigned a = 0; a < 9; a++)
        (a == c - 1)? _instance->actionButtons[a].setState(MENU_On) : _instance->actionButtons[a].setState(MENU_None);

    switch(c - 1){
        case 0: Topl_Program::cameraObj.setZoom(*Topl_Program::cameraObj.getZoom() * 0.5F); break;
        case 2: Topl_Program::cameraObj.setZoom(*Topl_Program::cameraObj.getZoom() * 1.5F); break;
        default: break;
    }
}

void modes_onPick(Geo_Actor* actor){
    int c = (actor != nullptr)? (char)(actor->getName().back()) - '0' : -1;
    for(unsigned m = 0; m < 6; m++){
        std::string modeText = "mode " + std::to_string(m + 1);
        Rasteron_Text timerTextObj = { 
            fontPaths[m].c_str(), modeText.c_str(), 
            (m != c - 1)? 0xFF111111 : 0xFF00FF00, (m != c - 1)? 0xFFEEEEEE : 0xFF111111 
        };
        _instance->modeButtons[m].setTextImage(&timerTextObj);
    }
}

static void slider_onPick(Geo_Actor* actor){ _instance->timelineSlider.setState(Topl_Program::pickerCoord[0]); }

Sandbox_Demo::~Sandbox_Demo(){
#ifdef RASTERON_H
    dealloc_queue(words_queue);
    dealloc_image(boxImg); dealloc_image(pyramidImg); dealloc_image(sphereImg); dealloc_image(hexImg); dealloc_image(paramImg);
#endif
}

void Sandbox_Demo::init(){
    srand(time(NULL));
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
    // canvas.addTexture("Backdrop", &canvasTex);
    Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, flatPipeline);
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
    scene.addGeometry("Experimental1", &paramActors[0]);
    // paramActors[1].setPos({ -0.5F, 0.0F, 0.0F });
    scene.addGeometry("Experimental2", &paramActors[1]);
    // paramActors[2].setPos({ 0.5F, 0.0F, 0.0F });
    scene.addGeometry("Experimental3", &paramActors[2]);
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
    scene.addTexture("Experimental1", &paramTex);
    scene.addTexture("Experimental2", &paramTex);
    scene.addTexture("Experimental3", &paramTex);
#endif

    Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, texPipeline);
    _renderer->buildScene(&scene);
    _renderer->texturizeScene(&scene);
    
    timerInfoMesh.scale({ 1.5f, 0.3F, 1.0f });
    timerInfoActor.setPos({ 0.0f, -0.835f, 0.0f }); // inputInfoActor.setPos({ 0.0f, -0.9f, 0.0f }); 
    overlay.addGeometry("timerInfo", &timerInfoActor);
    pickerInfoMesh.scale({ 1.5f, 0.3F, 1.0f });
    pickerInfoActor.setPos({ 0.0f, 0.85f, 0.0f });
    pickerInfoActor.isShown = false;
    overlay.addGeometry("pickerInfo", &pickerInfoActor);
    timelineLayout.configure(&overlay);
    timelineLayout.scale({ 2.75F, 0.25F, 1.0F });
    timelineLayout.shift({ -0.03f, -0.925f, 0.0f });
    // statusLayout.configure(&overlay);
    // statusLayout.scale({ 3.35F, 0.25F, 1.0F });
    // statusLayout.shift({ 0.0F, 0.85f, 0.0f });
    modeLayout.configure(&overlay);
    modeLayout.shift({ 0.865F, -0.87F, 0.0F });
    modeLayout.scale({ 0.4F, 0.4F, 1.0F });
    actionsLayout.configure(&overlay);
    actionsLayout.shift({ -0.9F, -0.87F, 0.0F });
    actionsLayout.scale({ 0.4F, 0.4F, 1.0F  });
#ifdef RASTERON_H // adding textures fro overlay
    overlay.addTexture("timerInfo", &timerCount_texture);
    overlay.addTexture("pickerInfo", &pickerObj_texture);
    overlay.addTexture("timelineLayout_cell1", &timelineSlider.stateImg);
    timelineLayout.getGeoActor(0)->pickerFunc = slider_onPick;
    modes_onPick(nullptr);
    for(unsigned m = 0; m < 6; m++){
        modeLayout.getGeoActor(m)->pickerFunc = modes_onPick;
        overlay.addTexture("modeLayout_cell" + std::to_string(m + 1), &_instance->modeButtons[m]);
    }
    for(unsigned a = 0; a < 9; a++){
        actionsLayout.getGeoActor(a)->pickerFunc = actions_onPick;
        overlay.addTexture("actionsLayout_cell" + std::to_string(a + 1), &actionButtons[a].stateImg);
    }
#endif
    _renderer->buildScene(&overlay);
    _renderer->texturizeScene(&overlay);
}

void Sandbox_Demo::loop(double frameTime){
    // Object Updates

    // _instance->boxActor.setPos(boxPos);
    // _instance->boxActor.setRot(boxRot);
    // _instance->boxActor.setSize(boxScale);
    // _instance->pyramidActor.setPos(pyramidPos);
    // _instance->pyramidActor.setRot(pyramidRot);
    // _instance->pyramidActor.setSize(pyramidScale);
    // _instance->sphereActor.setPos(spherePos);
    // _instance->sphereActor.setRot(sphereRot);
    // _instance->sphereActor.setSize(sphereScale);
    // _instance->hexActor.setPos(hexPos);
    // _instance->hexActor.setRot(hexRot);
    // _instance->hexActor.setSize(hexScale);

    _instance->paramActors[2].updateRot(*_instance->paramActors[2].getRot() * 0.005);
#ifdef RASTERON_H
    // if(_renderer->getFrameCount() % 10 == 0 || Platform::mouseControl.getIsMouseDown().second){
        // _renderer->texturizeScene(&canvas);
        // _renderer->texturizeScene(&scene);
        _renderer->texturizeScene(&overlay);
    // }
#endif

    // Capture Renders

    if(Platform::mouseControl.getIsMouseDown().second) {
        flatVShader.setMode(FLAT_MODE_SOLID); 

        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, flatPipeline);
        _renderer->setCamera(&Topl_Program::cameraObj);
        _renderer->updateScene(&scene);
        _renderer->drawScene(&scene);

        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, flatPipeline);
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
        flatVShader.setMode(FLAT_MODE_DIRECTION); // effectVShader.setMode(1);
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, flatPipeline);
        _renderer->setCamera(&Topl_Program::cameraObj);
        _renderer->updateScene(&canvas);
        _renderer->drawScene(&canvas);

        flatVShader.setMode(FLAT_MODE_COORD); 
        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, flatPipeline);
        _renderer->setCamera(&Topl_Program::cameraObj);
        _renderer->updateScene(&scene);
        _renderer->drawScene(&scene);

        flatVShader.setMode(FLAT_MODE_COORD);
        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, flatPipeline);
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

    /* {  
        flatVShader.setMode(FLAT_MODE_DIRECTION); effectVShader.setMode(1);
        texVShader.setMode(0);
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, flatPipeline);
        _renderer->updateScene(&canvas);
        _renderer->drawScene(&canvas);
    } */

    {
        flatVShader.setMode((Sandbox_Demo::isShaderVariant)? Sandbox_Demo::shaderMode % 3 : 0); 
        beamVShader.setMode((Sandbox_Demo::isShaderVariant)? Sandbox_Demo::shaderMode / 2 : 0); 
        texVShader.setMode((Sandbox_Demo::isShaderVariant)? -Sandbox_Demo::shaderMode : 0);
        texVShader.setTexScroll(Sandbox_Demo::texScroll);

        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, (Sandbox_Demo::pipelineIndex == 0)? texPipeline : (Sandbox_Demo::pipelineIndex == 1)? beamPipeline : flatPipeline);

        _renderer->setDrawMode(DRAW_Lines);
        _renderer->setCamera(&Topl_Program::cameraObj);
        _renderer->updateScene(&scene);
        _renderer->draw(&_instance->boxActor);
        _renderer->draw(&_instance->pyramidActor);
        _renderer->draw(&_instance->sphereActor);
        _renderer->draw(&_instance->hexActor);

        _renderer->setDrawMode(DRAW_Triangles);
        _renderer->draw(&_instance->paramActors[2]);

        _renderer->setDrawMode(DRAW_Triangles);
        _renderer->setCamera(&fixedCamera);
        _renderer->updateScene(&overlay);
        _renderer->drawScene(&overlay);
    }
}

int main(int argc, char** argv) {
    _instance = new Sandbox_Demo(argv[0], BACKEND_GL4);

    _instance->run();
    delete(_instance);
    return 0;
}