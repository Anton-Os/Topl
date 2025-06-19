#include "Meshform.hpp"

static bool isRotating = true;

// Callbacks

Vec3f rigidTForm(Vec3f target, Vec3f amount){
    static unsigned index = 0; 
    index++;
    return (index % 6 * (MESHFORM_TESS + 1) == 0)? target * amount : target;
}

Vec3f curveTForm(Vec3f target, Vec3f amount){
    static unsigned index = 0; 
    index++;

    return Vec3f({
        target.data[0] * (1.0F + (sinf(fabs(1.0F - amount.data[0]) * index) * MESHFORM_CURVE)),
        target.data[1] * (1.0F + (sinf(fabs(1.0F - amount.data[1]) * index) * MESHFORM_CURVE)),
        target.data[2] * (1.0F + (sinf(fabs(1.0F - amount.data[2]) * index) * MESHFORM_CURVE))
    });
}

Vec3f pullTForm(Vec3f target, Vec3f amount){
    Vec3f diffVec = amount - target;

    return (diffVec.len() < MESHFORM_SIZE * 3)? target + (amount * 0.33) : target;
}

Geo_Vertex vertexTform(const Geo_Vertex& vertex, unsigned primID, unsigned invocation){
    Vec3f randVec = Vec3f({ (((float)rand() / (float)RAND_MAX) - 1.0F) * 0.1F, (((float)rand() / (float)RAND_MAX) - 1.0F) * 0.1F, (((float)rand() / (float)RAND_MAX) - 1.0F) * 0.1F });
    return Geo_Vertex(vertex.position + randVec); // test
    // return (primID == 0)? vertex : Geo_Vertex(vertex.position * (primID % 3));
}

Geo_Vertex distanceTform(const Geo_Vertex& vertex, unsigned primID, unsigned invocation){
    float dist = vertex.position.len();
    return Geo_Vertex(vertex.position * dist); // test
    // return (primID == 0)? vertex : Geo_Vertex(vertex.position * (primID % 3));
}

Geo_Vertex midpointTform(const Geo_Vertex& vertex, const Geo_Vertex& midpoint, unsigned primID, unsigned invocation){
    Vec3f randVec = Vec3f({ (((float)rand() / (float)RAND_MAX) - 1.0F) * 0.1F, (((float)rand() / (float)RAND_MAX) - 1.0F) * 0.1F, (((float)rand() / (float)RAND_MAX) - 1.0F) * 0.1F });
    return Geo_Vertex(midpoint.position - randVec); // test
    // return (primID != 0)? vertex : Geo_Vertex(midpoint.position);
}

// Class Functions

void Meshform_Demo::onAnyKey(char key){
    static Vec3f incVec = Vec3f({ MESHFORM_INC, MESHFORM_INC, MESHFORM_INC });
    static Vec3f decVec = Vec3f({ MESHFORM_DEC, MESHFORM_DEC, MESHFORM_DEC });
    switch(tolower(key)){
        case 'i': for(unsigned m = 0; m < 3; m++) for(unsigned a = 0; a < 4; a++) orbActors[m][a].isShown = m == 0; break;
        case 'o': for(unsigned m = 0; m < 3; m++) for(unsigned a = 0; a < 4; a++) orbActors[m][a].isShown = m == 1; break;
        case 'p': for(unsigned m = 0; m < 3; m++) for(unsigned a = 0; a < 4; a++) orbActors[m][a].isShown = m == 2; break;
        /* case 'j': genShapes(MESHFORM_TESS + 1, std::make_pair(nullptr, decVec), std::make_pair(nullptr, incVec)); break;
        case 'k': genShapes(MESHFORM_TESS + 1, std::make_pair(curveTForm, incVec), std::make_pair(rigidTForm, incVec)); break;
        case 'l': genShapes(MESHFORM_TESS + 1, std::make_pair(pullTForm, decVec * -1.0F), std::make_pair(pullTForm, decVec)); break; */
#ifdef RASTERON_H
        case 'b': genTex3D(MESHFORM_GRADIENT, RAND_COLOR(), RAND_COLOR()); break;
        case 'n': genTex3D(MESHFORM_LINES, RAND_COLOR(), RAND_COLOR()); break;
        case 'm': genTex3D(MESHFORM_CHECKER, RAND_COLOR(), RAND_COLOR()); break;
#endif
    }

    if(isspace(key)) isRotating = !isRotating;
}

#ifdef RASTERON_H
void Meshform_Demo::genTex3D(unsigned short mode, unsigned color1, unsigned color2){
    if(textureThread != nullptr)
        if(textureThread->joinable()){
            textureThread->join();
            delete textureThread;
        } else return logMessage(MESSAGE_Exclaim, "Texture thread still running!");

    textureThread = new std::thread([this](unsigned short m, unsigned c1, unsigned c2){
        unsigned width = 1; unsigned height = 1;
        for(unsigned d = 0; d < volumeImg.getDepth(); d++){
            (d % 2 == 0)? width++ : height++;
            Rasteron_Image* sliceImg;
            switch(m){
                case MESHFORM_LINES: sliceImg = linedImgOp({ 256, 256 }, c1, c2, width, 0.0); break;
                case MESHFORM_CHECKER: sliceImg = checkeredImgOp({ 256, 256 }, { width, height, c1, c2 }); break;
                case MESHFORM_NOISE: sliceImg = noiseImgOp_diff({ 256, 256 }, { width, height, c1, c2 }, 1); break;
                default: sliceImg = solidImgOp({ 256, 256 }, blend_colors(c1, c2, d / 256.0)); break;
            }
            volumeImg.addSlice(sliceImg, d);
            RASTERON_DEALLOC(sliceImg);
        }
    }, mode, color1, color2);

    _renderer->texturizeScene(&scene);
}
#endif

