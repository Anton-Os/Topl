#include "Sandbox.hpp"

unsigned short Sandbox_Demo::mode = 0;
unsigned short Sandbox_Demo::option = 0;

static bool isRebuildReq = false;
static std::vector<Vec3f> sculptPoints;
static bool isRepaintReq = false;
static Img_Canvas canvasImg = Img_Canvas(RAND_COLOR());

void onAnyKey(char key){
    if(isdigit(key)){ Sandbox_Demo::mode = key - '0'; }
}

void onActionPanePress(MOUSE_Event event){
    if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
        Sandbox_Demo::mode = SANDBOX_ACTION_MENU;
        Sandbox_Demo::option = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;
    }
}

void onObjectPanePress(MOUSE_Event event){
    if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
        Sandbox_Demo::mode = SANDBOX_OBJECT_MENU;
        Sandbox_Demo::option = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;
    }
}

void onSculptPanePress(MOUSE_Event event){
    if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
        Sandbox_Demo::mode = SANDBOX_SCULPT_MENU;
        Sandbox_Demo::option = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;
    }
}

void onPaintPanePress(MOUSE_Event event){
    if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
        Sandbox_Demo::mode = SANDBOX_PAINT_MENU;
        Sandbox_Demo::option = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;
    }
}

void onEditAction(float x, float y){
    switch(Sandbox_Demo::mode){
        case SANDBOX_ACTION_MENU: std::cout << "Action operation!" << std::endl; break;
        case SANDBOX_OBJECT_MENU: std::cout << "Objects operation!" << std::endl; break;
        case SANDBOX_SCULPT_MENU:
            sculptPoints.push_back({ Platform::getCursorX(), Platform::getCursorY(), 0.0F });
            std::cout << "Sculpt operation!" << std::endl; break;
        case SANDBOX_PAINT_MENU: 
            canvasImg.draw(0.1F, { Platform::getCursorX(), Platform::getCursorY() }, RAND_COLOR());
            std::cout << "Paint operation!" << std::endl; break;
        default: std::cout << "Unknown operation" << std::endl;
    }
}

void sculptFinish(){
    if(!sculptPoints.empty()) {
        _DEMO->objectMeshes.push_back(new Geo_Mesh(sculptPoints.data(), sculptPoints.size()));
        _DEMO->objectTextures.push_back(nullptr); // non-textured object
        _DEMO->objectPhysics.push_back(nullptr); // non-dynamic object
        _DEMO->objectActors.push_back(Geo_Actor(_DEMO->objectMeshes.back()));
        isRebuildReq = true;
    }
}

void paintFinish(){
    if(Topl_Program::pickerObj != nullptr && !_DEMO->objectTextures.empty()){
        unsigned short index = 0; // TODO: Get coorect index
        if(_DEMO->objectTextures[index] == nullptr) _DEMO->objectTextures[index] = new Img_Base(RAND_COLOR());
        _DEMO->objectTextures[index]->setImage(canvasImg.getImage());
        isRepaintReq = true;
    }
}

