#include "Modelview.hpp"

#define TOPL_ENABLE_PHYSICS

static Vec3f puppet1_forceVec = VEC_3F_ZERO, puppet2_forceVec = VEC_3F_ZERO, puppet3_forceVec = VEC_3F_ZERO;

void Modelview_Demo::init(){
    Topl_Program::cameraObj.setZoom(0.5F);

    model1.configure(&scene3D);
    model1.shift({ -1.25F, -0.66F, 0.0F });
    model1.rotateAll({ 0.0F, 0.1F, 0.0F });
    model2.configure(&scene3D);
    model2.shift({ 0.0F, -0.99F, 0.0F });
    model3.configure(&scene3D);
    model3.shift({ 1.25F, -0.66F, 0.0F });
    model3.rotateAll({ 0.0F, -0.1F, 0.0F });

    _renderer->buildScene(&scene3D);

    puppet1.configure(&_instance->scene2D);
    puppet1.shift({ 0.0F, 0.5F, 0.0F });
    puppet2.configure(&_instance->scene2D);
    puppet2.shift({ -1.0F, 0.75F, 0.0F });
    puppet3.configure(&_instance->scene2D);
    puppet3.shift({ 1.0F, 0.75F, 0.0F });

    Topl_Program::timeline.addSequence_vec3f(&puppet1_forceVec, std::make_pair(60.0, Vec3f({ 0.0F, 50.0F, 0.0F })));
    Topl_Program::timeline.addSequence_vec3f(&puppet2_forceVec, std::make_pair(60.0, Vec3f({ -50.0F, 0.0F, 0.0F })));
    Topl_Program::timeline.addSequence_vec3f(&puppet3_forceVec, std::make_pair(60.0, Vec3f({ 50.0F, 0.0F, 0.0F })));

    _renderer->buildScene(&scene2D);
    _renderer->texturizeScene(&scene2D);
}

void Modelview_Demo::loop(double frameTime){
    
    {
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, (_renderer->getFrameCount() % 60 < 30)? flatPipeline : beamsPipeline);
        flatVShader.setMode((_renderer->getFrameCount() % 30 < 15)? 0 : 1);
        beamsVShader.setMode((_renderer->getFrameCount() % 30 < 15)? 0 : 3);

        model1.rotateAll({ 0.0, 0.0F, (float)frameTime / -500.0F });
        model2.rotateAll({ 0.0, 0.0F, (_renderer->getFrameCount() % 120 < 60)? (float)frameTime / 1000.0F : (float)frameTime / -1000.0F });
        model3.rotateAll({ 0.0, 0.0F, (float)frameTime / 500.0F });

        _renderer->setDrawMode((_renderer->getFrameCount() % 180 > 120)? DRAW_Strip : (_renderer->getFrameCount() % 180 > 60)? DRAW_Lines : DRAW_Points);

        _renderer->updateScene(&scene3D);
        _renderer->drawScene(&scene3D);
    }

    {
#ifdef TOPL_ENABLE_PHYSICS
        if(_renderer->getFrameCount() % 10 == 0){
            scene2D.addForce(puppet1.getGeoActor(rand() % 6)->getName(), puppet1_forceVec * ((_renderer->getFrameCount() % 120 < 60)? 10.0 : -10.0));
            scene2D.addForce(puppet2.getGeoActor(rand() % 6)->getName(), puppet2_forceVec * ((_renderer->getFrameCount() % 120 < 60)? 10.0 : -10.0));
            scene2D.addForce(puppet3.getGeoActor(rand() % 6)->getName(), puppet3_forceVec * ((_renderer->getFrameCount() % 120 < 60)? 10.0 : -10.0));
        }
        scene2D.resolvePhysics();
#endif

        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, texPipeline);
        _renderer->setDrawMode(DRAW_Triangles);

        _renderer->updateScene(&scene2D);
        _renderer->drawScene(&scene2D);
    }
}

int main(int argc, char** argv) {
    _instance = new Modelview_Demo(argv[0], BACKEND_GL4);
    _instance->run();

    delete(_instance);
    return 0;
}