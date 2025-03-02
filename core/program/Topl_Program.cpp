#include "program/Topl_Program.hpp"

Topl_Timeline Topl_Program::timeline = Topl_Timeline();
Vec3f Topl_Program::cursorPos = { 0.0F, 0.0F, 0.0F };
float Topl_Program::speed = 0.25F; // 0.1F

unsigned Topl_Program::shaderMode = 0;
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
#ifdef RASTERON_H
Rasteron_Queue* Topl_Program::cachedFrames = NULL;
#endif

Topl_Camera Topl_Program::camera = Topl_Camera();
Topl_Pipeline* Topl_Program::_savedPipeline = nullptr;

void Topl_Program::_backgroundCallback(MOUSE_Event event, Geo_Actor* actor){
	// std::cout << "BACKGROUND: Actor is " << actor->getName() << std::endl;
}

void Topl_Program::_overlayCallback(MOUSE_Event event, Geo_Actor* actor){
    std::pair<float, float> tracerPathDiff = Platform::mouseControl.getLastPathDiff();
    Geo_Billboard* billboard = nullptr;
    for(unsigned o = 0; o < 3; o++) 
        if(actor->getName().find(_overlays.billboards[o]->getPrefix()) != std::string::npos){
            billboard = _overlays.billboards[o];
            billboard->shift(Vec3f({ tracerPathDiff.first, tracerPathDiff.second, 0.0f }));
            for(unsigned p = 0; p < billboard->getActorCount() - 1; p++){
                if(actor == billboard->getGeoActor(p)){
#ifdef RASTERON_H
                    billboard->setState(p, event == MOUSE_RightBtn_Press || event == MOUSE_LeftBtn_Press);
                    billboard->setState(p, pickerCoord[0], pickerCoord[1]); // for elements that require relative offset
                    if(o == 0) // handling shader switching
                        switch(p){
                            case 0: Topl_Program::camera.setProjMatrix(Projection(PROJECTION_None, 1.0F).genProjMatrix()); break;
                            case 1: Topl_Program::camera.setProjMatrix(Projection(PROJECTION_None, 3.0F).genProjMatrix()); break;
                            case 2: Topl_Program::camera.setProjMatrix(Projection(PROJECTION_None, 0.33F).genProjMatrix()); break;
                            case 3: Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Orthographic, 1.0F).genProjMatrix()); break;
                            case 4: Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Orthographic, 3.0F).genProjMatrix()); break;
                            case 5: Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Orthographic, 0.33F).genProjMatrix()); break;
                            case 6: Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Perspective, 1.0F).genProjMatrix()); break;
                            case 7: Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Perspective, 3.0F).genProjMatrix()); break;
                            case 8: Topl_Program::camera.setProjMatrix(Projection(PROJECTION_Perspective, 0.33F).genProjMatrix()); break;
                            default: std::cout << std::to_string(p) << " billboard pane action from " << billboard->getPrefix() << std::endl;
                        }
                    else if(o == 1){
                        if(Topl_Program::lastPickerObj != nullptr){
                            switch(p){
                                case 0: case 1: case 2: case 3: if(positions_map.find(lastPickerObj) == positions_map.end()) positions_map.insert({ lastPickerObj, *lastPickerObj->getPos() }); break;
                                case 4: case 5: case 6: case 7: if(rotations_map.find(lastPickerObj) == rotations_map.end()) rotations_map.insert({ lastPickerObj, *lastPickerObj->getRot() }); break;
                                case 8: case 9: case 10: case 11: if(scales_map.find(lastPickerObj) == scales_map.end()) scales_map.insert({ lastPickerObj, *lastPickerObj->getSize() }); break;
                            }

                            switch(p){
                                case 0: Topl_Program::lastPickerObj->updatePos(Vec3f({ Topl_Program::speed, 0.0F, 0.0F })); break;
                                case 1: Topl_Program::lastPickerObj->updatePos(Vec3f({ -Topl_Program::speed, 0.0F, 0.0F })); break;
                                case 2: Topl_Program::lastPickerObj->updatePos(Vec3f({ 0.0F, Topl_Program::speed, 0.0F })); break;
                                case 3: Topl_Program::lastPickerObj->updatePos(Vec3f({ 0.0F, -Topl_Program::speed, 0.0F })); break;
                                case 4: Topl_Program::lastPickerObj->updateRot(Vec3f({ Topl_Program::speed, 0.0F, 0.0F })); break;
                                case 5: Topl_Program::lastPickerObj->updateRot(Vec3f({ -Topl_Program::speed, 0.0F, 0.0F })); break;
                                case 6: Topl_Program::lastPickerObj->updateRot(Vec3f({ 0.0F, Topl_Program::speed, 0.0F })); break;
                                case 7: Topl_Program::lastPickerObj->updateRot(Vec3f({ 0.0F, -Topl_Program::speed, 0.0F })); break;
                                case 8: Topl_Program::lastPickerObj->updateSize(Vec3f({ Topl_Program::speed, 0.0F, 0.0F })); break;
                                case 9: Topl_Program::lastPickerObj->updateSize(Vec3f({ -Topl_Program::speed, 0.0F, 0.0F })); break;
                                case 10: Topl_Program::lastPickerObj->updateSize(Vec3f({ 0.0F, Topl_Program::speed, 0.0F })); break;
                                case 11: Topl_Program::lastPickerObj->updateSize(Vec3f({ 0.0F, -Topl_Program::speed, 0.0F })); break;
                                default: std::cout << std::to_string(p) << " billboard pane action from " << billboard->getPrefix() << std::endl;
                            }

                            switch(p){
                                case 0: case 1: case 2: case 3: timeline.addSequence_vec3f(&positions_map[lastPickerObj], std::make_pair(TIMELINE_FORETELL * 2, *lastPickerObj->getPos())); break;
                                case 4: case 5: case 6: case 7: timeline.addSequence_vec3f(&rotations_map[lastPickerObj], std::make_pair(TIMELINE_FORETELL * 2, *lastPickerObj->getRot())); break;
                                case 8: case 9: case 10: case 11: timeline.addSequence_vec3f(&scales_map[lastPickerObj], std::make_pair(TIMELINE_FORETELL * 2, *lastPickerObj->getSize())); break;
                            }
                        }
                    }
                    else {
                        char keySim = (p + 1) + '0';
                        Topl_Program::_onAnyKey(keySim); // switch pipelines
                        _savedPipeline = _renderer->getPipeline();
                    }
