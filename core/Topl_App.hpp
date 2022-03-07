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

		switch (backend) {
		case APP_OpenGL_4:
			_renderer_GL4 = new Topl_Renderer_GL4(_platform->getParentWindow());
			_renderer = (Topl_Renderer*)_renderer_GL4;
			break;
		case APP_DirectX_11:
			_renderer_Drx11 = new Topl_Renderer_Drx11(_platform->getParentWindow());
			_renderer = (Topl_Renderer*)_renderer_Drx11;
			break;
		}
		srand(time(NULL)); // seed random number generation
#ifdef RASTERON_H
		initFreeType(&_freetypeLib);
#endif
    }
	~Topl_App() {
		delete(_platform);
		switch (_backend) {
		case APP_OpenGL_4:
			delete _renderer_GL4;
			break;
		case APP_DirectX_11:
			delete _renderer_Drx11;
			break;
		}
#ifdef RASTERON_H
		cleanupFreeType(&_freetypeLib);
#endif
	}

	void run() {
		init();

		while (1) { 
			_ticker.updateTimer();
			_renderer->clearView();

			loop(_ticker.getAbsSecs(), _renderer->getFrameCount());
			
			_renderer->switchFramebuff();
			_platform->handleEvents(false); // TODO: handle cursor updates periodically
		}
	}

protected:
    virtual void init() = 0;
    virtual void loop(double secs, unsigned long frame) = 0;

    const enum APP_Backend _backend;
 
	Platform* _platform = nullptr;
	union {
		Topl_Renderer_GL4* _renderer_GL4;
		Topl_Renderer_Drx11* _renderer_Drx11;
	};
	Topl_Renderer* _renderer;
	typedef union {
		Topl_Pipeline_GL4* pipeline_GL4;
		Topl_Pipeline_Drx11* pipeline_Drx11;
	} pipeline_t;
	std::vector<pipeline_t> _pipelines;

    Timer_Ticker _ticker;
#ifdef RASTERON_H
	FT_Library _freetypeLib; // required for working with fonts
#endif
};