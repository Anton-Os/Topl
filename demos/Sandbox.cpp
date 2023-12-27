#include "Sandbox.hpp"


static int mode = SANDBOX_MODE_MOVE;
// static bool isTexPipeline = true;
static int pipelineIndex = 0;
static bool isShaderModeVary = false;
static unsigned shaderMode = 0;
static Vec3f texScroll = { 0.0, 0.0, 0.0 };

#ifdef RASTERON_H
unsigned boxImgOp(double x, double y){ return (sin(x * 20) > 0.5)? 0xFF0000FF : 0xFFFFFF00; }
unsigned pyramidImgOp(double x, double y){ return (cos(y * 20) < 0.5)? 0xFFFF0000 : 0xFF00FFFF; }
unsigned sphereImgOp(double x, double y){ return (tan(x * y) > 0.25 && tan(x * y) < 0.75)? 0xFF8833CC : 0xFF88CC33; }
unsigned hexImgOp(double x, double y){ return (x > 0.4 && x < 0.6 && y > 0.4 && y < 0.6)? 0xFF3333333 : 0xFFEEEEEE; }

// Rasteron_Image* selectImg = noiseImgOp_white({1024, 1024}, 0x33000000, 0x3300FF00);
Rasteron_Image* boxImg = mapImgOp({1024, 1024}, boxImgOp); Rasteron_Image* pyramidImg = mapImgOp({1024, 1024}, pyramidImgOp);
Rasteron_Image* sphereImg = mapImgOp({1024, 1024}, sphereImgOp); Rasteron_Image* hexImg = mapImgOp({1024, 1024}, hexImgOp);

Img_Base boxTex = Img_Base(boxImg); Img_Base pyramidTex = Img_Base(pyramidImg);
Img_Base sphereTex = Img_Base(sphereImg); Img_Base hexTex = Img_Base(hexImg);

Rasteron_Queue* dialBtn_queues[3] = { loadUI_dial(MENU_XL, 4), loadUI_dial(MENU_XL, 8), loadUI_dial(MENU_XL, 16) };
Img_Base dialBtn_textures[3] = { getFrameAt(dialBtn_queues[0], 3), getFrameAt(dialBtn_queues[1], 0), getFrameAt(dialBtn_queues[2], 8) };
Rasteron_Queue* iconBtn_queues[3] = { loadUI_iconBtn(MENU_XL, "arrow_back"), loadUI_iconBtn(MENU_XL, "sync"), loadUI_iconBtn(MENU_XL, "arrow_forward") };
Img_Base iconBtn_textures[3] = { getFrameAt(iconBtn_queues[0], 0), getFrameAt(iconBtn_queues[1], 1), getFrameAt(iconBtn_queues[2], 2) };
Rasteron_Queue* checkBtn_queue = loadUI_checkBtn(MENU_XL);
Img_Base checkBtn_textures[3] = { getFrameAt(checkBtn_queue, 0), getFrameAt(checkBtn_queue, 1), getFrameAt(checkBtn_queue, 2) };

// Rasteron_Queue* slider_queues[9] = { loadUI_slider(MENU_XL, 2), loadUI_slider(MENU_XL, 3), loadUI_slider(MENU_XL, 4), loadUI_slider(MENU_XL, 5), loadUI_slider(MENU_XL, 6), loadUI_slider(MENU_XL, 7), loadUI_slider(MENU_XL, 8), loadUI_slider(MENU_XL, 9), loadUI_slider(MENU_XL, 10) };
Rasteron_Queue* slider_queue = loadUI_slider(MENU_XL, 9);
Img_Base slider_textures[9] = { getFrameAt(slider_queue, 0), getFrameAt(slider_queue, 1), getFrameAt(slider_queue, 2), getFrameAt(slider_queue, 3), getFrameAt(slider_queue, 4), getFrameAt(slider_queue, 5), getFrameAt(slider_queue, 6), getFrameAt(slider_queue, 7), getFrameAt(slider_queue, 8) };
Rasteron_Queue* words_queue = alloc_queue("words", { 64, 512 }, 9);
Img_Base words_textures[9] = { getFrameAt(words_queue, 0), getFrameAt(words_queue, 1), getFrameAt(words_queue, 2), getFrameAt(words_queue, 3), getFrameAt(words_queue, 4), getFrameAt(words_queue, 5), getFrameAt(words_queue, 6), getFrameAt(words_queue, 7), getFrameAt(words_queue, 8) };
#endif

