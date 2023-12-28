#include "Topl_Program.hpp"

Vec3f Topl_Program::cursorPos = { 0.0F, 0.0F, 0.0F };
bool Topl_Program::isInputEnabled = false;
std::string Topl_Program::userInput = "";

#ifdef RASTERON_H
unsigned Topl_Program::pickerVal = NO_COLOR;
const Geo_Actor* Topl_Program::pickerObj = NO_PICKER_OBJ;
Rasteron_Queue* Topl_Program::cachedFrames = NULL;
#endif

Topl_Camera Topl_Program::cameraObj = Topl_Camera();

Topl_Timeline::Topl_Timeline(){
	// void addRecurringEvent() // Update internal elapsed millisecs and all variables
}

void Topl_Timeline::addSequence_float(float* var, std::pair<millisec_t, float> timeTarget){
	assert(_float_map.size() < MAX_TIMELINE_ATTRIBS);
	
	auto sequence = std::find_if(_float_map.begin(), _float_map.end(), [var](const std::pair<float*, std::map<millisec_t, float>>& p){ return p.first == var; });
	
	if(sequence != _float_map.end()) sequence->second.insert({ timeTarget.first, timeTarget.second });
	else {
		std::cout << "Creating float with time target!" << std::endl; // TEST
		_float_map.insert({ var, std::map<millisec_t, float>() }); // create new object
		_float_map[var].insert({ TIMELINE_START, *var }); // create default state at timeline start
	}
}

static void onAnyKey(char k){
	if(isalnum(k) || isspace(k)){
		Topl_Program::userInput += k;
		std::cout << Topl_Program::userInput << std::endl;
	}
}

static void onPress(float x, float y){ 
	if(!Topl_Program::isInputEnabled) Topl_Program::userInput.clear();
	Topl_Program::cursorPos = { x, y, 0.0F }; 
}

Topl_Program::Topl_Program(const char* execPath, const char* name, BACKEND_Target backend) : _backend(backend) {
    _platform = new Platform(execPath, name);
	_platform->createWindow(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);
    _renderer = Topl_Factory::genRenderer(backend, _platform);
	_renderer->setCamera(&Topl_Program::cameraObj);
	_renderer->setDrawMode(DRAW_Triangles);


	Platform::keyControl.addAnyCallback(onAnyKey);

	Platform::mouseControl.addCallback(MOUSE_LeftBtn_Down, onPress);
    Platform::mouseControl.addCallback(MOUSE_RightBtn_Down, onPress);
	// Platform::mouseControl.addHoverCallback(onPress);
#ifdef RASTERON_H
	// ImageSize frameSize = { Platform::getViewportHeight(_platformCtx.window), Platform::getViewportWidth(_platformCtx.window) };
	ImageSize frameSize = { TOPL_WIN_HEIGHT, TOPL_WIN_WIDTH };
	Topl_Program::cachedFrames = alloc_queue("frames", frameSize, CACHED_FRAME_COUNT);
#endif
}

Topl_Program::~Topl_Program() {
    delete(_platform);
#ifdef RASTERON_H
	// for(unsigned f = 0; f < Topl_Program::cachedFrames->frameCount; f++)
	// 	writeFileImageRaw(getFrameAt(Topl_Program::cachedFrames, f)->name, IMG_Png, TOPL_WIN_HEIGHT, TOPL_WIN_WIDTH, getFrameAt(Topl_Program::cachedFrames, f)->data);
	if(Topl_Program::cachedFrames != NULL) dealloc_queue(Topl_Program::cachedFrames);
	cleanupFreeType();
#endif
}

void Topl_Program::run(){
    init();

    while (1) {
		// _timeline.dynamic_ticker.setTime(_timeline.ticker.getAbsMillisecs());
		_platform->handleEvents(ENABLE_CURSOR_UPDATE);

		_renderer->clearView(); // clears view to solid color
		loop(_timeline.ticker.getRelMillisecs()); // performs draws and updating
		_renderer->present(); // switches front and back buffers
#ifdef RASTERON_H
		addFrameAt(Topl_Program::cachedFrames, _renderer->frame().getImage(), _renderer->getFrameCount() % CACHED_FRAME_COUNT);
#endif
	}

}


#ifdef RASTERON_H
unsigned Topl_Program::colorPicker(Topl_Scene* scene){
	Topl_Program::pickerVal = _renderer->getPixelAt(Platform::getCursorX(), Platform::getCursorY());
	if((Topl_Program::pickerVal & 0x00FFFFFF) == (CLEAR_COLOR_CODE & 0x00FFFFFF)) 
		Topl_Program::pickerObj = nullptr;
	if(scene != nullptr){ 
		const Geo_Actor* actor = scene->getPickActor(Topl_Program::pickerVal);
		if(actor != nullptr) Topl_Program::pickerObj = actor; 
	}
	return Topl_Program::pickerVal;
}

Vec3f Topl_Program::coordPicker(Topl_Scene* scene){
	Topl_Program::pickerVal = _renderer->getPixelAt(Platform::getCursorX(), Platform::getCursorY());
	printf("Coordinate picker value: %X\n", Topl_Program::pickerVal);
	return Vec3f{
		((Topl_Program::pickerVal & 0xFF0000) >> 16) / 255.0f,
		((Topl_Program::pickerVal & 0xFF00) >> 8) / 255.0f, 
		(Topl_Program::pickerVal & 0xFF) / 255.0f,  
	};
}
#endif