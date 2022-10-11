#include "Topl_App.hpp"

const std::string Topl_App::fontsPath = std::string(ASSETS_DIR) + "fonts/";
const std::string Topl_App::imagesPath = std::string(ASSETS_DIR) + "images/";
const std::string Topl_App::modelsPath = std::string(ASSETS_DIR) + "models/";
const std::string Topl_App::resourcePath = std::string(ASSETS_DIR) + "res/";

Topl_App::Topl_App(const char* execPath, const char* name, APP_Backend backend) : _backend(backend) {
	logMessage(" -------------- Topl App -------------- ");
	
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

		if(_renderer->getFrameCount() % 5 == 0) _platform->handleEvents(DISABLE_CURSOR_UPDATE);
		else _platform->handleEvents(ENABLE_CURSOR_UPDATE); // handle cursor updates every 5th frame

        loop(_renderer->getFrameCount()); // performs draws and updating
        
        _renderer->switchFramebuff(); // switch front and back buffers for display
    }
}
