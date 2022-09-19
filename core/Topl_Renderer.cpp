#include "Topl_Renderer.hpp"

// Renderer

Topl_Renderer::Topl_Renderer(NATIVE_WINDOW window){
    _viewports = (Topl_Viewport*)malloc(sizeof(Topl_Viewport));

    *_viewports = Topl_Viewport(); // full screen viewport
    _viewportCount = 1;

    _platformCtx.window = window;
}

Topl_Renderer::Topl_Renderer(NATIVE_WINDOW window, std::initializer_list<Topl_Viewport> viewports){
    _viewports = (Topl_Viewport*)malloc(viewports.size() * sizeof(Topl_Viewport));

    for(unsigned short v = 0; v < viewports.size(); v++)
        *(_viewports + v) = *(viewports.begin() + v); // copy data member
    _viewportCount = viewports.size();

    _platformCtx.window = window;
}

void Topl_Renderer::setCamera(const Topl_Camera* camera){ 
    if(camera != nullptr) _activeCamera = camera;
    else _activeCamera = &_defaultCamera;
}

void Topl_Renderer::setPipeline(const Topl_Pipeline* pipeline){
    _entryShader = pipeline->entryShader;
    _isPipelineReady = pipeline->isReady;
}

bool Topl_Renderer::buildScene(const Topl_Scene* scene){
	if(scene->getActorCount() == 0) logMessage(MESSAGE_Exclaim, "No geometry for build call!");
    if(!_isPipelineReady) logMessage(MESSAGE_Exclaim, "Pipeline not set for build call!");
	if(scene->getActorCount() == 0 || !_isPipelineReady){
        _isBuilt = false;
        return false; // failure
    }

    build(scene);
    texturize(scene);
    return _isBuilt;
}

bool Topl_Renderer::updateScene(const Topl_Scene* scene){
    if(!_isPipelineReady) logMessage(MESSAGE_Exclaim, "Pipeline not set for update call!");
    if(!_isBuilt) logMessage(MESSAGE_Exclaim, "Scene not built for update call!");
    if(!_isPipelineReady || !_isBuilt) return false; // failure

    update(scene);
    return _isBuilt;
}

void Topl_Renderer::setDrawMode(enum DRAW_Mode mode){
    _drawMode = mode;
    drawMode(); // sets the proper draw mode
}

bool Topl_Renderer::renderScene(Topl_Scene* scene){
    if (!_isPipelineReady) logMessage(MESSAGE_Exclaim, "Pipeline not set for draw call!");
    if (!_isBuilt) logMessage(MESSAGE_Exclaim, "Scene not built for draw call!");
    if (_renderIDs == 0) logMessage(MESSAGE_Exclaim, "No render targets for draw call!");
    if (!_isPipelineReady || !_isBuilt || _renderIDs == 0) {
        _isDrawn = false;
        return false; // failure
    } else _isDrawn = true;

    // render(scene); // replace this!!!
    renderTarget(SPECIAL_SCENE_RENDER_ID); // pass scene data as target
    if(scene != nullptr) // scene render
        for(unsigned g = 0; g < scene->getActorCount(); g++)
            (getRenderID(scene->getGeoActor(g)) != 0)
                ? renderTarget(getRenderID(scene->getGeoActor(g)))
                : logMessage(MESSAGE_Exclaim, "renderID not found!");
    else // all render
        for(unsigned r = 0; r < _renderIDs; r++)
            renderTarget(r);

    _frameIDs++; // increment frame counter
    return _isDrawn; // render call sets variable to true on success
}

bool Topl_Renderer::renderAll(){ // draws all render objects
    renderScene(nullptr);
    return _isDrawn; // render call sets variable to true on success
}

unsigned Topl_Renderer::getPixelAt(float x, float y) {
    PixelPoint pixPoint = { x, y };

    Rasteron_Image* image = frame();
    unsigned offset = getPixCursorOffset(pixPoint, image);
    unsigned color = *(image->data + offset);
    deleteImg(image);
    return color; // return color computed at offsets
}

unsigned long Topl_Renderer::getRenderID(const Geo_Actor* actor){
    // for(std::map<unsigned long, const Geo_Actor*>::const_iterator m = _renderTargets_map.cbegin(); m != _renderTargets_map.cend(); m++)
	for (std::map<unsigned long, const Geo_Actor*>::const_iterator m = _renderTargets_map.cbegin(); m != _renderTargets_map.cend(); m++)
	    if(actor == m->second) return m->first;
    return 0; // No render target found!
}