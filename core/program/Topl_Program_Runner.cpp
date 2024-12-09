#include "program/Topl_Program.hpp"

void Topl_Program::preloop(){
#ifdef RASTERON_H
    if(isEnable_background){
        unsigned pickerColor = colorPicker(&_background.scene);
        Vec3f pickerCoord = coordPicker(&_background.scene);
    }

    if(isEnable_overlays){
        unsigned pickerColor = colorPicker(&_overlays.scene);
        Vec3f pickerCoord = coordPicker(&_overlays.scene);
    }
#endif
}

void Topl_Program::postloop(){
#ifdef RASTERON_H
    static unsigned index = 0;

    if(_renderer->getFrameCount() % 60 == 0){
        Img_Base frameImg = _renderer->frame();
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
            if(Platform::mouseControl.getIsMouseDown().second) preloop();
            _renderer->clear(); // clears view to solid color
            // Topl_Factory::switchPipeline(_renderer, _flatPipeline);
            if(isEnable_background) renderScene(&_background.scene, nullptr, shaderMode);
            loop(Topl_Program::timeline.persist_ticker.getRelMillisecs()); // performs draws and updating
            if(isEnable_overlays) renderScene(&_overlays.scene, nullptr, shaderMode);
            _renderer->present(); // switches front and back buffer
            if(isEnable_screencap) postloop();
        }
        /* if(Topl_Program::lastPickerCoord[0] != Topl_Program::pickerCoord[0] && Topl_Program::lastPickerCoord[1] != Topl_Program::pickerCoord[1])
            Topl_Program::lastPickerCoord = Topl_Program::pickerCoord;
        */
    }

    cleanup();
}


#ifdef RASTERON_H
unsigned Topl_Program::colorPicker(Topl_Scene* scene){
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
            if(actor->pickerFunc != nullptr){
                if(!Platform::mouseControl.getIsMouseDown().second) actor->pickerFunc(MOUSE_Hover);
                else actor->pickerFunc(Platform::mouseControl.getIsMouseDown().first);
            }
            std::cout << "Actor is " << actor->getName() << std::endl;
        }
    }

    _renderer->clear();

    return Topl_Program::pickerColor;
}

Vec3f Topl_Program::coordPicker(Topl_Scene* scene){
    _flatVShader.setMode(FLAT_COORD);
    Topl_Factory::switchPipeline(_renderer, _flatPipeline);
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
