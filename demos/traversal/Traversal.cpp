#include "Traversal.hpp"

static float speed = TRAVERSAL_SPEED;

void Traversal_Demo::onAnyKey(char key) {
    switch (tolower(key)) {
        case 'i': speed = TRAVERSAL_SPEED; break;
        case 'o': speed *= 10.0F; break;
        case 'p': speed /= 10.0F; break;
    }
}

void Traversal_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex) {
    if (menu == PROGRAM_AppBar){
        for (unsigned c = 0; c < TRAVERSAL_CORRIDORS; c++)
            for (unsigned r = 0; r < TRAVERSAL_RECURSION; r++)
                corridorActors[c][r].isShown = paneIndex / 3 == c && paneIndex % 3 == r % 3;
    }
}

void Traversal_Demo::init(){
    speed = TRAVERSAL_SPEED;

    Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Perspective, 2.0F).genProjMatrix());

    Platform::keyControl.addHandler(std::bind(&Traversal_Demo::onAnyKey, this, std::placeholders::_1));
    // Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Perspective, 10.0).genProjMatrix());

    for (unsigned c = 0; c < TRAVERSAL_CORRIDORS; c++) {
        if (c > 0) {
            squareCorridors[c].tesselate(c);
            hexCorridors[c].tesselate(c);
            circleCorridors[c].tesselate(c);
        }
        for (unsigned r = 0; r < TRAVERSAL_RECURSION; r++) {
            std::string corridorName = std::string("corridor") + std::to_string(c + 1) + "_" + std::to_string(r + 1);
            scene.addGeometry(corridorName, &corridorActors[c][r]);

            float scale = TRAVERSAL_RADIUS - ((1.0F / TRAVERSAL_RECURSION) * r * TRAVERSAL_RADIUS);
            corridorActors[c][r].setSize({ scale, scale, scale });
            // corridorActors[c][r].setRot({ ((float)MATH_HALF_PI / TRAVERSAL_RECURSION) * (float)r, 0.0F, 0.0F });
        }
    }
    _renderer->buildScene(&scene);
}

void Traversal_Demo::loop(double frameTime){
    static double totalTime = 0.0;

    Topl_Program::camera.setPos({ 0.0F, 0.0F, sinf((float)totalTime * speed) * TRAVERSAL_DEPTH });

    for (unsigned c = 0; c < TRAVERSAL_CORRIDORS; c++)
        for (unsigned r = 0; r < TRAVERSAL_RECURSION; r++) {
            float t = sinf((float)totalTime * speed * r);
            corridorActors[c][r].setPos({ 0.0F, 0.0F, t * TRAVERSAL_DEPTH * 2 });
            // corridorActors[c][r].updateRot({ (frameTime * speed) / (r % 2 == 0) ? (float)r : (float)-r, 0.0F, 0.0F });
            corridorActors[c][r].setRot({ t * (float)MATH_PI * (r % 2 == 0) ? (float)2.0F : (float)-2.0F, 0.0F, 0.0F });
            float scale = TRAVERSAL_RADIUS - ((1.0F / TRAVERSAL_RECURSION) * r * TRAVERSAL_RADIUS);
            corridorActors[c][r].setSize({ (float)pow(abs(t), 0.5) * scale, (float)pow(abs(t), 0.5) * scale, scale });
        }

    // squareCorridor.drawMin = _renderer->getFrameCount() % squareCorridor.getVertexCount();
    // hexCorridor.drawMin = _renderer->getFrameCount() % hexCorridor.getVertexCount();
    // circleCorridor.drawMin = _renderer->getFrameCount() % circleCorridor.getVertexCount();

    renderScene(&scene);

    totalTime += frameTime;
}

MAIN_ENTRY{
    Traversal = new Traversal_Demo(argv[0], BACKEND_GL4);
    Traversal->run();

    delete(Traversal);
    return 0;
}