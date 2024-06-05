#include "program/Topl_Program.hpp"

Topl_Timeline Topl_Program::timeline = Topl_Timeline();
Vec3f Topl_Program::cursorPos = { 0.0F, 0.0F, 0.0F };
bool Topl_Program::isCamera_keys = true;
bool Topl_Program::isCamera_mouse = false;
bool Topl_Program::isInputEnabled = false;
std::string Topl_Program::userInput = "";

#ifdef RASTERON_H
unsigned Topl_Program::pickerColor = NO_COLOR;
// unsigned Topl_Program::pickerVal_coord = NO_COLOR;
Vec3f Topl_Program::pickerCoord = { 0.0F, 0.0F, 0.0F };
Geo_Actor* Topl_Program::pickerObj = NO_PICKER_OBJ;
Rasteron_Queue* Topl_Program::cachedFrames = NULL;
#endif

Topl_Camera Topl_Program::cameraObj = Topl_Camera();


static void onAnyKey(char k){
	if(isalnum(k) || isspace(k)){
		Topl_Program::userInput += k;
		// std::cout << Topl_Program::userInput << std::endl;
	}

	if(Topl_Program::isCamera_keys){
		if(toupper(k) == 'W') Topl_Program::cameraObj.updatePos({ 0.0, 0.1F, 0.0 });
		else if(toupper(k) == 'S') Topl_Program::cameraObj.updatePos({ 0.0, -0.1F, 0.0 });
		else if(toupper(k) == 'A') Topl_Program::cameraObj.updatePos({ -0.1F, 0.0, 0.0 });
		else if(toupper(k) == 'D') Topl_Program::cameraObj.updatePos({ 0.1F, 0.0, 0.0 });
		else if(toupper(k) == 'X') Topl_Program::cameraObj.updatePos({ 0.0F, 0.0, -0.1f });
		else if(toupper(k) == 'V') Topl_Program::cameraObj.updatePos({ 0.0F, 0.0, 0.1f });
		else if(toupper(k) == 'Q') Topl_Program::cameraObj.updateRot({ -0.1F, 0.0, 0.0 });
		else if(toupper(k) == 'E') Topl_Program::cameraObj.updateRot({ 0.1F, 0.0, 0.0 });
		else if(toupper(k) == 'Z') Topl_Program::cameraObj.setZoom(*Topl_Program::cameraObj.getZoom() * 1.1);
		else if(toupper(k) == 'C') Topl_Program::cameraObj.setZoom(*Topl_Program::cameraObj.getZoom() * 0.9);
#ifdef RASTERON_H
		/* else if(k == ';'){
			Rasteron_Image* frameImg = _renderer->frame();
			writeFileImageRaw("Output.bmp", IMG_Bmp, frameImg->height, frameImg->width, frameImg->data);
			RASTERON_DEALLOC(frameImg);
		} */
#endif
	}
}

static void onDrag(float x, float y){
	static Vec3f savedColorVec = Vec3f{0.0, 0.0, 0.0};

	if(Platform::mouseControl.getIsMouseDown().second){
		Topl_Program::cursorPos = { x, y, 0.0F };
#ifdef RASTERON_H
		if(Topl_Program::isCamera_mouse){
			if(Platform::mouseControl.getIsMouseDown().first == MOUSE_LeftBtn_Press){
				if(savedColorVec[0] != Topl_Program::pickerCoord[0] || savedColorVec[1] != Topl_Program::pickerCoord[1])
					Topl_Program::cameraObj.updatePos({ savedColorVec[0] - Topl_Program::pickerCoord[0], savedColorVec[1] - Topl_Program::pickerCoord[1], 0.0 });
			} else if(Platform::mouseControl.getIsMouseDown().first == MOUSE_RightBtn_Press){
				if(savedColorVec[0] != Topl_Program::pickerCoord[0])
					Topl_Program::cameraObj.updateRot({ savedColorVec[0] - Topl_Program::pickerCoord[0], 0.0, 0.0 });
				if(savedColorVec[1] != Topl_Program::pickerCoord[1])
					Topl_Program::cameraObj.updateRot({ savedColorVec[1] - Topl_Program::pickerCoord[1], 0.0, 0.0 });
			} 
		}
#endif
	}
}

