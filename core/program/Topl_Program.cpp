#include "program/Topl_Program.hpp"

#define MOV_BTN_CASES case 0: case 1: case 2: case 3: case 4: case 5
#define ROT_BTN_CASES case 6: case 7: case 8: case 9: case 10: case 11
#define SIZ_BTN_CASES case 12: case 13: case 14: case 15: case 16: case 17

Topl_Timeline Topl_Program::timeline = Topl_Timeline();
Vec3f Topl_Program::cursorPos = { 0.0F, 0.0F, 0.0F };
float Topl_Program::speed = 0.25F; // 0.1F

unsigned Topl_Program::shaderMode = 0;
unsigned short Topl_Program::mode = 0;
Topl_EntryShader* Topl_Program::activeShader = nullptr;
bool Topl_Program::isCtrl_keys = true;
bool Topl_Program::isCtrl_shader = true;
bool Topl_Program::isCtrl_input = false;
std::string Topl_Program::userInput = "";

unsigned Topl_Program::pickerColor = 0;
unsigned Topl_Program::lastPickerColor = 0;
Vec3f Topl_Program::pickerCoord = { 0.0F, 0.0F, 0.0F };
Vec3f Topl_Program::lastPickerCoord = { 0.0F, 0.0F, 0.0F };
Geo_Actor* Topl_Program::pickerObj = NO_PICKER_OBJ;
Geo_Actor* Topl_Program::lastPickerObj = NO_PICKER_OBJ;
#ifdef TOPL_ENABLE_TEXTURES
Rasteron_Queue* Topl_Program::cachedFrames = NULL;
#endif

Topl_Camera Topl_Program::camera = Topl_Camera();
Topl_Pipeline* Topl_Program::_savedPipeline = nullptr;

void Topl_Program::_backgroundCallback(MOUSE_Event event, Geo_Actor* actor){
	std::cout << "BACKGROUND: Actor is " << actor->getName() << std::endl;
    Input_TracerPath lastPath = Platform::mouseControl.getTracerPaths()->back();
    if(lastPath.stepsCount % MAX_PATH_STEPS > 2){ 
        unsigned short steps = lastPath.stepsCount % MAX_PATH_STEPS;
        Vec2f cursorVec = Vec2f({ lastPath.steps[steps - 1].first - lastPath.steps[steps - 2].first, lastPath.steps[steps - 1].second - lastPath.steps[steps - 2].second  });
        if(cursorVec.len() > 0.005 && cursorVec.len() < 0.25){ // Are thses contraints valid?
            if(event == MOUSE_LeftBtn_Drag || event == MOUSE_LeftBtn_Press){ 
                Topl_Program::camera.updatePos({ cursorVec[0], cursorVec[1], 0.0F });
                _camPos = *camera.getPos(); 
            }
            else if(event == MOUSE_RightBtn_Drag || event == MOUSE_RightBtn_Press){ 
                Topl_Program::camera.updateRot({ cursorVec[0], cursorVec[1], 0.0F });
                _camRot = *camera.getRot(); 
            }
        }
    }
}

