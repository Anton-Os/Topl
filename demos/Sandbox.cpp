#include "Sandbox.hpp"


static int mode = SANDBOX_MODE_MOVE;
static unsigned texMode = 0;
static Vec3f texScroll = { 0.0, 0.0, 0.0 };

#ifdef RASTERON_H
unsigned boxImgOp(double x, double y){ return (sin(x * 20) > 0.5)? 0xFF0000FF : 0xFFFFFF00; }
unsigned pyramidImgOp(double x, double y){ return (cos(y * 20) < 0.5)? 0xFFFF0000 : 0xFF00FFFF; }
unsigned sphereImgOp(double x, double y){ return (tan(x * y) > 0.25 && tan(x * y) < 0.75)? 0xFF8833CC : 0xFF88CC33; }
unsigned hexImgOp(double x, double y){ return (x > 0.4 && x < 0.6 && y > 0.4 && y < 0.6)? 0xFF3333333 : 0xFFEEEEEE; }

Rasteron_Image* boxImg = mapImgOp({1024, 1024}, boxImgOp);
Rasteron_Image* pyramidImg = mapImgOp({1024, 1024}, pyramidImgOp);
Rasteron_Image* sphereImg = mapImgOp({1024, 1024}, sphereImgOp);
Rasteron_Image* hexImg = mapImgOp({1024, 1024}, hexImgOp);

Img_Base boxTex = Img_Base(boxImg); Img_Base pyramidTex = Img_Base(pyramidImg);
Img_Base sphereTex = Img_Base(sphereImg); Img_Base hexTex = Img_Base(hexImg);
#endif

static void updateActor(Geo_Actor* actor){
    switch(mode){
        case SANDBOX_MODE_HIDE: break; // TODO: hide actor
        case SANDBOX_MODE_MOVE: actor->setPos(Topl_Program::cursorPos); break;
        case SANDBOX_MODE_ROTATE: break; // TODO: rotate actor
        case SANDBOX_MODE_SCALE: break; // TODO: scale actor
        case SANDBOX_MODE_REWIND: break; // TODO: rewind time
        case SANDBOX_MODE_PLAYPAUSE: break; // TODO: play or pause time
        case SANDBOX_MODE_FORWARD: break; // TODO: fast forward time
        case SANDBOX_MODE_PAN: break; // TODO: pan scene
        case SANDBOX_MODE_LOOK: break; // TODO: rotate scene
        case SANDBOX_MODE_ZOOM: break; // TODO: zoom scene
        default: actor->setPos(Topl_Program::cursorPos);
    }
}

static void onAnyKey(char k){ 
    if(isdigit(k)){ 
        mode = k - '0'; // conversion from char to decimal
        std::cout << "Key mode: " << std::to_string(mode) << std::endl;
    } 
    else if(toupper(k) == 'W') Topl_Program::cameraObj.updatePos({ 0.0, 0.1F, 0.0 });
    else if(toupper(k) == 'S') Topl_Program::cameraObj.updatePos({ 0.0, -0.1F, 0.0 });
    else if(toupper(k) == 'A') Topl_Program::cameraObj.updatePos({ -0.1F, 0.0, 0.0 });
    else if(toupper(k) == 'D') Topl_Program::cameraObj.updatePos({ 0.1F, 0.0, 0.0 });
    else if(toupper(k) == 'Q') Topl_Program::cameraObj.updateRot({ -0.1F, 0.0, 0.0 });
    else if(toupper(k) == 'E') Topl_Program::cameraObj.updateRot({ 0.1F, 0.0, 0.0 });
} // sets sandbox mode

static void onHover(float x, float y){ 
    if(Platform::mouseControl.getIsMouseDown().second) {
        Topl_Program::cursorPos = { x, y, 0.0F }; 

        if(Topl_Program::pickerObj != NO_PICKER_OBJ){
            std::cout << "Topl_Program picker object: " << Topl_Program::pickerObj->getName() << std::endl;

            if(Topl_Program::pickerObj->getId() == _instance->boxActor.getId()) {
                // _instance->boxActor.setPos(Topl_Program::cursorPos);
                updateActor(&_instance->boxActor);
                boxTex.setColorImage(0xFF00FF00);
            } else if(Topl_Program::pickerObj->getId() == _instance->pyramidActor.getId()){
                // _instance->pyramidActor.setPos(Topl_Program::cursorPos);
                updateActor(&_instance->pyramidActor);
                pyramidTex.setColorImage(0xFF00FF00);
            } else if(Topl_Program::pickerObj->getId() == _instance->sphereActor.getId()){
                // _instance->sphereActor.setPos(Topl_Program::cursorPos);
                updateActor(&_instance->sphereActor);
                sphereTex.setColorImage(0xFF00FF00);
            } else if(Topl_Program::pickerObj->getId() == _instance->hexActor.getId()){ 
                // _instance->hexActor.setPos(Topl_Program::cursorPos);
                updateActor(&_instance->hexActor);
                hexTex.setColorImage(0xFF00FF00);
            }
            // else if(isLayoutSelect) layoutVec = layoutVec + (Topl_Program::cursorPos - layoutVec);
        }
    } else {
        Topl_Program::pickerObj = NO_PICKER_OBJ;
        boxTex.setImage(boxImg); pyramidTex.setImage(pyramidImg);
        sphereTex.setImage(sphereImg); hexTex.setImage(hexImg);
    }
}

