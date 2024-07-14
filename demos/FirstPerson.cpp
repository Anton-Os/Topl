#include "FirstPerson.hpp"

#define TOPL_ENABLE_PHYSICS

static Vec3f puppet1_forceVec = VEC_3F_ZERO, puppet2_forceVec = VEC_3F_ZERO, puppet3_forceVec = VEC_3F_ZERO;

void camPivotY1(){ Topl_Program::cameraObj.updateRot({ 0.0F, 0.01F, 0.0F }); }
void camPivotY2(){ Topl_Program::cameraObj.updateRot({ 0.0F, -0.01F, 0.0F }); }
void camPivotZ1(){ Topl_Program::cameraObj.updateRot({ 0.0F, 0.0F, 0.01F }); }
void camPivotZ2(){ Topl_Program::cameraObj.updateRot({ 0.0F, 0.0F, -0.01F }); }
void camProjId(){ Topl_Program::cameraObj.setProjMatrix(Projection(PROJECTION_None, 1.0F).genProjMatrix()); }
void camProjOrtho(){ Topl_Program::cameraObj.setProjMatrix(Projection(PROJECTION_Ortho, 1.0F).genProjMatrix()); }
void camProjPerspective(){ Topl_Program::cameraObj.setProjMatrix(Projection(PROJECTION_Perspective, 1.0F).genProjMatrix()); }
void camProjExperimental(){ Topl_Program::cameraObj.setProjMatrix(Projection(PROJECTION_Experimental, 1.0F).genProjMatrix()); }

/* void projCallback(){
    static unsigned invocation = 0;
    Topl_Program::cameraObj.setProjMatrix(Projection((invocation % 2 == 0)? PROJECTION_Ortho : PROJECTION_Perspective, 1.0F).genProjMatrix());
    invocation++;
} */

