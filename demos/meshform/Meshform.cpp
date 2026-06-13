#include "Meshform.hpp"

static bool isRotating = true;

// Class Functions

void Meshform_Demo::onAnyKey(keyboard_t key){
    static Vec3f incVec = Vec3f({ MESHFORM_INC, MESHFORM_INC, MESHFORM_INC });
    static Vec3f decVec = Vec3f({ MESHFORM_DEC, MESHFORM_DEC, MESHFORM_DEC });

    if (tolower(key) == 'i' || tolower(key) == 'o' || tolower(key) == 'p')
        for (unsigned m = 0; m < 3; m++) 
            switch(tolower(key)) {
                case 'i': for (unsigned a = 0; a < 4; a++) orbActors[m][a].isShown = m == 0; torusActors[m].isShown = m == 0; break;
                case 'o': for (unsigned a = 0; a < 4; a++) orbActors[m][a].isShown = m == 1; torusActors[m].isShown = m == 1; break;
                case 'p': for (unsigned a = 0; a < 4; a++) orbActors[m][a].isShown = m == 2; torusActors[m].isShown = m == 2; break;
            }
    else switch(tolower(key)){
        case 'j': genShapes(std::make_pair(nullptr, decVec), std::make_pair(nullptr, incVec)); break;
        case 'k': genShapes(std::make_pair(curveTForm, incVec), std::make_pair(rigidTForm, incVec)); break;
        case 'l': genShapes(std::make_pair(pullTForm, decVec * -1.0F), std::make_pair(pullTForm, decVec)); break;
#ifdef TOPL_ENABLE_TEXTURES
        case 'b': genTex3D(MESHFORM_GRADIENT, RAND_COLOR(), RAND_COLOR()); break;
        case 'n': genTex3D(MESHFORM_LINES, RAND_COLOR(), RAND_COLOR()); break;
        case 'm': genTex3D(MESHFORM_CHECKER, RAND_COLOR(), RAND_COLOR()); break;
#endif
    }

    if(isspace(key)) isRotating = !isRotating;
}

#ifdef TOPL_ENABLE_TEXTURES
void Meshform_Demo::genTex3D(unsigned short mode, unsigned color1, unsigned color2){
    unsigned c1 = color1, c2 = color2;
    unsigned width = 1; unsigned height = 1;
    for(unsigned d = 0; d < volume.getDepth(); d++){
        (d % 2 == 0)? width++ : height++;
        if (width > 16) width = 1; if (height > 16) height = 1;
        Rasteron_Image* sliceImg;
        switch(mode){
            case MESHFORM_LINES: sliceImg = linedImgOp({ 256, 256 }, c1, c2, width, 0.0); break;
            case MESHFORM_CHECKER: sliceImg = checkeredImgOp({ 256, 256 }, { width, height, c1, c2 }); break;
            case MESHFORM_NOISE: sliceImg = noiseImgOp_diff({ 256, 256 }, { width, height, c1, c2 }, 1); break;
            default: sliceImg = solidImgOp({ 256, 256 }, blend_colors(c1, c2, d / 256.0)); break;
        }
        volume.addSlice(sliceImg, d);
        RASTERON_DEALLOC(sliceImg);
    }

    _renderer->texturizeScene(&scene);
}
#endif

void Meshform_Demo::genShapes(std::pair<vTformCallback, Vec3f> tform1, std::pair<vTformCallback, Vec3f> tform2){
    if(tform1.first != nullptr && tform2.first != nullptr)
        for (unsigned o = 0; o < 4; o++) {
            freeforms[o]->modify(tform1.first, tform1.second);
            freeforms[o]->modify(tform2.first, tform2.second);
            for (unsigned m = 1; m < 3; m++) // no tranform for first shape, transforms 1 and 2 for other shapes
                orbs[o][m]->modify((o % 2 == 1) ? tform1.first : tform2.first, (o % 2 == 1) ? tform1.second : tform2.second);
        }
    _renderer->buildScene(&scene);
}

void Meshform_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex){
    if(menu == PROGRAM_Sculpt){
        Vec3f amount = Vec3f({ 
            (paneIndex % 3 == 0)? MESHFORM_AMOUNT : 0.0F, 
            (paneIndex % 3 == 1)? MESHFORM_AMOUNT : 0.0F, 
            (paneIndex % 3 == 2)? MESHFORM_AMOUNT : 0.0F, 
        });

        switch(paneIndex){
            case 0: case 1: case 2: genShapes(std::make_pair(rigidTForm, VEC_3F_ONES + amount), std::make_pair(rigidTForm, VEC_3F_ONES - amount));break;
            case 3: case 4: case 5: genShapes(std::make_pair(curveTForm, amount), std::make_pair(curveTForm, amount * -1.0F)); break; // Basic Callback Functions
            case 6: case 7: case 8: genShapes(std::make_pair(pullTForm, amount * 0.33F), std::make_pair(pullTForm, amount * 3.0F)); break; // Advanced Callback Functions
        };
    }
#ifdef TOPL_ENABLE_TEXTURES
    else if (menu == PROGRAM_Paint && isEnable_overlays) {
        for (unsigned d = 0; d < volume.getDepth(); d++) {
            const Sampler_2D* targetTex = getOverlaysScene()->getTexture(std::to_string((d % 8) + 1));
            if (targetTex != nullptr) volume.addSlice(resizeImgOp({ 256, 256 }, targetTex->getImage()), d);
        }
        _renderer->texturizeScene(&scene);
    }
#endif
}

