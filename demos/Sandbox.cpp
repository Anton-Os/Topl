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
    if(Topl_Program::pickerObj != NO_PICKER_OBJ){
        if(Topl_Program::pickerObj->getId() == _instance->boxActor.getId()){
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
        }
    }
    else Topl_Program::cameraObj.setZoom((positive)? *Topl_Program::cameraObj.getZoom() * 1.1 : *Topl_Program::cameraObj.getZoom() * 0.9); 
}

static void onHover(float x, float y){ 
    static Vec3f savedColorVec = Vec3f{0.0, 0.0, 0.0};
    // static Vec3f pickedColorVec = Vec3f{0.0, 0.0, 0.0};
    static bool isMotionEnable = false;

    if(Platform::mouseControl.getIsMouseDown().second) {
        Topl_Program::cursorPos = { x, y, 0.0F }; 
        if(Topl_Program::pickerObj != NO_PICKER_OBJ){
            if(isMotionEnable && Topl_Program::pickerObj->getId() == _instance->boxActor.getId())
                if(Platform::mouseControl.getIsMouseDown().first == MOUSE_LeftBtn_Down)
                    Topl_Program::timeline.addSequence_vec3f(&boxPos, std::make_pair(TIMELINE_AT, Topl_Program::getCamCursorPos()));
                else { 
                    boxRot = boxRot + Vec3f({ 0.0, (savedColorVec[1] - Topl_Program::pickerCoord[1]) * 5, (savedColorVec[0] - Topl_Program::pickerCoord[0]) * 5 });
                    Topl_Program::timeline.addSequence_vec3f(&boxRot, std::make_pair(TIMELINE_AT, boxRot));
                }
            else if(isMotionEnable && Topl_Program::pickerObj->getId() == _instance->pyramidActor.getId())
                if(Platform::mouseControl.getIsMouseDown().first == MOUSE_LeftBtn_Down)
                    Topl_Program::timeline.addSequence_vec3f(&pyramidPos, std::make_pair(TIMELINE_AT, Topl_Program::getCamCursorPos()));
                else { 
                    pyramidRot = pyramidRot + Vec3f({ 0.0, (savedColorVec[1] - Topl_Program::pickerCoord[1]) * 5, (savedColorVec[0] - Topl_Program::pickerCoord[0]) * 5 });
                    Topl_Program::timeline.addSequence_vec3f(&pyramidRot, std::make_pair(TIMELINE_AT, pyramidRot));
                }
            else if(isMotionEnable && Topl_Program::pickerObj->getId() == _instance->sphereActor.getId())
                if(Platform::mouseControl.getIsMouseDown().first == MOUSE_LeftBtn_Down)
                    Topl_Program::timeline.addSequence_vec3f(&spherePos, std::make_pair(TIMELINE_AT, Topl_Program::getCamCursorPos()));
                else { 
                    sphereRot = sphereRot + Vec3f({ 0.0, (savedColorVec[1] - Topl_Program::pickerCoord[1]) * 5, (savedColorVec[0] - Topl_Program::pickerCoord[0]) * 5 });
                    Topl_Program::timeline.addSequence_vec3f(&sphereRot, std::make_pair(TIMELINE_AT, sphereRot));
                }
            else if(isMotionEnable && Topl_Program::pickerObj->getId() == _instance->hexActor.getId())
                if(Platform::mouseControl.getIsMouseDown().first == MOUSE_LeftBtn_Down)
                    Topl_Program::timeline.addSequence_vec3f(&hexPos, std::make_pair(TIMELINE_AT, Topl_Program::getCamCursorPos()));
                else {
                    hexRot = hexRot + Vec3f({ 0.0, (savedColorVec[1] - Topl_Program::pickerCoord[1]) * 5, (savedColorVec[0] - Topl_Program::pickerCoord[0]) * 5 });
                    Topl_Program::timeline.addSequence_vec3f(&hexRot, std::make_pair(TIMELINE_AT, hexRot));
                }
                // else _instance->hexActor.updateRot({ 0.0, (savedColorVec[1] - Topl_Program::pickerCoord[1]) * 5, (savedColorVec[0] - Topl_Program::pickerCoord[0]) * 5 });
        }
        isMotionEnable = true;
    } else {
        isMotionEnable = false;
        Topl_Program::pickerObj = NO_PICKER_OBJ; // TODO: Handle Camera Events
    }
    savedColorVec = Topl_Program::pickerCoord;
}

