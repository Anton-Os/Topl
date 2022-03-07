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
    Topl_App(const char* execPath, const char* name, APP_Backend backend);
	~Topl_App();

	void run();

	void setActivePipeline(const Topl_Pipeline* pipeline){ _activePipeline = pipeline; }
	Topl_Pipeline* genPipeline(const std::string& vertexSource, const std::string& pixelSource);
	Topl_Pipeline* genPipeline(
		const std::string& vertexSource,
		const std::string& pixelSource,
		const std::string& tessCtrlSource,
		const std::string& tessEvalSource,
		const std::string& geomSource
	);

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

	union {
		Topl_Pipeline_GL4** _pipelines_GL4;
		Topl_Pipeline_Drx11** _pipelines_Drx11;
	};
	unsigned short _pipelineIndex = 0; // tracks the pipelines created
	const Topl_Pipeline* _activePipeline; // only one active pipeline at a time
	Topl_Shader** _shaders; // contains all shaders

    Timer_Ticker _ticker;
#ifdef RASTERON_H
	FT_Library _freetypeLib; // required for working with fonts
#endif
};