void Topl_Program::_overlayCallback(MOUSE_Event event, Geo_Actor* actor){
    static PROJECTION_Type projType = PROJECTION_None; // Projection(PROJECTION_None, 1.0F);
    static float projX = 1.0F, projY = 1.0F, projZ = 1.0F;
    static Vec3f pivot = *Topl_Program::camera.getRot();

    std::pair<float, float> tracerPathDiff = Platform::mouseControl.getLastPathDiff();
    Geo_Billboard* billboard = nullptr;
    for(unsigned o = 0; o < PROGRAM_BILLBOARDS; o++) 
        if(actor->getName().find(_overlays.billboards[o]->getPrefix()) != std::string::npos){
            std::cout << "Selected billboard at index " << std::to_string(o) << std::endl;
            billboard = _overlays.billboards[o];
            // billboard->shift(Vec3f({ tracerPathDiff.first, tracerPathDiff.second, 0.0f })); // moving when something is dragged
            for(unsigned p = 0; p < billboard->getActorCount() - 1; p++){
                if(actor == billboard->getGeoActor(p)){
#if defined(RASTERON_H) && PROGRAM_IS_OVERLAY
                    billboard->setState(p, event == MOUSE_RightBtn_Press || event == MOUSE_LeftBtn_Press);
                    billboard->setState(p, pickerCoord[0], pickerCoord[1]); // for elements that require relative offset
                    if(o == PROGRAM_AppBar) mode = PROGRAM_SUBMENUS - 1 - p; // onOverlayUpdate(PROGRAM_AppBar, p);
                    else if(o == PROGRAM_Sculpt){
                        _background.mesh = &_background.meshes[p];
                        if(isEnable_background) createBackground(nullptr);
                    }
                    else if(o == PROGRAM_Paint){ 
                        // if(p != PROGRAM_SUBMENUS - 1){
                            ImageSize size = { SAMPLER_WIDTH, SAMPLER_HEIGHT };
                            for(unsigned t = 0; t < 9; t++){
                                unsigned short i = t; // (t + p) % 8;
                                switch(p) {
                                    case 0: _overlays.textures[i] = Sampler_Gradient((SIDE_Type)(rand() % 5), RAND_COLOR(), RAND_COLOR()); break; // random gradients
                                    case 1: _overlays.textures[i] = Sampler_2D(linedImgOp(size, RAND_COLOR(), RAND_COLOR(), (rand() % 10) + 10, (rand() % 2 == 0)? 0.0 : 1.0)); break; // lines
                                    case 2: _overlays.textures[i] = Sampler_2D(checkeredImgOp(size, { (unsigned)(rand() % 15) + 5, (unsigned)(rand() % 15) + 5, RAND_COLOR(), RAND_COLOR() })); break; // lines
                                    case 3: _overlays.textures[i] = Sampler_Noise({ (unsigned)pow(2, t + 1), (unsigned)pow(2, t + 1), RAND_COLOR(), RAND_COLOR() }); break; // basic noise
                                    case 4: _overlays.textures[i] = Sampler_Noise({ (unsigned)pow(2, t + 1), (unsigned)pow(2, t + 1), RAND_COLOR(), RAND_COLOR() }, (rand() % 4) + 1); break; // octave noise
                                    case 5: _overlays.textures[i] = Sampler_2D(noiseImgOp_tiled(size, { (unsigned)pow(2, t + 1), (unsigned)pow(2, t + 1), RAND_COLOR(), RAND_COLOR() })); break; // tiled noise
                                    case 6: _overlays.textures[i] = Sampler_2D(noiseImgOp_add(size, { (unsigned)pow(2, t + 1), (unsigned)pow(2, t + 1), RAND_COLOR(), RAND_COLOR() }, (rand() % 4) + 1)); break; // added noise
                                    case 7: _overlays.textures[i] = Sampler_2D(noiseImgOp_diff(size, { (unsigned)pow(2, t + 1), (unsigned)pow(2, t + 1), RAND_COLOR(), RAND_COLOR() }, (rand() % 4) + 1)); break; // subtracted noise
                                    default: _overlays.textures[i] = Sampler_File(_overlays.scene.texImgPaths[t]);
                                }
                                if(t == 0) _background.scene.addTexture(&_overlays.textures[t]);
                                else _overlays.scene.addTexture(std::to_string(t + 1), &_overlays.textures[t]);
                            } 
                            _renderer->texturizeScene(&_background.scene);
                            _renderer->texturizeScene(&_overlays.scene);
                        /* } else {
                            _background.volumeImg.setColors(RAND_COLOR());
                            _background.scene.addVolumeTex("program_background", &_background.volumeImg);
                            _renderer->texturizeScene(&_background.scene);
                            // _renderer->texturizeScene(&_overlays.scene);
                        } */
                    } else if(o == PROGRAM_Media)
                        switch(p){
                            case 3: timeline.dynamic_ticker.setTime(TIMELINE_START); break;
                            case 4: timeline.dynamic_ticker.isPaused = !timeline.dynamic_ticker.isPaused; break;
                            case 5: timeline.dynamic_ticker.setTime(TIMELINE_END); break;
                        }
                    else if(o == PROGRAM_Timeline) timeline.dynamic_ticker.setTime(pickerCoord[0]);
                    else if(o == PROGRAM_Camera){
                        unsigned short s = PROGRAM_SUBMENUS - 1 - p;
                        float m = 0.1;
                        switch(s){
                            // case 0: case 1: case 2: pivot = pivot + Vec3f({ (s == 0)? m : 0.0F, (s == 1)? m : 0.0F, (s == 2)? m : 0.0F }); break;
                            case 0: projX *= 1.25; break; case 1: projY *= 1.25; break; case 2: projZ *= 1.25; break;
                            case 3: projType = PROJECTION_Orthographic; break;
                            case 4: projType = PROJECTION_Perspective; break;
                            case 5: projType = PROJECTION_Hyperspace; break;
                            case 6: projX *= 0.75; break; case 7: projY *= 0.75; break; case 8: projZ *= 0.75; break;
                            // case 6: case 7: case 8: pivot = pivot + Vec3f({ (s == 6)? m : 0.0F, (s == 7)? m : 0.0F, (s == 8)? m : 0.0F });
                        }
                        Topl_Program::camera.setProjMatrix(Projection(projType, projX, projX, projY, projY, projZ, projZ).genProjMatrix(*Topl_Program::camera.getPos()));
                        // Topl_Program::camera.setRot(pivot);
                        std::cout << "Camera position is " << Topl_Program::camera.getPos()->toString() 
                            << ", rotation is " << Topl_Program::camera.getRot()->toString() << std::endl
                            << "Camera projection is " << Topl_Program::camera.getProjMatrix()->toString() << std::endl;
                    }
                    else if(o == PROGRAM_Object){
                        if(Topl_Program::lastPickerObj != nullptr){
                            switch(p){
                                MOV_BTN_CASES: if(positions_map.find(lastPickerObj) == positions_map.end()) positions_map.insert({ lastPickerObj, *lastPickerObj->getPos() }); break;
                                ROT_BTN_CASES: if(rotations_map.find(lastPickerObj) == rotations_map.end()) rotations_map.insert({ lastPickerObj, *lastPickerObj->getRot() }); break;
                                SIZ_BTN_CASES: if(scales_map.find(lastPickerObj) == scales_map.end()) scales_map.insert({ lastPickerObj, *lastPickerObj->getSize() }); break;
                            }

                            switch(p){
                                case 0: Topl_Program::lastPickerObj->updatePos(Vec3f({ Topl_Program::speed, 0.0F, 0.0F })); break;
                                case 1: Topl_Program::lastPickerObj->updatePos(Vec3f({ -Topl_Program::speed, 0.0F, 0.0F })); break;
                                case 2: Topl_Program::lastPickerObj->updatePos(Vec3f({ 0.0F, Topl_Program::speed, 0.0F })); break;
                                case 3: Topl_Program::lastPickerObj->updatePos(Vec3f({ 0.0F, -Topl_Program::speed, 0.0F })); break;
                                case 4: Topl_Program::lastPickerObj->updatePos(Vec3f({ 0.0F, 0.0F, Topl_Program::speed })); break;
                                case 5: Topl_Program::lastPickerObj->updatePos(Vec3f({ 0.0F, 0.0F, -Topl_Program::speed })); break;
                                case 6: Topl_Program::lastPickerObj->updateRot(Vec3f({ Topl_Program::speed, 0.0F, 0.0F })); break;
                                case 7: Topl_Program::lastPickerObj->updateRot(Vec3f({ -Topl_Program::speed, 0.0F, 0.0F })); break;
                                case 8: Topl_Program::lastPickerObj->updateRot(Vec3f({ 0.0F, Topl_Program::speed, 0.0F })); break;
                                case 9: Topl_Program::lastPickerObj->updateRot(Vec3f({ 0.0F, -Topl_Program::speed, 0.0F })); break;
                                case 10: Topl_Program::lastPickerObj->updateRot(Vec3f({ 0.0F, 0.0F, Topl_Program::speed })); break;
                                case 11: Topl_Program::lastPickerObj->updateRot(Vec3f({ 0.0F, 0.0F, -Topl_Program::speed })); break;
                                case 12: Topl_Program::lastPickerObj->updateSize(Vec3f({ Topl_Program::speed, 0.0F, 0.0F })); break;
                                case 13: Topl_Program::lastPickerObj->updateSize(Vec3f({ -Topl_Program::speed, 0.0F, 0.0F })); break;
                                case 14: Topl_Program::lastPickerObj->updateSize(Vec3f({ 0.0F, Topl_Program::speed, 0.0F })); break;
                                case 15: Topl_Program::lastPickerObj->updateSize(Vec3f({ 0.0F, -Topl_Program::speed, 0.0F })); break;
                                case 16: Topl_Program::lastPickerObj->updateSize(Vec3f({ 0.0F, 0.0F, Topl_Program::speed })); break;
                                case 17: Topl_Program::lastPickerObj->updateSize(Vec3f({ 0.0F, 0.0F, -Topl_Program::speed })); break;
                                default: std::cout << std::to_string(p) << " billboard pane action from " << billboard->getPrefix() << std::endl;
                            }

                            switch(p){
                                MOV_BTN_CASES: timeline.addSequence_vec3f(&positions_map[lastPickerObj], std::make_pair(TIMELINE_FORETELL * 2, *lastPickerObj->getPos())); break;
                                ROT_BTN_CASES: timeline.addSequence_vec3f(&rotations_map[lastPickerObj], std::make_pair(TIMELINE_FORETELL * 2, *lastPickerObj->getRot())); break;
                                SIZ_BTN_CASES: timeline.addSequence_vec3f(&scales_map[lastPickerObj], std::make_pair(TIMELINE_FORETELL * 2, *lastPickerObj->getSize())); break;
                            }
                        }
                    }
                    else if(o == PROGRAM_Shader){
                        char keySim = (p + 1) + '0';
                        Topl_Program::_onAnyKey(keySim); // switch pipelines
                        _savedPipeline = _renderer->getPipeline();
                    }
                    onOverlayUpdate((PROGRAM_Menu)o, PROGRAM_SUBMENUS - 1 - p);
#endif
                }
            }
        }
    _renderer->texturizeScene(&_overlays.scene);
}

