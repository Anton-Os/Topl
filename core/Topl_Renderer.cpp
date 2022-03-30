#include "Topl_Renderer.hpp"

void Topl_Renderer::setCamera(const Topl_Camera* camera){ 
    if(camera != nullptr) _activeCamera = camera;
    else _activeCamera = &_defaultCamera;
}

void Topl_Renderer::setPipeline(const Topl_Pipeline* pipeline){
    _entryShader = pipeline->entryShader;
    _isPipelineReady = pipeline->isReady;
}

bool Topl_Renderer::buildScene(const Topl_Scene* scene){
    if(!_isPipelineReady){
        logMessage(MESSAGE_Exclaim, "Pipeline not set for build call!");
        _isSceneReady = false;
        return false; // failure
    }
    if(_renderCtxIndex >= MAX_RENDERER_CONTEXTS) {
        logMessage(MESSAGE_Exclaim, "Too many render contexts!");
        // _renderCtxIndex = 0;
        _isSceneReady = false;
        return false;
    }

    build(scene);
    _renderCtxIndex++;
    texturize(scene);
    return _isSceneReady;
}

bool Topl_Renderer::buildScene(const Topl_Scene* scene, const Topl_Camera* camera){
    _activeCamera = camera; // switch to new camera
    return (buildScene(scene)) ? true : false;
}

bool Topl_Renderer::updateScene(const Topl_Scene* scene){
    if(!_isPipelineReady) logMessage(MESSAGE_Exclaim, "Pipeline not set for update call!");
    if(!_isSceneReady) logMessage(MESSAGE_Exclaim, "Scene not built for update call!");
    if(!_isPipelineReady || !_isSceneReady) return false; // failure

    update(scene);
    return _isSceneReady;
}

bool Topl_Renderer::updateScene(const Topl_Scene* scene, const Topl_Camera* camera){
    _activeCamera = camera; // switch to new camera
    return (updateScene(scene)) ? true : false;
}

void Topl_Renderer::setDrawMode(enum DRAW_Mode mode){
    _drawMode = mode;
    drawMode(); // sets the proper draw mode
}

bool Topl_Renderer::renderScene(Topl_Scene* scene){
    if (!_isPipelineReady) logMessage(MESSAGE_Exclaim, "Pipeline not set for draw call!");
    if (!_isSceneReady) logMessage(MESSAGE_Exclaim, "Scene not built for draw call!");
    if (_renderIDs == 0) logMessage(MESSAGE_Exclaim, "No render targets for draw call!");
    if (!_isPipelineReady || !_isSceneReady || _renderIDs == 0) {
        _isSceneDrawn = false;
        return false; // failure
    }

    render(scene);
    _frameIDs++; // increment frame counter
    return _isSceneDrawn; // render call sets variable to true on success
}

/* bool renderAll(){ // draws all render objects
    if(!_isPipelineReady) logMessage(MESSAGE_Exclaim, "Pipeline not set for draw call!");
    if(!_isSceneReady) logMessage(MESSAGE_Exclaim, "Scene not built for draw call!");
    if(_renderIDs == 0) logMessage(MESSAGE_Exclaim, "No render targets for draw call!");
    if(!_isPipelineReady || !_isSceneReady || _renderIDs == 0){
        _isSceneDrawn = false;
        return false; // failure
    }

    render(nullptr);
    _frameIDs++; // increment frame counter
    return _isSceneDrawn; // render call sets variable to true on success
} */

unsigned Topl_Renderer::getPixelAt(float x, float y) {
    Rasteron_PixelPoint pixPoint = { x, y };

    Rasteron_Image* image = frame();
    unsigned offset = getPixCursorOffset(&pixPoint, image);
    unsigned color = *(image->data + offset);
    deleteImg(image);
    return color; // return color computed at offsets
}