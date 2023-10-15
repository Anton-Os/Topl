#define RENDER_TEST_ENABLE

#include "Topl_Main.hpp"

#include "meshes/Geo_Shape.hpp"

#include "Idle_Shader.hpp"
#include "Advance_Shader.hpp"

#define ACTOR_COUNT 1

Geo_ShapeTriangle triangle = Geo_ShapeTriangle();
Geo_Actor actor = Geo_Actor((Geo_Mesh*)&triangle);

Topl_Scene test_scene = Topl_Scene();

// OpenGL Test Renderer
struct Diagnostic_Renderer_GL4 : public Topl_Renderer_GL4{
	Diagnostic_Renderer_GL4(NATIVE_WINDOW window) 
	: Topl_Renderer_GL4(window){ genPipeline(&pipeline, &vertexShader, &pixelShader); }

	void build(const Topl_Scene* scene) override { 
		logMessage("Diagnostic Build (OpenGL)");
		setDrawMode(DRAW_Triangles);
		Topl_Renderer_GL4::build(scene); 
	}

	Topl_Pipeline_GL4 pipeline;
	GL4_Idle_VertexShader vertexShader;
	GL4_Idle_PixelShader pixelShader;
	/* GL4_Advance_GeometryShader geomShader;
	GL4_Advance_TessCtrlShader tessCtrlShader;
	GL4_Advance_TessEvalShader tessEvalShader; */
};

#ifdef _WIN32
// DirectX Test Renderer
struct Diagnostic_Renderer_Drx11 : public Topl_Renderer_Drx11 {
	Diagnostic_Renderer_Drx11(NATIVE_WINDOW window) 
	: Topl_Renderer_Drx11(window){ genPipeline(&pipeline, &vertexShader, &pixelShader); }

	void build(const Topl_Scene* scene) override {
		logMessage("Diagnostic Build (DirectX11)");
		setDrawMode(DRAW_Triangles);
		Topl_Renderer_Drx11::build(scene); 
	}

	Topl_Pipeline_Drx11 pipeline;
	Drx11_Idle_VertexShader vertexShader;
	Drx11_Idle_PixelShader pixelShader;
	/* Drx11_Advance_GeometryShader geomShader;
	Drx11_Advance_TessCtrlShader tessCtrlShader;
	Drx11_Advance_TessEvalShader tessEvalShader; */
};
#endif

#ifdef VULKAN_H
// Vulkan Test Renderer
struct Diagnostic_Renderer_Vulkan : public Topl_Renderer_Vulkan {
	Diagnostic_Renderer_Vulkan(NATIVE_WINDOW window)
	: Topl_Renderer_Vulkan(window){
		// genPipeline(&pipeline, &vertexShader, &pixelShader);
		_isBuilt = true;
		_renderIDs = 1;
	}

	Topl_Pipeline_Vulkan pipeline;
	Idle_VertexShader vertexShader = Idle_VertexShader("spirv/Idle_Vertex.glsl.spv");
	Idle_PixelShader pixelShader = Idle_PixelShader("spirv/Idle_Frag.glsl.spv");
};
#endif