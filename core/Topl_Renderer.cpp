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

	// TODO: Add renderable targets for any new elements
	// TODO: Vet renderable targets for any obscolete elements

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

/* void Topl_Renderer::setDrawMode(enum DRAW_Mode mode){
    _drawMode = mode;
    drawMode(); // sets the proper draw mode
} */

bool Topl_Renderer::renderScene(const Topl_Scene* scene){
	if (!_isPipelineReady || !_isBuilt || _renderIDs == 0) {
		logMessage(MESSAGE_Exclaim, "Error rendering! Check pipeline, build, and render targets");
        return false; // error
    } else _isPresented = false;

    renderTarget(SCENE_RENDERID); // render is scene block data
	if(scene != nullptr){ // Scene Targets
		if (_isDrawInOrder == REGULAR_DRAW_ORDER) // draw in regular order
			for (unsigned g = 0; g < scene->getActorCount(); g++)
				renderTarget(getRenderID(scene->getGeoActor(g)));
		else if (_isDrawInOrder == INVERSE_DRAW_ORDER) // draw in reverse order
			for (unsigned g = scene->getActorCount(); g > 0; g--)
				renderTarget(getRenderID(scene->getGeoActor(g - 1)));
	} else {
		if (_isDrawInOrder == REGULAR_DRAW_ORDER) // draw in regular order
			for (unsigned r = 0; r <= _renderIDs; r++) renderTarget(r);
		else if (_isDrawInOrder == INVERSE_DRAW_ORDER) // draw in reverse order
			for (unsigned r = _renderIDs; r > 0; r--) renderTarget(r);
	}

    return true;
}

void Topl_Renderer::present() {
	static Timer_Static ticker;
	_frameIDs++;
	swapBuffers(ticker.getRelMillisecs());
}

#ifdef RASTERON_H

void Topl_Renderer::texturize(const Topl_Scene* scene) {
	for(unsigned g = 0; g < scene->getActorCount(); g++) {
		actor_cptr actor = scene->getGeoActor(g);
		unsigned renderID = getRenderID(actor);

		if(actor != nullptr && renderID != INVALID_RENDERID){
			const Img_Base* texture = scene->getTexture(actor->getName());
			if(texture != nullptr) attachTex(texture->getImage(), renderID);

			const Img_Array* arrayTex = scene->getArrayTex(actor->getName());
			if (arrayTex != nullptr)
				for (unsigned p = 1; p < MAX_TEX_BINDINGS; p++) // begin from 1 not to override first texture
					attachTexAt(getFrameAt(arrayTex->getQueue(), p), renderID, p);
					// attachTexAt(arrayTex->getLayer((unsigned short)p)->getImage(), renderID, p);

			const Img_Volume* volumeTex = scene->getVolumeTex(actor->getName());
			if (volumeTex != nullptr) attachTex3D(volumeTex, renderID);
		}
	}
}

unsigned Topl_Renderer::getPixelAt(float x, float y) {
    PixelPoint pixPoint = { x, y };

    Img_Base image = frame();
    unsigned offset = pixelPointOffset_cursor(pixPoint, image.getImage());
    unsigned color = *(image.getImage()->data + offset);
    return color; // return color computed at offsets
}

#else
	void Topl_Renderer::texturize(const Topl_Scene* scene) {} // blank body
#endif

unsigned long Topl_Renderer::getRenderID(const Geo_Actor* actor){
	if(actor == nullptr) return INVALID_RENDERID;
	auto renderId_it = std::find_if(
		_renderObjMap.begin(), _renderObjMap.end(),
		[actor](const std::pair<unsigned long, const Geo_Actor*>& p){ return p.second == actor; }
	);
	return (renderId_it != _renderObjMap.end())? renderId_it->first : INVALID_RENDERID; 
}