#include "Topl_Program.hpp"

Vec3f Topl_Program::cursorPos = { 0.0F, 0.0F, 0.0F };
std::string Topl_Program::userInput = "";

#ifdef RASTERON_H
unsigned Topl_Program::pickerVal = NO_COLOR;
const Geo_Actor* Topl_Program::pickerObj = NO_PICKER_OBJ;
#endif

static void onAnyKey(char k){
	if(isalnum(k)){
		Topl_Program::userInput += k;
		std::cout << Topl_Program::userInput << std::endl;
	}
}

static void onPress(float x, float y){ 
	Topl_Program::userInput.clear();
	Topl_Program::cursorPos = { x, y, 0.0F }; 
}

void Topl_Timeline::addSequence_float(float* var, std::pair<millisec_t, float> timeTarget){
	auto sequence = std::find_if(_float_map.begin(), _float_map.end(), [var](const std::pair<float*, std::map<millisec_t, float>>& p){ return p.first == target; });
	// TODO: Add timeTarget cooresponding to float target
	// if(sequence != _float_map.end()) sequence->at(target);
}

Topl_Program::Topl_Program(const char* execPath, const char* name, BACKEND_Target backend) : _backend(backend) {
    _platform = new Platform(execPath, name);
	_platform->createWindow(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);
    _renderer = Topl_Factory::genRenderer(backend, _platform);
	_renderer->setDrawMode(DRAW_Triangles);

	Platform::keyControl.addAnyCallback(onAnyKey);

	Platform::mouseControl.addCallback(MOUSE_LeftBtn_Down, onPress);
    Platform::mouseControl.addCallback(MOUSE_RightBtn_Down, onPress);
	// Platform::mouseControl.addHoverCallback(onPress);
}

Topl_Program::~Topl_Program() {
    delete(_platform);
#ifdef RASTERON_H
	cleanupFreeType();
#endif
}

void Topl_Program::run(){
    init();

    while (1) {  
		_platform->handleEvents(ENABLE_CURSOR_UPDATE);

		_renderer->clearView(); // clears view to solid color
		loop(_timeline.ticker.getRelMillisecs()); // performs draws and updating
		_renderer->present(); // switches front and back buffers
	}
}


#ifdef RASTERON_H
unsigned Topl_Program::invokePicker(Topl_Scene* scene){
	Topl_Program::pickerVal = _renderer->getPixelAt(Platform::getCursorX(), Platform::getCursorY());
	if((Topl_Program::pickerVal & 0x00FFFFFF) == (CLEAR_COLOR_CODE & 0x00FFFFFF)) 
		Topl_Program::pickerObj = nullptr;
	if(scene != nullptr){ 
		const Geo_Actor* actor = scene->invokePicker(Topl_Program::pickerVal);
		if(actor != nullptr) Topl_Program::pickerObj = actor; 
	}
	return Topl_Program::pickerVal;
}
#endif