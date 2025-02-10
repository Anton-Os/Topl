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

void Topl_Program::postloop(){
     if(Topl_Program::pickerObj != nullptr){
        _editor.actor.setPos(*Topl_Program::pickerObj->getPos());
        _editor.actor.setRot(*Topl_Program::pickerObj->getRot());
        _editor.actor.setSize(*Topl_Program::pickerObj->getSize());
        _editor.mesh.drawMode = DRAW_Lines;
        _overlays.billboard_object.toggleShow(true); // Topl_Program::pickerObj->getName().find("billboard") == std::string::npos);
#ifdef RASTERON_H
        // std::cout << "Actor name is " << Topl_Program::pickerObj->getName() << std::endl;
        if(Platform::mouseControl.getIsMouseDown().second){
            std::string name = "| " + Topl_Program::pickerObj->getName() + " |";
            Rasteron_Text text = { _editor.fontPath.c_str(), name.c_str(), 0xFF111111, 0xFFEEEEEE };
            _editor.nameImg = Sampler_Text(text);
            _renderer->texturizeScene(&_editor.scene);
            _editor.nameMesh.drawMode = DRAW_Triangles;
            _editor.nameActor.setPos(*Topl_Program::pickerObj->getPos() + (Vec3f({ 0.0F, 0.35F, 0.0F} )) * *Topl_Program::pickerObj->getSize());
            _editor.nameActor.setSize({ _editor.nameImg.getImage()->width * 0.085f, (*_editor.nameActor.getSize()).data[1], (*_editor.nameActor.getSize()).data[2] });
        }
#endif
    }
    else _overlays.billboard_object.toggleShow(false);
#ifdef RASTERON_H
    static unsigned index = 0;

    if(_renderer->getFrameCount() % 60 == 0){
        Sampler_2D frameImg = _renderer->frame();
        // queue_addImg(cachedFrames, frameImg.getImage(), index % cachedFrames->frameCount);
        // std::cout << "cachedFrames image at " << std::to_string(index) << " is " << queue_getImg(cachedFrames, index)->name << std::endl;
        index++;
    }
#endif
}

void Topl_Program::run(){
    Topl_Program::timeline.dynamic_ticker.reset();
    init();

    while (_platform->handleEvents()) {
        if(!Topl_Program::timeline.dynamic_ticker.isPaused) Topl_Timeline::seqCallback(Topl_Program::timeline.dynamic_ticker.getAbsSecs());

        for(auto p = positions_map.begin(); p != positions_map.end(); p++) if(p->first != pickerObj && !Topl_Program::timeline.dynamic_ticker.isPaused) p->first->setPos(p->second);
        for(auto r = rotations_map.begin(); r != rotations_map.end(); r++) if(r->first != pickerObj && !Topl_Program::timeline.dynamic_ticker.isPaused) r->first->setRot(r->second);
        for(auto s = scales_map.begin(); s != scales_map.end(); s++) if(s->first != pickerObj && !Topl_Program::timeline.dynamic_ticker.isPaused) s->first->setSize(s->second);

        if(_renderer != nullptr){
            Topl_Pipeline* savedPipeline = _renderer->getPipeline();
            if(Platform::mouseControl.getIsMouseDown().second) preloop();
            _renderer->clear(); // clears view to solid color

            if(isEnable_background) renderScene(&_background.scene, _texPipeline, TEX_1); // nullptr, shaderMode);
            Topl_Factory::switchPipeline(_renderer, savedPipeline);
            setShadersMode(Topl_Program::shaderMode);
            loop(Topl_Program::timeline.persist_ticker.getRelMillisecs()); // performs draws and updating
            if(Topl_Program::pickerObj != nullptr) renderScene(&_editor.scene, _materialPipeline, 0); // nullptr, shaderMode);
            if(isEnable_overlays) renderScene(&_overlays.scene, _texPipeline, TEX_BASE); // nullptr, shaderMode);

            _renderer->present(); // switches front and back buffer
            if(isEnable_screencap) postloop();
            Topl_Factory::switchPipeline(_renderer, savedPipeline);
        }
        /* if(Topl_Program::lastPickerCoord[0] != Topl_Program::pickerCoord[0] && Topl_Program::lastPickerCoord[1] != Topl_Program::pickerCoord[1])
            Topl_Program::lastPickerCoord = Topl_Program::pickerCoord;
        */
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
            // std::cout << "Actor is " << actor->getName() << std::endl;
            if(actor->pickFunc != nullptr){
                if(!Platform::mouseControl.getIsMouseDown().second) actor->pickFunc(MOUSE_Hover, actor);
                else actor->pickFunc(Platform::mouseControl.getIsMouseDown().first, actor);
            }
            if(Platform::mouseControl.getIsMouseDown().second) Topl_Program::lastPickerObj = actor;
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

/* void Topl_Program::build_run(const Topl_Scene* scene){
    _threads[0] = std::thread([this](const Topl_Scene* s){ _renderer->buildScene(s); }, scene);
    _threads[0].join();
}

void Topl_Program::update_run(const Topl_Scene* scene){
    _threads[1] = std::thread([this](const Topl_Scene* s){ _renderer->updateScene(s); }, scene);
    _threads[1].join();
}

void Topl_Program::texturize_run(const Topl_Scene* scene){
    _threads[2] = std::thread([this](const Topl_Scene* s){ _renderer->texturizeScene(s); }, scene);
    _threads[2].join();
}

void Topl_Program::draw_run(const Topl_Scene* scene){
    // for(unsigned t = 0; t < 3; t++)
    //	if(_threads[t].joinable()) _threads[t].join();

    _threads[3] = std::thread([this](const Topl_Scene* s){ _renderer->drawScene(s); }, scene);
    _threads[3].join();
} */
