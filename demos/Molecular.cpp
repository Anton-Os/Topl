#include "Molecular.hpp"

void Molecular_Demo::init(){
    for(unsigned m = 0; m < 3; m++)
        for(unsigned c = 0; c < MOLECULAR_CONSTRUCTS; c++) {
            constructs[m][c].shift(Vec3f({ (float)rand() / (float)RAND_MAX - 0.5F, (float)rand() / (float)RAND_MAX - 0.5F - (m * 0.5F + 1.0F), 0.0F }));
            constructs[m][c].configure(&scene);
            if(c < MOLECULAR_CONSTRUCTS - 1){
                construct_links[c].preset(constructs[m][c].getOrigin(), constructs[m][c + 1].getOrigin());
                scene.addLink(&construct_links[c], constructs[m][c].getPrefix() + "hub",  constructs[m][c + 1].getPrefix() + "hub");
            }
        }

    _renderer->buildScene(&scene);

    globeScene.addGeometry(&globe);
    _renderer->buildScene(&globeScene);
}

void Molecular_Demo::loop(double frameTime){
    scene.camera = &Topl_Program::cameraObj;

    for(unsigned m = 0; m < 3; m++)
        for(unsigned c = 0; c < MOLECULAR_CONSTRUCTS; c++) 
            constructs[m][c].rotate({ ((float)rand() / (float)RAND_MAX - 0.5F) / 100.0F, ((float)rand() / (float)RAND_MAX - 0.5F) / 100.0F, 0.0F });

     if(_renderer->getFrameCount() % 60 == 0 && _renderer->getFrameCount() > 300){
        unsigned short setIndex = rand() % MOLECULAR_CONSTRUCTS;
        for(unsigned c = 0; c < MOLECULAR_CONSTRUCTS; c++)
            constructs[setIndex][c].scale({ 0.999F, 0.999F, 0.999F });
            
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

    // _beamsVShader.setMode(-3);

    Topl_Factory::switchPipeline(_renderer, _beamsPipeline);
    // _beamsVShader.setMode(3);
    _renderer->updateScene(&scene);
    _renderer->setDrawMode(DRAW_Lines);
    _renderer->drawScene(&scene);

    // _flatVShader.setMode(7);
    // _renderer->updateScene(&scene);
    _renderer->setDrawMode(DRAW_Triangles);
    // _renderer->drawScene(&scene);
    for(unsigned m = 0; m < 3; m++){
        _beamsVShader.setMode(m * 10);
        _renderer->updateScene(&scene);
        for(unsigned c = 0; c < MOLECULAR_CONSTRUCTS; c++)
            for(unsigned o = 0; o < constructs[m][c].getActorCount(); o++)
                _renderer->draw(constructs[m][c].getGeoActor(o));
    }
}

int main(int argc, char** argv) {
    _DEMO = new Molecular_Demo(argv[0], BACKEND_GL4);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}