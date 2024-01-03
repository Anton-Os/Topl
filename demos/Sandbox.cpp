#include "Sandbox.hpp"

int Sandbox_Demo::mode = 0;
int Sandbox_Demo::pipelineIndex = 0;
bool Sandbox_Demo::isShaderVariant = false;
unsigned Sandbox_Demo::shaderMode = 0;
Vec3f Sandbox_Demo::texScroll = { 0.0, 0.0, 0.0 };
std::string Sandbox_Demo::fontFilePath = std::string(FONTS_DIR) + "MajorMonoDisplay-Regular.ttf";

#ifdef RASTERON_H
unsigned boxImgOp(double x, double y){ return (sin(x * 20) > 0.5)? 0xFF0000FF : 0xFFFFFF00; }
unsigned pyramidImgOp(double x, double y){ return (cos(y * 20) < 0.5)? 0xFFFF0000 : 0xFF00FFFF; }
unsigned sphereImgOp(double x, double y){ return (tan(x * y) > 0.25 && tan(x * y) < 0.75)? 0xFF8833CC : 0xFF88CC33; }
unsigned hexImgOp(double x, double y){ return (x > 0.4 && x < 0.6 && y > 0.4 && y < 0.6)? 0xFF3333333 : 0xFFEEEEEE; }
#endif

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
}

static void onScroll(bool positive){
    Topl_Program::cameraObj.setZoom((positive)? *Topl_Program::cameraObj.getZoom() * 1.1 : *Topl_Program::cameraObj.getZoom() * 0.9); 
}

static void onHover(float x, float y){ 
    static Vec3f savedColorVec = Vec3f{0.0, 0.0, 0.0};
    static Vec3f pickedColorVec = Vec3f{0.0, 0.0, 0.0};
    static bool isMotionEnable = false;

    if(Platform::mouseControl.getIsMouseDown().second) {
        Topl_Program::cursorPos = { x, y, 0.0F }; 
        pickedColorVec = Vec3f{
            ((Topl_Program::pickerVal & 0xFF0000) >> 16) / 255.0f,
            ((Topl_Program::pickerVal & 0xFF00) >> 8) / 255.0f, 
            (Topl_Program::pickerVal & 0xFF) / 255.0f,  
	    };

        if(Topl_Program::pickerObj != NO_PICKER_OBJ){
            std::cout << "Topl_Program picker object: " << Topl_Program::pickerObj->getName() << std::endl;
            printf("Picker value: %X\n", Topl_Program::pickerVal);

            if(isMotionEnable && Topl_Program::pickerObj->getId() == _instance->boxActor.getId())
                (Platform::mouseControl.getIsMouseDown().first == MOUSE_LeftBtn_Down)
                    ? _instance->boxActor.setPos(Topl_Program::getCamCursorPos())
                    : _instance->boxActor.updateRot({ (savedColorVec[0] - pickedColorVec[0]) * 20, (savedColorVec[1] - pickedColorVec[1]) * 8, 0.0 });
            else if(isMotionEnable && Topl_Program::pickerObj->getId() == _instance->pyramidActor.getId())
                (Platform::mouseControl.getIsMouseDown().first == MOUSE_LeftBtn_Down)
                    ? _instance->pyramidActor.setPos(Topl_Program::getCamCursorPos())
                    : _instance->pyramidActor.updateRot({ (savedColorVec[0] - pickedColorVec[0]) * 20, (savedColorVec[1] - pickedColorVec[1]) * 8, 0.0 });
            else if(isMotionEnable && Topl_Program::pickerObj->getId() == _instance->sphereActor.getId())
                (Platform::mouseControl.getIsMouseDown().first == MOUSE_LeftBtn_Down)
                    ? _instance->sphereActor.setPos(Topl_Program::getCamCursorPos())
                    : _instance->sphereActor.updateRot({ (savedColorVec[0] - pickedColorVec[0]) * 20, (savedColorVec[1] - pickedColorVec[1]) * 8, 0.0 });
            else if(isMotionEnable && Topl_Program::pickerObj->getId() == _instance->hexActor.getId())
                (Platform::mouseControl.getIsMouseDown().first == MOUSE_LeftBtn_Down)
                    ? _instance->hexActor.setPos(Topl_Program::getCamCursorPos())
                    : _instance->hexActor.updateRot({ (savedColorVec[0] - pickedColorVec[0]) * 20, (savedColorVec[1] - pickedColorVec[1]) * 8, 0.0 });

            // Topl_Program::isInputEnabled = Topl_Program::pickerObj->getId() == _instance->inputInfoActor.getId();
        } else {
            if(isMotionEnable && Platform::mouseControl.getIsMouseDown().first == MOUSE_LeftBtn_Down){
                if(savedColorVec[0] != pickedColorVec[0] || savedColorVec[1] != pickedColorVec[1])
                    Topl_Program::cameraObj.updatePos({ savedColorVec[0] - pickedColorVec[0], savedColorVec[1] - pickedColorVec[1], 0.0 });
            } else if(isMotionEnable && Platform::mouseControl.getIsMouseDown().first == MOUSE_RightBtn_Down){
                if(savedColorVec[0] != pickedColorVec[0])
                    Topl_Program::cameraObj.updateRot({ savedColorVec[0] - pickedColorVec[0], 0.0, 0.0 });
                if(savedColorVec[1] != pickedColorVec[1])
                    Topl_Program::cameraObj.updateRot({ savedColorVec[1] - pickedColorVec[1], 0.0, 0.0 });
            }
        }
        isMotionEnable = true;
    } else {
        isMotionEnable = false;
        Topl_Program::pickerObj = NO_PICKER_OBJ; // TODO: Handle Camera Events
    }
    savedColorVec = pickedColorVec;
}

