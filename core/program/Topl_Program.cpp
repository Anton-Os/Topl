#include "program/Topl_Program.hpp"

Topl_Timeline Topl_Program::timeline = Topl_Timeline();
Vec3f Topl_Program::cursorPos = { 0.0F, 0.0F, 0.0F };
float Topl_Program::speed = 0.1F;

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
#ifdef RASTERON_H
Rasteron_Queue* Topl_Program::cachedFrames = NULL;
#endif

Topl_Camera Topl_Program::camera = Topl_Camera();

static void backgroundCallback(MOUSE_Event event){
	if(Topl_Program::pickerObj != nullptr) std::cout << "BACKGROUND: Actor is " << Topl_Program::pickerObj->getName() << std::endl;
}

static void overlayCallback(MOUSE_Event event){
	if(Topl_Program::pickerObj != nullptr) std::cout << "OVERLAYS: Actor is " << Topl_Program::pickerObj->getName() << std::endl;
}

void Topl_Program::_onAnyKey(char k){
    if(isspace(k) && k != 0x0D) Topl_Program::userInput += (isalpha(k))? tolower(k) : k;

    if(Topl_Program::isCtrl_keys && isalpha(k)){
        switch(tolower(k)){
            case 'w': Topl_Program::camera.updatePos({ 0.0, Topl_Program::speed, 0.0 }); break;
            case 's': Topl_Program::camera.updatePos({ 0.0, -Topl_Program::speed, 0.0 }); break;
            case 'a': Topl_Program::camera.updatePos({ -Topl_Program::speed, 0.0, 0.0 }); break;
            case 'd': Topl_Program::camera.updatePos({ Topl_Program::speed, 0.0, 0.0 }); break;
            case 'x': Topl_Program::camera.updatePos({ 0.0F, 0.0, -0.1f }); break;
            case 'v': Topl_Program::camera.updatePos({ 0.0F, 0.0, 0.1f }); break;
            case 'q': Topl_Program::camera.updateRot({ -Topl_Program::speed, 0.0, 0.0 }); break;
            case 'e': Topl_Program::camera.updateRot({ Topl_Program::speed, 0.0, 0.0 }); break;
            case 'r': Topl_Program::camera.updateRot({ 0.0F, -Topl_Program::speed, 0.0 }); break;
            case 'f': Topl_Program::camera.updateRot({ 0.0F, Topl_Program::speed, 0.0 }); break;
            case 't': Topl_Program::camera.updateRot({ 0.0F, 0.0, -Topl_Program::speed }); break;
            case 'g': Topl_Program::camera.updateRot({ 0.0F, 0.0, Topl_Program::speed }); break;
            case 'z': Topl_Program::camera.setZoom(*Topl_Program::camera.getZoom() * (1.0F + Topl_Program::speed)); break;
            case 'c': Topl_Program::camera.setZoom(*Topl_Program::camera.getZoom() * (1.0F - Topl_Program::speed)); break;
        }
#ifdef RASTERON_H
        /* else if(k == ';'){
            Rasteron_Image* frameImg = _renderer->frame();
            writeFileImageRaw("Output", IMG_Bmp, frameImg->height, frameImg->width, frameImg->data);
            RASTERON_DEALLOC(frameImg);
        } */
#endif
    }
    if(Topl_Program::isCtrl_shader){
        if(k == '-' || k == (char)0x25) Topl_Program::shaderMode--;
        else if(k == '+' || k == (char)0x27) Topl_Program::shaderMode++;
        else if(isdigit(k)){
            std::cout << "Numerical digit is " << k << std::endl;
            switch(tolower(k)){
                case '0': Topl_Factory::switchPipeline(_renderer, _flatPipeline); break;
                case '1': Topl_Factory::switchPipeline(_renderer, _texPipeline); break;
                case '2': Topl_Factory::switchPipeline(_renderer, _beamsPipeline); break;
                case '3': Topl_Factory::switchPipeline(_renderer, _effectPipeline); break;
                case '4': Topl_Factory::switchPipeline(_renderer, _canvasPipeline); break;
                case '5': Topl_Factory::switchPipeline(_renderer, _dynamicPipeline); break;
            }
        }

        if(k == (char)0x25 || k == (char)0x27 || k == '+' || k == '='){
            for(unsigned s = 0; s < 6; s++)
                _entryShaders[s]->setMode(Topl_Program::shaderMode);
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
#endif
    _renderer = Topl_Factory::genRenderer(_backend, _platform);
	// _renderer->setCamera(&Topl_Program::camera);
	_renderer->setDrawMode(DRAW_Triangles);

    Platform::keyControl.addHandler(std::bind(&Topl_Program::_onAnyKey, this, std::placeholders::_1));
    Platform::mouseControl.addHandler(std::bind(&Topl_Program::_onAnyPress, this, std::placeholders::_1, std::placeholders::_2));

    setPipelines();
	if(isEnable_background) createBackground();
	if(isEnable_overlays) createOverlays();

    // Texturing Data Generation
#ifdef RASTERON_H
	// Imaging Initialization
	_invertImage = INVERT_IMG_TRUE; // make sure images show up inverse

	// ImageSize frameSize = { Platform::getViewportHeight(_platformCtx.window), Platform::getViewportWidth(_platformCtx.window) };
	ImageSize frameSize = { TOPL_WIN_HEIGHT, TOPL_WIN_WIDTH };
	Topl_Program::cachedFrames = RASTERON_QUEUE_ALLOC("frames", frameSize, CACHED_FRAME_COUNT);
#endif
}

void Topl_Program::setPipelines(){
	if(_backend == BACKEND_GL4){
		_texVShader = Textured_VertexShader_GL4(); _texPShader = Textured_PixelShader_GL4();
		_beamsVShader = Beams_VertexShader_GL4(); _beamsPShader = Beams_PixelShader_GL4();
		_effectVShader = Effect_VertexShader_GL4(); _effectPShader = Effect_PixelShader_GL4();
		_canvasVShader = Canvas_VertexShader_GL4(); _canvasPShader = Canvas_PixelShader_GL4();
		_dynamicVShader = Dynamic_VertexShader_GL4(); _dynamicPShader = Dynamic_PixelShader_GL4();
		_flatVShader = Flat_VertexShader_GL4(); _flatPShader = Flat_PixelShader_GL4();
		// TODO: Include Advanced shaders
	}
#ifdef _WIN32
	else if(_backend == BACKEND_DX11){
		_texVShader = Textured_VertexShader_DX11(); _texPShader = Textured_PixelShader_DX11();
		_beamsVShader = Beams_VertexShader_DX11(); _beamsPShader = Beams_PixelShader_DX11();
		_effectVShader = Effect_VertexShader_DX11(); _effectPShader = Effect_PixelShader_DX11();
		_canvasVShader = Canvas_VertexShader_DX11(); _canvasPShader = Canvas_PixelShader_DX11();
		_dynamicVShader = Dynamic_VertexShader_DX11(); _dynamicPShader = Dynamic_PixelShader_DX11();
		_flatVShader = Flat_VertexShader_DX11(); _flatPShader = Flat_PixelShader_DX11();
		// TODO: Include Advanced shaders
	}
#endif
	_beamsPipeline = Topl_Factory::genPipeline(_backend, &_beamsVShader, &_beamsPShader);
	_texPipeline = Topl_Factory::genPipeline(_backend, &_texVShader, &_texPShader);
	_effectPipeline = Topl_Factory::genPipeline(_backend, &_effectVShader, &_effectPShader);
	_canvasPipeline = Topl_Factory::genPipeline(_backend, &_canvasVShader, &_canvasPShader);
	_dynamicPipeline = Topl_Factory::genPipeline(_backend, &_dynamicVShader, &_dynamicPShader);
	_flatPipeline = Topl_Factory::genPipeline(_backend, &_flatVShader, &_flatPShader);
}

void Topl_Program::createBackground(){
    _background.scene.camera = &_background.camera;
    _background.actor.setPos({ 0.0F, 0.0F, -1.0F });
    _background.actor.pickerFunc = &backgroundCallback;
    _renderer->buildScene(&_background.scene);
    _renderer->texturizeScene(&_background.scene);
    // std::bind(&Topl_Program::_backgroundCallback, this, MOUSE_RightBtn_Press);
}

void Topl_Program::createOverlays(){
    _overlays.billboard_camera.shift({ -0.75, -0.9F, 0.0F });
    _overlays.billboard_object.shift({ 0.0F, -0.9F, 0.0F });
    _overlays.billboard_shader.shift({ 0.75F, -0.9F, 0.0F });
    for(unsigned short o = 0; o < 3; o++){
        _overlays.billboards[o]->scale({ 0.5F, 0.33F, 1.0F });
        _overlays.billboards[o]->getGeoActor(_overlays.billboards[o]->getActorCount() - 1)->updateSize({ 0.0F, 0.015F, 0.0F });
        _overlays.billboards[o]->getGeoActor(_overlays.billboards[o]->getActorCount() - 1)->updatePos({ 0.0F, 0.01F, 0.0F });
        _overlays.billboards[o]->getGeoActor(_overlays.billboards[o]->getActorCount() - 1)->pickerFunc = &overlayCallback;
#ifdef RASTERON_H
        for(unsigned e = 0; e < _overlays.billboards[o]->getActorCount(); e++)
            if(e != _overlays.billboards[o]->getActorCount() - 1){
                _overlays.billboards[o]->overlay(e, &_overlays.button); // test overlay
                // _overlays.billboards[o]->getImgAt(e)->setImage(cornerImgOp(_overlays.billboards[o]->getImgAt(e)->getImage(), 1.5, 0.0, 0.0, 1.5));
            }
#endif
    }
    _overlays.scene.camera = &_overlays.camera;
    _renderer->buildScene(&_overlays.scene);
    _renderer->texturizeScene(&_overlays.scene);
    // std::bind(&Topl_Program::_overlayCallback, this, MOUSE_RightBtn_Press);
}

void Topl_Program::renderScene(Topl_Scene* scene, Topl_Pipeline* pipeline, int mode){
    if(pipeline != nullptr){
        Topl_Factory::switchPipeline(_renderer, pipeline);
        for(unsigned s = 0; s < 6; s++) // TODO: Improve this logic!
            if(_entryShaders[s]->getFilePath() == pipeline->entryShader->getFilePath()){ _entryShaders[s]->setMode(mode); break; } // find matching pipeline and set mode
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