void texModeCycle(){ (texMode < 8)? texMode++ : texMode = 0; }
void texScrollCycle(){ texScroll[0] += 0.05; }

static void box_shadercall(Topl_EntryShader* shader){ /* Add body for render block */ }
static void pyramid_shadercall(Topl_EntryShader* shader){ /* Add body for render block */ }
static void sphere_shadercall(Topl_EntryShader* shader){ /* Add body for render block */ }
static void hex_shadercall(Topl_EntryShader* shader){ /* Add body for render block */ }

void Sandbox_Demo::init(){
    srand(time(NULL));

    Platform::keyControl.addAnyCallback(onAnyKey);
    // Platform::mouseControl.addCallback(MOUSE_LeftBtn_Down, onPress);
    // Platform::mouseControl.addCallback(MOUSE_RightBtn_Down, onPress);
    Platform::mouseControl.addHoverCallback(onHover);

    _timeline.ticker.addPeriodicEvent(1000, texModeCycle);
    _timeline.ticker.addPeriodicEvent(50, texScrollCycle);

    boxMesh.scale({ 0.25f, 0.25f, 0.25f});
    boxActor.setPos({ 0.5f, 0.5f, 0.0f });
    boxActor.shaderFunc = box_shadercall;
    scene.addGeometry("Box", &boxActor);
    details.addGeometry("Box", &boxActor);
    pyramidMesh.scale({ 0.25f, 0.25f, 0.25f});
    pyramidActor.setPos({ -0.5f, 0.5f, 0.0f });
    pyramidActor.shaderFunc = pyramid_shadercall;
    scene.addGeometry("Pyramid", &pyramidActor);
    details.addGeometry("Pyramid", &pyramidActor);
    sphereMesh.scale({ 0.25f, 0.25f, 0.25f});
    sphereActor.setPos({ -0.5f, -0.5f, 0.0f });
    sphereActor.shaderFunc = sphere_shadercall;
    scene.addGeometry("Sphere", &sphereActor);
    details.addGeometry("Sphere", &sphereActor);
    hexMesh.scale({ 0.25f, 0.25f, 0.25f});
    hexActor.setPos({ 0.5f, -0.5f, 0.0f });
    hexActor.shaderFunc = hex_shadercall;
    scene.addGeometry("Hex", &hexActor);
    details.addGeometry("Hex", &hexActor);

#ifdef RASTERON_H
    scene.addTexture("Box", &boxTex);
    scene.addTexture("Pyramid", &pyramidTex);
    scene.addTexture("Sphere", &sphereTex);
    scene.addTexture("Hex", &hexTex);
#endif

    // chain.configure(&scene);
    // grid.configure(&scene);

    Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, texPipeline);
    _renderer->buildScene(&scene);
    Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, beamPipeline);
    _renderer->buildScene(&details);
    
    layout1.configure(&overlay);
    layout2.configure(&overlay);
    layout2.shift({ 0.55, 0.0, 0.0 });
    layout3.configure(&overlay);
    layout3.shift({ -0.55, 0.0, 0.0 });

    // Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, flatPipeline);
    _renderer->buildScene(&overlay);
}

void Sandbox_Demo::loop(double frameTime){
    // Object Updates
    
    {
        boxActor.updateRot({ 0.003F * (float)frameTime, 0.0F });
        pyramidActor.updateRot({ 0.0F, 0.0F, 0.003F * (float)frameTime });
        sphereActor.updateRot({ -0.003F * (float)frameTime, -0.003F * (float)frameTime, -0.003F * (float)frameTime });
        hexActor.updateRot({ 0.0F, 0.0F, 0.003F * (float)frameTime });

        // layout.shift(layoutVec - layout.getOrigin());
    }

    // Flat Render

    {
        _renderer->setDrawMode(DRAW_Strip);
        Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, flatPipeline);
        _renderer->updateScene(&scene);
        _renderer->renderScene(&scene);

        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, flatPipeline);
        _renderer->updateScene(&overlay);
        _renderer->renderScene(&overlay);

#ifdef RASTERON_H
        if(Platform::mouseControl.getIsMouseDown().second) {
            invokePicker(&scene);
            _renderer->texturize(&scene);
            invokePicker(&overlay);
            _renderer->texturize(&overlay);
        }
#endif
    }

    // Textured Render

    _renderer->clearView();

    {
        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, beamPipeline);
        beamVShader.setMode(0);
        texVShader.setMode(0);
        _renderer->updateScene(&scene);
        _renderer->renderScene(&scene);

        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, texPipeline);
        texVShader.setMode(texMode);
        texVShader.setTexScroll(texScroll);
        _renderer->updateScene(&overlay);
        _renderer->renderScene(&overlay);
    }
}

int main(int argc, char** argv) {
    _instance = new Sandbox_Demo(argv[0], BACKEND_DX11);

    _instance->run();
    delete(_instance);
#ifdef RASTERON_H
    dealloc_image(boxImg);
    dealloc_image(pyramidImg);
    dealloc_image(sphereImg);
    dealloc_image(hexImg);
#endif
    return 0;
}