static void updateActor(Geo_Actor* actor){
    std::cout << "Tracer steps: " << std::to_string(Platform::mouseControl.getTracerSteps()->size()) << ", Last Step: " << std::to_string(Platform::mouseControl.getTracerSteps()->back().step.first) << ", " << std::to_string(Platform::mouseControl.getTracerSteps()->back().step.second) << std::endl;
    if(!Platform::mouseControl.getTracerPaths()->empty()){
        auto lastPath = Platform::mouseControl.getTracerPaths()->back();
        std::cout << "Tracer paths: " << std::to_string(lastPath.stepsCount) << /* ", Last Path: " << std::to_string(lastPath.steps[lastPath.stepsCount - 1].first) << ", " << std::to_string(lastPath.steps[lastPath.stepsCount - 1].second) << */ std::endl;
    }

    switch(mode){
        case SANDBOX_MODE_HIDE: break; // TODO: hide actor
        case SANDBOX_MODE_MOVE: actor->setPos(Topl_Program::getCamCursorPos()); break;
        case SANDBOX_MODE_ROTATE: break; // TODO: rotate actor
        case SANDBOX_MODE_SCALE: break; // TODO: scale actor
        /* case SANDBOX_MODE_REWIND: break; // TODO: rewind time
        case SANDBOX_MODE_PLAYPAUSE: break; // TODO: play or pause time
        case SANDBOX_MODE_FORWARD: break; // TODO: fast forward time
        case SANDBOX_MODE_PAN: break; // TODO: pan scene
        case SANDBOX_MODE_PIVOT: break; // TODO: rotate scene
        case SANDBOX_MODE_ZOOM: break; // TODO: zoom scene */
        default: actor->setPos(Topl_Program::getCamCursorPos());
    }
}

static void onAnyKey(char k){ 
    if(isdigit(k)){ 
        mode = k - '0'; // conversion from char to decimal
        std::cout << "Key mode: " << std::to_string(mode) << std::endl;
    } 
    else if(toupper(k) == 'W') Topl_Program::cameraObj.updatePos({ 0.0, 0.1F, 0.0 });
    else if(toupper(k) == 'S') Topl_Program::cameraObj.updatePos({ 0.0, -0.1F, 0.0 });
    else if(toupper(k) == 'A') Topl_Program::cameraObj.updatePos({ -0.1F, 0.0, 0.0 });
    else if(toupper(k) == 'D') Topl_Program::cameraObj.updatePos({ 0.1F, 0.0, 0.0 });
    else if(toupper(k) == 'Q') Topl_Program::cameraObj.updateRot({ -0.1F, 0.0, 0.0 });
    else if(toupper(k) == 'E') Topl_Program::cameraObj.updateRot({ 0.1F, 0.0, 0.0 });
    else if(toupper(k) == 'Z') Topl_Program::cameraObj.setZoom(*Topl_Program::cameraObj.getZoom() + 0.1);
    else if(toupper(k) == 'C') Topl_Program::cameraObj.setZoom(*Topl_Program::cameraObj.getZoom() - 0.1);
    else if(k == 0x25) texScroll[0] -= 0.05f; // left arrow
    else if(k == 0x27) texScroll[0] += 0.05f; // right arrow
    else if(k == 0x26) texScroll[1] -= 0.05f; // up arrow
    else if(k == 0x28) texScroll[1] += 0.05f; // down arrow
    else if(toupper(k) == 'X') { isShaderModeVary = !isShaderModeVary; }
    else if(isspace(k)) (pipelineIndex < 2)? pipelineIndex++ : pipelineIndex = 0;
}