void Topl_Program::_onAnyKey(char k){
    std::cout << "Key is " << std::to_string(k) << std::endl;
    if(isspace(k) && k != 0x0D){ 
        isEnable_overlays = !isEnable_overlays; 
        // for(unsigned b = 0; b < PROGRAM_BILLBOARDS; b++) _overlays.billboards[b]->toggleShow();
        timeline.dynamic_ticker.isPaused = !timeline.dynamic_ticker.isPaused; // Topl_Program::userInput += (isalpha(k))? tolower(k) : k;
    }
    else if (isspace(k) && k == 0x0D) isEnable_background = !isEnable_background;
#ifdef TOPL_ENABLE_TEXTURES
    else if(k == ';' && isEnable_screencap){
        Sampler_2D frameImg = _renderer->frame();
        // TODO: Save to screenshot
        // queue_addImg(cachedFrames, frameImg.getImage(), 0); // index % cachedFrames->frameCount);
        // std::cout << "cachedFrames image at " << std::to_string(index) << " is " << queue_getImg(cachedFrames, index)->name << std::endl;
    }
#endif

    if(Topl_Program::isCtrl_keys && isalpha(k)){
        switch(tolower(k)){ // TODO: Add same logic for picked object?
            case 'w': Topl_Program::camera.updatePos({ 0.0, Topl_Program::speed, 0.0 }); break;
            case 's': Topl_Program::camera.updatePos({ 0.0, -Topl_Program::speed, 0.0 }); break;
            case 'a': Topl_Program::camera.updatePos({ -Topl_Program::speed, 0.0, 0.0 }); break;
            case 'd': Topl_Program::camera.updatePos({ Topl_Program::speed, 0.0, 0.0 }); break;
            case 'c': Topl_Program::camera.updatePos({ 0.0F, 0.0, -Topl_Program::speed }); break;
            case 'v': Topl_Program::camera.updatePos({ 0.0F, 0.0, Topl_Program::speed }); break;
            case 'q': Topl_Program::camera.updateRot({ -Topl_Program::speed, 0.0, 0.0 }); break;
            case 'e': Topl_Program::camera.updateRot({ Topl_Program::speed, 0.0, 0.0 }); break;
            case 'r': Topl_Program::camera.updateRot({ 0.0F, -Topl_Program::speed, 0.0 }); break;
            case 'f': Topl_Program::camera.updateRot({ 0.0F, Topl_Program::speed, 0.0 }); break;
            case 't': Topl_Program::camera.updateRot({ 0.0F, 0.0, -Topl_Program::speed }); break;
            case 'g': Topl_Program::camera.updateRot({ 0.0F, 0.0, Topl_Program::speed }); break;
            case 'z': Topl_Program::camera.setZoom(*Topl_Program::camera.getZoom() * (1.0F + Topl_Program::speed * 0.15F)); break;
            case 'x': Topl_Program::camera.setZoom(*Topl_Program::camera.getZoom() * (1.0F - Topl_Program::speed * 0.15F)); break;
        }

        if(tolower(k) == 'w' || tolower(k) == 's' || tolower(k) == 'a' || tolower(k) == 'd' || tolower(k) == 'x' || tolower(k) == 'v' || tolower(k) == 'c')
            Topl_Program::timeline.addSequence_vec3f(&_camPos, std::make_pair(TIMELINE_FORETELL, *(Topl_Program::camera.getPos())));
        else if(tolower(k) == 'q' || tolower(k) == 'e' || tolower(k) == 'r' || tolower(k) == 'f' || tolower(k) == 't' || tolower(k) == 'g')
            Topl_Program::timeline.addSequence_vec3f(&_camRot, std::make_pair(TIMELINE_FORETELL, *(Topl_Program::camera.getRot())));
        // else if(tolower(k) == 'z' || tolower(k) == 'c')
        //    Topl_Program::timeline.addSequence_float(&_camZoom, std::make_pair(TIMELINE_FORETELL, *(Topl_Program::camera.getZoom())));
    }
    if(Topl_Program::isCtrl_shader){
#ifdef _WIN32
        if (k == (char)0x25) Topl_Program::shaderMode--;
        else if (k == (char)0x26) Topl_Program::shaderMode += (Topl_Program::shaderMode > 0) ? 100 : -100;
        else if (k == (char)0x27) Topl_Program::shaderMode++;
        else if (k == (char)0x28) Topl_Program::shaderMode *= -1;
#else // for Linux?
        if (k == (char)0x69) Topl_Program::shaderMode--;
        else if (k == (char)0x67) Topl_Program::shaderMode += (Topl_Program::shaderMode > 0)? 100 : -100;
        else if (k == (char)0x6A) Topl_Program::shaderMode++;
        else if (k == (char)0x6C) Topl_Program::shaderMode *= -1;
#endif
        else if(isdigit(k)){
            switch(tolower(k)){
                case '0': Topl_Factory::switchPipeline(_renderer, _flatPipeline); break;
                case '1': Topl_Factory::switchPipeline(_renderer, _texPipeline); break;
                case '2': Topl_Factory::switchPipeline(_renderer, _beamsPipeline); break;
                case '3': Topl_Factory::switchPipeline(_renderer, _materialPipeline); break;
                case '4': Topl_Factory::switchPipeline(_renderer, _fieldPipeline); break;
                case '5': Topl_Factory::switchPipeline(_renderer, _patternPipeline); break;
                case '6': Topl_Factory::switchPipeline(_renderer, _effectPipeline); break;
                case '7': Topl_Factory::switchPipeline(_renderer, _canvasPipeline/*_geomPipeline */); break;
                case '8': Topl_Factory::switchPipeline(_renderer, _geomPipeline); break; // switch to drawing patch mode?
                case '9': Topl_Factory::switchPipeline(_renderer, _tessPipeline); break; // switch to drawing patch mode?
            }
        }

        if(k == (char)0x25 || k == (char)0x26 || k == (char)0x27 || k == (char)0x28 || k == '-' || k == '_' || k == '+' || k == '='){
            setShadersMode(Topl_Program::shaderMode);
            unsigned shaderConsoleMode = (Topl_Program::shaderMode > 0)? Topl_Program::shaderMode : Topl_Program::shaderMode * -1;
            std::cout << "Shader mode is " << ((Topl_Program::shaderMode < 0)? "-" : "") << std::to_string(shaderConsoleMode) << std::endl;
        }
    }
}

