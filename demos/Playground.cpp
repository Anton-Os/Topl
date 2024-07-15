#include "Playground.hpp"

void Playground_Demo::init(){
    modeBillboard.configure(&overlayScene);
    timeBillboard.configure(&overlayScene);
    actionsBillboard.configure(&overlayScene);
    objectsBillboard.configure(&overlayScene);
    sculptBillboard.configure(&overlayScene);
    paintBillboard.configure(&overlayScene);
    propsBillboard.configure(&overlayScene);

    std::string fontPath = std::string(FONTS_DIR) + "Tw-Cen-MT.ttf";
    _labels.push_back(Img_Label(MENU_Medium, { fontPath.c_str(), "Default", 0xFF333333, 0xFFEEEEEE }));
    modeBillboard.overlay(0, &_labels.back());
    _sliders.push_back(Img_Slider(MENU_Medium, 2));
    timeBillboard.overlay(0, &_sliders.back());
    /* for(unsigned p = 0; p < actionsBillboard.getParams()->getGridSize() - 1; p++){
        _buttons.push_back(Img_Button(MENU_Medium));
        actionsBillboard.overlay(p, &_buttons.back());
    }
    for(unsigned p = 0; p < objectsBillboard.getParams()->getGridSize() - 1; p++){
        _labels.push_back(Img_Label(MENU_Medium, { fontPath.c_str(), "X", 0xFF333333, 0xFFEEEEEE }));
        objectsBillboard.overlay(p, &_labels.back());
    }
    for(unsigned p = 0; p < sculptBillboard.getParams()->getGridSize() - 1; p++){
        _buttons.push_back(Img_Button(MENU_Medium));
        sculptBillboard.overlay(p, &_buttons.back());
    }
    for(unsigned p = 0; p < paintBillboard.getParams()->getGridSize() - 1; p++){
        _buttons.push_back(Img_Button(MENU_Medium));
        paintBillboard.overlay(p, &_buttons.back());
    }
    for(unsigned p = 0; p < propsBillboard.getParams()->getGridSize() - 1; p++){
        _dials.push_back(Img_Dial(MENU_Medium, 4));
        propsBillboard.overlay(p, &_dials.back());
    } */

    // _renderer->setPipeline(_flatPipeline);
    _renderer->buildScene(&overlayScene);
    _renderer->texturizeScene(&overlayScene);
}

void Playground_Demo::loop(double frameTime){
    _renderer->setCamera(&fixedCamera);
    _renderer->setDrawMode(DRAW_Triangles);

    _flatVShader.setMode(2);
    _renderer->setPipeline(_texPipeline);
    _renderer->updateScene(&overlayScene);
    _renderer->drawScene(&overlayScene);
}

int main(int argc, char** argv) {
    _DEMO = new Playground_Demo(argv[0], BACKEND_DX11);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}