void FirstPerson_Demo::init(){
    // Platform::keyControl.addCallback('p', projCallback);
    Platform::keyControl.addCallback('l', camPivotY1);
    Platform::keyControl.addCallback('k', camPivotY2);
    Platform::keyControl.addCallback('m', camPivotZ1);
    Platform::keyControl.addCallback('n', camPivotZ2);
    Platform::keyControl.addCallback('u', camProjExperimental);
    Platform::keyControl.addCallback('i', camProjId);
    Platform::keyControl.addCallback('o', camProjOrtho);
    Platform::keyControl.addCallback('p', camProjPerspective);

    Topl_Program::cameraObj.setZoom(0.5F);
    Topl_Program::cameraObj.setPos({ 0.0F, -0.5F, DEFAULT_Z });
    // Topl_Program::cameraObj.setProjMatrix(Projection(PROJECTION_Ortho, 1.0, 1.0, 1.0, 1.0, 10.0, 10.0).genProjMatrix());
    // Topl_Program::cameraObj.setProjMatrix(Projection(PROJECTION_Perspective, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0).genProjMatrix());

    _renderer->setPipeline(_texPipeline);

    floor.setPos({ 0.0F, -3.0F, 0.0F});
    floor.setSize({ FLOOR_SIZE, 1.0F, FLOOR_SIZE });
    floor.setRot({ 0.0F, MATH_HALF_PI, 0.0F });
    scene3D.addGeometry("floor", &floor);
    for(unsigned short p = 0; p < 4; p++){ 
        pillars[p].setSize({ 1.0F, 10.0F, 1.0F });
        pillars[p].setRot({ 0.0F, MATH_HALF_PI, 0.0F });
        scene3D.addGeometry("pillar" + std::to_string(p), &pillars[p]);
    }
    pillars[0].setPos({ 10.0F, 2.5F, 10.0F });
    pillars[1].setPos({ -10.0F, 2.5F, 10.0F });
    pillars[2].setPos({ 10.0F, 2.5F, -10.0F });
    pillars[3].setPos({ -10.0F, 2.5F, -10.0F });

    for(unsigned m = 0; m < 5; m++) models[m].configure(&scene3D);
    models[0].shift({ -1.5F, -1.75F, 1.0F });
    models[1].shift({ 0.0F, -1.75F, 0.0F });
    models[2].shift({ 1.5F, -1.75, 1.0F });
    models[3].shift({ -1.5F, -1.75F, -1.0F });
    models[4].shift({ 1.5F, -1.75, -1.0F });

    _renderer->buildScene(&scene3D);

    puppet1.configure(&_DEMO->scene2D);
    puppet1.shift({ 0.0F, -1.0, -5.0F });
    puppet2.configure(&_DEMO->scene2D);
    puppet2.shift({ -1.0F, -1.0, -5.0F });
    puppet3.configure(&_DEMO->scene2D);
    puppet3.shift({ 1.0F, -1.0, -5.0F });

    /* Phys_Connector puppet1_anchor, puppet2_anchor, puppet3_anchor;
    Vec3f anchorPos1 = Vec3f({0.0F, 0.5F, 0.0F}); Vec3f anchorPos2 = Vec3f({-1.0F, 0.75F, 0.0F}); Vec3f anchorPos3 = Vec3f({1.0F, 0.75F, 0.0F});
    scene2D.addAnchor(&puppet1_anchor, puppet1.getGeoActor(PUPPET_Body)->getName(), &anchorPos1);
    scene2D.addAnchor(&puppet2_anchor, puppet2.getGeoActor(PUPPET_Body)->getName(), &anchorPos2);
    scene2D.addAnchor(&puppet3_anchor, puppet3.getGeoActor(PUPPET_Body)->getName(), &anchorPos3); */

    Topl_Program::timeline.addSequence_vec3f(&puppet1_forceVec, std::make_pair(60.0, Vec3f({ 0.0F, 50.0F, 0.0F })));
    Topl_Program::timeline.addSequence_vec3f(&puppet2_forceVec, std::make_pair(60.0, Vec3f({ -50.0F, 0.0F, 0.0F })));
    Topl_Program::timeline.addSequence_vec3f(&puppet3_forceVec, std::make_pair(60.0, Vec3f({ 50.0F, 0.0F, 0.0F })));

    _renderer->buildScene(&scene2D);
    _renderer->texturizeScene(&scene2D);
}

void FirstPerson_Demo::loop(double frameTime){
    
    {
        // Topl_Factory::switchPipeline(_renderer, (_renderer->getFrameCount() % 60 < 30)? _flatPipeline : _beamsPipeline);
        // _flatVShader.setMode((_renderer->getFrameCount() % 30 < 15)? 0 : 1);
        Topl_Factory::switchPipeline(_renderer, _beamsPipeline);
        // _beamsVShader.setMode((_renderer->getFrameCount() % 120 < 40)? 0 : (_renderer->getFrameCount() % 120 < 80)? 4 : 3);

        models[0].rotateAll({ 0.0, 0.0F, (float)frameTime / -500.0F });
        models[1].rotateAll({ 0.0, 0.0F, (_renderer->getFrameCount() % 120 < 60)? (float)frameTime / 1000.0F : (float)frameTime / -1000.0F });
        models[2].rotateAll({ 0.0, 0.0F, (float)frameTime / 500.0F });
        models[3].rotateAll({ 0.0, 0.0F, (float)frameTime / -500.0F });
        models[4].rotateAll({ 0.0, 0.0F, (float)frameTime / 500.0F });

        // _renderer->setDrawMode((_renderer->getFrameCount() % 180 > 120)? DRAW_Strip : (_renderer->getFrameCount() % 180 > 60)? DRAW_Lines : DRAW_Points);
        _renderer->setDrawMode(DRAW_Triangles);

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

        Topl_Factory::switchPipeline(_renderer, _texPipeline);
        _renderer->setDrawMode(DRAW_Triangles);

        _renderer->updateScene(&scene2D);
        _renderer->drawScene(&scene2D);
    }
}

int main(int argc, char** argv) {
    _DEMO = new FirstPerson_Demo(argv[0], BACKEND_GL4);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}