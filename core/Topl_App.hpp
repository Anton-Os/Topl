#include "Platform.hpp"

#include "Topl_Renderer_GL4.hpp"
#ifdef _WIN32
#include "Topl_Renderer_Drx11.hpp"
#endif

#include "Topl_Scene.hpp"

enum APP_Backend {
    APP_OpenGL_4,
    APP_DirectX_11
    // Add Vulkan Support
};

class Topl_App {
public:
    Topl_App(const char* execPath, const char* name, APP_Backend backend) : _backend(backend) {
		_platform = new Platform(execPath, name);
		_platform->createWindow();

		srand(time(NULL)); // seed random number generation
    }
	~Topl_App() {
		delete(_platform);
	}

	void run() {
		init();

		while (1) { 
			_ticker.updateTimer();
			loop();
			_platform->handleEvents(false); // TODO: handle cursor updates periodically
		}
	}

protected:
    virtual void init() = 0;
    virtual void loop() = 0;

	union Renderer {
		Topl_Renderer_GL4 GL4;
#ifdef _WIN32
		Topl_Renderer_Drx11 Drx11;
#endif
	};
    const enum APP_Backend _backend;
    
	Renderer* _renderer;
    Platform* _platform = nullptr;

    Timer_Ticker _ticker;
    std::vector<Topl_Scene> _scenes;
    std::vector<Topl_Light> _lights;
    std::vector<Topl_Camera> _cameras;
};