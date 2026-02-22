#include "Penscape.hpp"

static DRAW_Mode drawMode = DRAW_Triangles;

void Penscape_Demo::createBrushes() {
    trigMeshBrushes[2]->scale({ 1.0F, 1.0F, BRUSH3D_SCALE });
    quadMeshBrushes[2]->scale({ 1.0F, 1.0F, BRUSH3D_SCALE });
    // hexMeshBrushes[2]->scale({ 1.0F, 1.0F, BRUSH3D_SCALE });
    circleMeshBrushes[2]->scale({ 1.0F, 1.0F, BRUSH3D_SCALE });

    for (unsigned b = 0; b < BRUSH3D_SEGMENTS; b++)
        for (unsigned a = 0; a < 3 * 2; a++) {
            trigMeshBrushes[a % 3]->shift(getRandTransform(BRUSH3D_SHIFT)); // { 0.0F, 0.0F, BRUSH3D_SHIFT});
            trigMeshBrushes[a % 3]->rotate({ BRUSH3D_ROT, BRUSH3D_ROT, BRUSH3D_ROT });
            trigMeshes[a % 3]->extend(*trigMeshBrushes[a % 3]);
            quadMeshBrushes[a % 3]->shift(getRandTransform(BRUSH3D_SHIFT)); // { 0.0F, 0.0F, BRUSH3D_SHIFT});
            quadMeshBrushes[a % 3]->rotate({ -BRUSH3D_ROT, BRUSH3D_ROT, BRUSH3D_ROT });
            quadMeshes[a % 3]->extend(*quadMeshBrushes[a % 3]);
            /* hexMeshBrushes[a % 3]->shift(getRandTransform(BRUSH3D_SHIFT)); // { 0.0F, 0.0F, BRUSH3D_SHIFT});
            hexMeshBrushes[a % 3]->rotate({ BRUSH3D_ROT, -BRUSH3D_ROT, BRUSH3D_ROT });
            hexMeshes[a % 3]->extend(*hexMeshBrushes[a % 3]); */
            circleMeshBrushes[a % 3]->shift(getRandTransform(BRUSH3D_SHIFT)); // { 0.0F, 0.0F, BRUSH3D_SHIFT});
            circleMeshBrushes[a % 3]->rotate({ BRUSH3D_ROT, BRUSH3D_ROT, -BRUSH3D_ROT });
            circleMeshes[a % 3]->extend(*circleMeshBrushes[a % 3]);
        }
}

void Penscape_Demo::onAnyKey(char key){
    for (unsigned a = 0; a < 3; a++)
        for (unsigned s = 0; s < 3; s++)
            switch(tolower(key)) {
                case 'i': brushes3D[a][s].isShown = a == 0; break;
                case 'o': brushes3D[a][s].isShown = a == 1; break;
                case 'p': brushes3D[a][s].isShown = a == 2; break;
            }
}

void Penscape_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex){
    if(menu == PROGRAM_Sculpt)
        for(unsigned a = 0; a < 3; a++)
            for(unsigned s = 0; s < 3; s++)
                brushes3D[a][s].isShown = Penscape_Demo::mode == (a * 3) + (s % 3);
}

void Penscape_Demo::init(){
    Platform::keyControl.addHandler(std::bind(&Penscape_Demo::onAnyKey, this, std::placeholders::_1));

    Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Perspective, 5.0F).genProjMatrix());

    createBrushes();
    for(unsigned a = 0; a < 3; a++){
        brushes3D[a][0] = Geo_Actor(trigMeshes[a]);
        brushes3D[a][1] = Geo_Actor(quadMeshes[a]);
        // brushes3D[a][2] = Geo_Actor(hexMeshes[a]);
        brushes3D[a][2] = Geo_Actor(circleMeshes[a]);
    }

    for(unsigned a = 0; a < 3; a++)
        for(unsigned s = 0; s < 3; s++){
            scene.addGeometry("brush" + std::to_string(a + 1) + "_" + std::to_string(s + 1), &brushes3D[a][s]);
            brushes3D[a][s].isShown = Penscape_Demo::mode == (a * 3) + (s % 3);
        }
    _renderer->buildScene(&scene);
}

void Penscape_Demo::loop(double frameTime){
    _renderer->setDrawMode(drawMode);

    for (unsigned t = 0; t < BRUSH3D_ITERS; t++) {
        float prog = (float)t / (float)BRUSH3D_ITERS;

        for (unsigned a = 0; a < 3; a++)
            for (unsigned s = 0; s < 3; s++)
                brushes3D[a][s].updateRot({ (MATH_PI * 2) / BRUSH3D_ITERS, 0.0F, 0.0F });
                // brushes3D[a][s].updateRot({ (float)pow((MATH_PI * 2) / BRUSH3D_ITERS, 0.95F + (prog / 20)), 0.0F, 0.0F });
        _renderer->updateScene(&scene);
        _renderer->drawScene(&scene);
    }
}

MAIN_ENTRY{
    Penscape = new Penscape_Demo(argv[0]);
    Penscape->run();

    delete(Penscape);
    return 0;
}
