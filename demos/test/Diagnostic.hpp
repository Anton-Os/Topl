#define RENDER_TEST_ENABLE

#include "Topl_App.hpp"

#include "primitives/Geo_Flat.hpp"

#include "Idle_Shader.hpp"
#include "Advance_Shader.hpp"

// OpenGL Test Renderer
struct Barebones_Renderer_GL4 : public Topl_Renderer_GL4 {
	Barebones_Renderer_GL4(NATIVE_WINDOW window) : Topl_Renderer_GL4(window) {
		genPipeline(&pipeline, &vertexShader, &fragmentShader);
		_renderIDs = 1;
		_isBuilt = true;
		config();
	}

	void config();

	Topl_Pipeline_GL4 pipeline;
	GL4_Idle_VertexShader vertexShader;
	GL4_Idle_FragmentShader fragmentShader;
	GL4_Advance_GeometryShader geomShader;
	GL4_Advance_TessCtrlShader tessCtrlShader;
	GL4_Advance_TessEvalShader tessEvalShader;
};

#ifdef _WIN32
// DirectX Test Renderer
struct Barebones_Renderer_Drx11 : public Topl_Renderer_Drx11 {
	Barebones_Renderer_Drx11(NATIVE_WINDOW window) : Topl_Renderer_Drx11(window) {
		genPipeline(&pipeline, &vertexShader, &fragmentShader);
		_renderIDs = 1;
		_isBuilt = true;
		// config();
	}

	void config();

	Topl_Pipeline_Drx11 pipeline;
	Drx11_Idle_VertexShader vertexShader;
	Drx11_Idle_FragmentShader fragmentShader;
	Drx11_Advance_GeometryShader geomShader;
	Drx11_Advance_TessCtrlShader tessCtrlShader;
	Drx11_Advance_TessEvalShader tessEvalShader;
};
#endif