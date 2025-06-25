#include "Traversal.hpp"

static unsigned sculptIndex = 0;

void Traversal_Demo::init(){
    Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Perspective, TRAVERSAL_DEPTH).genProjMatrix());

    for (unsigned c = 0; c < 3; c++)
        scene.addGeometry(std::string("corridor") + std::to_string(c + 1), &corridorActors[c]);
    _renderer->buildScene(&scene);
}

void Traversal_Demo::loop(double frameTime){
    // Topl_Program::camera.updatePos({ 0.0F, 0.0F, (float)frameTime * Topl_Program::speed * 0.001F });

    renderScene(&scene, nullptr, Topl_Program::shaderMode); 
}

MAIN_ENTRY {
    Traversal = new Traversal_Demo(argv[0], BACKEND_DX11);
    Traversal->run();

    delete(Traversal);
    return 0;
}