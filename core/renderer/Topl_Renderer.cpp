#include "renderer/Topl_Renderer.hpp"

/* void Topl_Renderer::setCamera(const Topl_Camera* camera){ 
    if(camera != nullptr) _activeCamera = camera;
    else _activeCamera = &_defaultCamera;
} */

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

	// Build Scene  
	_sceneBlockData.clear();
	_entryShader->genSceneBlock(scene, &_sceneBlockData); 
	build(SCENE_RENDERID);

	// Build Render Objects
	for(unsigned g = 0; g < scene->getActorCount(); g++){
		actor_cptr actor = scene->getGeoActor(g);
		Geo_Mesh* mesh = (Geo_Mesh*)actor->getMesh();
		if(mesh->getVertexCount() == 0) logMessage(MESSAGE_Exclaim, "No vertex mesh detected");
		unsigned long renderID = getRenderID(actor);

		if(renderID == INVALID_RENDERID){ // Actor will not be duplicated
			_renderIDs++;
			_renderObjMap.insert({ _renderIDs, scene->getGeoActor(g) });
			_renderTargetMap.insert({ scene->getGeoActor(g), _renderIDs });
		} // else logMessage(MESSAGE_Exclaim, "Override required!");

		_actorBlockData.clear();
		_entryShader->genActorBlock(actor, &_actorBlockData);
		_meshBlockData.clear();
		_entryShader->genMeshBlock(actor->getMesh(), &_meshBlockData);
		build(actor);
	}

	// _threads[0] = std::thread(&Topl_Renderer::buildScene, scene);

    if(scene->getIsTextured()) texturizeScene(scene);
	return _flags[BUILD_BIT];
}

bool Topl_Renderer::updateScene(const Topl_Scene* scene){
    if(!_flags[PIPELINE_BIT]) logMessage(MESSAGE_Exclaim, "Pipeline not set for update call!");
    if(!_flags[BUILD_BIT]) logMessage(MESSAGE_Exclaim, "Not built for update call!");
    if(!_flags[PIPELINE_BIT] || !_flags[BUILD_BIT]) return false; // failure

	// Update Scene 
	if(isSceneUpdate){
		_sceneBlockData.clear();
		_entryShader->genSceneBlock(scene, &_sceneBlockData); 
		update(SCENE_RENDERID);
	}

	// Update Render Objects
	for (unsigned g = (scene != ALL_SCENES)? 0 : 1; g < ((scene != ALL_SCENES)? scene->getActorCount() : _renderIDs); g++) {
		actor_cptr actor = (scene != ALL_SCENES)? scene->getGeoActor(g) : _renderObjMap[g];
		if(isActorUpdate){
			_actorBlockData.clear();
			_entryShader->genActorBlock(actor, &_actorBlockData); // TODO: Include mesh updates
		}
		if(isMeshUpdate){
			_meshBlockData.clear();
			_entryShader->genMeshBlock(actor->getMesh(), &_meshBlockData);
		}
		if(isActorUpdate || isMeshUpdate) update(actor); // conditionally update
	}

	// _threads[1] = std::thread(&Topl_Renderer::updateScene, scene);

    return _flags[BUILD_BIT];
}

/* void Topl_Renderer::setDrawMode(enum DRAW_Mode mode){
    _drawMode = mode;
    drawMode(); // sets the proper draw mode
} */

bool Topl_Renderer::drawScene(const Topl_Scene* scene){
	// TODO: Join worker threads?

	if(!_flags[PIPELINE_BIT]) logMessage(MESSAGE_Exclaim, "Pipeline not set for draw call!");
    if(!_flags[BUILD_BIT]) logMessage(MESSAGE_Exclaim, "Not built for draw call!");
	if(_renderIDs == 0) logMessage(MESSAGE_Exclaim, "renderIDs is 0!");

	if (!_flags[PIPELINE_BIT] || !_flags[BUILD_BIT] || _renderIDs == 0) return false;
	else _flags[DRAWN_BIT] = false;

	setViewport(&_activeViewport);

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

	// _threads[3] = std::thread(&Topl_Renderer::drawScene, scene);

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
	if(scene->getIsTextured()){
		for(unsigned t = 0; t < MAX_TEX_BINDINGS - 1; t++){ // unbound textures
			const Img_Base* texture = scene->getTexture(std::to_string(t + 1));
			if(texture != nullptr){
				if(_texTagMap.find(texture) == _texTagMap.end()) _texTagMap[texture] = std::string(*texture->tag); // saves current tag
				else if(_texTagMap[texture] == std::string(*texture->tag)) continue; // match so continue loop
                attachTexAt(texture, SCENE_RENDERID, t);
			}
		}
		
		for(unsigned g = 0; g < scene->getActorCount(); g++) {
			actor_cptr actor = scene->getGeoActor(g);
			unsigned renderID = getRenderID(actor);

			if(actor != nullptr && renderID != INVALID_RENDERID){ // bound textures
				const Img_Base* texture = scene->getTexture(actor->getName());
				if(texture != nullptr){
					if(_texTagMap.find(texture) == _texTagMap.end()) _texTagMap[texture] = std::string(*texture->tag); // saves current tag
					else if(_texTagMap[texture] == std::string(*texture->tag)) continue; // match so continue loop
                    attachTex(texture, renderID);
				}

				const Img_Volume* volumeTex = scene->getVolumeTex(actor->getName());
				if (volumeTex != nullptr) attachTex3D(volumeTex, renderID); // TODO: Check for tags!
			} else logMessage(MESSAGE_Exclaim, "Cannot retreive actor or renderID");
		}

		// _threads[2] = std::thread(&Topl_Renderer::updateScene, scene);
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
