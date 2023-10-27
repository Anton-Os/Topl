#include "Sandbox.hpp"

void Sandbox_Demo::init(){
    srand(time(NULL));

    boxMesh.scale({ 0.25f, 0.25f, 0.25f});
    boxActor.setPos({ 0.5f, 0.5f, 0.0f });
    scene.addGeometry("Box", &boxActor);
    pyramidMesh.scale({ 0.25f, 0.25f, 0.25f});
    pyramidActor.setPos({ -0.5f, 0.5f, 0.0f });
    scene.addGeometry("Pyramid", &pyramidActor);
    sphereMesh.scale({ 0.25f, 0.25f, 0.25f});
    sphereActor.setPos({ -0.5f, -0.5f, 0.0f });
    scene.addGeometry("Sphere", &sphereActor);
    hexMesh.scale({ 0.25f, 0.25f, 0.25f});
    hexActor.setPos({ 0.5f, -0.5f, 0.0f });
    scene.addGeometry("Hex", &hexActor);

    Topl_Factory::switchPipeline(TARGET_DirectX11, _renderer, beamPipeline);
    _renderer->buildScene(&scene);
}

void Sandbox_Demo::loop(double frameTime){
    logMessage("Frame: " + std::to_string(_renderer->getFrameCount()));

    boxActor.updateRot({ 0.01F * (float)frameTime, 0.0F, 0.0F });
    pyramidActor.updateRot({ -0.01F * (float)frameTime, 0.0F, 0.0F });
    sphereActor.updateRot({ 0.0F, 0.01F * (float)frameTime, 0.0F });
    hexActor.updateRot({ 0.0F, 0.0F, 0.01F * (float)frameTime });

    _renderer->updateScene(&scene);
    _renderer->renderScene(&scene);
}

int main(int argc, char** argv) {
    Sandbox_Demo demo = Sandbox_Demo(argv[0], TARGET_DirectX11);

    demo.run();
    return 0;
}