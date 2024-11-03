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

Topl_Camera Topl_Program::cameraObj = Topl_Camera();

static void onAnyKey(char k){
	if(isspace(k) && k != 0x0D) Topl_Program::userInput += (isalpha(k))? tolower(k) : k;

	if(Topl_Program::isCtrl_keys && isalpha(k)){
		switch(tolower(k)){
			case 'w': Topl_Program::cameraObj.updatePos({ 0.0, Topl_Program::speed, 0.0 }); break;
			case 's': Topl_Program::cameraObj.updatePos({ 0.0, -Topl_Program::speed, 0.0 }); break;
			case 'a': Topl_Program::cameraObj.updatePos({ -Topl_Program::speed, 0.0, 0.0 }); break;
			case 'd': Topl_Program::cameraObj.updatePos({ Topl_Program::speed, 0.0, 0.0 }); break;
			case 'x': Topl_Program::cameraObj.updatePos({ 0.0F, 0.0, -0.1f }); break;
			case 'v': Topl_Program::cameraObj.updatePos({ 0.0F, 0.0, 0.1f }); break;
			case 'q': Topl_Program::cameraObj.updateRot({ -Topl_Program::speed, 0.0, 0.0 }); break;
			case 'e': Topl_Program::cameraObj.updateRot({ Topl_Program::speed, 0.0, 0.0 }); break;
			case 'r': Topl_Program::cameraObj.updateRot({ 0.0F, -Topl_Program::speed, 0.0 }); break;
			case 'f': Topl_Program::cameraObj.updateRot({ 0.0F, Topl_Program::speed, 0.0 }); break;
			case 't': Topl_Program::cameraObj.updateRot({ 0.0F, 0.0, -Topl_Program::speed }); break;
			case 'g': Topl_Program::cameraObj.updateRot({ 0.0F, 0.0, Topl_Program::speed }); break;
			case 'z': Topl_Program::cameraObj.setZoom(*Topl_Program::cameraObj.getZoom() * (1.0F + Topl_Program::speed)); break;
			case 'c': Topl_Program::cameraObj.setZoom(*Topl_Program::cameraObj.getZoom() * (1.0F - Topl_Program::speed)); break;
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
		if(k == '-') Topl_Program::shaderMode--;
		else if(k == '+') Topl_Program::shaderMode++;
		else if(isdigit(k)){
			switch(tolower(k)){
				case 0: Topl_Program::activeShader = nullptr;
				// case 1: Topl_Program::activeShader = &Topl_Progarm::_texVShader; _renderer->setPipeline(Topl_Program::texPipeline) break;
				// case 2: Topl_Program::activeShader = &Topl_Progarm::_flatVShader; _renderer->setPipeline(Topl_Program::flatPipeline) break;
				// case 3: Topl_Program::activeShader = &Topl_Progarm::_beamsVShader; _renderer->setPipeline(Topl_Program::beamsPipeline) break;
				// case 4: Topl_Program::activeShader = &Topl_Progarm::_effectVShader; _renderer->setPipeline(Topl_Program::effectPipeline) break;
				// Utilize other pipelines
			}
		}

		if((k == '-' || k == '+') && Topl_Program::activeShader != nullptr) Topl_Program::activeShader->setMode(Topl_Program::shaderMode);
	}
}

static void onPress(float x, float y){
	if(!Topl_Program::isCtrl_input) Topl_Program::userInput.clear();
	Topl_Program::cursorPos = { x, y, 0.0F }; 

	if(Topl_Program::pickerObj != nullptr) Topl_Program::lastPickerColor = Topl_Program::pickerColor;
	Topl_Program::lastPickerCoord = Topl_Program::pickerCoord;
}

Topl_Program::Topl_Program(const char* execPath, const char* name, BACKEND_Target backend) : _backend(backend) {
    srand(time(NULL));

	// Event Handling

	_platform = new Platform(execPath, name); // special initialization for Android?
	_platform->createWindow(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);
    _renderer = Topl_Factory::genRenderer(backend, _platform);
	// _renderer->setCamera(&Topl_Program::cameraObj);
	_renderer->setDrawMode(DRAW_Triangles);

	Platform::keyControl.addAnyCallback(onAnyKey);
	Platform::mouseControl.addCallback(MOUSE_LeftBtn_Press, onPress);
	// Platform::mouseControl.addDragCallback(onDrag);

	// Preset Pipeline Generation

	if(_backend == BACKEND_GL4){
		_texVShader = Textured_VertexShader_GL4(); _texPShader = Textured_PixelShader_GL4();
		_beamsVShader = Beams_VertexShader_GL4(); _beamsPShader = Beams_PixelShader_GL4();
		_effectVShader = Effect_VertexShader_GL4(); _effectPShader = Effect_PixelShader_GL4();
		_canvasVShader = Canvas_VertexShader_GL4(); _canvasPShader = Canvas_PixelShader_GL4();
		_dynamicVShader = Dynamic_VertexShader_GL4(); _dynamicPShader = Dynamic_PixelShader_GL4();
		_flatVShader = Flat_VertexShader_GL4(); _flatPShader = Flat_PixelShader_GL4();
		// TODO: Include Advanced shaders
	}
	else if(_backend == BACKEND_DX11){
		_texVShader = Textured_VertexShader_DX11(); _texPShader = Textured_PixelShader_DX11();
		_beamsVShader = Beams_VertexShader_DX11(); _beamsPShader = Beams_PixelShader_DX11();
		_effectVShader = Effect_VertexShader_DX11(); _effectPShader = Effect_PixelShader_DX11();
		_canvasVShader = Canvas_VertexShader_DX11(); _canvasPShader = Canvas_PixelShader_DX11();
		_dynamicVShader = Dynamic_VertexShader_DX11(); _dynamicPShader = Dynamic_PixelShader_DX11();
		_flatVShader = Flat_VertexShader_DX11(); _flatPShader = Flat_PixelShader_DX11();
		// TODO: Include Advanced shaders
	}

	_beamsPipeline = Topl_Factory::genPipeline(_backend, &_beamsVShader, &_beamsPShader);
	_texPipeline = Topl_Factory::genPipeline(_backend, &_texVShader, &_texPShader);
	_effectPipeline = Topl_Factory::genPipeline(_backend, &_effectVShader, &_effectPShader);
	// _canvasPipeline = Topl_Factory::genPipeline(_backend, &_canvasVShader, &_canvasPShader);
	// _dynamicPipeline = Topl_Factory::genPipeline(_backend, &_dynamicVShader, &_dynamicPShader);
	_flatPipeline = Topl_Factory::genPipeline(_backend, &_flatVShader, &_flatPShader);
#ifdef RASTERON_H
	// Imaging Initialization
	_invertImage = INVERT_IMG_TRUE; // make sure images show up inverse

	// ImageSize frameSize = { Platform::getViewportHeight(_platformCtx.window), Platform::getViewportWidth(_platformCtx.window) };
	ImageSize frameSize = { TOPL_WIN_HEIGHT, TOPL_WIN_WIDTH };
	Topl_Program::cachedFrames = RASTERON_QUEUE_ALLOC("frames", frameSize, CACHED_FRAME_COUNT);
#endif
}

void Topl_Program::cleanup() {
#ifdef RASTERON_H
	for(unsigned f = 0; f < Topl_Program::cachedFrames->frameCount; f++){ // exports frames
		std::string frameName = "frame" + std::to_string(f + 1) + ".bmp";
		Rasteron_Image* frameImg = queue_getImg(Topl_Program::cachedFrames, f);
		writeFileImageRaw(frameName.c_str(), IMG_Bmp, frameImg->height, frameImg->width, frameImg->data);
	}
	if(Topl_Program::cachedFrames != NULL) RASTERON_QUEUE_DEALLOC(Topl_Program::cachedFrames);
	cleanupFreeType();
#endif
	delete(_renderer);
	delete(_platform);
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
			_renderer->clear(); // clears view to solid color
			Topl_Factory::switchPipeline(_renderer, _flatPipeline); // TODO: Remove Backend component
			loop(Topl_Program::timeline.persist_ticker.getRelMillisecs()); // performs draws and updating
			_renderer->present(); // switches front and back buffers
		}
#ifdef RASTERON_H
		/* Img_Base frameImg = _renderer->frame();
		queue_addImg(Topl_Program::cachedFrames, frameImg.getImage(), _renderer->getFrameCount() % CACHED_FRAME_COUNT);
		RASTERON_DEALLOC(frameImg.getImage()); */
#endif
		/* if(Topl_Program::lastPickerCoord[0] != Topl_Program::pickerCoord[0] && Topl_Program::lastPickerCoord[1] != Topl_Program::pickerCoord[1])
			Topl_Program::lastPickerCoord = Topl_Program::pickerCoord;
	 	*/
	}

	cleanup();
}


