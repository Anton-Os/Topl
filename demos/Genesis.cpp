#include "Genesis.hpp"

void Genesis_Demo::init(){
    // chains.push_back(Geo_Chain("genesis_chain", &scene, &orb, VEC_3F_RAND, (rand() % 4) + 2 ));
    for(unsigned g = 0; g < 9; g++){
        grids.push_back(Geo_Grid(std::string("grid") + std::to_string(g), &scene, &orbs[g], Geo_Grid_Params(std::make_pair(g + 3, GENESIS_SIZE / (g + 1)))));
        grids.back().toggleShow(g == mode);
    }

    _renderer->buildScene(&scene);
}

void Genesis_Demo::loop(double frameTime){
    // TODO: Include rendering logic
    renderScene(&scene, nullptr, Topl_Program::shaderMode); 
}

void Genesis_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex){
    if(menu == PROGRAM_AppBar)
        for(unsigned g = 0; g < 9; g++)
            grids[g].toggleShow(g == mode);
}

MAIN_ENTRY {
    Genesis = new Genesis_Demo(argv[0], BACKEND_GL4);
    Genesis->run();

    delete(Genesis);
    return 0;
}