#include "Topl_Main.hpp"

Topl_Main::Topl_Main(const char* execPath, const char* name, APP_Backend backend) : _backend(backend) {
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
		loop(_ticker.getRelMillisecs()); // performs draws and updating
		_renderer->present(); // switches front and back buffers
    }
}
