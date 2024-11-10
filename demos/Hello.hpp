#define RENDER_TEST_ENABLE

#include "Topl_Factory.hpp"

#include "meshes/Geo_Surface.hpp"

#include "Idle_Shader.hpp"
#include "Advance_Shader.hpp"

// OpenGL Test Renderer
struct Hello_Renderer_GL4 : public Topl_Renderer_GL4{
	Hello_Renderer_GL4(NATIVE_PLATFORM_CONTEXT* context) : Topl_Renderer_GL4(context){
		genPipeline(&geoPipeline, &vertexShader, &pixelShader, &geomShader, nullptr, nullptr);
		genPipeline(&tessPipeline, &vertexShader, &pixelShader, nullptr, &tessCtrlShader, &tessEvalShader);
		genPipeline(&fullPipeline, &vertexShader, &pixelShader, &geomShader, &tessCtrlShader, &tessEvalShader);
		genPipeline(&basePipeline, &vertexShader, &pixelShader);

        setPipeline(&tessPipeline);
	}

	GL4::Pipeline basePipeline, geoPipeline, tessPipeline, fullPipeline;

	Idle_VertexShader_GL4 vertexShader;
	Idle_PixelShader_GL4 pixelShader;
	Advance_GeometryShader_GL4 geomShader;
	Advance_TessCtrlShader_GL4 tessCtrlShader;
	Advance_TessEvalShader_GL4 tessEvalShader;
};

#ifdef _WIN32
// DirectX Test Renderer
struct Hello_Renderer_DX11 : public Topl_Renderer_DX11 {
	Hello_Renderer_DX11(NATIVE_PLATFORM_CONTEXT* context) : Topl_Renderer_DX11(context){
		genPipeline(&geoPipeline, &vertexShader, &pixelShader, &geomShader, nullptr, nullptr);
		genPipeline(&tessPipeline, &vertexShader, &pixelShader, nullptr, &tessCtrlShader, &tessEvalShader);
		genPipeline(&fullPipeline, &vertexShader, &pixelShader, &geomShader, &tessCtrlShader, &tessEvalShader);
		genPipeline(&basePipeline, &vertexShader, &pixelShader);

		setPipeline(&tessPipeline);
	}

	DX11::Pipeline basePipeline, geoPipeline, tessPipeline, fullPipeline;

	Idle_VertexShader_DX11 vertexShader;
	Idle_PixelShader_DX11 pixelShader;
	Advance_GeometryShader_DX11 geomShader;
	Advance_TessCtrlShader_DX11 tessCtrlShader;
	Advance_TessEvalShader_DX11 tessEvalShader;
};
#endif

#ifdef TOPL_ENABLE_VULKAN
// VK Test Renderer
struct Hello_Renderer_VK : public Topl_Renderer_VK {
	Hello_Renderer_VK(NATIVE_PLATFORM_CONTEXT* context)
	: Topl_Renderer_VK(context){
		genPipeline(&pipeline, &vertexShader, &pixelShader);
		_flags[BUILD_BIT] = true;
		_renderIDs = 1;
	}

	void build(const Geo_Actor* actor) {
		// TODO: Implement drawing for testing purposes
	}

	void draw(const Geo_Actor* actor) {
		// TODO: Implement drawing for testing purposes
	}

	VK::Pipeline pipeline;
	Idle_VertexShader vertexShader = Idle_VertexShader("spirv/Idle_Vertex.glsl.spv");
	Idle_PixelShader pixelShader = Idle_PixelShader("spirv/Idle_Frag.glsl.spv");
};
#endif
