#include "Molecular.hpp"

void Molecular_Demo::init(){
    for(unsigned c = 0; c < MOLECULAR_CONSTRUCTS; c++) {
        constructs[c].shift(Vec3f({ (float)rand() / (float)RAND_MAX - 0.5F, (float)rand() / (float)RAND_MAX - 0.5F, 0.0F }));
        constructs[c].configure(&scene);
        if(c < MOLECULAR_CONSTRUCTS - 1){
            construct_links[c].preset(constructs[c].getOrigin(), constructs[c + 1].getOrigin());
            scene.addLink(&construct_links[c], constructs[c].getPrefix() + "hub",  constructs[c + 1].getPrefix() + "hub");
        }
    }

    _renderer->buildScene(&scene);

    globeScene.addGeometry(&globe);
    _renderer->buildScene(&globeScene);
}

void Molecular_Demo::loop(double frameTime){
    scene.camera = Topl_Program::cameraObj;

    if(_renderer->getFrameCount() % 60 == 0 && _renderer->getFrameCount() > 300){
        unsigned short constructIndex = rand() % MOLECULAR_CONSTRUCTS;
        /* scene.addForce(
            constructs[constructIndex].getPrefix() + "hub", 
            Vec3f({ ((float)rand() / (float)RAND_MAX - 0.5F) * 1000.0F, ((float)rand() / (float)RAND_MAX - 0.5F) * 1000.0F, 0.0F })
        ); */
        /* for(unsigned m = 1; m < constructs[constructIndex].getActorCount(); m++){
            scene.addForce(
                constructs[constructIndex].getPrefix() + "node" + std::to_string(m),
                Vec3f({ ((float)rand() / (float)RAND_MAX - 0.5F) * 1000.0F, ((float)rand() / (float)RAND_MAX - 0.5F) * 1000.0F, 0.0F })
            );
            constructs[constructIndex].getGeoActor(m)->updatePos({ 0.0F, 0.1F, 0.0F });
        } */
    }
    // scene.resolvePhysics();

    // _beamsVShader.setMode(-3);
    /* _flatVShader.setMode(8);
    _renderer->setDrawMode(DRAW_Points);
    _renderer->setPipeline(_flatPipeline);
    _renderer->updateScene(&globeScene);
    _renderer->drawScene(&globeScene); */

    Topl_Factory::switchPipeline(_renderer, _beamsPipeline);
    _flatVShader.setMode(5);
    _renderer->updateScene(&scene);
    _renderer->setDrawMode(DRAW_Lines);
    _renderer->drawScene(&scene);
    _flatVShader.setMode(7);
    _renderer->updateScene(&scene);
    _renderer->setDrawMode(DRAW_Triangles);
    _renderer->drawScene(&scene);
    /* _flatVShader.setMode(6);
    _renderer->updateScene(&scene);
    _renderer->setDrawMode(DRAW_Points);
    _renderer->drawScene(&scene); */
}

int main(int argc, char** argv) {
    _DEMO = new Molecular_Demo(argv[0], BACKEND_GL4);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}