#include "Platform.hpp"

#include "Topl_Renderer_GL4.hpp"
#ifdef _WIN32
#include "Topl_Renderer_Drx11.hpp"
#endif

#define EVENT_HANDLE_TIME 1.0 / 

enum APP_Backend {
    APP_OpenGL_4,
    APP_DirectX_11
    // Add Vulkan Support
};

class Topl_Factory { // supplies and generates all interfaces for Topl_App class
public:
	~Topl_Factory();
	static Topl_Renderer* genRenderer(APP_Backend backend, Platform* platform);

	static Topl_Pipeline* genPipeline(APP_Backend backend, entry_shader_cptr vertexShader, shader_cptr pixelShader);
	static Topl_Pipeline* genPipeline(APP_Backend backend,
		entry_shader_cptr vertexSource,
		shader_cptr pixelSource,
		shader_cptr tessCtrlSource,
		shader_cptr tessEvalSource,
		shader_cptr geomSource
	);
private:
	// Internally Managed Structures
	static Topl_Renderer_GL4* GL4_renderer;
	static Topl_Renderer_Drx11* Drx11_renderer;

	static Topl_Pipeline_GL4** GL4_pipelines;
	static unsigned GL4_pipeIndex;
	static Topl_Pipeline_Drx11** Drx11_pipelines;
	static unsigned Drx11_pipeIndex;
};

#define FRAME_CACHE_COUNT 1024

class Topl_App {
public:
    Topl_App(const char* execPath, const char* name, APP_Backend backend);
	~Topl_App();

	void run();

	void beginFrameCap(); // starts frame capture

protected:
    virtual void init() = 0;
    virtual void loop(double secs, unsigned long frame) = 0;

	// Rendering
	const enum APP_Backend _backend;
	Platform* _platform = nullptr;
	Topl_Renderer* _renderer = nullptr;
	const Topl_Pipeline* _activePipeline = nullptr;

	// Utility
    Timer_Ticker _ticker;
#ifdef RASTERON_H
	FT_Library _freetypeLib; // required for working with fonts
	Topl_Frames _frameCache = Topl_Frames("frame-cache", TOPL_WIN_HEIGHT, TOPL_WIN_WIDTH, FRAME_CACHE_COUNT);
#endif

	// Paths
	const std::string fontsPath = std::string(ASSETS_DIR) + "fonts/";
	const std::string imagesPath = std::string(ASSETS_DIR) + "images/";
	const std::string modelsPath = std::string(ASSETS_DIR) + "models/";
	const std::string othersPath = std::string(ASSETS_DIR) + "others/";
};