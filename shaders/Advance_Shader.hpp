#include "Topl_Pipeline.hpp"

// Control Shaders

struct Advance_TessCtrlShader : public Topl_Shader {
	Advance_TessCtrlShader() : Topl_Shader(){} // Blank Constructor
	Advance_TessCtrlShader(std::string fileName) : Topl_Shader(SHDR_TessCtrl, fileName){ }

	// TODO: Include embedding logic
};

struct Advance_TessCtrlShader_GL4 : public Advance_TessCtrlShader {
	Advance_TessCtrlShader_GL4() : Advance_TessCtrlShader(genPrefix_glsl() + "Advance_TessCtrl.glsl"){}
};

struct Advance_TessCtrlShader_DX11 : public Advance_TessCtrlShader {
	Advance_TessCtrlShader_DX11() : Advance_TessCtrlShader(genPrefix_hlsl() + "Advance_Hull.hlsl"){ }
};

// Evaluation Shaders

struct Advance_TessEvalShader : public Topl_Shader {
	Advance_TessEvalShader() : Topl_Shader(){} // Blank Constructor
	Advance_TessEvalShader(std::string fileName) : Topl_Shader(SHDR_TessEval, fileName){ }

	// TODO: Include embedding logic
};

struct Advance_TessEvalShader_GL4 : public Advance_TessEvalShader {
	Advance_TessEvalShader_GL4() : Advance_TessEvalShader(genPrefix_glsl() + "Advance_TessEval.glsl"){ }
};

struct Advance_TessEvalShader_DX11 : public Advance_TessEvalShader {
	Advance_TessEvalShader_DX11() : Advance_TessEvalShader(genPrefix_hlsl() + "Advance_Domain.hlsl"){ }
};

// Geometry Shaders

struct Advance_GeometryShader : public Topl_Shader {
	Advance_GeometryShader() : Topl_Shader(){} // Blank Constructor
	Advance_GeometryShader(std::string fileName) : Topl_Shader(SHDR_Geom, fileName){ }

	// TODO: Include embedding logic
};

struct Advance_GeometryShader_GL4 : public Advance_GeometryShader {
	Advance_GeometryShader_GL4() : Advance_GeometryShader(genPrefix_glsl() + "Advance_Geometry.glsl"){
		_embedMap.insert({ "Entry", 
			"#define GEOM_INPUT layout(points)\n#define GEOM_OUTPUT layout (triangle_strip, max_vertices = 3)"
		});
	}
};

struct Advance_GeometryShader_DX11 : public Advance_GeometryShader {
	Advance_GeometryShader_DX11() : Advance_GeometryShader(genPrefix_hlsl() + "Advance_Geometry.hlsl"){
		_embedMap.insert({ "Entry", 
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
	Advance_ComputeShader_GL4() : Advance_ComputeShader(genPrefix_glsl() + "Advance_Compute.glsl"){ }
};

struct Advance_ComputeShader_DX11 : public Advance_ComputeShader {
	Advance_ComputeShader_DX11() : Advance_ComputeShader(genPrefix_hlsl() + "Advance_Compute.hlsl"){ }
};
