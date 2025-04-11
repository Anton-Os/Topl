#include "Meshscape.hpp"

static float brushScroll = 0.0F;

void onBrushScroll(bool isUp){ brushScroll += (isUp)? 0.01F : -0.01F; }

// Construct

Geo_Mesh* Meshscape_Construct::getCompositeMesh(){
    if(getActorCount() >= 1){
        compositeMesh = Geo_Mesh(*_geoActors.front().getMesh());
        compositeMesh.shift(*_geoActors.front().getPos());
        for(unsigned b = 1; b < getActorCount(); b++){
            Geo_Mesh extendedMesh = Geo_Mesh(*_geoActors[b].getMesh());
            extendedMesh.shift(*_geoActors[b].getPos());
            compositeMesh.extend(extendedMesh);
        }
        return &compositeMesh;
    } else return nullptr;
}

// Demo

void Meshscape_Demo::init(){
    Platform::mouseControl.addHandler(std::bind(&Meshscape_Demo::onAnyPress, this, std::placeholders::_1, std::placeholders::_2));
    Platform::mouseControl.setOnScroll(onBrushScroll);

    for(unsigned b = 0; b < MESHSCAPE_BRUSHES; b++) constructs.push_back(Meshscape_Construct(&brushMeshes[b], { Vec3f({ (b % 2 == 0)? 0.5F : -0.5F, ((b / 2) % 2 == 0)? 0.5F : -0.5F, 0.0F }) }));
    for(unsigned b = 0; b < MESHSCAPE_BRUSHES; b++) constructs[b].configure(&scene);

    _renderer->buildScene(&scene);
}

void Meshscape_Demo::loop(double frameTime){
    renderScene((sculptActors.empty())? &scene : &sculptScene, nullptr, Topl_Program::shaderMode);
}

void Meshscape_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex){
    if(menu == PROGRAM_AppBar)
        switch(paneIndex){
            // TODO: Set parameters appropriately
        }
    else if(menu == PROGRAM_Sculpt) sculptMode = paneIndex % MESHSCAPE_BRUSHES;
}

void Meshscape_Demo::onAnyPress(enum MOUSE_Event event, std::pair<float, float> cursor){
    static unsigned sculptActorCount = 0;
    
    if(event == MOUSE_LeftBtn_Release){
        sculptActors.push_back(new Geo_Actor(&brushMeshes[sculptMode]));
        sculptActors.back()->setPos(Vec3f({ cursor.first, cursor.second, brushScroll }));
        sculptScene.addGeometry(std::string("sculptActor") + std::to_string(sculptActorCount + 1), sculptActors.back());
        _renderer->buildScene(&sculptScene); // new geometry added

        sculptActorCount++;
    }
}

// Main

MAIN_ENTRY {
    Meshscape = new Meshscape_Demo(argv[0], BACKEND_GL4);
    Meshscape->run();

    delete(Meshscape);
    return 0;
}