#include "Traversal.hpp"

void Traversal_Demo::onAnyKey(char key) {
    switch (tolower(key)) {
        case 'i': for (unsigned c = 0; c < 3; c++) corridorActors[c].isShown = c == 0; break;
        case 'o': for (unsigned c = 0; c < 3; c++) corridorActors[c].isShown = c == 1; break;
        case 'p': for (unsigned c = 0; c < 3; c++) corridorActors[c].isShown = c == 2; break;
    }
}

void Traversal_Demo::renderRecursive(Geo_Actor* actor, TFORM_Type type, unsigned short count) {
    const Vec3f* tformPtr = (type == TFORM_Scale)? actor->getSize() : (type == TFORM_Rotate)? actor->getRot() : actor->getPos();
    Vec3f transform = *tformPtr;

    Geo_Mesh* meshes[3] = { &squareCorridor, &hexCorridor, &circleCorridor };

    for (unsigned c = 0; c < count; c++) { // inscribed objects
        float inc = 1.0F - ((1.0F / count) * (c + 1));
        switch (type) {
        case TFORM_Shift: actor->setPos(transform * inc); break;
        case TFORM_Rotate: actor->setRot(transform * inc); break;
        case TFORM_Scale: actor->setSize(transform * inc); break;
        }
        for(unsigned m = 0; m < 3; m++){ 
            unsigned vCount = meshes[m]->getVertexCount();
            // meshes[m]->drawMin = (vCount / count) * c;
            // meshes[m]->drawMax = ((vCount / count) * c) + vCount;
        }
        _renderer->update(actor);
        _renderer->draw(actor);
    }

    (type == TFORM_Scale)? actor->setSize(transform) : (type == TFORM_Rotate)? actor->setRot(transform) : actor->setPos(transform);
    
    _renderer->update(actor);
}

void Traversal_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex) {
    if (menu == PROGRAM_AppBar){
        for (unsigned c = 0; c < 3; c++) corridorActors[c].isShown = paneIndex / 3 == c;
    }
}

void Traversal_Demo::init(){
    squareCorridor.tesselate(TRAVERSAL_TESS);
    hexCorridor.tesselate(TRAVERSAL_TESS);
    circleCorridor.tesselate(TRAVERSAL_TESS);

    Platform::keyControl.addHandler(std::bind(&Traversal_Demo::onAnyKey, this, std::placeholders::_1));
    Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Perspective, 10.0).genProjMatrix());

    for(unsigned c = 0; c < 3; c++)
        scene.addGeometry(std::string("corridor") + std::to_string(c + 1), &corridorActors[c]);
    _renderer->buildScene(&scene);
}

void Traversal_Demo::loop(double frameTime){
    camera.updatePos({ 0.0F, 0.0F, (float)frameTime * Topl_Program::speed * TRAVERSAL_SPEED });

    renderScene(&scene, nullptr, Topl_Program::shaderMode);

    TFORM_Type tformType = (mode % 3 == 0)? TFORM_Scale : (mode % 3 == 1)? TFORM_Rotate : TFORM_Shear;
    for(unsigned a = 0; a < 3; a++) renderRecursive(&corridorActors[a], tformType, TRAVERSAL_RECURSION);
}

MAIN_ENTRY {
    Traversal = new Traversal_Demo(argv[0], BACKEND_GL4);
    Traversal->run();

    delete(Traversal);
    return 0;
}