static void onPress(float x, float y){ 
	if(!Topl_Program::isInputEnabled) Topl_Program::userInput.clear();
	Topl_Program::cursorPos = { x, y, 0.0F }; 
}

Topl_Program::Topl_Program(const char* execPath, const char* name, BACKEND_Target backend) : _backend(backend) {
    srand(time(NULL));

	// Event Handling
	
	_platform = new Platform(execPath, name);
	_platform->createWindow(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);
    _renderer = Topl_Factory::genRenderer(backend, _platform);
	_renderer->setCamera(&Topl_Program::cameraObj);
	_renderer->setDrawMode(DRAW_Triangles);

	Platform::keyControl.addAnyCallback(onAnyKey);
	Platform::mouseControl.addCallback(MOUSE_LeftBtn_Press, onPress);
	// Platform::mouseControl.addDragCallback(onDrag); // for camera

	// Preset Pipeline Generation

	if(_backend == BACKEND_GL4){
		_texVShader = Textured_VertexShader_GL4(); _texPShader = Textured_PixelShader_GL4();
		_beamsVShader = Beams_VertexShader_GL4(); _beamsPShader = Beams_PixelShader_GL4();
		_flatVShader = Flat_VertexShader_GL4(); _flatPShader = Flat_PixelShader_GL4();
		_effectVShader = Effect_VertexShader_GL4(); _effectPShader = Effect_PixelShader_GL4();
	}
	else if(_backend == BACKEND_DX11){
		_texVShader = Textured_VertexShader_DX11(); _texPShader = Textured_PixelShader_DX11();
		_beamsVShader = Beams_VertexShader_DX11(); _beamsPShader = Beams_PixelShader_DX11();
		_flatVShader = Flat_VertexShader_DX11(); _flatPShader = Flat_PixelShader_DX11();
		_effectVShader = Effect_VertexShader_DX11(); _effectPShader = Effect_PixelShader_DX11();	
	}

	_flatPipeline = Topl_Factory::genPipeline(_backend, &_flatVShader, &_flatPShader);
	_beamsPipeline = Topl_Factory::genPipeline(_backend, &_beamsVShader, &_beamsPShader);
	_effectPipeline = Topl_Factory::genPipeline(_backend, &_effectVShader, &_effectPShader);
	_texPipeline = Topl_Factory::genPipeline(_backend, &_texVShader, &_texPShader);

	// Imaging Initialization

#ifdef RASTERON_H
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
    init();

    while (true) {
		if(!Topl_Program::timeline.dynamic_ticker.isPaused)
			Topl_Timeline::seqCallback(Topl_Program::timeline.dynamic_ticker.getAbsSecs());

		_renderer->clear(); // clears view to solid color
		loop(Topl_Program::timeline.persist_ticker.getRelMillisecs()); // performs draws and updating
		_renderer->present(); // switches front and back buffers
#ifdef RASTERON_H
		Img_Base frameImg = _renderer->frame();
		queue_addImg(Topl_Program::cachedFrames, frameImg.getImage(), _renderer->getFrameCount() % CACHED_FRAME_COUNT);
		RASTERON_DEALLOC(frameImg.getImage());
#endif
		_platform->handleEvents();
	}

	cleanup();
}


#ifdef RASTERON_H
unsigned Topl_Program::colorPicker(Topl_Scene* scene){
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
	return Topl_Program::pickerColor;
}

Vec3f Topl_Program::coordPicker(Topl_Scene* scene){
	unsigned color = _renderer->getPixelAt(Platform::getCursorX(), Platform::getCursorY());
	Topl_Program::pickerCoord = Vec3f{
		((color & 0xFF0000) >> 16) / 255.0f,
		((color & 0xFF00) >> 8) / 255.0f, 
		(color & 0xFF) / 255.0f,  
	};
	return Topl_Program::pickerCoord;
}
#endif
