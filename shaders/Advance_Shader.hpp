#include "Topl_Pipeline.hpp"

// Control Shaders

struct Advance_TessCtrlShader : public Topl_Shader {
	Advance_TessCtrlShader() : Topl_Shader(){} // Blank Constructor
	Advance_TessCtrlShader(std::string fileName) : Topl_Shader(SHDR_TessCtrl, fileName){ }

	// TODO: Include embedding logic
};

struct Advance_TessCtrlShader_GL4 : public Advance_TessCtrlShader {
	Advance_TessCtrlShader_GL4() : Advance_TessCtrlShader(genPrefix_glsl() + "extended/" + "Advance_TessCtrl.glsl"){
		_embedMap.insert({ "Tess_Ctrl_Output", "layout(vertices = 3) out;" });
	}
	Advance_TessCtrlShader_GL4(std::string outputStr) : Advance_TessCtrlShader(genPrefix_glsl() + "extended/" + "Advance_TessCtrl.glsl"){
		_embedMap.insert({ "Tess_Ctrl_Output", outputStr });
	}
};

struct Advance_TessCtrlShader_DX11 : public Advance_TessCtrlShader {
	Advance_TessCtrlShader_DX11() : Advance_TessCtrlShader(genPrefix_hlsl() + "extended/" + "Advance_Hull.hlsl"){
		/* _embedMap.insert({ "Tess_Ctrl_InOut", 
			std::string("struct HS_INPUT { float4 pos : SV_POSITION; };")
			+ "\n" + "struct HS_OUTPUT { float4 pos : SV_POSITION; };"
		}); */
	}
};

// Evaluation Shaders

struct Advance_TessEvalShader : public Topl_Shader {
	Advance_TessEvalShader() : Topl_Shader(){} // Blank Constructor
	Advance_TessEvalShader(std::string fileName) : Topl_Shader(SHDR_TessEval, fileName){ }

	// TODO: Include embedding logic
};

struct Advance_TessEvalShader_GL4 : public Advance_TessEvalShader {
	Advance_TessEvalShader_GL4() : Advance_TessEvalShader(genPrefix_glsl() + "extended/" + "Advance_TessEval.glsl"){ 
		_embedMap.insert({ "Tess_Eval_Input", "layout(triangles, equal_spacing, ccw) in;" });
	}
	Advance_TessEvalShader_GL4(std::string inputStr) : Advance_TessEvalShader(genPrefix_glsl() + "extended/" + "Advance_TessEval.glsl"){ 
		_embedMap.insert({ "Tess_Eval_Input", inputStr });
	}
};

struct Advance_TessEvalShader_DX11 : public Advance_TessEvalShader {
	Advance_TessEvalShader_DX11() : Advance_TessEvalShader(genPrefix_hlsl() + "extended/" + "Advance_Domain.hlsl"){ }
};

// Geometry Shaders

struct Advance_GeometryShader : public Topl_Shader {
	Advance_GeometryShader() : Topl_Shader(){} // Blank Constructor
	Advance_GeometryShader(std::string fileName) : Topl_Shader(SHDR_Geom, fileName){ }

	// TODO: Include embedding logic
};

struct Advance_GeometryShader_GL4 : public Advance_GeometryShader {
	Advance_GeometryShader_GL4() : Advance_GeometryShader(genPrefix_glsl() + "extended/" + "Advance_Geometry.glsl"){
		_embedMap.insert({ "Geometry_Input", "layout(points) in;" });
		_embedMap.insert({ "Geometry_Output", "layout (triangle_strip, max_vertices = 3) out;" });
	}

	Advance_GeometryShader_GL4(std::string inputStr, std::string outputStr) : Advance_GeometryShader(genPrefix_glsl() + "extended/" + "Advance_Geometry.glsl"){
		_embedMap.insert({ "Geometry_Input", inputStr });
		_embedMap.insert({ "Geometry_Output", outputStr });
	}
};

struct Advance_GeometryShader_DX11 : public Advance_GeometryShader {
	Advance_GeometryShader_DX11() : Advance_GeometryShader(genPrefix_hlsl() + "extended/" + "Advance_Geometry.hlsl"){
		_embedMap.insert({ "Geometry_InOut", 
			"#define GEOM_MAIN_ENTRY void main(point GS_OUTPUT input[1], inout TriangleStream<GS_OUTPUT> outStream)" 
		});
	}
};

// Compute Shaders

struct Advance_ComputeShader : public Topl_Shader {
	Advance_ComputeShader() : Topl_Shader(){} // Blank Constructor
	Advance_ComputeShader(std::string fileName) : Topl_Shader(SHDR_Compute, fileName){ }

	// TODO: Include embedding logic
};

struct Advance_ComputeShader_GL4 : public Advance_ComputeShader {
	Advance_ComputeShader_GL4() : Advance_ComputeShader(genPrefix_glsl() + "extended/" + "Advance_Compute.glsl"){ }
};

struct Advance_ComputeShader_DX11 : public Advance_ComputeShader {
	Advance_ComputeShader_DX11() : Advance_ComputeShader(genPrefix_hlsl() + "extended/" + "Advance_Compute.hlsl"){ }
};
