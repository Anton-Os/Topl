#include "Sandbox.hpp"

static unsigned pickerVal = 0;
static unsigned short pickerPane = 0;

static Vec3f cursorVec = { 0.0F, 0.0F, 0.0F };
static Vec3f boxVec = { 0.5f, 0.5f, 0.0f };
static Vec3f pyramidVec = { -0.5f, 0.5f, 0.0f };
static Vec3f sphereVec = { -0.5f, -0.5f, 0.0f };
static Vec3f hexVec = { 0.5f, -0.5f, 0.0f };

static void onPress(float x, float y){ cursorVec = { x, y, 0.0F }; }
static void onHover(float x, float y){ if(Platform::mouseControl.getIsHeld().second) cursorVec = { x, y, 0.0F }; }

static void boxPicker(){ std::cout << "Box pressed!" << std::endl; }
static void pyramidPicker(){ std::cout << "Pyramid pressed!" << std::endl; }
static void spherePicker(){ std::cout << "Sphere pressed!" << std::endl; }
static void hexPicker(){ std::cout << "Hexagon pressed!" << std::endl; }

void shapePosUpdates(){
    static unsigned updateCall = 0;

    if(updateCall % 40 < 10) {
        boxVec[0] -= 0.1F; pyramidVec[1] -= 0.1F;
        sphereVec[0] += 0.1F; hexVec[1] += 0.1F;
    }
    else if(updateCall % 40 < 20) {
        boxVec[1] -= 0.1F; pyramidVec[0] += 0.1F;
        sphereVec[1] += 0.1F; hexVec[0] -= 0.1F;
    }
    else if(updateCall % 40 < 30) {
        boxVec[0] += 0.1F; pyramidVec[1] += 0.1F;
        sphereVec[0] -= 0.1F; hexVec[1] -= 0.1F;
    }
    else {
        boxVec[1] += 0.1F; pyramidVec[0] -= 0.1F;
        sphereVec[1] -= 0.1F; hexVec[0] += 0.1F;
    }

    updateCall++;
}

void Sandbox_Demo::init(){
    srand(time(NULL));

    Platform::mouseControl.addCallback(MOUSE_LeftBtn_Down, onPress);
    // Platform::mouseControl.addHoverCallback(onHover);

    _timeline.ticker.addPeriodicEvent(1000, shapePosUpdates);

    boxMesh.scale({ 0.25f, 0.25f, 0.25f});
    boxActor.setPos(boxVec);
    boxActor.pickerFunc = boxPicker;
    scene.addGeometry("Box", &boxActor);
    pyramidMesh.scale({ 0.25f, 0.25f, 0.25f});
    pyramidActor.setPos(pyramidVec);
    pyramidActor.pickerFunc = pyramidPicker;
    scene.addGeometry("Pyramid", &pyramidActor);
    sphereMesh.scale({ 0.25f, 0.25f, 0.25f});
    sphereActor.setPos(sphereVec);
    sphereActor.pickerFunc = spherePicker;
    scene.addGeometry("Sphere", &sphereActor);
    hexMesh.scale({ 0.25f, 0.25f, 0.25f});
    hexActor.setPos(hexVec);
    hexActor.pickerFunc = hexPicker;
    scene.addGeometry("Hex", &hexActor);

#ifdef RASTERON_H
    scene.addTexture("Box", &colorTex1);
    scene.addTexture("Pyramid", &colorTex2);
    scene.addTexture("Sphere", &colorTex3);
    scene.addTexture("Hex", &colorTex4);
#endif

    // chain.configure(&scene);
    // grid.configure(&scene);

    // Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, beamPipeline);
    _renderer->buildScene(&scene);

    layout1.configure(&overlay);
    layout2.configure(&overlay);
    layout2.shift({ 0.55F, 0.0F, 0.0F });
    layout3.configure(&overlay);
    layout3.shift({ -0.55F, 0.0F, 0.0F });

    // Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, flatPipeline);
    _renderer->buildScene(&overlay);
}

void Sandbox_Demo::loop(double frameTime){
    {
        boxActor.setPos(boxVec);
        pyramidActor.setPos(pyramidVec);
        sphereActor.setPos(sphereVec);
        hexActor.setPos(hexVec);

        boxActor.updateRot({ 0.003F * (float)frameTime, 0.0F, 0.0F });
        pyramidActor.updateRot({ -0.003F * (float)frameTime, 0.0F, 0.0F });
        sphereActor.updateRot({ 0.0F, 0.003F * (float)frameTime, 0.0F });
        hexActor.updateRot({ 0.0F, 0.0F, 0.003F * (float)frameTime });

        _renderer->setDrawMode(DRAW_Strip);
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, flatPipeline);
        _renderer->updateScene(&scene);
        _renderer->renderScene(&scene);

        _renderer->setDrawMode(DRAW_Lines);
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, flatPipeline);
        _renderer->updateScene(&scene);
        _renderer->renderScene(&scene);

#ifdef RASTERON_H
    if(Platform::mouseControl.getIsHeld().second) {
        pickerVal = invokePicker(&scene);
        // char hexPickColor[1024];
        // sprintf(hexPickColor, "%X\0", pickerVal);
        // std::cout << "Picker Hex: " << hexPickColor << " ";
    }
#endif
    }

    {
        layout1.shift(cursorVec - layout1.getOrigin());

        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, texPipeline);
        _renderer->updateScene(&overlay);
        _renderer->renderScene(&overlay);
    }
}

int main(int argc, char** argv) {
    Sandbox_Demo demo = Sandbox_Demo(argv[0], BACKEND_GL4);

    demo.run();
    return 0;
}