#include "Meshform.hpp"

void Meshform_Demo::onAnyKey(char key){
    static Vec3f incVec = Vec3f({ MESHFORM_INC, MESHFORM_INC, MESHFORM_INC });
    static Vec3f decVec = Vec3f({ MESHFORM_DEC, MESHFORM_DEC, MESHFORM_DEC });
    switch(tolower(key)){
        case 'i': for(unsigned m = 0; m < 3; m++) for(unsigned a = 0; a < 4; a++) orbActors[m][a].isShown = m == 0; break;
        case 'o': for(unsigned m = 0; m < 3; m++) for(unsigned a = 0; a < 4; a++) orbActors[m][a].isShown = m == 1; break;
        case 'p': for(unsigned m = 0; m < 3; m++) for(unsigned a = 0; a < 4; a++) orbActors[m][a].isShown = m == 2; break;
        case 'j': genShapes(MESHFORM_TESS + 1, std::make_pair(nullptr, decVec), std::make_pair(nullptr, incVec)); break;
        case 'k': genShapes(MESHFORM_TESS + 1, std::make_pair(curveTForm, decVec), std::make_pair(rigidTForm, incVec)); break;
        case 'l': genShapes(MESHFORM_TESS + 1, std::make_pair(rigidTForm, decVec), std::make_pair(curveTForm, incVec)); break;
        case 'b': genTex3D(MESHFORM_GRADIENT, RAND_COLOR(), RAND_COLOR()); break;
        case 'n': genTex3D(MESHFORM_LINES, RAND_COLOR(), RAND_COLOR()); break;
        case 'm': genTex3D(MESHFORM_CHECKER, RAND_COLOR(), RAND_COLOR()); break;
    }

    if(tolower(key) == 'j' || tolower(key) == 'k' || tolower(key) == 'l')
        _renderer->buildScene(&scene);
}

#ifdef RASTERON_H
void Meshform_Demo::genTex3D(unsigned short mode, unsigned color1, unsigned color2){
    if(textureThread == nullptr)
        textureThread = new std::thread([this](unsigned short m, unsigned c1, unsigned c2){
            SIDE_Type side = SIDE_Bottom;
            ImageSize size = { 256, 256 };

            for(unsigned d = 0; d < volumeImg.getDepth(); d++){
                Rasteron_Image* sliceImg;
                switch(m){
                    case MESHFORM_LINES: sliceImg = linedImgOp(size, c1, c2, d, 0.0); break;
                    case MESHFORM_CHECKER: sliceImg = checkeredImgOp(size, { d, d, c1, c2 }); break;
                    // case MESHFORM_NOISE: sliceImg = noiseImgOp_tiled(size, { d, d, c1, c2 }); break;
                    default: sliceImg = solidImgOp(size, colors_blend(c1, c2, d / 256.0)); break;
                }
                volumeImg.addSlice(sliceImg, d);
                RASTERON_DEALLOC(sliceImg);
            }
        }, mode, color1, color2);
    else if(textureThread->joinable()){ // TODO: Refine this logic
        textureThread->join();
        _renderer->texturizeScene(&scene);
        delete textureThread;
        textureThread = nullptr;
    }
}
#endif


void Meshform_Demo::genShapes(unsigned tessCount, std::pair<vTformCallback, Vec3f> transform1, std::pair<vTformCallback, Vec3f> transform2){
    for(unsigned o = 0; o < 3; o++){ // iterate through all shapes
        if(tessCount > 0){
            trigOrbs[o]->tesselate(tessCount);
            quadOrbs[o]->tesselate(tessCount);
            hexOrbs[o]->tesselate(tessCount);
            decOrbs[o]->tesselate(tessCount);
        }
        if(transform1.first != nullptr && transform2.first != nullptr && o > 0){
            trigOrbs[o]->modify((o % 2 == 1)? transform1.first : transform2.first, (o % 2 == 1)? transform1.second : transform2.second);
            quadOrbs[o]->modify((o % 2 == 1)? transform1.first : transform2.first, (o % 2 == 1)? transform1.second : transform2.second);
            hexOrbs[o]->modify((o % 2 == 1)? transform1.first : transform2.first, (o % 2 == 1)? transform1.second : transform2.second);
            decOrbs[o]->modify((o % 2 == 1)? transform1.first : transform2.first, (o % 2 == 1)? transform1.second : transform2.second);
        }
    }
}

void Meshform_Demo::init(){
    Platform::keyControl.addHandler(std::bind(&Meshform_Demo::onAnyKey, this, std::placeholders::_1));

    for(unsigned m = 0; m < 3; m++){
        orbActors[m][0].setPos({ 0.5F, 0.5F, 0.0F });
        scene.addGeometry("trigOrb" + std::to_string(m), &orbActors[m][0]);
        orbActors[m][1].setPos({ -0.5F, -0.5F, 0.0F });
        scene.addGeometry("quadOrb" + std::to_string(m), &orbActors[m][1]);
        orbActors[m][2].setPos({ 0.5F, -0.5F, 0.0F });
        scene.addGeometry("hexOrb" + std::to_string(m), &orbActors[m][2]);
        orbActors[m][3].setPos({ -0.5F, 0.5F, 0.0F });
        scene.addGeometry("decOrb" + std::to_string(m), &orbActors[m][3]);
        for(unsigned a = 0; a < 4; a++) orbActors[m][a].isShown = m == MESHFORM_INDEX;

        scene.addVolumeTex("trigOrb" + std::to_string(m), &volumeImg);
        scene.addVolumeTex("quadOrb" + std::to_string(m), &volumeImg);
        scene.addVolumeTex("hexOrb" + std::to_string(m), &volumeImg);
        scene.addVolumeTex("decOrb" + std::to_string(m), &volumeImg);
    }

    _texVShader.setAntialiasing(0.01F, 6);
    Topl_Factory::switchPipeline(_renderer, _texPipeline);
    Topl_Program::shaderMode = 8;

    _renderer->buildScene(&scene);
#ifdef RASTERON_H
    _renderer->texturizeScene(&scene);
#endif
}

void Meshform_Demo::loop(double frameTime){
    for(unsigned a = 0; a < 4; a++){ 
        Vec3f rotationVec = Vec3f({ (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX });
        rotationVec = rotationVec * (0.001 * frameTime);
        for(unsigned o = 0; o < 3; o++) _DEMO->orbActors[o][a].updateRot(rotationVec);
    }

    _renderer->updateScene(&scene);
    _renderer->setDrawMode(DRAW_Fan);
    _renderer->drawScene(&scene);
    // _renderer->setDrawMode(DRAW_Lines);
    // _renderer->drawScene(&scene);
}

MAIN_ENTRY {
    _DEMO = new Meshform_Demo(argv[0], BACKEND_DX11);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}