void Topl_Program::_onAnyPress(enum MOUSE_Event event, std::pair<float, float> cursor){
    float x = cursor.first; float y = cursor.second;

    if(!Topl_Program::isCtrl_input) Topl_Program::userInput.clear();
    Topl_Program::cursorPos = { x, y, 0.0F };

    if(Topl_Program::pickerObj != nullptr) Topl_Program::lastPickerColor = Topl_Program::pickerColor;
    Topl_Program::lastPickerCoord = Topl_Program::pickerCoord;
}


#ifndef __ANDROID__
void Topl_Program::setup(const char* execPath, const char* name) {
#else
void Topl_Program::setup(android_app * app) {
#endif
    srand(time(NULL));

    // Event Handling
#ifndef __ANDROID__
    _platform = new Platform(execPath, name);
    _platform->createWindow(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);
#else
    _platform = new Platform(app);
    while (_platform->getParentWindow() == nullptr && _platform->handleEvents() && !app->destroyRequested)
        _platform->awaitWindow(); // waiting for window on Android
#endif
    _renderer = Topl_Factory::genRenderer(_backend, _platform);
    // _renderer->setCamera(&Topl_Program::camera);
    _renderer->setDrawMode(DRAW_Triangles);

    Platform::keyControl.addHandler(std::bind(&Topl_Program::_onAnyKey, this, std::placeholders::_1));
    Platform::mouseControl.addHandler(std::bind(&Topl_Program::_onAnyPress, this, std::placeholders::_1, std::placeholders::_2));

    setPipelines();
    _renderer->buildScene(&_editor.scene);
#ifdef TOPL_ENABLE_TEXTURES
    if (isEnable_background) createBackground(&_background.image);
    if (isEnable_overlays) createOverlays(0.85);
    _editor.nameActor.updateSize({ (float)_editor.nameActor.getName().length(), 0.0F, 0.0F });
#if RASTERON_ENABLE_FONT
    _editor.nameImg = Sampler_Text({ _editor.fontPath.c_str(), "000000", 0xFF111111, 0xFFEEEEEE });
    _editor.scene.addTexture(_editor.nameActor.getName(), &_editor.nameImg);
#endif
    _renderer->texturizeScene(&_editor.scene);

    ImageSize frameSize = { TOPL_WIN_HEIGHT, TOPL_WIN_WIDTH };
    Topl_Program::cachedFrames = RASTERON_QUEUE_ALLOC("frames", frameSize, CACHED_FRAME_COUNT);
#else
    if (isEnable_background) createBackground(nullptr);
#endif
}

void Topl_Program::createBackground(Sampler_2D* backgroundTex){
    _background.actor.setPos({ 0.0F, 0.0F, -1.0F });
    if(_background.mesh->getTessLevel() < PROGRAM_BK_TESS) _background.mesh->tesselate(PROGRAM_BK_TESS);
    _background.actor.setMesh(_background.mesh);
    if(_background.actor.pickFunc == nullptr) _background.actor.pickFunc = std::bind(&Topl_Program::_backgroundCallback, this, std::placeholders::_1, std::placeholders::_2);
#ifdef TOPL_ENABLE_TEXTURES
    if(backgroundTex != nullptr){
        _background.scene.addTexture("program_background", backgroundTex);
        for(unsigned t = 1; t < 8; t++) _background.scene.addTexture(std::to_string(t), backgroundTex);
        _background.scene.addVolumeTex("program_background", &_background.volumeImg);
    }
#endif
    _renderer->buildScene(&_background.scene);
    _renderer->texturizeScene(&_background.scene);
}

void Topl_Program::createOverlays(double size){
    _overlays.billboard_appbar.scale({ 1.5F, 0.15F, 1.0F });
    _overlays.billboard_appbar.shift({ 0.0F, 0.87F, 0.0F });
    _overlays.billboard_camera.scale({ 1.5F, 0.15F, 1.0F });
    _overlays.billboard_camera.shift({ 0.0F, 0.97F, 0.0F });
    _overlays.billboard_timeline.scale({ 1.5F, 0.15F, 1.0F });
    _overlays.billboard_timeline.shift({ 0.0F, -0.975F, 0.0F });
    _overlays.billboard_sculpt.scale({ 0.12F, 1.15F, 1.0F });
    _overlays.billboard_sculpt.shift({ -0.965F, 0.0F, 0.0F });
    _overlays.billboard_paint.scale({ 0.12F, 1.15F, 1.0F });
    _overlays.billboard_paint.shift({ 0.965F, 0.0F, 0.0F });
    _overlays.billboard_media.shift({ -0.278F, -0.845F, 0.0F });
    _overlays.billboard_object.shift({ 0.0F, -0.845F, 0.0F });
    _overlays.billboard_shader.shift({ 0.278F, -0.845F, 0.0F });
#if defined(RASTERON_H) && PROGRAM_IS_OVERLAY
    _overlays.billboard_timeline.overlay(0, &_overlays.timeSlider);
    for(unsigned b = 0; b < PROGRAM_SUBMENUS; b++){ 
        _overlays.billboard_appbar.overlay(b, &_overlays.numberButtons[PROGRAM_SUBMENUS - 1 - b]);
        _overlays.billboard_camera.overlay(b, (b > 2 && b < 6)? (Sampler_UI*)&_overlays.cameraButtons[5 - b] : (b < 3)? (Sampler_UI*)&_overlays.plusButton : (Sampler_UI*)&_overlays.minusButton);
        _overlays.billboard_sculpt.overlay(b, &_overlays.sculptButtons[b]);
        _overlays.billboard_paint.overlay(b, &_overlays.paintButtons[b]);
        _overlays.billboard_shader.overlay(b, &_overlays.pipelineButtons[b]);
        if(b < _overlays.billboard_media.getActorCount() - 1) _overlays.billboard_media.overlay(b, (b > 2)? (Sampler_UI*)&_overlays.mediaButtons[b - 3] : (Sampler_UI*)&_overlays.mediaLabels[b]);
    }
    for(unsigned b = 0; b < _overlays.billboard_object.getActorCount() - 1; b++) // _overlays.billboard_object.overlay(b, (b % 2 == 0)? &_overlays.plusButton : &_overlays.minusButton);
        switch(14 - b){
            case 0: case 5: case 10: _overlays.billboard_object.overlay(b, &_overlays.axisLabels[b / 5]); break;
            case 1: case 6: case 11: _overlays.billboard_object.overlay(b, &_overlays.minusButton); break;
            case 2: case 7: case 12: _overlays.billboard_object.overlay(b, &_overlays.plusButton); break;
            case 3: case 8: case 13: _overlays.billboard_object.overlay(b, &_overlays.minusButton); break;
            case 4: case 9: case 14: _overlays.billboard_object.overlay(b, &_overlays.plusButton); break;
        }
#endif
    // _overlays.billboard_object.expandHorz(std::make_pair(0, 1), 1);
    // _texVShader.setParams(_overlays.billboard_object.getGeoActor(5), { 1, 0.0, VEC_3F_ZERO, VEC_3F_ONES });

    for(unsigned short o = 0; o < PROGRAM_BILLBOARDS; o++){
        if(o < PROGRAM_Timeline) _overlays.billboards[o]->scale({ ((o != PROGRAM_Object)? 0.5F : 0.715F) * (float)size, 0.33F * (float)size, 1.0F });
        if(o < PROGRAM_Sculpt) _overlays.billboards[o]->getGeoActor(_overlays.billboards[o]->getActorCount() - 1)->updateSize({ 0.0F, (o < PROGRAM_Timeline)? 0.015F : 0.045F, 0.0F });
        _overlays.billboards[o]->getGeoActor(_overlays.billboards[o]->getActorCount() - 1)->updatePos({ 0.0F, 0.01F, 0.0F });
        _overlays.billboards[o]->getGeoActor(_overlays.billboards[o]->getActorCount() - 1)->pickFunc = std::bind(&Topl_Program::_overlayCallback, this, std::placeholders::_1, std::placeholders::_2);
#ifdef TOPL_ENABLE_TEXTURES
        for(unsigned e = 0; e < _overlays.billboards[o]->getActorCount(); e++)
            if(e != _overlays.billboards[o]->getActorCount() - 1)
                _overlays.billboards[o]->getGeoActor(e)->pickFunc = std::bind(&Topl_Program::_overlayCallback, this, std::placeholders::_1, std::placeholders::_2);
            else _overlays.billboards[o]->getGeoActor(e)->updateSize({ 0.01F, 0.01F, 0.0F });
#endif
    }

    _renderer->buildScene(&_overlays.scene);
    _renderer->texturizeScene(&_overlays.scene);
}

void Topl_Program::renderScene(Topl_Scene* scene, Topl_Pipeline* pipeline, int mode){
    if(pipeline != nullptr){
        Topl_Factory::switchPipeline(_renderer, pipeline);
        setShadersMode(mode);
    }

    // _renderer->setDrawMode(DRAW_Triangles);
    _renderer->updateScene(scene);
    _renderer->drawScene(scene);
}

void Topl_Program::cleanup() {
#ifdef TOPL_ENABLE_TEXTURES
#if RASTERON_ENABLE_FONT
    cleanupFreeType();
#endif
#endif
	delete(_renderer);
	delete(_platform);
}
