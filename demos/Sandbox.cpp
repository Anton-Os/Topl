#include "Sandbox.hpp"

static unsigned pickerVal = 0;
static unsigned short pickerPane = 0;
static unsigned texMode = 0;

#ifdef RASTERON_H
Img_Base texture1 = Img_Base(0xFF0000FF); Img_Base texture2 = Img_Base(0xFF00FF00);
Img_Base texture3 = Img_Base(0xFFFFFF00); Img_Base texture4 = Img_Base(0xFFFF00FF);
#endif

static Vec3f cursorVec = { 0.0F, 0.0F, 0.0F };
static bool isBoxSelect = false;
static Vec3f boxVec = { 0.5f, 0.5f, 0.0f };
static bool isPyramidSelect = false;
static Vec3f pyramidVec = { -0.5f, 0.5f, 0.0f };
static bool isSphereSelect = false;
static Vec3f sphereVec = { -0.5f, -0.5f, 0.0f };
static bool isHexSelect = false;
static Vec3f hexVec = { 0.5f, -0.5f, 0.0f };
static bool isLayoutSelect1 = false;
static Vec3f layoutVec1 = { 0.0f, 0.0f, 0.0f };
static bool isLayoutSelect2 = false;
static Vec3f layoutVec2 = { 0.55f, 0.0f, 0.0f };
static bool isLayoutSelect3 = false;
static Vec3f layoutVec3 = { -0.55f, 0.0f, 0.0f };

static void onPress(float x, float y){ cursorVec = { x, y, 0.0F }; }
static void onHover(float x, float y){ 
    if(Platform::mouseControl.getIsHeld().second) {
        cursorVec = { x, y, 0.0F }; 
        std::cout << "Hover Callback!" << std::endl;
        if(isBoxSelect) boxVec = boxVec + (cursorVec - boxVec);
        else if(isPyramidSelect) pyramidVec = pyramidVec + (cursorVec - pyramidVec);
        else if(isSphereSelect) sphereVec = sphereVec + (cursorVec - sphereVec);
        else if(isHexSelect) hexVec = hexVec + (cursorVec - hexVec);
        else if(isLayoutSelect1) layoutVec1 = layoutVec1 + (cursorVec - layoutVec1);
        else if(isLayoutSelect2) layoutVec2 = layoutVec2 + (cursorVec - layoutVec2);
        else if(isLayoutSelect3) layoutVec3 = layoutVec3 + (cursorVec - layoutVec3);
    } else {
        isBoxSelect = false; isPyramidSelect = false; isSphereSelect = false; isHexSelect = false;
        isLayoutSelect1 = false; isLayoutSelect2 = false; isLayoutSelect3 = false;
    }
}

static void boxPicker(){ isBoxSelect = true; }
static void boxPass(){ /* Add body for render block */ }
static void pyramidPicker(){ isPyramidSelect = true; }
static void pyramidPass(){ /* Add body for render block */ }
static void spherePicker(){ isSphereSelect = true; }
static void spherePass(){ /* Add body for render block */ }
static void hexPicker(){ isHexSelect = true; }
static void hexPass(){ /* Add body for render block */ }
static void layoutPicker1(){ 
    std::cout << "Layout 1 pressed" << std::endl;
    isLayoutSelect1 = true; 
}
static void layoutPicker2(){ 
    std::cout << "Layout 2 pressed" << std::endl;
    isLayoutSelect2 = true; 
}
static void layoutPicker3(){
    std::cout << "Layout 3 pressed" << std::endl;
     isLayoutSelect3 = true; 
}

