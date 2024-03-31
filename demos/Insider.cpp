#include "Insider.hpp"

static ColorGrid colorGrid;
static PROJECTION_Type projType = PROJECTION_Perspective;
static double projFactor = 3.5;

static void onAnyKey(char k){
    if(toupper(k) == 'I') projType = (projType == PROJECTION_Perspective)? PROJECTION_Ortho : PROJECTION_Perspective;
    else if(toupper(k) == 'O') projFactor -= 0.05;
    else if(toupper(k) == 'P') projFactor += 0.05;

    if(toupper(k) == 'I' || toupper(k) == 'O' || toupper(k) == 'P')
        Topl_Program::cameraObj.setProjMatrix(Projection(projType, projFactor).genProjMatrix());
}

void Insider_Demo::init(){
    Platform::keyControl.addAnyCallback(onAnyKey);
    Topl_Program::cameraObj.setPos({ 0.0F, 0.5F, CAM_DEPTH });
    Topl_Program::cameraObj.setZoom(1.5);

   floorActor.setPos({ 0.0F, -0.5F, 0.0F });
    floorActor.setRot({ 0.0F, MATH_HALF_PI + 0.1, 0.0F });
    floorActor.setSize({ 5.0F, 5.0F, 0.0F });
    wallActors[0].setPos({ 0.0F, 0.0F, 1.0F });
    wallActors[0].setSize({ 5.0F, 5.0F, 0.0F });
    wallActors[1].setPos({ 0.0F, 0.0F, -1.0F });
    wallActors[1].setSize({ 5.0F, 5.0F, 0.0F });
    wallActors[2].setPos({ 1.0F, 0.0F, 0.0F });
    wallActors[2].setSize({ 5.0F, 5.0F, 0.0F });
    wallActors[2].setRot({ 0.0F, 0.0F, MATH_HALF_PI + 0.1 });
    wallActors[3].setPos({ -1.0F, 0.0F, 0.0F });
    wallActors[3].setSize({ 5.0F, 5.0F, 0.0F });
    wallActors[3].setRot({ 0.0F, 0.0F, -MATH_HALF_PI - 0.1 });

    floorTexture.setColorImage(0xFFFF8833);
    colorGrid = { 4, 4, 0xFF333333, 0xFFEEEEEE };
    Rasteron_Image* noiseImg1 = noiseImgOp_grid({ 1024, 1024 }, colorGrid);
    wallTextures[0].setImage(noiseImg1);
    colorGrid = { 8, 8, 0xFF00FF00, 0xFF0000FF };
    Rasteron_Image* noiseImg2 = noiseImgOp_grid({ 1024, 1024 }, colorGrid);
    wallTextures[1].setImage(noiseImg2);
    colorGrid = { 16, 16, 0xFFFF00FF, 0xFFFFFF00 };
    Rasteron_Image* noiseImg3 = noiseImgOp_grid({ 1024, 1024 }, colorGrid);
    wallTextures[2].setImage(noiseImg3);
    colorGrid = { 32, 32, RAND_COLOR(), RAND_COLOR() };
    Rasteron_Image* noiseImg4 = noiseImgOp_grid({ 1024, 1024 }, colorGrid);
    wallTextures[3].setImage(noiseImg4);

    scene.addGeometry("Floor", &floorActor);
    scene.addTexture("Floor", &floorTexture);
    for(unsigned w = 0; w < 4; w++){
        scene.addGeometry("Wall" + std::to_string(w), &wallActors[w]);
        scene.addTexture("Wall" + std::to_string(w), &wallTextures[w]);
    }

    _renderer->buildScene(&scene);

    // TODO: Add actors to objects scene with cones and custom meshes
}

void Insider_Demo::loop(double frameTime){
    _renderer->updateScene(&scene);
    _renderer->drawScene(&scene);
}

int main(int argc, char** argv){
    _instance = new Insider_Demo(argv[0], BACKEND_DX11);
    _instance->run();

    delete(_instance);
    return 0;
}