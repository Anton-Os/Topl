#define RENDER_TEST_ENABLE

#include "Topl_Program.hpp"

#include "meshes/Geo_Surface.hpp"

#include "Idle_Shader.hpp"
#include "Advance_Shader.hpp"

// OpenGL Test Renderer
struct Hello_Renderer_GL4 : public Topl_Renderer_GL4{
	Hello_Renderer_GL4(NATIVE_WINDOW window) 
	: Topl_Renderer_GL4(window){ 
		genPipeline(&geoOnlyPipeline, &vertexShader, &pixelShader, &geomShader, nullptr, nullptr);
		genPipeline(&tessOnlyPipeline, &vertexShader, &pixelShader, nullptr, &tessCtrlShader, &tessEvalShader);
		genPipeline(&fullPipeline, &vertexShader, &pixelShader, &geomShader, &tessCtrlShader, &tessEvalShader);
		genPipeline(&basePipeline, &vertexShader, &pixelShader); 
	}

	void build(const Topl_Scene* scene) override { 
		logMessage("Hello Build (OpenGL)");
		setDrawMode(DRAW_Triangles);
		Topl_Renderer_GL4::build(scene); 
	}

	Topl_Pipeline_GL4 basePipeline, geoOnlyPipeline, tessOnlyPipeline, fullPipeline;

	Idle_VertexShader_GL4 vertexShader;
	Idle_PixelShader_GL4 pixelShader;
	Advance_GeometryShader_GL4 geomShader;
	Advance_TessCtrlShader_GL4 tessCtrlShader;
	Advance_TessEvalShader_GL4 tessEvalShader;
};

#ifdef _WIN32
// DirectX Test Renderer
struct Hello_Renderer_DX11 : public Topl_Renderer_DX11 {
	Hello_Renderer_DX11(NATIVE_WINDOW window) 
	: Topl_Renderer_DX11(window){ 
		genPipeline(&geoOnlyPipeline, &vertexShader, &pixelShader, &geomShader, nullptr, nullptr);
		genPipeline(&tessOnlyPipeline, &vertexShader, &pixelShader, nullptr, &tessCtrlShader, &tessEvalShader);
		genPipeline(&fullPipeline, &vertexShader, &pixelShader, &geomShader, &tessCtrlShader, &tessEvalShader);
		genPipeline(&basePipeline, &vertexShader, &pixelShader);
	}

	void build(const Topl_Scene* scene) override {
		logMessage("Hello Build (DirectX11)");
		setDrawMode(DRAW_Triangles);
		Topl_Renderer_DX11::build(scene); 
	}

	Topl_Pipeline_DX11 basePipeline, geoOnlyPipeline, tessOnlyPipeline, fullPipeline;

	Idle_VertexShader_DX11 vertexShader;
	Idle_PixelShader_DX11 pixelShader;
	Advance_GeometryShader_DX11 geomShader;
	Advance_TessCtrlShader_DX11 tessCtrlShader;
	Advance_TessEvalShader_DX11 tessEvalShader;
};
#endif

#ifdef TOPL_ENABLE_VULKAN
// Vulkan Test Renderer
struct Hello_Renderer_Vulkan : public Topl_Renderer_Vulkan {
	Hello_Renderer_Vulkan(NATIVE_WINDOW window)
	: Topl_Renderer_Vulkan(window){
		genPipeline(&pipeline, &vertexShader, &pixelShader);
		_flags[BUILD_BIT] = true;
		_renderIDs = 1;
	}

	Topl_Pipeline_Vulkan pipeline;
	Idle_VertexShader vertexShader = Idle_VertexShader("spirv/Idle_Vertex.glsl.spv");
	Idle_PixelShader pixelShader = Idle_PixelShader("spirv/Idle_Frag.glsl.spv");
};
#endif