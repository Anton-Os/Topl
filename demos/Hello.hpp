#include "Topl_Factory.hpp"

#include "meshes/Geo_Surface.hpp"

#include "legacy/idle/Idle_Shader.hpp"
#include "legacy/advanced/Advance_Shader.hpp"

#ifdef TOPL_ENABLE_AUDIO
	#include <miniaudio/miniaudio.h> // Audio Library
	#include <kissfft/kiss_fft.h> // FFT Library
	#include <kissfft/kiss_fftr.h> // FFT Library
#endif

// OpenGL Test Renderer
#ifndef __ANDROID__
struct Hello_Renderer_GL : public Topl_Renderer_GL{
    Hello_Renderer_GL(NATIVE_PLATFORM_CONTEXT* context) : Topl_Renderer_GL(context){
        // genPipeline(&geoPipeline, &vertexShader, &pixelShader, { &geomShader });
        // genPipeline(&tessPipeline, &vertexShader, &pixelShader, { &tessCtrlShader, &tessEvalShader });
        // genPipeline(&fullPipeline, &vertexShader, &pixelShader, { &geomShader, &tessCtrlShader, &tessEvalShader });
		genPipeline(&basePipeline, &vertexShader, &pixelShader);
        // genComputePipeline(&calcPipeline, &computeShader);

        setDrawPipeline(true);
		setDrawMode(DRAW_Points);
	}

    void setDrawPipeline(bool isNonCompute){ setPipeline((!isNonCompute)? &calcPipeline : &basePipeline); }

	GL::Pipeline basePipeline, geoPipeline, tessPipeline, fullPipeline, calcPipeline;

	Idle_VertexShader_GL vertexShader;
	Idle_PixelShader_GL pixelShader;
	Advance_GeometryShader_GL geomShader;
	Advance_TessCtrlShader_GL tessCtrlShader;
	Advance_TessEvalShader_GL tessEvalShader;
	Advance_ComputeShader_GL computeShader;
};
#else
#include <game-activity/GameActivity.cpp> // TODO: Move to a different file
#include <game-text-input/gametextinput.cpp> // TODO: Move to a different file

struct Hello_Renderer_GL : public Droidl_Renderer {
    Hello_Renderer_GL(NATIVE_PLATFORM_CONTEXT* context) : Droidl_Renderer(context){
        genPipeline(&pipeline, &vertexShader, &pixelShader);
        setDrawMode(DRAW_Triangles);
    }

    void update(const Geo_Actor* actor){ Droidl_Renderer::update(actor); }

    GL::Pipeline pipeline;

    Idle_VertexShader_GL vertexShader;
    Idle_PixelShader_GL pixelShader;
};
#endif

#ifdef _WIN32
struct Hello_Renderer_DX : public Topl_Renderer_DX {
	Hello_Renderer_DX(NATIVE_PLATFORM_CONTEXT* context) : Topl_Renderer_DX(context){
		genPipeline(&geoPipeline, &vertexShader, &pixelShader, { &geomShader });
		genPipeline(&tessPipeline, &vertexShader, &pixelShader, { &tessCtrlShader, &tessEvalShader });
		genPipeline(&fullPipeline, &vertexShader, &pixelShader, { &geomShader, &tessCtrlShader, &tessEvalShader });
		genPipeline(&basePipeline, &vertexShader, &pixelShader);
        genComputePipeline(&calcPipeline, &computeShader);

        setDrawPipeline(true);
		setDrawMode(DRAW_Patch);
	}

    void setDrawPipeline(bool isNonCompute){ setPipeline((!isNonCompute)? &calcPipeline : &fullPipeline); }

	DX::Pipeline basePipeline, geoPipeline, tessPipeline, fullPipeline, calcPipeline;

	Idle_VertexShader_DX vertexShader;
	Idle_PixelShader_DX pixelShader;
	Advance_GeometryShader_DX geomShader;
	Advance_TessCtrlShader_DX tessCtrlShader;
	Advance_TessEvalShader_DX tessEvalShader;
	Advance_ComputeShader_DX computeShader;
};
#endif

#ifdef TOPL_ENABLE_VULKAN && !defined(__ANDROID__)
struct Hello_Renderer_VK : public Topl_Renderer_VK {
	Hello_Renderer_VK(NATIVE_PLATFORM_CONTEXT* context) : Topl_Renderer_VK(context){
		setDrawMode(DRAW_Triangles);
		genPipeline(&pipeline, &vertexShader, &pixelShader);
		_flags[BUILD_BIT] = true;
		_renderIDs = 1;
	}

	VK::Pipeline pipeline;
	Idle_VertexShader vertexShader = Idle_VertexShader("legacy/_spirv/Idle_Vertex.glsl.spv");
	Idle_PixelShader pixelShader = Idle_PixelShader("legacy/_spirv/Idle_Frag.glsl.spv");
};
#endif
