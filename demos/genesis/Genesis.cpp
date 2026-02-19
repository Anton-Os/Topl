#include "Genesis.hpp"

static bool isRotating = false;
static bool isPulsing = true;

static unsigned sculptIndex = 0;

void Genesis_Demo::init(){
    for(unsigned g = 0; g < 9 * 9; g++){
        Geo_Grid_Params params = Geo_Grid_Params(std::make_pair(GENESIS_COUNT, GENESIS_SIZE));
        grids.push_back(Geo_Grid(std::string("grid") + std::to_string(g), &scene, &orbs[g % 9], params));
        grids.back().toggleShow(g == mode); // grids.back().toggleShow((g * 10) + s == mode);
    }

    _renderer->buildScene(&scene);
}

void Genesis_Demo::loop(double frameTime){
    Geo_Grid* grid = &grids[mode + (sculptIndex * 9)];
    for (unsigned g = 0; g < grid->getActorCount(); g++) {
        if(isRotating) grid->getGeoActor(g)->updateRot(VEC_3F_RAND * frameTime * 0.0000005F);
        if (isPulsing) {
            grid->getGeoActor(g)->setPos(*grid->getGeoActor(g)->getPos());
        }
    }

    renderScene(&scene); 
}

void Genesis_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex){
    if(menu == PROGRAM_Sculpt) sculptIndex = paneIndex;
    if(menu == PROGRAM_AppBar || menu == PROGRAM_Sculpt)
        for(unsigned g = 0; g < 9 * 9; g++)
            grids[g].toggleShow(g == mode + (sculptIndex * 9));
}

MAIN_ENTRY {
    Genesis = new Genesis_Demo(argv[0], BACKEND_DX11);
    Genesis->run();

    delete(Genesis);
    return 0;
}