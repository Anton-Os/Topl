#include "Penscape.hpp"

static DRAW_Mode drawMode = DRAW_Triangles;
static unsigned brushSegments = PENSCAPE_ITERS;

void Penscape_Demo::createBrushes() {
    trigMeshBrushes[2]->scale({ 1.0F, 1.0F, PENSCAPE_SCALE });
    quadMeshBrushes[2]->scale({ 1.0F, 1.0F, PENSCAPE_SCALE });
    // hexMeshBrushes[2]->scale({ 1.0F, 1.0F, PENSCAPE_SCALE });
    circleMeshBrushes[2]->scale({ 1.0F, 1.0F, PENSCAPE_SCALE });

    for (unsigned b = 0; b < PENSCAPE_SEGMENTS; b++)
        for (unsigned a = 0; a < 3 * 2; a++) {
            trigMeshBrushes[a % 3]->shift(getRandTransform(PENSCAPE_SHIFT)); // { 0.0F, 0.0F, PENSCAPE_SHIFT});
            trigMeshBrushes[a % 3]->rotate({ PENSCAPE_ROT, PENSCAPE_ROT, PENSCAPE_ROT });
            trigMeshes[a % 3]->extend(*trigMeshBrushes[a % 3]);
            trigMeshes[a % 3]->setInstances({ PENSCAPE_MESH_INSTANCES });
            quadMeshBrushes[a % 3]->shift(getRandTransform(PENSCAPE_SHIFT)); // { 0.0F, 0.0F, PENSCAPE_SHIFT});
            quadMeshBrushes[a % 3]->rotate({ -PENSCAPE_ROT, PENSCAPE_ROT, PENSCAPE_ROT });
            quadMeshes[a % 3]->extend(*quadMeshBrushes[a % 3]);
            quadMeshes[a % 3]->setInstances({ PENSCAPE_MESH_INSTANCES });
            /* hexMeshBrushes[a % 3]->shift(getRandTransform(PENSCAPE_SHIFT)); // { 0.0F, 0.0F, PENSCAPE_SHIFT});
            hexMeshBrushes[a % 3]->rotate({ PENSCAPE_ROT, -PENSCAPE_ROT, PENSCAPE_ROT });
            hexMeshes[a % 3]->extend(*hexMeshBrushes[a % 3]); 
            hexMeshes[a % 3]->setInstances({ PENSCAPE_MESH_INSTANCES });*/
            circleMeshBrushes[a % 3]->shift(getRandTransform(PENSCAPE_SHIFT)); // { 0.0F, 0.0F, PENSCAPE_SHIFT});
            circleMeshBrushes[a % 3]->rotate({ PENSCAPE_ROT, PENSCAPE_ROT, -PENSCAPE_ROT });
            circleMeshes[a % 3]->extend(*circleMeshBrushes[a % 3]);
            circleMeshes[a % 3]->setInstances({ PENSCAPE_MESH_INSTANCES });
        }
}

void Penscape_Demo::onAnyKey(keyboard_t key){
    // for (unsigned a = 0; a < 3; a++)
    //      for (unsigned s = 0; s < 3; s++)
    switch(tolower(key)) {
    case 'i': drawMode = DRAW_Triangles; break; // brushes3D[a][s].isShown = a == 0; break;
    case 'o': drawMode = DRAW_Lines; break; // brushes3D[a][s].isShown = a == 1; break;
    case 'p': drawMode = DRAW_Points; break; // brushes3D[a][s].isShown = a == 2; break;
    case 'k': if(brushSegments > 0) brushSegments--; break;
    case 'l': if(brushSegments < PENSCAPE_ITERS) brushSegments++; break;
    }
}

void Penscape_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex){
    if(menu == PROGRAM_Sculpt)
        for(unsigned a = 0; a < 3; a++)
            for(unsigned s = 0; s < 3; s++)
                brushes3D[a][s].isShown = paneIndex == (a * 3) + (s % 3);
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
    unsigned f = _renderer->getFrameCount();
    _renderer->setDrawMode(drawMode);

    for (unsigned t = 0; t < brushSegments; t++) {
        float prog = (float)t / (float)brushSegments;

        for (unsigned a = 0; a < 3; a++)
            for (unsigned s = 0; s < 3; s++)
                brushes3D[a][s].updateRot({ (float)(MATH_PI * 2) / brushSegments, 0.0F, 0.0F});
                // brushes3D[a][s].updateRot({ (float)pow((MATH_PI * 2) / PENSCAPE_ITERS, 0.95F + (prog / 20)), 0.0F, 0.0F });
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
