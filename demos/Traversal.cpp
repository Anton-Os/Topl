#include "Traversal.hpp"

void Traversal_Demo::onAnyKey(char key) {
    switch (tolower(key)) {
        case 'i': for (unsigned c = 0; c < 3; c++) corridorActors[c].isShown = c == 0; break;
        case 'o': for (unsigned c = 0; c < 3; c++) corridorActors[c].isShown = c == 1; break;
        case 'p': for (unsigned c = 0; c < 3; c++) corridorActors[c].isShown = c == 2; break;
    }
}

void Traversal_Demo::init(){
    squareCorridor.tesselate(TRAVERSAL_TESS);
    hexCorridor.tesselate(TRAVERSAL_TESS);
    circleCorridor.tesselate(TRAVERSAL_TESS);

    Platform::keyControl.addHandler(std::bind(&Traversal_Demo::onAnyKey, this, std::placeholders::_1));
    Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Perspective, 10.0).genProjMatrix());

    for (unsigned c = 0; c < 3; c++)
        scene.addGeometry(std::string("corridor") + std::to_string(c + 1), &corridorActors[c]);
    _renderer->buildScene(&scene);
}

void Traversal_Demo::loop(double frameTime){
    for (unsigned c = 0; c < 3; c++) corridorActors[c].updatePos({ 0.0F, 0.0F, (float)frameTime * Topl_Program::speed * -0.0000025F });

    renderScene(&scene, nullptr, Topl_Program::shaderMode); 
}

MAIN_ENTRY {
    Traversal = new Traversal_Demo(argv[0], BACKEND_DX11);
    Traversal->run();

    delete(Traversal);
    return 0;
}