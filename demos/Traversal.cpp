#include "Traversal.hpp"

static float speed = TRAVERSAL_SPEED;

void Traversal_Demo::onAnyKey(char key) {
    switch (tolower(key)) {
        case 'i': speed = TRAVERSAL_SPEED; break;
        case 'o': speed = TRAVERSAL_SPEED * 10; break;
        case 'p': speed = TRAVERSAL_SPEED * 100; break;
    }
}

void Traversal_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex) {
    if (menu == PROGRAM_AppBar){
        for (unsigned c = 0; c < TRAVERSAL_CORRIDORS; c++)
            for (unsigned r = 0; r < TRAVERSAL_RECURSION; r++)
                corridorActors[c][r].isShown = paneIndex / 3 == c && r < TRAVERSAL_RECURSION - (paneIndex % 3);
    }
}

void Traversal_Demo::init(){
    speed = TRAVERSAL_SPEED;

    squareCorridor.tesselate(TRAVERSAL_TESS);
    hexCorridor.tesselate(TRAVERSAL_TESS);
    circleCorridor.tesselate(TRAVERSAL_TESS);

    Platform::keyControl.addHandler(std::bind(&Traversal_Demo::onAnyKey, this, std::placeholders::_1));
    // Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Perspective, 10.0).genProjMatrix());

    for(unsigned c = 0; c < TRAVERSAL_CORRIDORS; c++)
        for (unsigned r = 0; r < TRAVERSAL_RECURSION; r++) {
            std::string corridorName = std::string("corridor") + std::to_string(c + 1) + "_" + std::to_string(r + 1);
            scene.addGeometry(corridorName, &corridorActors[c][r]);

            float scale = TRAVERSAL_RADIUS - ((1.0F / TRAVERSAL_RECURSION) * r * TRAVERSAL_RADIUS);
            corridorActors[c][r].setSize({ scale, scale, scale });
            // corridorActors[c][r].setRot({ ((float)MATH_HALF_PI / TRAVERSAL_RECURSION) * (float)r, 0.0F, 0.0F });
        }
    _renderer->buildScene(&scene);
}

void Traversal_Demo::loop(double frameTime){
    for (unsigned c = 0; c < TRAVERSAL_CORRIDORS; c++)
        for (unsigned r = 1; r < TRAVERSAL_RECURSION; r++) {
            corridorActors[c][r].setPos({ 0.0F, 0.0F, sinf(frameTime * speed * r) * TRAVERSAL_DEPTH });
            if(_renderer->getFrameCount() % 10 == 0)
                corridorActors[c][r].updateRot({ (frameTime * speed * 0.000001F) / (r % 2 == 0)? (float)r : (float)-r, 0.0F, 0.0F});
        }

    // squareCorridor.drawMin = _renderer->getFrameCount() % squareCorridor.getVertexCount();
    // hexCorridor.drawMin = _renderer->getFrameCount() % hexCorridor.getVertexCount();
    // circleCorridor.drawMin = _renderer->getFrameCount() % circleCorridor.getVertexCount();

    renderScene(&scene, nullptr, Topl_Program::shaderMode);

    // TFORM_Type tformType = (mode % 3 == 0)? TFORM_Scale : (mode % 3 == 1)? TFORM_Rotate : TFORM_Shear;
    // for(unsigned a = 0; a < 3; a++) renderRecursive(&corridorActors[a], tformType, TRAVERSAL_RECURSION);
}

MAIN_ENTRY{
    Traversal = new Traversal_Demo(argv[0], BACKEND_GL4);
    Traversal->run();

    delete(Traversal);
    return 0;
}