#include "Playground.hpp"


unsigned short Playground_Demo::mode = 0;
unsigned short Playground_Demo::option = 0;

void onAnyKey(char key){
    if(isdigit(key)){ 
        Playground_Demo::mode = key - '0';
    }
}

void onActionPanePress(MOUSE_Event event){
    if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
        Playground_Demo::mode = PLAYGROUND_ACTION_MENU;
        Playground_Demo::option = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;
        std::cout << "Action operation!" << std::endl;
    }
}

void onObjectPanePress(MOUSE_Event event){
    if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
        Playground_Demo::mode = PLAYGROUND_OBJECT_MENU;
        Playground_Demo::option = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;
        std::cout << "Object operation!" << std::endl;
    }
}

void onSculptPanePress(MOUSE_Event event){
    if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
        Playground_Demo::mode = PLAYGROUND_SCULPT_MENU;
        Playground_Demo::option = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;
        std::cout << "Sculpt operation!" << std::endl;
    }
}

void onPaintPanePress(MOUSE_Event event){
    if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
        Playground_Demo::mode = PLAYGROUND_PAINT_MENU;
        Playground_Demo::option = std::stoi(Topl_Program::pickerObj->getNameExt()) - 1;
        std::cout << "Paint operation!" << std::endl;
    }
}

void Playground_Demo::init(){
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

    Topl_Factory::switchPipeline(_renderer, _texPipeline);
    _renderer->buildScene(&overlayScene);
    _renderer->texturizeScene(&overlayScene);

    backdropScene.addGeometry(&backdropActor);
    _renderer->buildScene(&backdropScene);

    for(unsigned i = 1; i < 8; i++){ 
        _images.push_back(new Img_Base(checkeredImgOp({ 1024, 1024 }, { i * 2, i * 2, RAND_COLOR(), RAND_COLOR() })));
        backdropScene.addTexture(std::to_string(i), _images.back());
    }
}

void Playground_Demo::loop(double frameTime){
    updateOverlay();

    _renderer->setCamera(&fixedCamera);

    _texVShader.setMode(Playground_Demo::mode + 1);
    _renderer->setDrawMode(DRAW_Triangles);
    Topl_Factory::switchPipeline(_renderer, _texPipeline);
    _renderer->updateScene(&backdropScene);
    _renderer->drawScene(&backdropScene);

    _renderer->setDrawMode(DRAW_Triangles);
    _texVShader.setMode(0);
    Topl_Factory::switchPipeline(_renderer, _texPipeline);
    _renderer->updateScene(&overlayScene);
    _renderer->drawScene(&overlayScene);
}

void Playground_Demo::updateOverlay(){
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
    _DEMO = new Playground_Demo(argv[0], BACKEND_GL4);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}