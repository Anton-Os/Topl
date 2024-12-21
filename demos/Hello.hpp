#define RENDER_TEST_ENABLE

#include "Topl_Factory.hpp"

#include "meshes/Geo_Surface.hpp"

#include "Idle_Shader.hpp"
#include "Advance_Shader.hpp"

// OpenGL Test Renderer
#ifndef __ANDROID__
struct Hello_Renderer_GL4 : public Topl_Renderer_GL4{
    Hello_Renderer_GL4(NATIVE_PLATFORM_CONTEXT* context) : Topl_Renderer_GL4(context){
		genPipeline(&geoPipeline, &vertexShader, &pixelShader, { &geomShader });
		genPipeline(&tessPipeline, &vertexShader, &pixelShader, { &tessCtrlShader, &tessEvalShader });
		genPipeline(&fullPipeline, &vertexShader, &pixelShader, { &geomShader, &tessCtrlShader, &tessEvalShader });
		genPipeline(&basePipeline, &vertexShader, &pixelShader);
        genComputePipeline(&calcPipeline, &computeShader);

        setDrawPipeline(true);
		setDrawMode(DRAW_Points);
	}

    void setDrawPipeline(bool isNonCompute){ setPipeline((!isNonCompute)? &calcPipeline : &geoPipeline); }

	GL4::Pipeline basePipeline, geoPipeline, tessPipeline, fullPipeline, calcPipeline;

	Idle_VertexShader_GL4 vertexShader;
	Idle_PixelShader_GL4 pixelShader;
	Advance_GeometryShader_GL4 geomShader;
	Advance_TessCtrlShader_GL4 tessCtrlShader;
	Advance_TessEvalShader_GL4 tessEvalShader;
	Advance_ComputeShader_GL4 computeShader;
};
#else
struct Hello_Renderer_GL4 : public Droidl_Renderer {
    Hello_Renderer_GL4(NATIVE_PLATFORM_CONTEXT* context) : Droidl_Renderer(context){
        // genPipeline(&basePipeline, &vertexShader, &pixelShader);
        setDrawMode(DRAW_Triangles);
    }

    void setDrawPipeline(bool isNonCompute){ /* empty */ }

    GL4::Pipeline pipeline;

    Idle_VertexShader_GL4 vertexShader;
    Idle_PixelShader_GL4 pixelShader;
};
#endif

#ifdef _WIN32
struct Hello_Renderer_DX11 : public Topl_Renderer_DX11 {
	Hello_Renderer_DX11(NATIVE_PLATFORM_CONTEXT* context) : Topl_Renderer_DX11(context){
		genPipeline(&geoPipeline, &vertexShader, &pixelShader, { &geomShader });
		genPipeline(&tessPipeline, &vertexShader, &pixelShader, { &tessCtrlShader, &tessEvalShader });
		genPipeline(&fullPipeline, &vertexShader, &pixelShader, { &geomShader, &tessCtrlShader, &tessEvalShader });
		genPipeline(&basePipeline, &vertexShader, &pixelShader);
        genComputePipeline(&calcPipeline, &computeShader);

        setDrawPipeline(true);
		setDrawMode(DRAW_Patch);
	}

    void setDrawPipeline(bool isNonCompute){ setPipeline((!isNonCompute)? &calcPipeline : &fullPipeline); }

	DX11::Pipeline basePipeline, geoPipeline, tessPipeline, fullPipeline, calcPipeline;

	Idle_VertexShader_DX11 vertexShader;
	Idle_PixelShader_DX11 pixelShader;
	Advance_GeometryShader_DX11 geomShader;
	Advance_TessCtrlShader_DX11 tessCtrlShader;
	Advance_TessEvalShader_DX11 tessEvalShader;
	Advance_ComputeShader_DX11 computeShader;
};
#endif

#ifdef TOPL_ENABLE_VULKAN
struct Hello_Renderer_VK : public Topl_Renderer_VK {
	Hello_Renderer_VK(NATIVE_PLATFORM_CONTEXT* context) : Topl_Renderer_VK(context){
		setDrawMode(DRAW_Triangles);
		genPipeline(&pipeline, &vertexShader, &pixelShader);
		_flags[BUILD_BIT] = true;
		_renderIDs = 1;
	}

    void setDrawPipeline(bool isNonCompute){ } // dummy invocation

	VK::Pipeline pipeline;
	Idle_VertexShader vertexShader = Idle_VertexShader("spirv/Idle_Vertex.glsl.spv");
	Idle_PixelShader pixelShader = Idle_PixelShader("spirv/Idle_Frag.glsl.spv");
};
#endif