void shaderModeCycle(){ (Sandbox_Demo::shaderMode < 8)? Sandbox_Demo::shaderMode++ : Sandbox_Demo::shaderMode = 0; }
void overlayTexUpdate(){
    unsigned secs = Topl_Program::timeline.dynamic_ticker.getAbsMillisecs() / MILLISEC_IN_SEC; // make accessible inside of program timer
    unsigned splitsecs = Topl_Program::timeline.dynamic_ticker.getAbsMillisecs() / 10;

    // Rasteron_Text inputTextObj = { Sandbox_Demo::fontFilePath.c_str(), Topl_Program::userInput.c_str(), 0xAAFF00FF, 0xFF333333 };
    // (!Topl_Program::userInput.empty())? _instance->userInput_texture.setTextImage(&inputTextObj) : _instance->userInput_texture.setColorImage(0xFFFFFF00);
    std::string timeText = ((secs < 60)? "" : (secs % 60 < 10)? std::to_string(secs / 60) + ":0" : std::to_string(secs / 60) + ":") + std::to_string(secs % 60) + ":00"; //+ std::to_string(splitsecs);
    Rasteron_Text timerTextObj = { Sandbox_Demo::fontFilePath.c_str(), timeText.c_str(), 0xAA00FFFF, 0xFF333333 };
    _instance->timerCount_texture.setTextImage(&timerTextObj);

}

static void default_shadercall(Topl_EntryShader* shader){ 
    /* Add body for render block */ 
}
static void gridCell_pickercall(Geo_Actor* actor){
    int c = (char)(actor->getName().back()) - '0';
    if(c == 1 || c == 2 || c == 3)
        _instance->dialBtn_textures[c - 1].setImage(getFrameAt(_instance->dialBtn_queues[c - 1], (_instance->dialBtn_queues[c - 1]->index + 1 < _instance->dialBtn_queues[c - 1]->frameCount)? _instance->dialBtn_queues[c - 1]->index + 1 : 0));
    else if(c == 4 || c == 5 || c == 6)
        _instance->iconBtn_textures[c - 4].setImage(getFrameAt(_instance->iconBtn_queues[c - 4], (_instance->iconBtn_queues[c - 4]->index + 1 < 3)? _instance->iconBtn_queues[c - 4]->index + 1 : 0));
    if(c == 7 || c == 8 || c == 9)
        _instance->checkBtn_textures[c - 7].setImage(getFrameAt(_instance->checkBtn_queue, 0));
}

Sandbox_Demo::~Sandbox_Demo(){
#ifdef RASTERON_H
    for(unsigned q = 0; q < 3; q++){
        dealloc_queue(iconBtn_queues[q]);
        dealloc_queue(dialBtn_queues[q]);
    }
    dealloc_queue(checkBtn_queue);
    dealloc_queue(slider_queue); // for(unsigned s = 0; s < 9; s++) dealloc_queue(slider_queues[s]);
    dealloc_queue(words_queue);

    dealloc_image(boxImg);
    dealloc_image(pyramidImg);
    dealloc_image(sphereImg);
    dealloc_image(hexImg);
#endif
}