void Meshform_Demo::genShapes(std::pair<vTformCallback, Vec3f> transform1, std::pair<vTformCallback, Vec3f> transform2){
    geometryThread = new std::thread([this](Geo_Orb* orbMehes[4][3], std::pair<vTformCallback, Vec3f> tform1, std::pair<vTformCallback, Vec3f> tform2){
            if(tform1.first != nullptr && tform2.first != nullptr)
                for(unsigned o = 0; o < 4; o++)
                    for(unsigned m = 1; m < 3; m++) // no tranform for first shape, transforms 1 and 2 for other shapes
                        orbs[o][m]->modify((o % 2 == 1)? tform1.first : tform2.first, (o % 2 == 1)? tform1.second : tform2.second);
        }, orbs, transform1, transform2
    );
    std::cout << "Geometry rebuilding" << std::endl;
    rebuildGeometry(geometryThread);
}

void Meshform_Demo::rebuildGeometry(std::thread* thread){
    if(thread != nullptr){
        thread->join(); // TODO: Perform blocking call
        delete thread;

        std::cout << "Building scene" << std::endl;
        _renderer->buildScene(&scene);
    }
}

void Meshform_Demo::renderRecursive(Geo_Actor* actor, TFORM_Type type, unsigned short count){
    Vec3f transform = VEC_3F_RAND; // = *(actor->getSize());
    switch(type){
        case TFORM_Shift: transform = *(actor->getPos()); break;
        case TFORM_Rotate: transform = *(actor->getRot()); break;
        case TFORM_Scale: transform = *(actor->getSize()); break;
    }
    Vec3f texScale = Vec3f({ 1.0F, 1.0F, 1.0F });

    for(unsigned c = 0; c < count; c++){ // inscribed objects
        float inc = 1.0F - ((1.0F / count) * (c + 1));
        switch(type){
            case TFORM_Shift: actor->setPos(transform * inc); break;
            case TFORM_Rotate: actor->setRot(transform * inc); break;
            case TFORM_Scale: actor->setSize(transform * inc); break;
        }
        _texVShader.setTexScale(texScale * inc);
        _renderer->update(actor);
        _renderer->draw(actor);
    }

    switch(type){
        case TFORM_Shift: actor->setPos(transform); break;
        case TFORM_Rotate: actor->setRot(transform); break;
        case TFORM_Scale: actor->setSize(transform); break;
    }
    _texVShader.setTexScale(texScale);
}

void Meshform_Demo::onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex){
    if(menu == PROGRAM_AppBar){
        Vec3f amount = Vec3f({ (paneIndex % 3 == 0)? MESHFORM_AMOUNT : 0.0F, (paneIndex % 3 == 1)? MESHFORM_AMOUNT : 0.0F, (paneIndex % 3 == 2)? MESHFORM_AMOUNT : 0.0F, });

        switch(paneIndex){
            case 0: case 1: case 2: 
                genShapes(std::make_pair(rigidTForm, VEC_3F_ONES + amount), std::make_pair(rigidTForm, VEC_3F_ONES - amount));
            break; // Tesselation & Built-In Functions
            case 3: case 4: case 5: break; 
                genShapes(std::make_pair(curveTForm, amount), std::make_pair(curveTForm, amount * -1.0F));
            break;    // Basic Callback Functions
            case 6: case 7: case 8: 
                genShapes(std::make_pair(pullTForm, amount * 0.33F), std::make_pair(pullTForm, amount * 3.0F));
            break; // Advanced Callback Functions
        };
    }
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
#ifdef RASTERON_H
        scene.addVolumeTex("trigOrb" + std::to_string(m), &volumeImg);
        scene.addVolumeTex("quadOrb" + std::to_string(m), &volumeImg);
        scene.addVolumeTex("hexOrb" + std::to_string(m), &volumeImg);
        scene.addVolumeTex("decOrb" + std::to_string(m), &volumeImg);
#endif
    }

    Topl_Factory::switchPipeline(_renderer, _texPipeline);
    Topl_Program::shaderMode = 8;

    _renderer->buildScene(&scene);
#ifdef RASTERON_H
    _renderer->texturizeScene(&scene);
#endif
}

void Meshform_Demo::loop(double frameTime){
    for(unsigned a = 0; a < 4; a++)
        if(isRotating){
            Vec3f rotationVec = Vec3f({ (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX });
            rotationVec = rotationVec * 0.001;
            for(unsigned o = 0; o < 3; o++) orbActors[o][a].updateRot(rotationVec);
            if(a == 0) torusActor.updateRot(rotationVec * -1.0F);
        }

    // torus->drawMode = DRAW_Lines;
#ifdef RASTERON_H
    _texVShader.setFlip(0);
    // _texVShader.setAntialiasing(0.001F, 10);
    _texVShader.setSlice((_renderer->getFrameCount() % 256) * (1.0F / 256.0));
#endif
    _renderer->setDrawMode(DRAW_Triangles);
    _renderer->updateScene(&scene);
    _renderer->drawScene(&scene);

    renderRecursive(&torusActor, TFORM_Shift, 8);
    for(unsigned a = 0; a < 4; a++)
        for(unsigned o = 0; o < 3; o++) 
            if(orbActors[o][a].isShown) renderRecursive(&orbActors[o][a], TFORM_Rotate, 3);
}

MAIN_ENTRY{
    _DEMO = new Meshform_Demo(argv[0], BACKEND_GL4);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}