void Meshform_Demo::init(){
    Platform::keyControl.addHandler(std::bind(&Meshform_Demo::onAnyKey, this, std::placeholders::_1));

    // torusActor.isShown = false;
    for(unsigned m = 0; m < 3; m++){
        scene.addGeometry("torus" + std::to_string(m + 1), &torusActors[m]);
        torusActors[m].setRot({0.0F, MATH_PI / 2.0F, 0.0F});

        orbActors[m][0].setPos({ 0.5F, 0.5F, 0.0F });
        scene.addGeometry("trigOrb" + std::to_string(m + 1), &orbActors[m][0]);
        orbActors[m][1].setPos({ -0.5F, -0.5F, 0.0F });
        scene.addGeometry("quadOrb" + std::to_string(m + 1), &orbActors[m][1]);
        orbActors[m][2].setPos({ 0.5F, -0.5F, 0.0F });
        scene.addGeometry("hexOrb" + std::to_string(m + 1), &orbActors[m][2]);
        orbActors[m][3].setPos({ -0.5F, 0.5F, 0.0F });
        scene.addGeometry("decOrb" + std::to_string(m + 1), &orbActors[m][3]);
        // for (unsigned a = 0; a < 4; a++) orbActors[m][a].isShown = m == MESHFORM_INDEX;
#ifdef TOPL_ENABLE_TEXTURES
        scene.addVolumeTex("torus" + std::to_string(m + 1), &volume);
        scene.addVolumeTex("trigOrb" + std::to_string(m + 1), &volume);
        scene.addVolumeTex("quadOrb" + std::to_string(m + 1), &volume);
        scene.addVolumeTex("hexOrb" + std::to_string(m + 1), &volume);
        scene.addVolumeTex("decOrb" + std::to_string(m + 1), &volume);
#endif
    }

    for (unsigned f = 0; f < 4; f++) {
        // freeforms[f]->drawMode = DRAW_Lines;
        // if(MESHFORM_TESS > 0) freeforms[f]->tesselate(MESHFORM_TESS);
        scene.addGeometry("freeform" + std::to_string(f + 1), &freeformActors[f]);
#ifdef TOPL_ENABLE_TEXTURES
        scene.addVolumeTex("freeform" + std::to_string(f + 1), &volume);
#endif
    }

    freeformActors[0].setPos({ 1.5, 0.0F, 0.0F });
    freeformActors[1].setPos({ -1.5, 0.0F, 0.0F });
    freeformActors[2].setPos({ 0.0F, 1.5, 0.0F });
    freeformActors[3].setPos({ 0.0F, -1.5, 0.0F });

    Topl_Factory::switchPipeline(_renderer, _texPipeline);
    Topl_Program::shaderMode = 8;

    _renderer->buildScene(&scene);
#ifdef TOPL_ENABLE_TEXTURES
    _renderer->texturizeScene(&scene);
#endif
}

void Meshform_Demo::loop(double frameTime){
    if (isRotating)
        for (unsigned m = 0; m < 3; m++) {
            torusActors[m].updateRot({ 0.0F, 0.0F, (float)frameTime * 0.0000001F * (m + 1)});
            for (unsigned a = 0; a < 4; a++) {
                Vec3f rotationVec = Vec3f({ (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX });
                rotationVec = rotationVec * 0.001;
                orbActors[m][a].updateRot(rotationVec);
                freeformActors[a].updateRot(rotationVec);
            }
        }

    // torus->drawMode = DRAW_Lines;
#ifdef TOPL_ENABLE_TEXTURES
    _texVShader.setFlip(0);
    // _texVShader.setAntialiasing(0.001F, 10);
    _texVShader.setSlice((_renderer->getFrameCount() % 1024) * (1.0F / 1024.0));
#endif
    _renderer->setDrawMode(DRAW_Triangles);
    _renderer->updateScene(&scene);
    _renderer->drawScene(&scene);
}

MAIN_ENTRY {
    Meshform = new Meshform_Demo(argv[0], BACKEND_GL4);
    Meshform->run();

    delete(Meshform);
    return 0;
}
