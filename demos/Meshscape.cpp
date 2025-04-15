#include "Meshscape.hpp"

static float brushScroll = 0.0F;

void onBrushScroll(bool isUp){ brushScroll += (isUp)? 0.05F : -0.05F; }

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
    
    for(unsigned b = 0; b < MESHSCAPE_BRUSHES; b++){ 
        grids.push_back(Geo_Grid(std::string("grid") + std::to_string(b), &scene, &brushMeshes[b], Geo_Grid_Params(std::make_pair(6, MESHSCAPE_SIZE))));
        grids[b].configure(&scene);
        grids[b].toggleShow(false);
    }

    _renderer->buildScene(&scene);
}

void Meshscape_Demo::loop(double frameTime){
    if(mode > 0) for(unsigned g = 0; g < grids.size(); g++) updateConstruct(&grids[g]);

    renderScene(&scene, nullptr, Topl_Program::shaderMode);
    if(!sculptActors.empty()) renderScene(&sculptScene, nullptr, Topl_Program::shaderMode);
}

void Meshscape_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex){
    if(menu == PROGRAM_Sculpt){ 
        sculptMode = paneIndex % MESHSCAPE_BRUSHES;
        if(sculptActors.size() > 0) onAnyPress(MOUSE_RightBtn_Release, std::make_pair(0.0F, 0.0F)); // simulate right key press
    }
    else if(menu == PROGRAM_Paint) for(unsigned b = 0; b < MESHSCAPE_BRUSHES; b++) grids[b].toggleShow(b == paneIndex);
}

void Meshscape_Demo::updateConstruct(Geo_Construct* construct){
    Vec3f transformVec = (VEC_3F_RAND - Vec3f({ 0.5F, 0.5F, 0.5F })) * Vec3f({ 0.01F, 0.01F, 0.01F });

    if(mode == 1 || mode == 2 || mode == 3)
        switch(mode){
            case 1: construct->shift(transformVec); break;
            case 2: construct->rotate(transformVec); break;
            case 3: construct->scale(transformVec + Vec3f({ 1.0F, 1.0F, 1.0F})); break;
        }
    else {
        for(unsigned c = 0; c < construct->getActorCount(); c++){
            Geo_Actor* targetActor = construct->getGeoActor(c);
            transformVec = (VEC_3F_RAND - Vec3f({ 0.5F, 0.5F, 0.5F })) * Vec3f({ 0.01F, 0.01F, 0.01F });
            switch(mode){
                case 4: targetActor->updatePos(transformVec); break;
                case 5: targetActor->updateRot(transformVec); break;
                case 6: targetActor->updateSize(transformVec); break;
                // TODO: Perform mesh manipulation
            }
        }
    }
}

void Meshscape_Demo::onAnyPress(enum MOUSE_Event event, std::pair<float, float> cursor){
    Vec3f translation = Vec3f({ 
        cursor.first + camera.getPos()->data[0], 
        cursor.second + camera.getPos()->data[1], 
        brushScroll + camera.getPos()->data[2] + 1.0F 
    });

    if(event == MOUSE_LeftBtn_Release){
        sculptActors.push_back(new Geo_Actor(&brushMeshes[sculptMode]));
        sculptActors.back()->setPos(translation);
        sculptScene.addGeometry(std::string("sculptActor") + std::to_string(sculptActors.size()), sculptActors.back());
        _renderer->buildScene(&sculptScene); // new geometry added
    } else if(event == MOUSE_RightBtn_Release && !sculptActors.empty()){
        // constructs.push_back(Meshscape_Construct(&brushMeshes[b], { Vec3f({ (b % 2 == 0)? 0.5F : -0.5F, ((b / 2) % 2 == 0)? 0.5F : -0.5F, 0.0F }) }));
        constructs.push_back(Meshscape_Construct(sculptActors));
        constructs.back().shift(translation);
        constructs.back().configure(&scene);
        _renderer->buildScene(&scene);
        // for(auto s = sculptActors.begin(); s != sculptActors.end(); s++) delete *s;
        // sculptActors.clear();
    }
}

// Main

MAIN_ENTRY {
    Meshscape = new Meshscape_Demo(argv[0], BACKEND_DX11);
    Meshscape->run();

    delete(Meshscape);
    return 0;
}