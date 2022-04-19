#include "Topl_Renderer.hpp"

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
    if(!_isPipelineReady){
        logMessage(MESSAGE_Exclaim, "Pipeline not set for build call!");
        _isBuilt = false;
        return false; // failure
    }

    _activeRenderCtx = getRenderContext(scene);
    build(scene);
    /* if(_activeRenderCtx->targetCount == 0){
        logMessage(MESSAGE_Exclaim, "No render targets for active render context!");
        _isBuilt = false;
        return false;
    } */
    texturize(scene);
    drawMode(); // sets default draw mode
    return _isBuilt;
}

bool Topl_Renderer::updateScene(const Topl_Scene* scene){
    if(!_isPipelineReady) logMessage(MESSAGE_Exclaim, "Pipeline not set for update call!");
    if(!_isBuilt) logMessage(MESSAGE_Exclaim, "Scene not built for update call!");
    if(!_isPipelineReady || !_isBuilt) return false; // failure

    _activeRenderCtx = getRenderContext(scene);
    // if(_activeRenderCtx->targetCount == 0) { _isBuilt = false; } else
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
    }

    _activeRenderCtx = getRenderContext(scene);
    // if(_activeRenderCtx->targetCount == 0) { _isBuilt = false; } else
    render(scene);
    _frameIDs++; // increment frame counter
    return _isDrawn; // render call sets variable to true on success
}

/* bool renderAll(){ // draws all render objects
    if(!_isPipelineReady) logMessage(MESSAGE_Exclaim, "Pipeline not set for draw call!");
    if(!_isBuilt) logMessage(MESSAGE_Exclaim, "Scene not built for draw call!");
    if(_renderIDs == 0) logMessage(MESSAGE_Exclaim, "No render targets for draw call!");
    if(!_isPipelineReady || !_isBuilt || _renderIDs == 0){
        _isDrawn = false;
        return false; // failure
    }

    render(nullptr);
    _frameIDs++; // increment frame counter
    return _isDrawn; // render call sets variable to true on success
} */

unsigned Topl_Renderer::getPixelAt(float x, float y) {
    Rasteron_PixelPoint pixPoint = { x, y };

    Rasteron_Image* image = frame();
    unsigned offset = getPixCursorOffset(&pixPoint, image);
    unsigned color = *(image->data + offset);
    deleteImg(image);
    return color; // return color computed at offsets
}

Topl_RenderContext* Topl_Renderer::getRenderContext(const Topl_Scene*const scene){
    if(_renderCtxIndex >= MAX_RENDERER_CONTEXTS - 1){
        logMessage(MESSAGE_Exclaim, "Max render contexts exceeded!");
        return &_renderContexts[MAX_RENDERER_CONTEXTS - 1]; // fetch last available render context
    }

     for(unsigned r = 0; r < _renderCtxIndex; r++)
        if(_renderContexts[r].scene == scene) return &_renderContexts[r];
    
    _renderCtxIndex++; // move to next available render context
    return &_renderContexts[_renderCtxIndex - 1];
}