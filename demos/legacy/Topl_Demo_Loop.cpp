#include "Topl_Demo.hpp"

void Topl_Demo::preloop(){
#ifdef TOPL_ENABLE_TEXTURES
    if(isEnable_background){
    // if(isEnable_background && _renderer->getFrameCount() % 3 == 0){
        unsigned pickerColor = colorPicker(&_background.scene);
        Vec3f pickerCoord = coordPicker(&_background.scene);
    }

    if(isEnable_overlays){
    // if(isEnable_overlays && _renderer->getFrameCount() % 3 == 0){
        unsigned pickerColor = colorPicker(&_overlays.scene);
        Vec3f pickerCoord = coordPicker(&_overlays.scene);
    }
#endif
}

void Topl_Demo::updateTimelines(){
    if(!Topl_Demo::timeline.dynamic_ticker.isPaused) Topl_Timeline::seqCallback(Topl_Demo::timeline.dynamic_ticker.getAbsSecs());

    Topl_Demo::camera.setPos(_camPos);
    Topl_Demo::camera.setRot(_camRot);
    // Topl_Demo::camera.setZoom(_camZoom);

    for(auto p = positions_map.begin(); p != positions_map.end(); p++) if(p->first != pickerObj && !Topl_Demo::timeline.dynamic_ticker.isPaused) p->first->setPos(p->second);
    for(auto r = rotations_map.begin(); r != rotations_map.end(); r++) if(r->first != pickerObj && !Topl_Demo::timeline.dynamic_ticker.isPaused) r->first->setRot(r->second);
    for(auto s = scales_map.begin(); s != scales_map.end(); s++) if(s->first != pickerObj && !Topl_Demo::timeline.dynamic_ticker.isPaused) s->first->setSize(s->second);

#ifdef TOPL_ENABLE_TEXTURES
    if(isEnable_overlays){
        double secsElapsed = Topl_Demo::timeline.dynamic_ticker.getAbsSecs();
        if(secsElapsed < TIMELINE_END) _overlays.billboard_timeline.setState(0, secsElapsed / TIMELINE_END, 0.0F);

        /*if (_renderer->getFrameCount() % 60 == 0) {
            std::string minStr = "_" + (((unsigned)secsElapsed / 60) > 10)? std::to_string((unsigned)secsElapsed / 60) : ("0" + std::to_string((unsigned)secsElapsed / 60)) + "_";
            _overlays.mediaLabels[2].setText({ _overlays.fontPath.c_str(), minStr.c_str(), 0xFF111111, 0xFFEEEEEE });
            std::string secsStr = "_" + (((unsigned)secsElapsed % 60) > 10)? std::to_string((unsigned)secsElapsed % 60) : ("0" + std::to_string((unsigned)secsElapsed % 60)) + "_";
            _overlays.mediaLabels[1].setText({ _overlays.fontPath.c_str(), secsStr.c_str(), 0xFF111111, 0xFFEEEEEE });

            _renderer->texturizeScene(&_overlays.scene); // TODO: Remove this logic
        } */
    }
#endif
}

void Topl_Demo::postloop(){
    if(Topl_Demo::lastPickerObj != nullptr){
        _editor.actor.setPos(*Topl_Demo::lastPickerObj->getPos());
        _editor.actor.setRot(*Topl_Demo::lastPickerObj->getRot());
        _editor.actor.setSize(*Topl_Demo::lastPickerObj->getSize() * 0.75F);
        _editor.mesh.drawMode = DRAW_Lines;
        _editor.actor.isShown = (!_editor.actor.isShown)? Topl_Demo::pickerObj != nullptr && isEnable_overlays : isEnable_overlays;
#ifdef TOPL_ENABLE_TEXTURES
#if RASTERON_ENABLE_FONT
        _editor.nameActor.setPos(*Topl_Demo::lastPickerObj->getPos() + (Vec3f({ 0.0F, 0.35F, 0.0F })) * *Topl_Demo::lastPickerObj->getSize());
        _editor.nameActor.setSize({ _editor.nameImg.getImage()->width * 0.085f, (*_editor.nameActor.getSize()).data[1], (*_editor.nameActor.getSize()).data[2] });
        _editor.nameActor.isShown = (!_editor.nameActor.isShown) ? Topl_Demo::pickerObj != nullptr && isEnable_overlays : isEnable_overlays;

        if(Platform::mouseControl.getIsMouseDown().second){
            std::string name = "| " + Topl_Demo::lastPickerObj->getName() + " |";
            Rasteron_Text text = { _editor.fontPath.c_str(), name.c_str(), 0xFF111111, 0xFFEEEEEE };
            _editor.nameImg = Topl_Sampler_Text(text);
            _renderer->texturizeScene(&_editor.scene);
            _editor.nameMesh.drawMode = DRAW_Triangles;
        }
#endif
#endif
    }
}