void Sandbox_Demo::init(){
    srand(time(NULL));
    std::replace(Sandbox_Demo::fontFilePath.begin(), Sandbox_Demo::fontFilePath.end(), '/', '\\'); // make sure font file parsed correctly

    Platform::keyControl.addAnyCallback(onAnyKey);
    // Platform::mouseControl.addCallback(MOUSE_LeftBtn_Down, onPress);
    // Platform::mouseControl.addCallback(MOUSE_RightBtn_Down, onPress);
    Platform::mouseControl.setScrollCallback(onScroll);
    Platform::mouseControl.addHoverCallback(onHover);

    Topl_Program::timeline.persist_ticker.addPeriodicEvent(2500, shaderModeCycle);
    Topl_Program::timeline.persist_ticker.addPeriodicEvent(1000, overlayTexUpdate);

    canvasActor.setPos({ 0.0f, 0.0f, -1.0F});
    canvas.addGeometry("Backdrop", &canvasActor);
    Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, effectPipeline);
    _renderer->buildScene(&canvas);

    boxMesh.scale({ 0.25f, 0.25f, 0.25f});
    boxActor.setPos({ 0.5f, 0.5f, 0.0f });
    boxActor.shaderFunc = default_shadercall;  
    scene.addGeometry("Box", &boxActor);
    details.addGeometry("Box", &boxActor);
    pyramidMesh.scale({ 0.25f, 0.25f, 0.25f});
    pyramidActor.setPos({ -0.5f, 0.5f, 0.0f });
    pyramidActor.shaderFunc = default_shadercall;
    scene.addGeometry("Pyramid", &pyramidActor);
    details.addGeometry("Pyramid", &pyramidActor);
    sphereMesh.scale({ 0.25f, 0.25f, 0.25f});
    sphereActor.setPos({ -0.5f, -0.5f, 0.0f });
    sphereActor.shaderFunc = default_shadercall;
    scene.addGeometry("Sphere", &sphereActor);
    details.addGeometry("Sphere", &sphereActor);
    hexMesh.scale({ 0.25f, 0.25f, 0.25f});
    hexActor.setPos({ 0.5f, -0.5f, 0.0f });
    hexActor.shaderFunc = default_shadercall;
    scene.addGeometry("Hex", &hexActor);
    details.addGeometry("Hex", &hexActor);

#ifdef RASTERON_H // Adding textures for scene
    boxImg = mapImgOp({1024, 1024}, boxImgOp);
    boxTex.setImage(boxImg);
    scene.addTexture("Box", &boxTex);
    pyramidImg = mapImgOp({1024, 1024}, pyramidImgOp);
    pyramidTex.setImage(pyramidImg);
    scene.addTexture("Pyramid", &pyramidTex);
    sphereImg = mapImgOp({1024, 1024}, sphereImgOp);
    sphereTex.setImage(sphereImg);
    scene.addTexture("Sphere", &sphereTex);
    hexImg = mapImgOp({1024, 1024}, hexImgOp);
    hexTex.setImage(hexImg);
    scene.addTexture("Hex", &hexTex);
#endif

    // chain.configure(&scene);
    // grid.configure(&scene);

    Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, texPipeline);
    _renderer->buildScene(&scene);
    // Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, texPipeline);
    // _renderer->buildScene(&details);
    
    infoBoxMesh.scale({ 5.0f, 1.0, 1.0f });
    timerInfoActor.setPos({ 0.0f, 0.9f, 0.0f }); // inputInfoActor.setPos({ 0.0f, -0.9f, 0.0f }); 

    layout1.configure(&overlay);
    layout2.configure(&overlay);
    layout2.shift({ 0.55, 0.0, 0.0 });
    layout3.configure(&overlay);
    layout3.shift({ -0.55, 0.0, 0.0 });
    overlay.addGeometry("timerInfo", &timerInfoActor);
    // overlay.addGeometry("inputInfo", &inputInfoActor);

    Geo_Actor* gridActors[9] = { layout1.getGeoActor(0), layout1.getGeoActor(1), layout1.getGeoActor(2), layout1.getGeoActor(3), layout1.getGeoActor(4), layout1.getGeoActor(5), layout1.getGeoActor(6), layout1.getGeoActor(7), layout1.getGeoActor(8), };
    for(unsigned g = 0; g < 9; g++){ gridActors[g]->pickerFunc = gridCell_pickercall; }

#ifdef RASTERON_H // adding textures fro overlay
    overlay.addTexture("gridLayout_cell1", &dialBtn_textures[0]);
    overlay.addTexture("gridLayout_cell2", &dialBtn_textures[1]);
    overlay.addTexture("gridLayout_cell3", &dialBtn_textures[2]);
    overlay.addTexture("gridLayout_cell4", &iconBtn_textures[0]);
    overlay.addTexture("gridLayout_cell5", &iconBtn_textures[1]);
    overlay.addTexture("gridLayout_cell6", &iconBtn_textures[2]);
    overlay.addTexture("gridLayout_cell7", &checkBtn_textures[0]);
    overlay.addTexture("gridLayout_cell8", &checkBtn_textures[1]);
    overlay.addTexture("gridLayout_cell9", &checkBtn_textures[2]);

    overlay.addTexture("timerInfo", &timerCount_texture);
    // overlay.addTexture("inputInfo", &userInput_texture);
    for(unsigned s = 0; s < 9; s++){
        Rasteron_Image* flipImg = flipImgOp(slider_textures[s].getImage(), FLIP_Counter);
        slider_textures[s].setImage(flipImg); // images need to be flipped vertically
        overlay.addTexture("vertLayout_cell" + std::to_string(s + 1), &slider_textures[s]);
        dealloc_image(flipImg);

        std::string horzCellText = "option " + std::to_string(s + 1);
        Rasteron_Text textObj = { Sandbox_Demo::fontFilePath.c_str(), horzCellText.c_str(), *getFrameAt(words_queue, s)->data, RAND_COLOR()};
        words_textures[s].setTextImage(&textObj);
        overlay.addTexture("horzLayout_cell" + std::to_string(s + 1), &words_textures[s]);
    }