static void cameraUpdate(double m){ Topl_Program::cameraObj.setPos(cameraPos); Topl_Program::cameraObj.setRot(cameraRot); Topl_Program::cameraObj.setZoom(cameraZoom); }
static void shaderModeUpdate(){ (Sandbox_Demo::shaderMode < 8)? Sandbox_Demo::shaderMode++ : Sandbox_Demo::shaderMode = 0; }
static void overlayTexUpdate(){
    unsigned secs = Topl_Program::timeline.dynamic_ticker.getAbsSecs(); // make accessible inside of program timer
    unsigned splitsecs = Topl_Program::timeline.dynamic_ticker.getAbsMillisecs() / 10;

    std::string timeText = ((secs < 60)? "" : (secs % 60 < 10)? std::to_string(secs / 60) + ":0" : std::to_string(secs / 60) + ":") + std::to_string(secs % 60) + ":00"; //+ std::to_string(splitsecs);
    Rasteron_Text timerTextObj = { Sandbox_Demo::fontFilePath.c_str(), timeText.c_str(), 0xAA00FFFF, 0xFF333333 };
    _instance->timerCount_texture.setTextImage(&timerTextObj);

}

static void default_shadercall(Topl_EntryShader* shader){ }

static void gridCell_pickercall(Geo_Actor* actor){
    int c = (char)(actor->getName().back()) - '0';

    if(c == 7 || c == 8 || c == 9) // check operations
        _instance->checks[c - 7].setState((_instance->checks[c - 7].state != MENU_On)? MENU_On : MENU_Off);
    else if(c == 4 || c == 5 || c == 6) // icon operations
        _instance->icons[c - 4].setState((_instance->icons[c - 4].state != MENU_On)? MENU_On : MENU_Off);
    else if(c == 1 || c == 2 || c == 3) // dial opertions
        _instance->dials[c - 1].setState(Topl_Program::pickerCoord[0], Topl_Program::pickerCoord[1]);
}

static void sliderCell_pickercall(Geo_Actor* actor){
    int c = (char)(actor->getName().back()) - '0';
    _instance->sliders[c - 1].setState(Topl_Program::pickerCoord[0]);
}

static void textCell_pickercall(Geo_Actor* actor){
    int c = (char)(actor->getName().back()) - '0';

    for(unsigned s = 0; s < 9; s++){
        std::string sliderCellText = "select   option   " + std::to_string(s + 1);
        std::replace(fontPaths[s].begin(), fontPaths[s].end(), '/', '\\');

        Rasteron_Text textObj = (s != c - 1)
            ? Rasteron_Text({ fontPaths[s].c_str(), sliderCellText.c_str(), *getFrameAt(_instance->words_queue, s)->data, 0xEE000000 | (0xEEFFFFFF - *getFrameAt(_instance->words_queue, s)->data)} ) 
            : Rasteron_Text({ fontPaths[s].c_str(), sliderCellText.c_str(),  0xEEFFFF00, 0xEE00FF00 });

        _instance->words_textures[s].setTextImage(&textObj);
    }

    // _instance->words_textures[c - 1].setColorImage(0xFF00FF00);
}

Sandbox_Demo::~Sandbox_Demo(){
#ifdef RASTERON_H
    dealloc_queue(words_queue);
    dealloc_image(boxImg); dealloc_image(pyramidImg); dealloc_image(sphereImg); dealloc_image(hexImg);
#endif
}

