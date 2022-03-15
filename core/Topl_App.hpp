#include "Platform.hpp"

#include "Topl_Renderer_GL4.hpp"
#ifdef _WIN32
#include "Topl_Renderer_Drx11.hpp"
#endif


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
	static Topl_Renderer_GL4* _renderer_GL4;
	static Topl_Renderer_Drx11* _renderer_Drx11;

	static Topl_Pipeline_GL4** _pipelines_GL4;
	static unsigned _pipelineIndex_GL4;
	static Topl_Pipeline_Drx11** _pipelines_Drx11;
	static unsigned _pipelineIndex_Drx11;
};

class Topl_App {
public:
    Topl_App(const char* execPath, const char* name, APP_Backend backend);
	~Topl_App();

	void run();

	void setActivePipeline(const Topl_Pipeline* pipeline){ _activePipeline = pipeline; }

protected:
    virtual void init() = 0;
    virtual void loop(double secs, unsigned long frame) = 0;

    const enum APP_Backend _backend;
 
	Platform* _platform = nullptr;
	Topl_Renderer* _renderer = nullptr;
	const Topl_Pipeline* _activePipeline = nullptr;

    Timer_Ticker _ticker;
#ifdef RASTERON_H
	FT_Library _freetypeLib; // required for working with fonts
#endif
};