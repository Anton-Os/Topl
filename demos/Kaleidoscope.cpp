#include "Kaleidoscope.hpp"

static DRAW_Mode drawMode = DRAW_Triangles;

// Construct

void Kaleidoscope_Construct::init() {
    unsigned short minDivs = (divisions.first < divisions.second)? divisions.first : divisions.second;
    unsigned short maxDivs = (divisions.first < divisions.second)? divisions.second : divisions.first;

    for(unsigned s = 0; s < KALEIDOSCOPE_SLICES; s++){
        float z = KALEIDOSCOPE_Z;

        if(tessCount != 0) meshes[s] = new Geo_Ext2D({ (float)rand() / (float)RAND_MAX, (minDivs == maxDivs)? minDivs : (unsigned)(rand() % maxDivs) + minDivs }, z, (unsigned)abs(tessCount));
        else meshes[s] = new Geo_Surface({ (float)rand() / (float)RAND_MAX, (minDivs == maxDivs)? minDivs : (unsigned)(rand() % maxDivs) + minDivs }, z);
        
        if(tessCount <= 0) meshes[s]->tesselate(KALEIDOSCOPE_TESS);

        _geoActors.push_back(Geo_Actor(meshes[s]));
        _geoActors.back().setPos(Vec3f({ 0.0F, 0.0F, -1.0F + (float)((2.0F / KALEIDOSCOPE_SLICES) * s )}));

        spinFactors[s] = (((float)rand() / (float)RAND_MAX) - 0.5) * 0.01;
    }
}

// Demo

void Kaleidoscope_Demo::onAnyKey(char key){
    switch(tolower(key)){
        case 'u': drawMode = DRAW_Triangles; break;
        case 'i': drawMode = DRAW_Lines; break;
        case 'o': drawMode = DRAW_Points; break;
        case 'p': drawMode = DRAW_Strip; break;
        case 'v': constructs[mode].rotate({ 0.1F, 0.0F, 0.0F }); break;
        case 'b': constructs[mode].rotate({ -0.1F, 0.0F, 0.0F }); break;
        case 'n': constructs[mode].rotate({ 0.0F, 0.1F, 0.0F }); break;
        case 'm': constructs[mode].rotate({ 0.0F, -0.1F, 0.0F }); break;
    }
}

void Kaleidoscope_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex){
    if(menu == PROGRAM_AppBar) for(unsigned c = 0; c < 9; c++) constructs[c].toggleShow(Topl_Program::mode == c);
}

void Kaleidoscope_Demo::init(){
    Platform::keyControl.addHandler(std::bind(&Kaleidoscope_Demo::onAnyKey, this, std::placeholders::_1));

    for(unsigned c = 0; c < 9; c++) constructs[c].configure(&scene);

    _renderer->setPipeline(_flatPipeline);
    _renderer->buildScene(&scene);

    for(unsigned c = 0; c < 9; c++) constructs[c].toggleShow(Topl_Program::mode == c);
}

void Kaleidoscope_Demo::loop(double frameTime){
    for(unsigned s = 0; s < constructs[mode].getActorCount(); s++)
        constructs[mode].getGeoActor(s)->updateRot(Vec3f({ constructs[mode].getSpinFactor(s), 0.0F, 0.0F }));

    if(_renderer->getPipeline() == _patternPipeline){
        for(unsigned p = 0; p < PATTERN_POINTS_MAX; p++)
            _patternVShader.setCtrlPoint(p, Vec3f({ 0.0F, 0.0F, -1.0F + ((2.0F / PATTERN_POINTS_MAX) * p) }));
    }

    _renderer->setDrawMode(drawMode);
    // _renderer->updateScene(&scene);
    renderScene(&scene, nullptr, -Topl_Program::shaderMode); // _renderer->drawScene(&scene);
}

// Main

MAIN_ENTRY {
    Kaleidoscope = new Kaleidoscope_Demo(argv[0], BACKEND_DX11);
    Kaleidoscope->run();

    delete(Kaleidoscope);

    return 0;
}