void Sandbox_Demo::init(){
    Platform::keyControl.addAnyCallback(onAnyKey);
    Platform::keyControl.addCallback('g', sculptFinish);
    Platform::keyControl.addCallback('h', paintFinish);
    Platform::mouseControl.addCallback(MOUSE_LeftBtn_Release, onEditAction);

    modeBillboard.configure(&overlayScene);
    timeBillboard.configure(&overlayScene);
    actionsBillboard.configure(&overlayScene);
    objectsBillboard.configure(&overlayScene);
    sculptBillboard.configure(&overlayScene);
    paintBillboard.configure(&overlayScene);
    propsBillboard.configure(&overlayScene);

    std::string fontPath = std::string(FONTS_DIR) + "Tw-Cen-MT.ttf";
    _labels.push_back(new Img_Label(MENU_Medium, { fontPath.c_str(), "Default", 0xFF333333, 0xFFEEEEEE }));
    modeBillboard.overlay(0, _labels.back());
    modeBillboard.toggleShow(false);
    _sliders.push_back(new Img_Slider(MENU_Medium, 2));
    timeBillboard.overlay(0, _sliders.back());
    for(unsigned p = 0; p < objectsBillboard.getParams()->getGridSize(); p++){
        std::string numberText = "[" + std::to_string(p + 1) + "]";
        _labels.push_back(new Img_Label(MENU_Medium, { fontPath.c_str(), numberText.c_str(), 0xFF333333, 0xFFEEEEEE }));
        objectsBillboard.overlay(p, _labels.back());
        objectsBillboard.getGeoActor(p)->pickerFunc = onObjectPanePress;
        objectsBillboard.getGeoActor(p)->updateSize({ -0.25F, -0.05F, 0.0F });
    }
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

    editorChain.configure(&editsScene);
    editorGrid.configure(&editsScene);

    Topl_Factory::switchPipeline(_renderer, _texPipeline);
    _renderer->buildScene(&overlayScene);
    _renderer->texturizeScene(&overlayScene);

    backdropScene.addGeometry(&backdropActor);
    _renderer->buildScene(&backdropScene);

    for(unsigned i = 1; i < 8; i++){ 
        _images.push_back(new Img_Base(checkeredImgOp({ 1024, 1024 }, { i * 2, i * 2, RAND_COLOR(), RAND_COLOR() })));
        backdropScene.addTexture(std::to_string(i), _images.back());
    }

    _renderer->buildScene(&editsScene);
}

void Sandbox_Demo::loop(double frameTime){
    updateOverlay();

    _renderer->setCamera(&fixedCamera);

    _texVShader.setMode(Sandbox_Demo::mode + 1);
    _effectVShader.setMode(Sandbox_Demo::mode);
    _renderer->setDrawMode(DRAW_Triangles);
    Topl_Factory::switchPipeline(_renderer, _effectPipeline);
    _renderer->updateScene(&backdropScene);
    _renderer->drawScene(&backdropScene);

    _renderer->setDrawMode(DRAW_Triangles);
    _texVShader.setMode(0);
    Topl_Factory::switchPipeline(_renderer, _texPipeline);
    _renderer->updateScene(&overlayScene);
    _renderer->drawScene(&overlayScene);

    Topl_Factory::switchPipeline(_renderer, _flatPipeline);
    _renderer->updateScene(&editsScene);
    _renderer->drawScene(&editsScene);

    if(isRebuildReq){
        for(unsigned short g = mainScene.getActorCount(); g < objectActors.size(); g++) // Add physics
            mainScene.addGeometry("object" + std::to_string(g), &objectActors[g]);

        if(mainScene.getActorCount() > 0) _renderer->buildScene(&mainScene);
        isRebuildReq = false;
    }

    if(isRepaintReq){
        for(unsigned short i = 0; i < objectTextures.size(); i++)
            if(objectTextures[i] != nullptr) mainScene.addTexture("object" + std::to_string(i), objectTextures[i]);

        if(mainScene.getIsTextured()) _renderer->texturizeScene(&mainScene);
        isRepaintReq = false;
    }

    // _renderer->updateScene(&mainScene);
    // _renderer->drawScene(&mainScene);
}

void Sandbox_Demo::updateOverlay(){
    static bool texturizeReq = false;

    colorPicker(&overlayScene);

    if(Topl_Program::pickerObj != nullptr){
        unsigned short index = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;

        if(Topl_Program::pickerObj->getName().find("actions_board") != std::string::npos)
            actionsBillboard.setState(index, Platform::mouseControl.getIsMouseDown().second);
        if(Topl_Program::pickerObj->getName().find("objects_board") != std::string::npos)
            objectsBillboard.setState(index, Platform::mouseControl.getIsMouseDown().second);
        if(Topl_Program::pickerObj->getName().find("sculpt_board") != std::string::npos)
            sculptBillboard.setState(index, Platform::mouseControl.getIsMouseDown().second);
        if(Topl_Program::pickerObj->getName().find("paint_board") != std::string::npos)
            paintBillboard.setState(index, Platform::mouseControl.getIsMouseDown().second);

        _renderer->texturizeScene(&overlayScene);
        _renderer->texturizeScene(&backdropScene);
    } else {
        actionsBillboard.resetState(); 
        objectsBillboard.resetState();
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