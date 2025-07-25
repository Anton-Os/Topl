#include "Genesis.hpp"

static unsigned sculptIndex = 0;

void Genesis_Demo::init(){
    // chains.push_back(Geo_Chain("genesis_chain", &scene, &orb, VEC_3F_RAND, (rand() % 4) + 2 ));
    for(unsigned g = 0; g < 9 * 9; g++){
        // for(unsigned s = 0; s < 9; s++){
        Geo_Grid_Params params = Geo_Grid_Params(std::make_pair(GENESIS_COUNT, GENESIS_SIZE));
        grids.push_back(Geo_Grid(std::string("grid") + std::to_string(g), &scene, &orbs[g % 9], params));
        grids.back().toggleShow(g == mode); // grids.back().toggleShow((g * 10) + s == mode);
    }

    _renderer->buildScene(&scene);
}

void Genesis_Demo::loop(double frameTime){
    // TODO: Include rendering logic
    renderScene(&scene, nullptr, Topl_Program::shaderMode); 
}

void Genesis_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex){
    if(menu == PROGRAM_Sculpt) sculptIndex = paneIndex;
    if(menu == PROGRAM_AppBar || menu == PROGRAM_Sculpt)
        for(unsigned g = 0; g < 9 * 9; g++)
            grids[g].toggleShow(g == mode + (sculptIndex * 9));
    
}

MAIN_ENTRY {
    Genesis = new Genesis_Demo(argv[0], BACKEND_GL4);
    Genesis->run();

    delete(Genesis);
    return 0;
}