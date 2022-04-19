#include "Topl_App.hpp"

Topl_App::Topl_App(const char* execPath, const char* name, APP_Backend backend) : _backend(backend) {
	logMessage(" -------------- Topl App -------------- ");
	
    srand(time(NULL)); // seed random number generation
#ifdef RASTERON_H
	initFreeType(&_freetypeLib);
#endif
    _platform = new Platform(execPath, name);
    _platform->createWindow();

    _renderer = Topl_Factory::genRenderer(backend, _platform);
}

Topl_App::Topl_App(const char* execPath, const char* name, APP_Backend backend, std::initializer_list<Topl_Viewport> viewports) : _backend(backend) {
	logMessage(" -------------- Topl App -------------- ");
	
    srand(time(NULL)); // seed random number generation
#ifdef RASTERON_H
	initFreeType(&_freetypeLib);
#endif
    _platform = new Platform(execPath, name);
    _platform->createWindow();

    _renderer = Topl_Factory::genRenderer(backend, _platform, viewports);
}

Topl_App::~Topl_App() {
    delete(_platform);
#ifdef RASTERON_H
	cleanupFreeType(&_freetypeLib);
#endif
}

void Topl_App::run(){
    init();
    // _ticker.addPeriodicEvent(20, _platform->handleEvents(true)); // make cursor updates periodic

    while (1) { 
        _ticker.updateTimer();
        _renderer->clearView();

        loop(_renderer->getFrameCount());
        
        _renderer->switchFramebuff();
        _platform->handleEvents(false);
    }
}