#endif
                }
            }
        }
    _renderer->texturizeScene(&_overlays.scene);
}

void Topl_Program::_onAnyKey(char k){
    std::cout << "Key is " << std::to_string(k) << std::endl;
    if(isspace(k) && k != 0x0D) timeline.dynamic_ticker.isPaused = !timeline.dynamic_ticker.isPaused; // Topl_Program::userInput += (isalpha(k))? tolower(k) : k;

    if(Topl_Program::isCtrl_keys && isalpha(k)){
        switch(tolower(k)){
            case 'w': Topl_Program::camera.updatePos({ 0.0, Topl_Program::speed, 0.0 }); break;
            case 's': Topl_Program::camera.updatePos({ 0.0, -Topl_Program::speed, 0.0 }); break;
            case 'a': Topl_Program::camera.updatePos({ -Topl_Program::speed, 0.0, 0.0 }); break;
            case 'd': Topl_Program::camera.updatePos({ Topl_Program::speed, 0.0, 0.0 }); break;
            case 'x': Topl_Program::camera.updatePos({ 0.0F, 0.0, -Topl_Program::speed }); break;
            case 'v': Topl_Program::camera.updatePos({ 0.0F, 0.0, Topl_Program::speed }); break;
            case 'q': Topl_Program::camera.updateRot({ -Topl_Program::speed, 0.0, 0.0 }); break;
            case 'e': Topl_Program::camera.updateRot({ Topl_Program::speed, 0.0, 0.0 }); break;
            case 'r': Topl_Program::camera.updateRot({ 0.0F, -Topl_Program::speed, 0.0 }); break;
            case 'f': Topl_Program::camera.updateRot({ 0.0F, Topl_Program::speed, 0.0 }); break;
            case 't': Topl_Program::camera.updateRot({ 0.0F, 0.0, -Topl_Program::speed }); break;
            case 'g': Topl_Program::camera.updateRot({ 0.0F, 0.0, Topl_Program::speed }); break;
            case 'z': Topl_Program::camera.setZoom(*Topl_Program::camera.getZoom() * (1.0F + Topl_Program::speed)); break;
            case 'c': Topl_Program::camera.setZoom(*Topl_Program::camera.getZoom() * (1.0F - Topl_Program::speed)); break;
            case '[': case '{': isCtrl_shader = !isCtrl_shader; break;
            case ']': case '}': isEnable_overlays = !isEnable_overlays; break;
        }

        if(tolower(k) == 'w' || tolower(k) == 's' || tolower(k) == 'a' || tolower(k) == 'd' || tolower(k) == 'x' || tolower(k) == 'v')
            Topl_Program::timeline.addSequence_vec3f(&_camPos, std::make_pair(TIMELINE_FORETELL, *(Topl_Program::camera.getPos())));
        else if(tolower(k) == 'q' || tolower(k) == 'e' || tolower(k) == 'r' || tolower(k) == 'f' || tolower(k) == 't' || tolower(k) == 'g')
            Topl_Program::timeline.addSequence_vec3f(&_camRot, std::make_pair(TIMELINE_FORETELL, *(Topl_Program::camera.getRot())));
        // else if(tolower(k) == 'z' || tolower(k) == 'c')
        //    Topl_Program::timeline.addSequence_float(&_camZoom, std::make_pair(TIMELINE_FORETELL, *(Topl_Program::camera.getZoom())));
#ifdef RASTERON_H
        /* else if(k == ';'){
            Rasteron_Image* frameImg = _renderer->frame();
            writeFileImageRaw("Output", Sampler_Bmp, frameImg->height, frameImg->width, frameImg->data);
            RASTERON_DEALLOC(frameImg);
        } */
#endif
    }
    if(Topl_Program::isCtrl_shader){
        if(k == (char)0x25) Topl_Program::shaderMode--;
        else if(k == (char)0x26) Topl_Program::shaderMode += (Topl_Program::shaderMode > 0)? 100 : -100;
        else if(k == (char)0x27) Topl_Program::shaderMode++;
        else if(k == (char)0x28) Topl_Program::shaderMode *= -1;
        else if(isdigit(k)){
            switch(tolower(k)){
                case '0': Topl_Factory::switchPipeline(_renderer, _flatPipeline); break;
                case '1': Topl_Factory::switchPipeline(_renderer, _texPipeline); break;
                case '2': Topl_Factory::switchPipeline(_renderer, _beamsPipeline); break;
                case '3': Topl_Factory::switchPipeline(_renderer, _materialPipeline); break;
                case '4': Topl_Factory::switchPipeline(_renderer, _canvasPipeline); break;
                case '5': Topl_Factory::switchPipeline(_renderer, _patternPipeline); break;
                case '6': Topl_Factory::switchPipeline(_renderer, _effectPipeline); break;
                case '7': Topl_Factory::switchPipeline(_renderer, _geomPipeline); break; // tesselation stages
                case '8': Topl_Factory::switchPipeline(_renderer, _tessPipeline); break; // geometry stage
                case '9': Topl_Factory::switchPipeline(_renderer, _longPipeline); break; // tesselation & geometry stages
            }

            if(tolower(k) == '8' || tolower(k) == '9') _renderer->setDrawMode(DRAW_Patch);
        }

        if(k == (char)0x25 || k == (char)0x26 || k == (char)0x27 || k == (char)0x28 || k == '-' || k == '_' || k == '+' || k == '='){
            setShadersMode(Topl_Program::shaderMode);
            if(Topl_Program::shaderMode > 0) std::cout << "Shader mode is " << std::to_string(Topl_Program::shaderMode) << std::endl;
            else  std::cout << "Shader mode is -" << std::to_string(-1 * Topl_Program::shaderMode) << std::endl;
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
Topl_Program::Topl_Program(const char* execPath, const char* name, BACKEND_Target backend) : _backend(backend) {
#else
Topl_Program::Topl_Program(android_app* app) : _backend(BACKEND_GL4){
#endif
    srand(time(NULL));

	// Event Handling
#ifndef __ANDROID__
	_platform = new Platform(execPath, name);
    _platform->createWindow(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);
#else
    _platform = new Platform(app);
    while(_platform->getParentWindow() == nullptr && _platform->handleEvents() && !app->destroyRequested)
        _platform->awaitWindow(); // waiting for window on Android
#endif
    _renderer = Topl_Factory::genRenderer(_backend, _platform);
	// _renderer->setCamera(&Topl_Program::camera);
	_renderer->setDrawMode(DRAW_Triangles);

    Platform::keyControl.addHandler(std::bind(&Topl_Program::_onAnyKey, this, std::placeholders::_1));
    Platform::mouseControl.addHandler(std::bind(&Topl_Program::_onAnyPress, this, std::placeholders::_1, std::placeholders::_2));

    setPipelines();
    _renderer->buildScene(&_editor.scene);
#ifdef RASTERON_H
    if(isEnable_background) createBackground(&_background.image);
    if(isEnable_overlays) createOverlays(0.85);
    _editor.nameActor.updateSize({ (float)_editor.nameActor.getName().length(), 0.0F, 0.0F });
    // _editor.nameImg.setTextImage({ _editor.fontPath.c_str(), "000000", 0xFF111111, 0xFFEEEEEE });
    _editor.scene.addTexture(_editor.nameActor.getName(), &_editor.nameImg);
    _renderer->texturizeScene(&_editor.scene);
#else
    if(isEnable_background) createBackground(nullptr);
#endif

    // Texturing Data Generation
#ifdef RASTERON_H
    _invertImage = INVERT_IMG_TRUE; // Imaging Initialization

	// ImageSize frameSize = { Platform::getViewportHeight(_platformCtx.window), Platform::getViewportWidth(_platformCtx.window) };
	ImageSize frameSize = { TOPL_WIN_HEIGHT, TOPL_WIN_WIDTH };
	Topl_Program::cachedFrames = RASTERON_QUEUE_ALLOC("frames", frameSize, CACHED_FRAME_COUNT);
#endif
}

void Topl_Program::setPipelines(){
	if(_backend == BACKEND_GL4){
		_texVShader = Textured_VertexShader_GL4(); _texPShader = Textured_PixelShader_GL4();
		_beamsVShader = Beams_VertexShader_GL4(); _beamsPShader = Beams_PixelShader_GL4();
        _materialVShader = Material_VertexShader_GL4(); _materialPShader = Material_PixelShader_GL4();
		_effectVShader = Effect_VertexShader_GL4(); _effectPShader = Effect_PixelShader_GL4();
		_canvasVShader = Canvas_VertexShader_GL4(); _canvasPShader = Canvas_PixelShader_GL4();
		_patternVShader = Pattern_VertexShader_GL4(); _patternPShader = Pattern_PixelShader_GL4();
		_flatVShader = Flat_VertexShader_GL4(); _flatPShader = Flat_PixelShader_GL4();
		for(unsigned p = 0; p < PROGRAM_PIPELINES; p++){
            _geomShaders[p] = Advance_GeometryShader_GL4();
            _tessCtrlShaders[p] = Advance_TessCtrlShader_GL4();
            _tessEvalShaders[p] = Advance_TessEvalShader_GL4();
        }
	}
#ifdef _WIN32
	else if(_backend == BACKEND_DX11){
		_texVShader = Textured_VertexShader_DX11(); _texPShader = Textured_PixelShader_DX11();
		_beamsVShader = Beams_VertexShader_DX11(); _beamsPShader = Beams_PixelShader_DX11();
        _materialVShader = Material_VertexShader_DX11(); _materialPShader = Material_PixelShader_DX11();
		_effectVShader = Effect_VertexShader_DX11(); _effectPShader = Effect_PixelShader_DX11();
		_canvasVShader = Canvas_VertexShader_DX11(); _canvasPShader = Canvas_PixelShader_DX11();
		_patternVShader = Pattern_VertexShader_DX11(); _patternPShader = Pattern_PixelShader_DX11();
		_flatVShader = Flat_VertexShader_DX11(); _flatPShader = Flat_PixelShader_DX11();
		for(unsigned p = 0; p < PROGRAM_PIPELINES; p++){
            _geomShaders[p] = Advance_GeometryShader_DX11();
            _tessCtrlShaders[p] = Advance_TessCtrlShader_DX11();
            _tessEvalShaders[p] = Advance_TessEvalShader_DX11();
        }
	}
#endif
	_texPipeline = Topl_Factory::genPipeline(_backend, &_texVShader, &_texPShader);
	_beamsPipeline = Topl_Factory::genPipeline(_backend, &_beamsVShader, &_beamsPShader);
    _materialPipeline = Topl_Factory::genPipeline(_backend, &_materialVShader, &_materialPShader);
	_canvasPipeline = Topl_Factory::genPipeline(_backend, &_canvasVShader, &_canvasPShader);
	_patternPipeline = Topl_Factory::genPipeline(_backend, &_patternVShader, &_patternPShader);
	_flatPipeline = Topl_Factory::genPipeline(_backend, &_flatVShader, &_flatPShader);
#ifndef __linux__
    _effectPipeline = Topl_Factory::genPipeline(_backend, &_effectVShader, &_effectPShader); // TODO: Figure out why this fails!
#endif
    _geomPipeline = Topl_Factory::genPipeline(_backend, &_flatVShader, &_flatPShader, { &_geomShaders[0] }); // flat shader for now
    _tessPipeline = Topl_Factory::genPipeline(_backend, &_flatVShader, &_flatPShader, { &_tessCtrlShaders[0], &_tessEvalShaders[0] }); // flat shader for now
    _longPipeline = Topl_Factory::genPipeline(_backend, &_flatVShader, &_flatPShader, { &_geomShaders[0], &_tessCtrlShaders[0], &_tessEvalShaders[0] }); // flat shader for now
    Topl_Factory::switchPipeline(_renderer, _texPipeline);
}

void Topl_Program::createBackground(Sampler_2D* backgroundTex){
    _background.mesh.tesselate(PROGRAM_BK_TESS);
    _background.actor.setPos({ 0.0F, 0.0F, -1.0F });
    _background.actor.pickFunc = std::bind(&Topl_Program::_backgroundCallback, this, std::placeholders::_1, std::placeholders::_2);
#ifdef RASTERON_H
    if(backgroundTex != nullptr){
        _background.scene.addTexture("program_background", backgroundTex);
        for(unsigned t = 1; t < 8; t++) _background.scene.addTexture(std::to_string(t), backgroundTex);
    }
#endif
    _renderer->buildScene(&_background.scene);
    _renderer->texturizeScene(&_background.scene);
}

void Topl_Program::createOverlays(double size){
    _overlays.billboard_camera.shift({ -0.75F, -0.9F, 0.0F });
    _overlays.billboard_object.shift({ 0.0F, -0.9F, 0.0F });
    _overlays.billboard_shader.shift({ 0.75F, -0.9F, 0.0F });

    // modifiers and overlays
    // _overlays.billboard_shader.overlay(7, &_overlays.slider); // slider at bottom center of shader billboard
    // for(unsigned b = 0; b < 3; b++) _overlays.billboard_camera.overlay(b + 6, &_overlays.dials[b]);
    for(unsigned b = 0; b < 9; b++){ 
        _overlays.billboard_camera.overlay(b, &_overlays.objectButtons[b]);
        _overlays.billboard_shader.overlay(b, &_overlays.objectButtons[b]);
    }
    for(unsigned b = 0; b < _overlays.billboard_object.getActorCount() - 1; b++) _overlays.billboard_object.overlay(b, (b % 2 == 0)? &_overlays.plusButton : &_overlays.minusButton);
    
    for(unsigned short o = 0; o < 3; o++){
        _overlays.billboards[o]->scale({ 0.5F * (float)size, 0.33F * (float)size, 1.0F });
        _overlays.billboards[o]->getGeoActor(_overlays.billboards[o]->getActorCount() - 1)->updateSize({ 0.0F, 0.015F, 0.0F });
        _overlays.billboards[o]->getGeoActor(_overlays.billboards[o]->getActorCount() - 1)->updatePos({ 0.0F, 0.01F, 0.0F });
        _overlays.billboards[o]->getGeoActor(_overlays.billboards[o]->getActorCount() - 1)->pickFunc = std::bind(&Topl_Program::_overlayCallback, this, std::placeholders::_1, std::placeholders::_2);
#ifdef RASTERON_H
        for(unsigned e = 0; e < _overlays.billboards[o]->getActorCount(); e++)
            if(e != _overlays.billboards[o]->getActorCount() - 1){
                _overlays.billboards[o]->getGeoActor(e)->pickFunc = std::bind(&Topl_Program::_overlayCallback, this, std::placeholders::_1, std::placeholders::_2);
                _overlays.button_map.insert({ _overlays.billboards[o]->getGeoActor(e), new Sampler_Button(MENU_Medium) });
                _overlays.billboards[o]->overlay(e, _overlays.button_map.at(_overlays.billboards[o]->getGeoActor(e)));
            }
        // if(paneTex != nullptr) for(unsigned t = 1; t < 8; t++) _overlays.scene.addTexture(std::to_string(t), paneTex);
#endif
    }
    // _overlays.billboard_shader.expandHorz(std::make_pair(1, 2), 1);
    // _overlays.billboard_object.expandVert(std::make_pair(2, 1), 1);
    // _texVShader.setParams() // dont forgot to add parameters!

    _renderer->buildScene(&_overlays.scene);
    _renderer->texturizeScene(&_overlays.scene);
}

void Topl_Program::renderScene(Topl_Scene* scene, Topl_Pipeline* pipeline, int mode){
    if(pipeline != nullptr){
        Topl_Factory::switchPipeline(_renderer, pipeline);
        setShadersMode(mode);
    }

    _renderer->setDrawMode(DRAW_Triangles);
    _renderer->updateScene(scene);
    _renderer->drawScene(scene);
}

void Topl_Program::cleanup() {
#ifdef RASTERON_H
	for(unsigned f = 0; f < Topl_Program::cachedFrames->frameCount; f++){ // exports frames
		std::string frameName = "frame" + std::to_string(f + 1) + ".bmp";
		std::cout << " Writing frame " << frameName << std::endl;
		Rasteron_Image* frameImg = queue_getImg(Topl_Program::cachedFrames, f);
        writeFileImageRaw(frameName.c_str(), IMG_Bmp, frameImg->height, frameImg->width, frameImg->data);
	}
	if(Topl_Program::cachedFrames != NULL) RASTERON_QUEUE_DEALLOC(Topl_Program::cachedFrames);
	cleanupFreeType();
#endif
	// for(unsigned o = 0; o < PROGRAM_OVERLAYS; o++) delete _overlays.billboards[o];
	delete(_renderer);
	delete(_platform);
}
