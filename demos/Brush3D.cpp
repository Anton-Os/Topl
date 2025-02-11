#include "Brush3D.hpp"

unsigned short Brush3D_Demo::mode = 0;

void Brush3D_Demo::onAnyKey(char key){
    /* switch(tolower(key)){
        case 'h': Brush3D_Demo::mode = 0; break;
        case 'j': Brush3D_Demo::mode = 1; break;
        case 'k': Brush3D_Demo::mode = 2; break;
        case 'l': Brush3D_Demo::mode = 3; break;
    }

    if(tolower(key) == 'h' || tolower(key) == 'j' || tolower(key) == 'k' || tolower(key) == 'l')
        for(unsigned a = 0; a < 4; a++) brushes3D[a].isShown = Brush3D_Demo::mode == a; */
}

void Brush3D_Demo::init(){
    for(unsigned a = 0; a < 4; a++){
        scene.addGeometry("brush" + std::to_string(a + 1), &brushes3D[a]);
        // brushes3D[a].isShown = mode == a;
    }
    _renderer->buildScene(&scene);
}

void Brush3D_Demo::loop(double frameTime){
    _renderer->updateScene(&scene);
    _renderer->drawScene(&scene);
}

MAIN_ENTRY {
    _DEMO = new Brush3D_Demo(argv[0], BACKEND_DX11);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}