#endif

    // Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, flatPipeline);
    _renderer->buildScene(&overlay);
}

void Sandbox_Demo::loop(double frameTime){
    // Object Updates
    
    {
        // boxActor.updateRot({ 0.003F * (float)frameTime, 0.0F });
        // pyramidActor.updateRot({ 0.0F, 0.0F, 0.003F * (float)frameTime });
        // sphereActor.updateRot({ -0.003F * (float)frameTime, -0.003F * (float)frameTime, -0.003F * (float)frameTime });
        // hexActor.updateRot({ 0.0F, 0.0F, 0.003F * (float)frameTime });

#ifdef RASTERON_H
        if(_renderer->getFrameCount() % 10 == 0 || Platform::mouseControl.getIsMouseDown().second){
            _renderer->texturize(&scene);
            _renderer->texturize(&overlay);
        }
#endif
    }

    // Capture Renders

    if(Platform::mouseControl.getIsMouseDown().second) {
        flatVShader.setMode(FLAT_MODE_SOLID); 

        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, flatPipeline);
        // flatVShader.setMode(2);
        _renderer->updateScene(&scene);
        _renderer->renderScene(&scene);

        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, flatPipeline);
        _renderer->updateScene(&overlay);
        _renderer->renderScene(&overlay);
#ifdef RASTERON_H
        colorPicker(&scene);
        colorPicker(&overlay);
#endif
    }

    if(Platform::mouseControl.getIsMouseDown().second) {
        flatVShader.setMode(FLAT_MODE_DIRECTION); // effectVShader.setMode(1);
        Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, flatPipeline);
        _renderer->updateScene(&canvas);
        _renderer->renderScene(&canvas);

        flatVShader.setMode(FLAT_MODE_COORD); 
        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, flatPipeline);
        _renderer->updateScene(&scene);
        _renderer->renderScene(&scene);

        flatVShader.setMode(FLAT_MODE_COORD);
        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, flatPipeline);
        _renderer->updateScene(&overlay);
        _renderer->renderScene(&overlay);
#ifdef RASTERON_H
        Vec3f coordPickVec = coordPicker(&scene);
        if(coordPickVec[0] == 0.0 && coordPickVec[1] == 0.0) coordPickVec = coordPicker(&canvas);
        // coordPicker(&overlay);
#endif
        _renderer->clearView();
    }

    // Display Renders

    /* {  
        flatVShader.setMode(FLAT_MODE_DIRECTION); // effectVShader.setMode(1);
        Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, flatPipeline);
        _renderer->updateScene(&canvas);
        _renderer->renderScene(&canvas);
    } */

    {
        flatVShader.setMode((Sandbox_Demo::isShaderVariant)? Sandbox_Demo::shaderMode % 3 : 0); 
        beamVShader.setMode((Sandbox_Demo::isShaderVariant)? Sandbox_Demo::shaderMode / 2 : 0); 
        texVShader.setMode((Sandbox_Demo::isShaderVariant)? -Sandbox_Demo::shaderMode : 0);
        texVShader.setTexScroll(Sandbox_Demo::texScroll);

        Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, (Sandbox_Demo::pipelineIndex == 0)? texPipeline : (Sandbox_Demo::pipelineIndex == 1)? beamPipeline : flatPipeline);

        _renderer->setDrawMode(DRAW_Triangles);
        // Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, (isTexPipeline)? texPipeline : beamPipeline);
        _renderer->updateScene(&scene);
        _renderer->renderScene(&scene);

        _renderer->setDrawMode(DRAW_Triangles);
        // Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, (isTexPipeline)? texPipeline : beamPipeline);
        _renderer->updateScene(&overlay);
        _renderer->renderScene(&overlay);
    }
}

int main(int argc, char** argv) {
    _instance = new Sandbox_Demo(argv[0], BACKEND_DX11);

    _instance->run();
    delete(_instance);
    return 0;
}