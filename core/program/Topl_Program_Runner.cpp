#include "program/Topl_Program.hpp"

void Topl_Program::preloop(){
#ifdef RASTERON_H
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

void Topl_Program::updatePipelines(){
    float timeElapse = (float)timeline.dynamic_ticker.getAbsSecs();
    Vec3f scroll = (*camera.getPos() * -Topl_Program::speed); // + Vec3f({ sin((*camera.getRot)[0]), cos((*camera.getRot)[0]), 0.0F }); // TODO: Include rotation
    Vec3f scale = Vec3f({ 0.5F, 0.5F, 0.5F }) * ((1.0F / *(camera.getZoom())) * 0.5F); // (1.0F / ((*camera.getZoom() + 4.0F) * 0.1F));
    Input_TracerStep tracerStep = (Platform::mouseControl.getTracerSteps()->size() > 0)? (*Platform::mouseControl.getTracerSteps()).back() : Input_TracerStep();

    if(_renderer->getPipeline() == _texPipeline) _texVShader.setParams(&_background.actor, { 0, (timeElapse / 10) - floor(timeElapse / 10), scroll, scale });
    else if(_renderer->getPipeline() == _materialPipeline){ 
        _materialVShader.setTexCoordParams(scroll, scale);
        _materialVShader.setLight(Topl_Light(Vec3f({ sin(timeElapse), cos(timeElapse), sin(timeElapse) + cos(timeElapse) })));
    } else if(_renderer->getPipeline() == _effectPipeline)
        _effectVShader.setEffect(EFFECT_SIZE - (EFFECT_SIZE / timeElapse), ((unsigned)floor(timeElapse / 5.0) % EFFECT_ITER) + 3);
    else if(_renderer->getPipeline() == _beamsPipeline){ 
        _beamsVShader.setLight(LIGHT_Sky, Topl_Light(Vec3f({ sin((*(camera.getRot())).data[0]), cos((*(camera.getRot())).data[0]), tan((*(camera.getRot())).data[1]) }), { BEAMS_SKY_LIGHT.value }));
        _beamsVShader.setLight(LIGHT_Flash, Topl_Light(*camera.getPos(), { BEAMS_FLASH_LIGHT.value }));
        if(Platform::mouseControl.getTracerSteps()->size() > 0) _beamsVShader.setLight(LIGHT_Lamp, Topl_Light(Vec3f({ (float)tracerStep.step.first, (float)tracerStep.step.second, 0.0F }), { BEAMS_LAMP_LIGHT.value }));
    } else if(_renderer->getPipeline() == _patternPipeline) 
        for(unsigned p = 0; p < PATTERN_POINTS_MAX && p < Platform::mouseControl.getTracerSteps()->size(); p++){
            tracerStep = (*Platform::mouseControl.getTracerSteps())[Platform::mouseControl.getTracerSteps()->size() - p - 1];
            _patternVShader.setCtrlPoint(p, Vec3f({
                 (float)tracerStep.step.first + (sin((float)tracerStep.step.first + timeElapse / 100.0F) * 0.05F), 
                 (float)tracerStep.step.second + (sin((float)tracerStep.step.second + timeElapse / 100.0F) * 0.05F), 
                 (1.0F / Platform::mouseControl.getTracerSteps()->size()) * p
            }));
        }
}

void Topl_Program::updateTimelines(){
    if(!Topl_Program::timeline.dynamic_ticker.isPaused) Topl_Timeline::seqCallback(Topl_Program::timeline.dynamic_ticker.getAbsSecs());

    Topl_Program::camera.setPos(_camPos);
    Topl_Program::camera.setRot(_camRot);
    // Topl_Program::camera.setZoom(_camZoom);

    for(auto p = positions_map.begin(); p != positions_map.end(); p++) if(p->first != pickerObj && !Topl_Program::timeline.dynamic_ticker.isPaused) p->first->setPos(p->second);
    for(auto r = rotations_map.begin(); r != rotations_map.end(); r++) if(r->first != pickerObj && !Topl_Program::timeline.dynamic_ticker.isPaused) r->first->setRot(r->second);
    for(auto s = scales_map.begin(); s != scales_map.end(); s++) if(s->first != pickerObj && !Topl_Program::timeline.dynamic_ticker.isPaused) s->first->setSize(s->second);

    if(isEnable_overlays){
        double secsElapsed = Topl_Program::timeline.dynamic_ticker.getAbsSecs();
        if(secsElapsed < TIMELINE_END) _overlays.billboard_timeline.setState(0, secsElapsed / TIMELINE_END, 0.0F);
        if(_renderer->getFrameCount() % 60 == 0){ 
            /* std::string minStr = "_" + (((unsigned)secsElapsed / 60) > 10)? std::to_string((unsigned)secsElapsed / 60) : ("0" + std::to_string((unsigned)secsElapsed / 60)) + "_";
            _overlays.mediaLabels[2].setText({ _overlays.fontPath.c_str(), minStr.c_str(), 0xFF111111, 0xFFEEEEEE });
            std::string secsStr = "_" + (((unsigned)secsElapsed % 60) > 10)? std::to_string((unsigned)secsElapsed % 60) : ("0" + std::to_string((unsigned)secsElapsed % 60)) + "_";
            _overlays.mediaLabels[1].setText({ _overlays.fontPath.c_str(), secsStr.c_str(), 0xFF111111, 0xFFEEEEEE }); */

            _renderer->texturizeScene(&_overlays.scene); // TODO: Remove this logic
        }
    }
}

void Topl_Program::postloop(){
    if(Topl_Program::lastPickerObj != nullptr){
        _editor.actor.setPos(*Topl_Program::lastPickerObj->getPos());
        _editor.actor.setRot(*Topl_Program::lastPickerObj->getRot());
        _editor.actor.setSize(*Topl_Program::lastPickerObj->getSize() * 0.75F);
        _editor.mesh.drawMode = DRAW_Lines;
        _editor.actor.isShown = (!_editor.actor.isShown)? Topl_Program::pickerObj != nullptr && isEnable_overlays : isEnable_overlays;
        _editor.nameActor.isShown = (!_editor.nameActor.isShown)? Topl_Program::pickerObj != nullptr && isEnable_overlays : isEnable_overlays;
        _editor.nameActor.setPos(*Topl_Program::lastPickerObj->getPos() + (Vec3f({ 0.0F, 0.35F, 0.0F} )) * *Topl_Program::lastPickerObj->getSize());
        _editor.nameActor.setSize({ _editor.nameImg.getImage()->width * 0.085f, (*_editor.nameActor.getSize()).data[1], (*_editor.nameActor.getSize()).data[2] });
#ifdef RASTERON_H
        if(Platform::mouseControl.getIsMouseDown().second){
            std::string name = "| " + Topl_Program::lastPickerObj->getName() + " |";
            Rasteron_Text text = { _editor.fontPath.c_str(), name.c_str(), 0xFF111111, 0xFFEEEEEE };
            _editor.nameImg = Sampler_Text(text);
            _renderer->texturizeScene(&_editor.scene);
            _editor.nameMesh.drawMode = DRAW_Triangles;
        }
#endif
    }
#ifdef RASTERON_H
    if(isEnable_screencap){
        static unsigned index = 0;

        if(_renderer->getFrameCount() % 60 == 0){
            Sampler_2D frameImg = _renderer->frame();
            // queue_addImg(cachedFrames, frameImg.getImage(), index % cachedFrames->frameCount);
            // std::cout << "cachedFrames image at " << std::to_string(index) << " is " << queue_getImg(cachedFrames, index)->name << std::endl;
            index++;
        }
    }
#endif
}

void Topl_Program::run(){
    Topl_Program::timeline.dynamic_ticker.reset();
    init();

    while (_platform->handleEvents()) {
        updateTimelines();

        if(_renderer != nullptr){
            _savedPipeline = _renderer->getPipeline();
            if(Platform::mouseControl.getIsMouseDown().second) preloop();
            _renderer->clear(); 

            if(isCtrl_shader) updatePipelines();
            setShadersMode(Topl_Program::shaderMode);
            if(isEnable_background) renderScene(&_background.scene, nullptr, shaderMode);
            Topl_Factory::switchPipeline(_renderer, _savedPipeline);
            loop(Topl_Program::timeline.persist_ticker.getAbsMillisecs()); // performs draws and updating
            if(Topl_Program::lastPickerObj != nullptr) renderScene(&_editor.scene, _texPipeline, TEX_BASE); // nullptr, shaderMode);
            _renderer->setDrawMode(DRAW_Triangles);
            if(isEnable_overlays) renderScene(&_overlays.scene, _texPipeline, TEX_BASE); // nullptr, shaderMode);
            _renderer->present(); // switches front and back buffer
            if(isEnable_screencap) postloop();
            Topl_Factory::switchPipeline(_renderer, _savedPipeline);
        }
    }

    cleanup();
}


#ifdef RASTERON_H
unsigned Topl_Program::colorPicker(Topl_Scene* scene){
    Topl_Pipeline* pipeline = _renderer->getPipeline(); // saving

    _flatVShader.setMode(FLAT_ID);
    Topl_Factory::switchPipeline(_renderer, _flatPipeline);
    _renderer->setDrawMode(DRAW_Triangles);
    _renderer->updateScene(scene);
    _renderer->drawScene(scene); // TODO: Make sure to draw actors as triangles

    Topl_Program::pickerColor = _renderer->getPixelAt(Platform::getCursorX(), Platform::getCursorY());
    // if((Topl_Program::pickerColor & 0x00FFFFFF) == 0xFFFFFF) Topl_Program::pickerObj = nullptr;
    if(scene != nullptr){
        Geo_Actor* actor = scene->getPickActor(Topl_Program::pickerColor);
        Topl_Program::pickerObj = actor;
        if(actor != nullptr){
            if(actor->pickFunc != nullptr){
                if(!Platform::mouseControl.getIsMouseDown().second) actor->pickFunc(MOUSE_Hover, actor);
                else actor->pickFunc(Platform::mouseControl.getIsMouseDown().first, actor);
            }
            if(Platform::mouseControl.getIsMouseDown().second && actor->getName().find("program_") == std::string::npos) // exclude program objects
                Topl_Program::lastPickerObj = actor; // only set last picker object if mouse is down and its not a default object
            if(lastPickerObj != nullptr) std::cout << "Last picked object is " << lastPickerObj->getName() << std::endl;
        }
    }

    _renderer->clear();

    Topl_Factory::switchPipeline(_renderer, pipeline); // restoring
    return Topl_Program::pickerColor;
}

Vec3f Topl_Program::coordPicker(Topl_Scene* scene){
    Topl_Pipeline* pipeline = _renderer->getPipeline(); // saving

    _flatVShader.setMode(FLAT_COORD);
    Topl_Factory::switchPipeline(_renderer, _flatPipeline); // TODO: Switch back to previous pipeline?
    _renderer->setDrawMode(DRAW_Triangles);
    _renderer->updateScene(scene);
    _renderer->drawScene(scene); // TODO: Make sure to draw actors as triangles

    unsigned color = _renderer->getPixelAt(Platform::getCursorX(), Platform::getCursorY());
    Topl_Program::pickerCoord = Vec3f{
        ((color & 0xFF0000) >> 16) / 255.0f,
        ((color & 0xFF00) >> 8) / 255.0f,
        (color & 0xFF) / 255.0f,
    };

    // _renderer->clear();

    Topl_Factory::switchPipeline(_renderer, pipeline); // restoring
    return Topl_Program::pickerCoord;
}
#endif