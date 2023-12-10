#include "Sandbox.hpp"

static unsigned texMode = 0;

#ifdef RASTERON_H
Img_Base texture1 = Img_Base(0xFF0000FF); Img_Base texture2 = Img_Base(0xFF00FF00);
Img_Base texture3 = Img_Base(0xFFFFFF00); Img_Base texture4 = Img_Base(0xFFFF00FF);
#endif

static void onHover(float x, float y){ 
    if(Platform::mouseControl.getIsMouseDown().second) {
        Topl_Main::cursorPos = { x, y, 0.0F }; 

        if(Topl_Main::pickerObj != NO_PICKER_OBJ){
            std::cout << "Topl_Main picker object: " << Topl_Main::pickerObj->getName() << std::endl;

            if(Topl_Main::pickerObj->getId() == instance->boxActor.getId()) instance->boxActor.setPos(Topl_Main::cursorPos);
            else if(Topl_Main::pickerObj->getId() == instance->pyramidActor.getId()) instance->pyramidActor.setPos(Topl_Main::cursorPos);
            else if(Topl_Main::pickerObj->getId() == instance->sphereActor.getId()) instance->sphereActor.setPos(Topl_Main::cursorPos);
            else if(Topl_Main::pickerObj->getId() == instance->hexActor.getId()) instance->hexActor.setPos(Topl_Main::cursorPos);
            // else if(isLayoutSelect) layoutVec = layoutVec + (Topl_Main::cursorPos - layoutVec);
        }
    } else Topl_Main::pickerObj = NO_PICKER_OBJ;
}

static void boxPass(){ /* Add body for render block */ }
static void pyramidPass(){ /* Add body for render block */ }
static void spherePass(){ /* Add body for render block */ }
static void hexPass(){ /* Add body for render block */ }

void texModeUpdates(){ 
    static unsigned updateCall = 0;

    if(texMode < MAX_TEX_BINDINGS) texMode++; else texMode = 0; 

    if(updateCall % 4 == 0){
        texture1.setColorImage(0xFF0000FF); texture2.setColorImage(0xFF00FF00);
        texture3.setColorImage(0xFFFFFF00); texture4.setColorImage(0xFFFF00FF);
    } else if(updateCall % 4 == 1){
        texture1.setColorImage(0xFFFF00FF); texture2.setColorImage(0xFF0000FF);
        texture3.setColorImage(0xFF00FF00); texture4.setColorImage(0xFFFFFF00);
    } else if(updateCall % 4 == 2){
        texture1.setColorImage(0xFFFFFF00); texture2.setColorImage(0xFFFF00FF);
        texture3.setColorImage(0xFF0000FF); texture4.setColorImage(0xFF00FF00);
    } else if(updateCall % 4 == 3){
        texture1.setColorImage(0xFF00FF00); texture2.setColorImage(0xFFFFFF00);
        texture3.setColorImage(0xFFFF00FF); texture4.setColorImage(0xFF0000FF);
    }

    updateCall++;
}

void Sandbox_Demo::init(){
    srand(time(NULL));

    // Platform::mouseControl.addCallback(MOUSE_LeftBtn_Down, onPress);
    // Platform::mouseControl.addCallback(MOUSE_RightBtn_Down, onPress);
    Platform::mouseControl.addHoverCallback(onHover);

    _timeline.ticker.addPeriodicEvent(1000, texModeUpdates);

    boxMesh.scale({ 0.25f, 0.25f, 0.25f});
    boxActor.setPos({ 0.5f, 0.5f, 0.0f });
    boxActor.shaderFunc = boxPass;
    scene.addGeometry("Box", &boxActor);
    pyramidMesh.scale({ 0.25f, 0.25f, 0.25f});
    pyramidActor.setPos({ -0.5f, 0.5f, 0.0f });
    pyramidActor.shaderFunc = pyramidPass;
    scene.addGeometry("Pyramid", &pyramidActor);
    sphereMesh.scale({ 0.25f, 0.25f, 0.25f});
    sphereActor.setPos({ -0.5f, -0.5f, 0.0f });
    sphereActor.shaderFunc = spherePass;
    scene.addGeometry("Sphere", &sphereActor);
    hexMesh.scale({ 0.25f, 0.25f, 0.25f});
    hexActor.setPos({ 0.5f, -0.5f, 0.0f });
    hexActor.shaderFunc = hexPass;
    scene.addGeometry("Hex", &hexActor);

#ifdef RASTERON_H
    scene.addTexture("Box", &texture1);
    scene.addTexture("Pyramid", &texture2);
    scene.addTexture("Sphere", &texture3);
    scene.addTexture("Hex", &texture4);
#endif

    // chain.configure(&scene);
    // grid.configure(&scene);

    // Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, beamPipeline);
    _renderer->buildScene(&scene);

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
#ifdef RASTERON_H
        if(_renderer->getFrameCount() % 60 == 0){
            _renderer->texturize(&scene);
            _renderer->texturize(&overlay);
        }
#endif

        boxActor.updateRot({ 0.003F * (float)frameTime, 0.0F, 0.0F });
        pyramidActor.updateRot({ -0.003F * (float)frameTime, 0.0F, 0.0F });
        sphereActor.updateRot({ 0.0F, 0.003F * (float)frameTime, 0.0F });
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
        invokePicker(&overlay);
    }
#endif
    }

    // Textured Render

    {
        _renderer->setDrawMode(DRAW_Points);
        Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, texPipeline);
        texVShader.setTexMode(texMode);
        _renderer->updateScene(&scene);
        _renderer->renderScene(&scene);

        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_DX11, _renderer, texPipeline);
        texVShader.setTexMode(texMode);
        _renderer->updateScene(&overlay);
        _renderer->renderScene(&overlay);
    }
}

int main(int argc, char** argv) {
    instance = new Sandbox_Demo(argv[0], BACKEND_DX11);

    instance->run();
    delete(instance);
    return 0;
}