static void onHover(float x, float y){ 
    if(Platform::mouseControl.getIsMouseDown().second) {
        Topl_Program::cursorPos = { x, y, 0.0F }; 

        if(Topl_Program::pickerObj != NO_PICKER_OBJ){
            std::cout << "Topl_Program picker object: " << Topl_Program::pickerObj->getName() << std::endl;

            if(Topl_Program::pickerObj->getId() == _instance->boxActor.getId()) {
                updateActor(&_instance->boxActor);
                boxTex.setColorImage(0xAA00FF00);
            } else if(Topl_Program::pickerObj->getId() == _instance->pyramidActor.getId()){
                updateActor(&_instance->pyramidActor);
                pyramidTex.setColorImage(0xAA00FF00);
            } else if(Topl_Program::pickerObj->getId() == _instance->sphereActor.getId()){
                updateActor(&_instance->sphereActor);
                sphereTex.setColorImage(0xAA00FF00);
            } else if(Topl_Program::pickerObj->getId() == _instance->hexActor.getId()){ 
                updateActor(&_instance->hexActor);
                hexTex.setColorImage(0xAA00FF00);
            }
            // else if(isLayoutSelect) layoutVec = layoutVec + (Topl_Program::cursorPos - layoutVec);

            // TODO: Handle GUI Events
        }
    } else {
        Topl_Program::pickerObj = NO_PICKER_OBJ;
        boxTex.setImage(boxImg); pyramidTex.setImage(pyramidImg);
        sphereTex.setImage(sphereImg); hexTex.setImage(hexImg);

        // TODO: Handle Camera Events
    }
}

void shaderModeCycle(){ (shaderMode < 8)? shaderMode++ : shaderMode = 0; }
void texScrollCycle(){ /* texScroll[0] += 0.05; */ }

static void default_shadercall(Topl_EntryShader* shader){ 
    /* Add body for render block */ 
}
static void gridCell_pickercall(Geo_Actor* actor){
    int c = (char)(actor->getName().back()) - '0';
    if(c == 1 || c == 2 || c == 3)
        dialBtn_textures[c - 1].setImage(getFrameAt(dialBtn_queues[c - 1], (dialBtn_queues[c - 1]->index + 1 < dialBtn_queues[c - 1]->frameCount)? dialBtn_queues[c - 1]->index + 1 : 0));
    else if(c == 4 || c == 5 || c == 6)
        iconBtn_textures[c - 4].setImage(getFrameAt(iconBtn_queues[c - 4], (iconBtn_queues[c - 4]->index + 1 < 3)? iconBtn_queues[c - 4]->index + 1 : 0));
    if(c == 7 || c == 8 || c == 9)
        checkBtn_textures[c - 7].setImage(getFrameAt(checkBtn_queue, 0));
}

void Sandbox_Demo::init(){
    srand(time(NULL));

    Platform::keyControl.addAnyCallback(onAnyKey);
    // Platform::mouseControl.addCallback(MOUSE_LeftBtn_Down, onPress);
    // Platform::mouseControl.addCallback(MOUSE_RightBtn_Down, onPress);
    Platform::mouseControl.addHoverCallback(onHover);

    _timeline.ticker.addPeriodicEvent(2500, shaderModeCycle);
    _timeline.ticker.addPeriodicEvent(50, texScrollCycle);

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

#ifdef RASTERON_H
    scene.addTexture("Box", &boxTex);
    scene.addTexture("Pyramid", &pyramidTex);
    scene.addTexture("Sphere", &sphereTex);
    scene.addTexture("Hex", &hexTex);
#endif

    // chain.configure(&scene);
    // grid.configure(&scene);

    Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, texPipeline);
    _renderer->buildScene(&scene);
    // Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, texPipeline);
    // _renderer->buildScene(&details);
    
    layout1.configure(&overlay);
    layout2.configure(&overlay);
    layout2.shift({ 0.55, 0.0, 0.0 });
    layout3.configure(&overlay);
    layout3.shift({ -0.55, 0.0, 0.0 });

    Geo_Actor* gridActors[9] = { layout1.getGeoActor(0), layout1.getGeoActor(1), layout1.getGeoActor(2), layout1.getGeoActor(3), layout1.getGeoActor(4), layout1.getGeoActor(5), layout1.getGeoActor(6), layout1.getGeoActor(7), layout1.getGeoActor(8), };
    for(unsigned g = 0; g < 9; g++){ gridActors[g]->pickerFunc = gridCell_pickercall; }

