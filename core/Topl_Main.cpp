#include "Topl_Main.hpp"

Vec3f Topl_Main::cursorPos = { 0.0F, 0.0F, 0.0F };

#ifdef RASTERON_H
unsigned Topl_Main::pickerVal = NO_COLOR;
const Geo_Actor* Topl_Main::pickerObj = NO_PICKER_OBJ;
#endif

static void onPress(float x, float y){ Topl_Main::cursorPos = { x, y, 0.0F }; }

Topl_Main::Topl_Main(const char* execPath, const char* name, BACKEND_Target backend) : _backend(backend) {
    _platform = new Platform(execPath, name);
	_platform->createWindow(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);
    _renderer = Topl_Factory::genRenderer(backend, _platform);
	_renderer->setDrawMode(DRAW_Triangles);

	Platform::mouseControl.addCallback(MOUSE_LeftBtn_Down, onPress);
    Platform::mouseControl.addCallback(MOUSE_RightBtn_Down, onPress);
	// Platform::mouseControl.addHoverCallback(onPress);
}

Topl_Main::~Topl_Main() {
    delete(_platform);
#ifdef RASTERON_H
	cleanupFreeType();
#endif
}

void Topl_Main::run(){
    init();

    while (1) {  
		_platform->handleEvents(ENABLE_CURSOR_UPDATE);

		_renderer->clearView(); // clears view to solid color
		loop(_timeline.ticker.getRelMillisecs()); // performs draws and updating
		_renderer->present(); // switches front and back buffers
	}
}


#ifdef RASTERON_H
unsigned Topl_Main::invokePicker(Topl_Scene* scene){
	Topl_Main::pickerVal = _renderer->getPixelAt(Platform::getCursorX(), Platform::getCursorY());
	if((Topl_Main::pickerVal & 0x00FFFFFF) == (CLEAR_COLOR_CODE & 0x00FFFFFF)) 
		Topl_Main::pickerObj = nullptr;
	if(scene != nullptr){ 
		const Geo_Actor* actor = scene->invokePicker(Topl_Main::pickerVal);
		if(actor != nullptr) Topl_Main::pickerObj = actor; 
	}
	return Topl_Main::pickerVal;
}
#endif