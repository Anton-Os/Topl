#include "Topl_App.hpp"

Topl_App::Topl_App(const char* execPath, const char* name, APP_Backend backend) : _backend(backend) {
    srand(time(NULL)); // seed random number generation

    _platform = new Platform(execPath, name);    
	_platform->createWindow();
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

    while (1) { 
        _ticker.updateTimer();
        _renderer->clearView();

		if(_renderer->getFrameCount() % 10 == 0) _platform->handleEvents(ENABLE_CURSOR_UPDATE);
		else _platform->handleEvents(DISABLE_CURSOR_UPDATE); // handle cursor updates every nth frame

        loop(_renderer->getFrameCount()); // performs draws and updating
        
        _renderer->switchFramebuff(); // switch front and back buffers for display
    }
}
