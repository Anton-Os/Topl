#include "Topl_App.hpp"

Topl_App::Topl_App(const char* execPath, const char* name, APP_Backend backend) : _backend(backend) {
    _platform = new Platform(execPath, name);
	_platform->createWindow(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);
    _renderer = Topl_Factory::genRenderer(backend, _platform);
}

Topl_App::~Topl_App() {
    delete(_platform);
#ifdef RASTERON_H
	cleanupFreeType();
#endif
}

void Topl_App::run(){
    init();
	double loopInterval = LOOP_RESET;

    while (1) { 
        _ticker.updateTimer();
		loopInterval += _ticker.getRelMillisecs();

		if (loopInterval > LOOP_INTERVAL) {
			_platform->handleEvents(ENABLE_CURSOR_UPDATE);

			_renderer->clearView(); // clears view to solid color
			loop(_renderer->getFrameCount()); // performs draws and updating
			_renderer->switchFramebuff(); // switches front and back buffers

			loopInterval = LOOP_RESET; // reset value 
		}
		else continue;
    }
}