void Sandbox_Demo::init(){
    srand(time(NULL));
    std::replace(Sandbox_Demo::fontFilePath.begin(), Sandbox_Demo::fontFilePath.end(), '/', '\\'); // make sure font file parsed correctly

    Platform::keyControl.addAnyCallback(onAnyKey);
    Platform::mouseControl.setScrollCallback(onScroll);
    Platform::mouseControl.addHoverCallback(onHover);

    Topl_Program::timeline.dynamic_ticker.addRecurringEvent(cameraUpdate);
    Topl_Program::timeline.persist_ticker.addPeriodicEvent(2500, shaderModeUpdate);
    Topl_Program::timeline.persist_ticker.addPeriodicEvent(1000, overlayTexUpdate);

    canvasActor.setPos({ 0.0f, 0.0f, -1.0F});
    canvas.addGeometry("Backdrop", &canvasActor);
    // canvasTex.setFileImage("F:\\Design\\Motivation-Build.png"); // placeholder image
    // canvas.addTexture("Backdrop", &canvasTex);
    Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, flatPipeline);
    _renderer->buildScene(&canvas);
    _renderer->texturizeScene(&canvas);

    boxMesh.scale({ 0.25f, 0.25f, 0.25f});
    // boxActor.setPos({ 0.5f, 0.5f, 0.0f });
    boxActor.shaderFunc = default_shadercall;  
    scene.addGeometry("Box", &boxActor);
    pyramidMesh.scale({ 0.25f, 0.25f, 0.25f});
    // pyramidActor.setPos({ -0.5f, 0.5f, 0.0f });
    pyramidActor.shaderFunc = default_shadercall;
    scene.addGeometry("Pyramid", &pyramidActor);
    sphereMesh.scale({ 0.25f, 0.25f, 0.25f});
    // sphereActor.setPos({ -0.5f, -0.5f, 0.0f });
    sphereActor.shaderFunc = default_shadercall;
    scene.addGeometry("Orb", &sphereActor);
    hexMesh.scale({ 0.25f, 0.25f, 0.25f});
    // hexActor.setPos({ 0.5f, -0.5f, 0.0f });
    hexActor.shaderFunc = default_shadercall;
    scene.addGeometry("Hex", &hexActor);

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
#endif

    Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, texPipeline);
    _renderer->buildScene(&scene);
    _renderer->texturizeScene(&scene);
    
    infoBoxMesh.scale({ 5.0f, 1.0, 1.0f });
    timerInfoActor.setPos({ 0.0f, 0.75f, 0.0f }); // inputInfoActor.setPos({ 0.0f, -0.9f, 0.0f }); 

    layout1.configure(&overlay);
    layout2.configure(&overlay);
    layout2.shift({ 0.55, 0.0, 0.0 });
    layout3.configure(&overlay);
    layout3.shift({ -0.55, 0.0, 0.0 });
    overlay.addGeometry("timerInfo", &timerInfoActor);

    Geo_Actor* gridCellActor[9] = { layout1.getGeoActor(0), layout1.getGeoActor(1), layout1.getGeoActor(2), layout1.getGeoActor(3), layout1.getGeoActor(4), layout1.getGeoActor(5), layout1.getGeoActor(6), layout1.getGeoActor(7), layout1.getGeoActor(8), };
    for(unsigned g = 0; g < 9; g++){ gridCellActor[g]->pickerFunc = gridCell_pickercall; }

    Geo_Actor* textCellActors[9] = { layout2.getGeoActor(0), layout2.getGeoActor(1), layout2.getGeoActor(2), layout2.getGeoActor(3), layout2.getGeoActor(4), layout2.getGeoActor(5), layout2.getGeoActor(6), layout2.getGeoActor(7), layout2.getGeoActor(8) };
    for(unsigned g = 0; g < 9; g++){ textCellActors[g]->pickerFunc = textCell_pickercall; }

    Geo_Actor* sliderCellActors[9] = { layout3.getGeoActor(0), layout3.getGeoActor(1), layout3.getGeoActor(2), layout3.getGeoActor(3), layout3.getGeoActor(4), layout3.getGeoActor(5), layout3.getGeoActor(6), layout3.getGeoActor(7), layout3.getGeoActor(8) };
    for(unsigned g = 0; g < 9; g++){ sliderCellActors[g]->pickerFunc = sliderCell_pickercall; }