#ifdef RASTERON_H
unsigned Topl_Program::colorPicker(Topl_Scene* scene){
	_flatVShader.setMode(FLAT_MODE_SOLID);
	_renderer->setDrawMode(DRAW_Triangles);
	// _renderer->setCamera(&Topl_Program::cameraObj); // remove this?
	_renderer->updateScene(scene);
	_renderer->drawScene(scene);

	Topl_Program::pickerColor = _renderer->getPixelAt(Platform::getCursorX(), Platform::getCursorY());
	if((Topl_Program::pickerColor & 0x00FFFFFF) == (CLEAR_COLOR_CODE & 0x00FFFFFF)) 
		Topl_Program::pickerObj = nullptr;
	if(scene != nullptr){ 
		Geo_Actor* actor = scene->getPickActor(Topl_Program::pickerColor);
		if(actor != nullptr){
			Topl_Program::pickerObj = actor;
			if(actor->pickerFunc != nullptr){
				if(!Platform::mouseControl.getIsMouseDown().second) actor->pickerFunc(MOUSE_Hover);
				else actor->pickerFunc(Platform::mouseControl.getIsMouseDown().first);
			}
		}
	}

	_renderer->clear();

	return Topl_Program::pickerColor;
}

Vec3f Topl_Program::coordPicker(Topl_Scene* scene){
 	_flatVShader.setMode(FLAT_MODE_COORD);
	_renderer->setDrawMode(DRAW_Triangles);
	// _renderer->setCamera(&Topl_Program::cameraObj); // remove this?
	_renderer->updateScene(scene);
	_renderer->drawScene(scene);

	unsigned color = _renderer->getPixelAt(Platform::getCursorX(), Platform::getCursorY());
	Topl_Program::pickerCoord = Vec3f{
		((color & 0xFF0000) >> 16) / 255.0f,
		((color & 0xFF00) >> 8) / 255.0f, 
		(color & 0xFF) / 255.0f,  
	};
	
	_renderer->clear();

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
