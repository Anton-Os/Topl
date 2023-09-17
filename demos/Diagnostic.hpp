#define RENDER_TEST_ENABLE

#include "Topl_Main.hpp"

#include "primitives/Geo_Flat.hpp"

#include "Idle_Shader.hpp"
#include "Advance_Shader.hpp"

#define ACTOR_COUNT 99999

struct TestConfig {
	void buildTest(Topl_Renderer* renderer) {
		renderer->setDrawMode(DRAW_Strip);

		for(unsigned a = 0; a < ACTOR_COUNT; a++){
			actors[a] = Geo_Actor((Geo_Renderable*)&triangle);
			scene.addGeometry(&actors[a]);
		}
		renderer->buildScene(&scene);
	}

	// TODO: Include other tests

	Topl_Scene scene;
	Timer_Ticker ticker;

	Geo_FlatTriangle triangle = Geo_FlatTriangle();
	Geo_Actor actors[ACTOR_COUNT];
};

// OpenGL Test Renderer
struct Diagnostic_Renderer_GL4 : public Topl_Renderer_GL4, public TestConfig {
	Diagnostic_Renderer_GL4(NATIVE_WINDOW window) 
	: Topl_Renderer_GL4(window), TestConfig() {
		genPipeline(&pipeline, &vertexShader, &fragmentShader);
		buildTest(this);
	}

	Topl_Pipeline_GL4 pipeline;
	GL4_Idle_VertexShader vertexShader;
	GL4_Idle_FragmentShader fragmentShader;
	GL4_Advance_GeometryShader geomShader;
	GL4_Advance_TessCtrlShader tessCtrlShader;
	GL4_Advance_TessEvalShader tessEvalShader;
};

#ifdef _WIN32
// DirectX Test Renderer
struct Diagnostic_Renderer_Drx11 : public Topl_Renderer_Drx11, public TestConfig {
	Diagnostic_Renderer_Drx11(NATIVE_WINDOW window) 
	: Topl_Renderer_Drx11(window), TestConfig() {
		genPipeline(&pipeline, &vertexShader, &fragmentShader);
		buildTest(this);
	}

	Topl_Pipeline_Drx11 pipeline;
	Drx11_Idle_VertexShader vertexShader;
	Drx11_Idle_FragmentShader fragmentShader;
	Drx11_Advance_GeometryShader geomShader;
	Drx11_Advance_TessCtrlShader tessCtrlShader;
	Drx11_Advance_TessEvalShader tessEvalShader;
};
#endif

#ifdef VULKAN_H
// Vulkan Test Renderer
struct Diagnostic_Renderer_Vulkan : public Topl_Renderer_Vulkan, public TestConfig {
	Diagnostic_Renderer_Vulkan(NATIVE_WINDOW window)
	: Topl_Renderer_Vulkan(window), TestConfig(){
		_isBuilt = true;
		_renderIDs = 1;
	}

	Topl_Pipeline_Vulkan pipeline;
};
#endif