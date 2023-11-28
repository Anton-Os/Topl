#include "Sandbox.hpp"

static Vec3f cursorVec = { 0.0F, 0.0F, 0.0F };
static Vec3f boxVec = { 0.5f, 0.5f, 0.0f };
static Vec3f pyramidVec = { -0.5f, 0.5f, 0.0f };
static Vec3f sphereVec = { -0.5f, -0.5f, 0.0f };
static Vec3f hexVec = { 0.5f, -0.5f, 0.0f };

static void onPress(float x, float y){ cursorVec = { x, y, 0.0F }; }
static void onHover(float x, float y){ 
    if(Platform::mouseControl.getIsHeld().second) cursorVec = { x, y, 0.0F }; 
}

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

    // Platform::mouseControl.addCallback(MOUSE_LeftBtn_Down, onPress);
    Platform::mouseControl.addHoverCallback(onHover);

    _ticker.addPeriodicEvent(1000, shapePosUpdates);

    boxMesh.scale({ 0.25f, 0.25f, 0.25f});
    boxActor.setPos(boxVec);
    scene.addGeometry("Box", &boxActor);
    pyramidMesh.scale({ 0.25f, 0.25f, 0.25f});
    pyramidActor.setPos(pyramidVec);
    scene.addGeometry("Pyramid", &pyramidActor);
    sphereMesh.scale({ 0.25f, 0.25f, 0.25f});
    sphereActor.setPos(sphereVec);
    scene.addGeometry("Sphere", &sphereActor);
    hexMesh.scale({ 0.25f, 0.25f, 0.25f});
    hexActor.setPos(hexVec);
    scene.addGeometry("Hex", &hexActor);

    // chain.configure(&scene);
    // grid.configure(&scene);

    // Topl_Factory::switchPipeline(TARGET_OpenGL, _renderer, beamPipeline);
    _renderer->buildScene(&scene);

    layout1.configure(&overlay);
    layout2.configure(&overlay);
    layout2.shift({ 0.55F, 0.0F, 0.0F });
    layout3.configure(&overlay);
    layout3.shift({ -0.55F, 0.0F, 0.0F });

    // Topl_Factory::switchPipeline(TARGET_OpenGL, _renderer, flatPipeline);
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
        Topl_Factory::switchPipeline(TARGET_OpenGL, _renderer, beamPipeline);
        _renderer->updateScene(&scene);
        _renderer->renderScene(&scene);

        _renderer->setDrawMode(DRAW_Lines);
        Topl_Factory::switchPipeline(TARGET_OpenGL, _renderer, flatPipeline);
        _renderer->updateScene(&scene);
        _renderer->renderScene(&scene);
    }

    {
        layout1.shift(cursorVec - layout1.getOrigin());

        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(TARGET_OpenGL, _renderer, flatPipeline);
        _renderer->updateScene(&overlay);
        _renderer->renderScene(&overlay);
    }

    // cursorVec = { 0.0F, 0.0F, 0.0F };
}

int main(int argc, char** argv) {
    Sandbox_Demo demo = Sandbox_Demo(argv[0], TARGET_OpenGL);

    demo.run();
    return 0;
}