#ifdef RASTERON_H // adding textures fro overlay
    overlay.addTexture("gridLayout_cell1", &dials[0].stateImg);
    overlay.addTexture("gridLayout_cell2", &dials[1].stateImg);
    overlay.addTexture("gridLayout_cell3", &dials[2].stateImg);
    overlay.addTexture("gridLayout_cell4", &icons[0].stateImg);
    overlay.addTexture("gridLayout_cell5", &icons[1].stateImg);
    overlay.addTexture("gridLayout_cell6", &icons[2].stateImg);
    overlay.addTexture("gridLayout_cell7", &checks[0].stateImg);
    overlay.addTexture("gridLayout_cell8", &checks[1].stateImg);
    overlay.addTexture("gridLayout_cell9", &checks[2].stateImg);

    overlay.addTexture("timerInfo", &timerCount_texture);

    for(unsigned s = 0; s < 9; s++){
        overlay.addTexture("vertLayout_cell" + std::to_string(s + 1), &_instance->sliders[s].stateImg);

        std::string sliderCellText = "select   option   " + std::to_string(s + 1);
        std::replace(fontPaths[s].begin(), fontPaths[s].end(), '/', '\\');
        Rasteron_Text textObj = { fontPaths[s].c_str(), sliderCellText.c_str(), *getFrameAt(words_queue, s)->data, 0xEE000000 | (0xEEFFFFFF - *getFrameAt(words_queue, s)->data)};
        words_textures[s].setTextImage(&textObj);
        overlay.addTexture("horzLayout_cell" + std::to_string(s + 1), &words_textures[s]);
    }
#endif

    // Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, flatPipeline);
    _renderer->buildScene(&overlay);
    _renderer->texturizeScene(&overlay);
}

void Sandbox_Demo::loop(double frameTime){
    // Object Updates

    _instance->boxActor.setPos(boxPos);
    _instance->boxActor.setRot(boxRot);
    _instance->boxActor.setSize(boxScale);
    _instance->pyramidActor.setPos(pyramidPos);
    _instance->pyramidActor.setRot(pyramidRot);
    _instance->pyramidActor.setSize(pyramidScale);
    _instance->sphereActor.setPos(spherePos);
    _instance->sphereActor.setRot(sphereRot);
    _instance->sphereActor.setSize(sphereScale);
    _instance->hexActor.setPos(hexPos);
    _instance->hexActor.setRot(hexRot);
    _instance->hexActor.setSize(hexScale);
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
        // flatVShader.setMode(2);
        _renderer->updateScene(&scene);
        _renderer->drawScene(&scene);

        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, flatPipeline);
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
        _renderer->updateScene(&canvas);
        _renderer->drawScene(&canvas);

        flatVShader.setMode(FLAT_MODE_COORD); 
        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, flatPipeline);
        _renderer->updateScene(&scene);
        _renderer->drawScene(&scene);

        flatVShader.setMode(FLAT_MODE_COORD);
        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, flatPipeline);
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

        _renderer->setDrawMode(DRAW_Triangles);
        // Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, (isTexPipeline)? texPipeline : beamPipeline);
        _renderer->updateScene(&scene);
        _renderer->draw(&_instance->boxActor);
        _renderer->draw(&_instance->pyramidActor);
        _renderer->draw(&_instance->sphereActor);
        _renderer->draw(&_instance->hexActor);

        _renderer->setDrawMode(DRAW_Triangles);
        // Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, (isTexPipeline)? texPipeline : beamPipeline);
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