#include "Topl_Main.hpp"

Topl_Main::Topl_Main(const char* execPath, const char* name, BACKEND_Target backend) : _backend(backend) {
    _platform = new Platform(execPath, name);
	_platform->createWindow(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);
    _renderer = Topl_Factory::genRenderer(backend, _platform);
	_renderer->setDrawMode(DRAW_Triangles);
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
	unsigned pickerColor = _renderer->getPixelAt(Platform::getCursorX(), Platform::getCursorY());
	if(scene == nullptr) return pickerColor;
	else {
		pickerCallback callback = scene->invokePicker(pickerColor);

		if(callback != nullptr) callback();
		return (callback != nullptr)? pickerColor : 0; // return 0 if no matches found
	}
}
#endif