#ifdef RASTERON_H
    overlay.addTexture("gridLayout_cell1", &dialBtn_textures[0]);
    overlay.addTexture("gridLayout_cell2", &dialBtn_textures[1]);
    overlay.addTexture("gridLayout_cell3", &dialBtn_textures[2]);
    overlay.addTexture("gridLayout_cell4", &iconBtn_textures[0]);
    overlay.addTexture("gridLayout_cell5", &iconBtn_textures[1]);
    overlay.addTexture("gridLayout_cell6", &iconBtn_textures[2]);
    overlay.addTexture("gridLayout_cell7", &checkBtn_textures[0]);
    overlay.addTexture("gridLayout_cell8", &checkBtn_textures[1]);
    overlay.addTexture("gridLayout_cell9", &checkBtn_textures[2]);

    for(unsigned s = 0; s < 9; s++){
        Rasteron_Image* flipImg = flipImgOp(slider_textures[s].getImage(), FLIP_Counter);
        slider_textures[s].setImage(flipImg); // images need to be flipped vertically
        overlay.addTexture("vertLayout_cell" + std::to_string(s + 1), &slider_textures[s]);
        dealloc_image(flipImg);

        std::string fontFilePath = std::string(FONTS_DIR) + "MajorMonoDisplay-Regular.ttf";
		std::replace(fontFilePath.begin(), fontFilePath.end(), '/', '\\');
        std::string horzCellText = "option " + std::to_string(s + 1);
        Rasteron_Text textObj = { fontFilePath.c_str(), horzCellText.c_str(), *getFrameAt(words_queue, s)->data, RAND_COLOR()};
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
        boxActor.updateRot({ 0.003F * (float)frameTime, 0.0F });
        pyramidActor.updateRot({ 0.0F, 0.0F, 0.003F * (float)frameTime });
        sphereActor.updateRot({ -0.003F * (float)frameTime, -0.003F * (float)frameTime, -0.003F * (float)frameTime });
        hexActor.updateRot({ 0.0F, 0.0F, 0.003F * (float)frameTime });

        // layout.shift(layoutVec - layout.getOrigin());
    }

    // Flat Render

    {
        flatVShader.setMode(0); 

        _renderer->setDrawMode(DRAW_Strip);
        Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, flatPipeline);
        // flatVShader.setMode(2);
        _renderer->updateScene(&scene);
        _renderer->renderScene(&scene);

        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, flatPipeline);
        _renderer->updateScene(&overlay);
        _renderer->renderScene(&overlay);

#ifdef RASTERON_H
        if(Platform::mouseControl.getIsMouseDown().second) {
            invokePicker(&scene);
            _renderer->texturize(&scene);
            invokePicker(&overlay);
            _renderer->texturize(&overlay);
        }
#endif
    }

    // Textured Render

    _renderer->clearView();

    {  
        effectVShader.setMode(1);
        Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, effectPipeline);
        _renderer->updateScene(&canvas);
        _renderer->renderScene(&canvas);
    }

    {
        flatVShader.setMode((isShaderModeVary)? shaderMode % 3 : 0); 
        beamVShader.setMode((isShaderModeVary)? shaderMode / 2 : 0); 
        texVShader.setMode((isShaderModeVary)? -shaderMode : 0);
        texVShader.setTexScroll(texScroll);

        Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, (pipelineIndex == 0)? texPipeline : (pipelineIndex == 1)? beamPipeline : flatPipeline);

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
    return 0;
}