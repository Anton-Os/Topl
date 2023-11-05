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

    // chain.configure(&scene);
    // grid.configure(&scene);

    Topl_Factory::switchPipeline(TARGET_DirectX11, _renderer, beamPipeline);
    _renderer->buildScene(&scene);

    layout1.configure(&overlay);
    // layout2.configure(&overlay);
    // layout3.configure(&overlay);

    Topl_Factory::switchPipeline(TARGET_DirectX11, _renderer, flatPipeline);
    _renderer->buildScene(&overlay);

}

void Sandbox_Demo::loop(double frameTime){
    {
        boxActor.updateRot({ 0.01F * (float)frameTime, 0.0F, 0.0F });
        pyramidActor.updateRot({ -0.01F * (float)frameTime, 0.0F, 0.0F });
        sphereActor.updateRot({ 0.0F, 0.01F * (float)frameTime, 0.0F });
        hexActor.updateRot({ 0.0F, 0.0F, 0.01F * (float)frameTime });

        Topl_Factory::switchPipeline(TARGET_DirectX11, _renderer, beamPipeline);
        _renderer->updateScene(&scene);
        _renderer->renderScene(&scene);
    }

    {
        Topl_Factory::switchPipeline(TARGET_DirectX11, _renderer, flatPipeline);
        _renderer->updateScene(&overlay);
        _renderer->renderScene(&overlay);
    }
}

int main(int argc, char** argv) {
    Sandbox_Demo demo = Sandbox_Demo(argv[0], TARGET_DirectX11);

    demo.run();
    return 0;
}