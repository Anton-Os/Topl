#include "Meshform.hpp"

void Meshform_Demo::onAnyKey(char key){
    static Vec3f incVec = Vec3f({ MESHFORM_INC, MESHFORM_INC, MESHFORM_INC });
    static Vec3f decVec = Vec3f({ MESHFORM_DEC, MESHFORM_DEC, MESHFORM_DEC });
    switch(tolower(key)){
        case 'i': for(unsigned m = 0; m < 3; m++) for(unsigned a = 0; a < 4; a++) orbActors[m][a].isShown = m == 0; break;
        case 'o': for(unsigned m = 0; m < 3; m++) for(unsigned a = 0; a < 4; a++) orbActors[m][a].isShown = m == 1; break;
        case 'p': for(unsigned m = 0; m < 3; m++) for(unsigned a = 0; a < 4; a++) orbActors[m][a].isShown = m == 2; break;
        case 'j': genShapes(MESHFORM_TESS + 1, std::make_pair(nullptr, decVec), std::make_pair(nullptr, incVec)); break;
        case 'k': genShapes(MESHFORM_TESS + 1, std::make_pair(curveTForm, incVec), std::make_pair(rigidTForm, incVec)); break;
        case 'l': genShapes(MESHFORM_TESS + 1, std::make_pair(pullTForm, decVec * -1.0F), std::make_pair(pullTForm, decVec)); break;
        case 'b': genTex3D(MESHFORM_GRADIENT, RAND_COLOR(), RAND_COLOR()); break;
        case 'n': genTex3D(MESHFORM_LINES, RAND_COLOR(), RAND_COLOR()); break;
        case 'm': genTex3D(MESHFORM_CHECKER, RAND_COLOR(), RAND_COLOR()); break;
    }
}

#ifdef RASTERON_H
void Meshform_Demo::genTex3D(unsigned short mode, unsigned color1, unsigned color2){
    if(textureThread != nullptr)
        if(textureThread->joinable()){
            textureThread->join();
            delete textureThread;
        } else return logMessage(MESSAGE_Exclaim, "Texture thread still running!");

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

    _renderer->texturizeScene(&scene);
}
#endif


void Meshform_Demo::genShapes(unsigned tessCount, std::pair<vTformCallback, Vec3f> transform1, std::pair<vTformCallback, Vec3f> transform2){
    if(geometryThread != nullptr)
        if(geometryThread->joinable()){
            geometryThread->join();
            delete geometryThread;
        } else return logMessage(MESSAGE_Exclaim, "Geometry thread still running!");

    geometryThread = new std::thread([this](unsigned tess, std::pair<vTformCallback, Vec3f> tform1, std::pair<vTformCallback, Vec3f> tform2, 
        Geo_TrigOrb* tOrbs[3], Geo_QuadOrb* qOrbs[3], Geo_HexOrb* hOrbs[3], Geo_DecOrb* dOrbs[3]){
        
        for(unsigned o = 0; o < 3; o++){ // iterate through all shapes
            if(tess > 0){
                tOrbs[o]->tesselate(tess);
                qOrbs[o]->tesselate(tess);
                hOrbs[o]->tesselate(tess);
                dOrbs[o]->tesselate(tess);
            }
            if(tform1.first != nullptr && tform2.first != nullptr && o > 0){
                tOrbs[o]->modify((o % 2 == 1)? tform1.first : tform2.first, (o % 2 == 1)? tform1.second : tform2.second);
                qOrbs[o]->modify((o % 2 == 1)? tform1.first : tform2.first, (o % 2 == 1)? tform1.second : tform2.second);
                hOrbs[o]->modify((o % 2 == 1)? tform1.first : tform2.first, (o % 2 == 1)? tform1.second : tform2.second);
                dOrbs[o]->modify((o % 2 == 1)? tform1.first : tform2.first, (o % 2 == 1)? tform1.second : tform2.second);
            }
        }
    }, tessCount, transform1, transform2, trigOrbs, quadOrbs, hexOrbs, decOrbs);
    geometryThread->join(); // TODO: Perform blocking call

    _renderer->buildScene(&scene);
}

void Meshform_Demo::renderInscribed(Geo_Actor* actor, unsigned short count){
    Vec3f size = *(actor->getSize());
    Vec3f texScale = Vec3f({ 1.0F, 1.0F, 1.0F });

    for(unsigned c = 0; c < count; c++){ // inscribed objects
        float inc = 1.0F - ((1.0F / count) * (c + 1));
        actor->setSize(size * inc);
        _texVShader.setTexScale(texScale * inc);
        _renderer->update(actor);
        _renderer->draw(actor);
    }

    actor->setSize(size); 
    _texVShader.setTexScale(texScale);
}

void Meshform_Demo::init(){
    Platform::keyControl.addHandler(std::bind(&Meshform_Demo::onAnyKey, this, std::placeholders::_1));

    scene.addGeometry("torus", &torusActor);
    // torusActor.isShown = false;
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
        for(unsigned o = 0; o < 3; o++) orbActors[o][a].updateRot(rotationVec);
        if(a == 0) torusActor.updateRot(rotationVec * -1.0F);
    }

    // torus->drawMode = DRAW_Lines;
#ifdef RASTERON_H
    _texVShader.setFlip(0);
    _texVShader.setAntialiasing(0.01F, 30);
    _texVShader.setSlice((_renderer->getFrameCount() % 256) * (1.0F / 256.0));
#endif
    _renderer->setDrawMode(DRAW_Triangles);
    _renderer->updateScene(&scene);
    _renderer->drawScene(&scene);

    for(unsigned a = 0; a < 4; a++)
        for(unsigned o = 0; o < 3; o++) 
            if(orbActors[o][a].isShown) renderInscribed(&orbActors[o][a], 3);
}

MAIN_ENTRY {
    _DEMO = new Meshform_Demo(argv[0], BACKEND_DX11);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}
