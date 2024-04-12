#include "Topl_Renderer.hpp"

void Topl_Renderer::setCamera(const Topl_Camera* camera){ 
    if(camera != nullptr) _activeCamera = camera;
    else _activeCamera = &_defaultCamera;
}

void Topl_Renderer::setPipeline(const Topl_Pipeline* pipeline){
    _entryShader = pipeline->entryShader;
    _flags[PIPELINE_BIT] = pipeline->isReady;
}

bool Topl_Renderer::buildScene(const Topl_Scene* scene){
	if(scene->getActorCount() == 0) logMessage(MESSAGE_Exclaim, "No geometry for build call!");
    if(!_flags[PIPELINE_BIT]) logMessage(MESSAGE_Exclaim, "Pipeline not set for build call!");
	if(scene->getActorCount() == 0 || !_flags[PIPELINE_BIT] || scene == nullptr){
        _flags[BUILD_BIT] = false;
        return false; // failure
    }

    build(scene);
    if(scene->getIsTextured()) texturizeScene(scene);
	return _flags[BUILD_BIT];
}

bool Topl_Renderer::updateScene(const Topl_Scene* scene){
    if(!_flags[PIPELINE_BIT]) logMessage(MESSAGE_Exclaim, "Pipeline not set for update call!");
    if(!_flags[BUILD_BIT]) logMessage(MESSAGE_Exclaim, "Not built for update call!");
    if(!_flags[PIPELINE_BIT] || !_flags[BUILD_BIT]) return false; // failure

    update(scene);
    return _flags[BUILD_BIT];
}

/* void Topl_Renderer::setDrawMode(enum DRAW_Mode mode){
    _drawMode = mode;
    drawMode(); // sets the proper draw mode
} */

bool Topl_Renderer::drawScene(const Topl_Scene* scene){
	if (!_flags[PIPELINE_BIT] || !_flags[BUILD_BIT] || _renderIDs == 0) {
		logMessage(MESSAGE_Exclaim, "Error rendering! Check pipeline, build, and render targets");
        return false; // error
    } else _flags[DRAWN_BIT] = false;

	// std::thread thread(&Topl_Renderer::draw, this, SCENE_RENDERID);

    draw(SCENE_RENDERID); // render is scene block data
	if(scene != ALL_SCENES){ // Scene Targets
		if (_flags[DRAW_ORDER_BIT] == DRAW_NORMAL) // draw in regular order
			for (unsigned a = 0; a < scene->getActorCount(); a++)
				draw(scene->getGeoActor(a));
		else if (_flags[DRAW_ORDER_BIT] == DRAW_INVERSE) // draw in reverse order
			for (unsigned a = scene->getActorCount(); a > 0; a--)
				draw(scene->getGeoActor(a - 1));
	} else { // All Targets
		if (_flags[DRAW_ORDER_BIT] == DRAW_NORMAL) // draw in regular order
			for (unsigned r = 0; r <= _renderIDs; r++) draw(_renderObjMap[r]);
		else if (_flags[DRAW_ORDER_BIT] == DRAW_INVERSE) // draw in reverse order
			for (unsigned r = _renderIDs; r > 0; r--) draw(_renderObjMap[r]);
	}

    return true;
}

void Topl_Renderer::present() {
	static Timer_Persist ticker;
	_frameIDs++;
	if (!_flags[DRAWN_BIT]) { 
		swapBuffers(ticker.getRelMillisecs());
		_flags[DRAWN_BIT] = true;
	}
}

#ifdef RASTERON_H

void Topl_Renderer::texturizeScene(const Topl_Scene* scene) {
	for(unsigned g = 0; g < scene->getActorCount(); g++) {
		actor_cptr actor = scene->getGeoActor(g);
		unsigned renderID = getRenderID(actor);

		if(actor != nullptr && renderID != INVALID_RENDERID){
			const Img_Base* texture = scene->getTexture(actor->getName());
			if(texture != nullptr) attachTex(texture->getImage(), renderID); // TODO: Check for refresh!

			/* const Img_Array* arrayTex = scene->getArrayTex(actor->getName());
			if (arrayTex != nullptr)
				for (unsigned p = 1; p < MAX_TEX_BINDINGS; p++) // begin from 1 not to override first texture
					attachTexAt(queue_getImg(arrayTex->getQueue(), p), renderID, p); // TODO: Check for refresh!
					// attachTexAt(arrayTex->getLayer((unsigned short)p)->getImage(), renderID, p); */

			const Img_Volume* volumeTex = scene->getVolumeTex(actor->getName());
			if (volumeTex != nullptr) attachTex3D(volumeTex, renderID); // TODO: Check for refresh!
		} else logMessage(MESSAGE_Exclaim, "Cannot retreive actor or renderID");
	}
}


unsigned Topl_Renderer::getPixelAt(float x, float y) {
    PixelPoint pixPoint = { x, y };

    Img_Base image = frame();
    unsigned offset = pixPoint_cursorOffset(pixPoint, image.getImage());
    unsigned color = *(image.getImage()->data + offset);
    return color; // return color computed at offsets
}

#else
	void Topl_Renderer::texturizeScene(const Topl_Scene* scene) {} // blank body
#endif

unsigned long Topl_Renderer::getRenderID(const Geo_Actor* actor){
	if(actor == nullptr) return INVALID_RENDERID;
	return (_renderTargetMap.find(actor) != _renderTargetMap.end())? _renderTargetMap.at(actor) : INVALID_RENDERID;
}