#include "Kaleidoscope.hpp"

static DRAW_Mode drawMode = DRAW_Triangles;

unsigned short sculptIndex = 0;

// Construct

void Kaleidoscope_Construct::init() {
    unsigned short minDivs = (divisions.first < divisions.second)? divisions.first : divisions.second;
    unsigned short maxDivs = (divisions.first < divisions.second)? divisions.second : divisions.first;

    for(unsigned s = 0; s < KALEIDOSCOPE_SLICES; s++){
        float z = KALEIDOSCOPE_Z;

        Shape2D shape = { (float)rand() / (float)RAND_MAX, (minDivs == maxDivs) ? minDivs : (unsigned)(rand() % maxDivs) + minDivs };
        if(tessCount != 0) meshes[s] = new Geo_Ext2D(shape, z, (unsigned)abs(tessCount));
        else meshes[s] = new Geo_Surface(shape, z);
        
        if(tessCount <= 0) meshes[s]->tesselate(KALEIDOSCOPE_TESS);

        _geoActors.push_back(Geo_Actor(meshes[s]));
        _geoActors.back().setPos(Vec3f({ 0.0F, 0.0F, -1.0F + (float)((2.0F / KALEIDOSCOPE_SLICES) * s )}));

        spinFactors[s] = (((float)rand() / (float)RAND_MAX) - 0.5) * 0.025;
        sizeFactors[s] = (((float)rand() / (float)RAND_MAX) - 0.5);
    }
}

// Demo

void Kaleidoscope_Demo::onAnyKey(char key){
    switch(tolower(key)){
        case 'u': drawMode = DRAW_Triangles; break;
        case 'i': drawMode = DRAW_Lines; break;
        case 'o': drawMode = DRAW_Points; break;
        case 'p': drawMode = DRAW_Strip; break;
        case 'v': constructs[sculptIndex].rotate({ 0.1F, 0.0F, 0.0F }); break;
        case 'b': constructs[sculptIndex].rotate({ -0.1F, 0.0F, 0.0F }); break;
        case 'n': constructs[sculptIndex].rotate({ 0.0F, 0.1F, 0.0F }); break;
        case 'm': constructs[sculptIndex].rotate({ 0.0F, -0.1F, 0.0F }); break;
    }
}

void Kaleidoscope_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex){
    if (menu == PROGRAM_Sculpt) {
        sculptIndex = paneIndex;
        for (unsigned c = 0; c < 9; c++) constructs[c].toggleShow(paneIndex == c);
    }
}

void Kaleidoscope_Demo::init(){
    Platform::keyControl.addHandler(std::bind(&Kaleidoscope_Demo::onAnyKey, this, std::placeholders::_1));

    for(unsigned c = 0; c < 9; c++) constructs[c].configure(&scene);

    _renderer->setPipeline(_flatPipeline);
    _renderer->buildScene(&scene);

    // Topl_Program::mode = 8; // test
    // for(unsigned c = 0; c < 9; c++) constructs[c].toggleShow(Topl_Program::mode == c);
}

void Kaleidoscope_Demo::loop(double frameTime){
    static double totalTime = 0.0F;

    for (unsigned s = 0; s < constructs[sculptIndex].getActorCount(); s++) {
        constructs[sculptIndex].getGeoActor(s)->updateRot(Vec3f({ sin(constructs[sculptIndex].getSpinFactor(s) * 2), 0.0F, 0.0F }));
        float scale = sin(constructs[sculptIndex].getSizeFactor(s) * totalTime * 0.000001) * 0.35F;
        constructs[sculptIndex].getGeoActor(s)->setSize(Vec3f({ 1.0F + scale, 1.0F + scale, 1.0F + scale }));
    }

    _renderer->setDrawMode(drawMode);
    // _renderer->updateScene(&scene);
    renderScene(&scene); // _renderer->drawScene(&scene);

    totalTime += frameTime;
}

// Main

MAIN_ENTRY {
    Kaleidoscope = new Kaleidoscope_Demo(argv[0], BACKEND_GL4);
    Kaleidoscope->run();

    delete(Kaleidoscope);

    return 0;
}