void Topl_Demo::run(){
    Topl_Demo::timeline.dynamic_ticker.reset();
    init();

    while (_platform->handleEvents()) {
        updateTimelines();
        if(backgroundThread.joinable() && userInput != "") backgroundThread.join();

        if(_renderer != nullptr){
            _savedPipeline = _renderer->getPipeline();
            if(Platform::mouseControl.getIsMouseDown().second) preloop();
            _renderer->clear(); 

            if(isCtrl_shader) updatePipelines();
            setShadersMode(Topl_Demo::shaderMode);
            if(isEnable_background) renderScene(&_background.scene);
            Topl_Factory::switchPipeline(_renderer, _savedPipeline);
            loop(Topl_Demo::timeline.persist_ticker.getAbsMillisecs()); // performs draws and updating
            if(Topl_Demo::lastPickerObj != nullptr) renderScene(&_editor.scene, _texPipeline, TEX_BASE); // nullptr, shaderMode);
            _renderer->setDrawMode(DRAW_Triangles);
#ifdef TOPL_ENABLE_TEXTURES
            if(isEnable_overlays) renderScene(&_overlays.scene, _texPipeline, TEX_BASE); // nullptr, shaderMode);
#endif
            _renderer->present(); // switches front and back buffer
            if(isEnable_screencap) postloop();
            Topl_Factory::switchPipeline(_renderer, _savedPipeline);
        }
    }

    cleanup();
}


#ifdef TOPL_ENABLE_TEXTURES
unsigned Topl_Demo::colorPicker(Topl_Scene* scene){
    Topl_Pipeline* pipeline = _renderer->getPipeline(); // saving

    _coloredVShader.setMode(COLORED_ID);
    Topl_Factory::switchPipeline(_renderer, _coloredPipeline);
    _renderer->setDrawMode(DRAW_Triangles);
    _renderer->updateScene(scene);
    _renderer->drawScene(scene); // TODO: Make sure to draw actors as triangles

    Topl_Demo::pickerColor = _renderer->getPixelAt(Platform::getCursorX(), Platform::getCursorY());
    // if((Topl_Demo::pickerColor & 0x00FFFFFF) == 0xFFFFFF) Topl_Demo::pickerObj = nullptr;
    if(scene != nullptr){
        Geo_Actor* actor = scene->getPickActor(Topl_Demo::pickerColor);
        Topl_Demo::pickerObj = actor;
        if(actor != nullptr){
            if(actor->pickFunc != nullptr){
                if(!Platform::mouseControl.getIsMouseDown().second) actor->pickFunc(MOUSE_Hover, actor);
                else actor->pickFunc(Platform::mouseControl.getIsMouseDown().first, actor);
            }
            if(Platform::mouseControl.getIsMouseDown().second && actor->getName().find("program_") == std::string::npos) // exclude program objects
                Topl_Demo::lastPickerObj = actor; // only set last picker object if mouse is down and its not a default object
            if(lastPickerObj != nullptr) std::cout << "Last picked object is " << lastPickerObj->getName() << std::endl;
        }
    }

    _renderer->clear();

    Topl_Factory::switchPipeline(_renderer, pipeline); // restoring
    return Topl_Demo::pickerColor;
}

Vec3f Topl_Demo::coordPicker(Topl_Scene* scene){
    Topl_Pipeline* pipeline = _renderer->getPipeline(); // saving

    _coloredVShader.setMode(COLORED_COORD);
    Topl_Factory::switchPipeline(_renderer, _coloredPipeline); // TODO: Switch back to previous pipeline?
    _renderer->setDrawMode(DRAW_Triangles);
    _renderer->updateScene(scene);
    _renderer->drawScene(scene); // TODO: Make sure to draw actors as triangles

    unsigned color = _renderer->getPixelAt(Platform::getCursorX(), Platform::getCursorY());
    Topl_Demo::pickerCoord = Vec3f{
        ((color & 0xFF0000) >> 16) / 255.0f,
        ((color & 0xFF00) >> 8) / 255.0f,
        (color & 0xFF) / 255.0f,
    };

    // _renderer->clear();

    Topl_Factory::switchPipeline(_renderer, pipeline); // restoring
    return Topl_Demo::pickerCoord;
}
#endif
