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
    if(!_isBuilt) logMessage(MESSAGE_Exclaim, "Not built for update call!");
    if(!_isPipelineReady || !_isBuilt) return false; // failure

    update(scene);
    return _isBuilt;
}

void Topl_Renderer::setDrawMode(enum DRAW_Mode mode){
    _drawMode = mode;
    drawMode(); // sets the proper draw mode
}

bool Topl_Renderer::renderAll() {
	if (!_isPipelineReady || !_isBuilt || _renderIDs == 0) {
		logMessage(MESSAGE_Exclaim, "Error rendering! Check pipeline, build, and render targets");
		_isDrawn = false;
		return false; // error
	}
	else _isDrawn = true;

	// Render All Targets in Sequence

	renderTarget(SCENE_RENDER_ID); // first target is scene block data
	if (_isDrawInOrder == REGULAR_DRAW_ORDER) // draw in regular order
		for (unsigned r = 0; r <= _renderIDs; r++) renderTarget(r);
	else if (_isDrawInOrder == INVERSE_DRAW_ORDER) // draw in reverse order
		for (unsigned r = _renderIDs; r > 0; r--) renderTarget(r);

	_frameIDs++; // increment frame counter
	return _isDrawn;
}

bool Topl_Renderer::renderScene(const Topl_Scene* scene){
	if (!_isPipelineReady || !_isBuilt || _renderIDs == 0) {
		logMessage(MESSAGE_Exclaim, "Error rendering! Check pipeline, build, and render targets");
        _isDrawn = false;
        return false; // error
    } else _isDrawn = true;

	// Render Scene Targets in Sequence

    renderTarget(SCENE_RENDER_ID); // first target is scene block data
	if(scene != nullptr){ // Scene Targets
		if (_isDrawInOrder == REGULAR_DRAW_ORDER) { // draw in regular order
			for (unsigned g = 0; g < scene->getActorCount(); g++)
				(getRenderID(scene->getGeoActor(g)) != 0)
				? renderTarget(getRenderID(scene->getGeoActor(g)))
				: logMessage(MESSAGE_Exclaim, "renderID not found!");
		}
		else if (_isDrawInOrder == INVERSE_DRAW_ORDER) { // draw in reverse order
			for (unsigned g = scene->getActorCount(); g > 0; g--)
				(getRenderID(scene->getGeoActor(g - 1)) != 0)
					? renderTarget(getRenderID(scene->getGeoActor(g - 1)))
					: logMessage(MESSAGE_Exclaim, "renderID not found!");
		}
	}

    _frameIDs++; // increment frame counter
    return _isDrawn;
}

void Topl_Renderer::texturize(const Topl_Scene* scene) {
	for (unsigned g = 0; g < scene->getActorCount(); g++) {
		actor_cptr actor = scene->getGeoActor(g);
		unsigned renderID = getRenderID(actor);

		const Rasteron_Image* texture = scene->getTexture(actor->getName());
		if (texture != nullptr) attachTexture(texture, renderID);
		// else logMessage(MESSAGE_Exclaim, "Texure cannot be null!");

		const Img_Material* material = scene->getMaterial(actor->getName());
		if (material != nullptr) attachMaterial(material, renderID);
		// else logMessage(MESSAGE_Exclaim, "Material cannot be null!");
	}
}

unsigned Topl_Renderer::getPixelAt(float x, float y) {
    PixelPoint pixPoint = { x, y };

    Rasteron_Image* image = frame();
    unsigned offset = getPixOffset_cursor(pixPoint, image);
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