static void shapeUpdates(){
    static unsigned updateCall = 0;

    /* if(updateCall % 40 < 10) {
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
    } */

    updateCall++;
}

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

    Platform::mouseControl.addCallback(MOUSE_LeftBtn_Down, onPress);
    Platform::mouseControl.addCallback(MOUSE_RightBtn_Down, onPress);
    Platform::mouseControl.addHoverCallback(onHover);

    _timeline.ticker.addPeriodicEvent(1000, shapeUpdates);
    _timeline.ticker.addPeriodicEvent(1000, texModeUpdates);

    boxMesh.scale({ 0.25f, 0.25f, 0.25f});
    boxActor.setPos(boxVec);
    boxActor.pickerFunc = boxPicker;
    boxActor.shaderFunc = boxPass;
    scene.addGeometry("Box", &boxActor);
    pyramidMesh.scale({ 0.25f, 0.25f, 0.25f});
    pyramidActor.setPos(pyramidVec);
    pyramidActor.pickerFunc = pyramidPicker;
    pyramidActor.shaderFunc = pyramidPass;
    scene.addGeometry("Pyramid", &pyramidActor);
    sphereMesh.scale({ 0.25f, 0.25f, 0.25f});
    sphereActor.setPos(sphereVec);
    sphereActor.pickerFunc = spherePicker;
    sphereActor.shaderFunc = spherePass;
    scene.addGeometry("Sphere", &sphereActor);
    hexMesh.scale({ 0.25f, 0.25f, 0.25f});
    hexActor.setPos(hexVec);
    hexActor.pickerFunc = hexPicker;
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

    // Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, beamPipeline);
    _renderer->buildScene(&scene);

    layout1.configure(&overlay);
    layout1.getGeoActor(layout1.getParams()->getGridSize())->pickerFunc = layoutPicker1;
    layout1.shift(layoutVec1);
    layout2.configure(&overlay);
    layout2.getGeoActor(layout2.getParams()->getGridSize())->pickerFunc = layoutPicker2;
    layout2.shift(layoutVec2);
    layout3.configure(&overlay);
    layout3.getGeoActor(layout3.getParams()->getGridSize())->pickerFunc = layoutPicker3;
    layout3.shift(layoutVec3);

    // Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, flatPipeline);
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

        boxActor.setPos(boxVec);
        pyramidActor.setPos(pyramidVec);
        sphereActor.setPos(sphereVec);
        hexActor.setPos(hexVec);

        boxActor.updateRot({ 0.003F * (float)frameTime, 0.0F, 0.0F });
        pyramidActor.updateRot({ -0.003F * (float)frameTime, 0.0F, 0.0F });
        sphereActor.updateRot({ 0.0F, 0.003F * (float)frameTime, 0.0F });
        hexActor.updateRot({ 0.0F, 0.0F, 0.003F * (float)frameTime });

        layout1.shift(layoutVec1 - layout1.getOrigin());
        layout2.shift(layoutVec2 - layout2.getOrigin());
        layout3.shift(layoutVec3 - layout3.getOrigin());
    }

    // Flat Render

    {
        _renderer->setDrawMode(DRAW_Strip);
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, flatPipeline);
        _renderer->updateScene(&scene);
        _renderer->renderScene(&scene);

        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, flatPipeline);
        _renderer->updateScene(&overlay);
        _renderer->renderScene(&overlay);

#ifdef RASTERON_H
    if(Platform::mouseControl.getIsHeld().second) {
        pickerVal = invokePicker(&scene);
        pickerVal = invokePicker(&overlay);
    }
#endif
    }

    // Textured Render

    {
        _renderer->setDrawMode(DRAW_Lines);
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, texPipeline);
        texVShader.setTexMode(texMode);
        _renderer->updateScene(&scene);
        _renderer->renderScene(&scene);

        _renderer->setDrawMode(DRAW_Triangles);
        Topl_Factory::switchPipeline(BACKEND_GL4, _renderer, texPipeline);
        texVShader.setTexMode(texMode);
        _renderer->updateScene(&overlay);
        _renderer->renderScene(&overlay);
    }
}

int main(int argc, char** argv) {
    Sandbox_Demo demo = Sandbox_Demo(argv[0], BACKEND_GL4);

    demo.run();
    return 0;
}