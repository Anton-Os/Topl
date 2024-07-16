#include "Playground.hpp"


unsigned short Playground_Demo::mode = 0;
unsigned short Playground_Demo::category = 0;

void onAnyKey(char key){
    if(isdigit(key)){ 
        Playground_Demo::mode = key - '0';
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
    _sliders.push_back(new Img_Slider(MENU_Medium, 2));
    timeBillboard.overlay(0, _sliders.back());
    for(unsigned p = 0; p < objectsBillboard.getParams()->getGridSize(); p++){
        std::string numberText = "[" + std::to_string(p + 1) + "]";
        _labels.push_back(new Img_Label(MENU_Medium, { fontPath.c_str(), numberText.c_str(), 0xFF333333, 0xFFEEEEEE }));
        objectsBillboard.overlay(p, _labels.back());
    }
    for(unsigned p = 0; p < actionsBillboard.getParams()->getGridSize(); p++){
        _buttons.push_back(new Img_Button(MENU_Medium, "sort"));
        actionsBillboard.overlay(p, _buttons.back());
    }
    for(unsigned p = 0; p < sculptBillboard.getParams()->getGridSize(); p++){
        _buttons.push_back(new Img_Button(MENU_Medium, "sort"));
        sculptBillboard.overlay(p, _buttons.back());
    }
    for(unsigned p = 0; p < paintBillboard.getParams()->getGridSize(); p++){
        _buttons.push_back(new Img_Button(MENU_Medium, "sort"));
        paintBillboard.overlay(p, _buttons.back());
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
}

void Playground_Demo::loop(double frameTime){
    if(Platform::mouseControl.getIsMouseDown().second){ 
        colorPicker(&overlayScene);
    }

    _renderer->setCamera(&fixedCamera);
    _renderer->setDrawMode(DRAW_Triangles);

    _effectVShader.setMode(EFFECT_MODE_FRACTALS);
    Topl_Factory::switchPipeline(_renderer, _effectPipeline);
    _renderer->updateScene(&backdropScene);
    _renderer->drawScene(&backdropScene);

    // _flatVShader.setMode(2);
    // _texVShader.setMode(9);
    Topl_Factory::switchPipeline(_renderer, _texPipeline);
    _renderer->updateScene(&overlayScene);
    _renderer->drawScene(&overlayScene);
}

int main(int argc, char** argv) {
    _DEMO = new Playground_Demo(argv[0], BACKEND_GL4);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}