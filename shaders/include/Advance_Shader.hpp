#include "Topl_Pipeline.h"

// Control Shaders

struct Advance_TessCtrlShader : public Topl_Shader {
	Advance_TessCtrlShader() : Topl_Shader(){} // Blank Constructor
	Advance_TessCtrlShader(std::string fileName) : Topl_Shader(SHDR_TessCtrl, fileName){ }
};

struct GL4_Advance_TessCtrlShader : public Advance_TessCtrlShader {
	GL4_Advance_TessCtrlShader() : Advance_TessCtrlShader(genPrefix_glsl() + "TessCtrl_Advance.glsl"){ }
};

struct Drx11_Advance_TessCtrlShader : public Advance_TessCtrlShader {
	Drx11_Advance_TessCtrlShader() : Advance_TessCtrlShader(genPrefix_hlsl() + "TessHull_Advance.hlsl"){ }
};

// Evaluation Shaders

struct Advance_TessEvalShader : public Topl_Shader {
	Advance_TessEvalShader() : Topl_Shader(){} // Blank Constructor
	Advance_TessEvalShader(std::string fileName) : Topl_Shader(SHDR_TessEval, fileName){ }
};

struct GL4_Advance_TessEvalShader : public Advance_TessEvalShader {
	GL4_Advance_TessEvalShader() : Advance_TessEvalShader(genPrefix_glsl() + "TessEval_Advance.glsl"){ }
};

struct Drx11_Advance_TessEvalShader : public Advance_TessEvalShader {
	Drx11_Advance_TessEvalShader() : Advance_TessEvalShader(genPrefix_hlsl() + "TessDomain_Advance.hlsl"){ }
};

// Geometry Shaders

struct Advance_GeometryShader : public Topl_Shader {
	Advance_GeometryShader() : Topl_Shader(){} // Blank Constructor
	Advance_GeometryShader(std::string fileName) : Topl_Shader(SHDR_Geom, fileName){ }
};

struct GL4_Advance_GeometryShader : public Advance_GeometryShader {
	GL4_Advance_GeometryShader() : Advance_GeometryShader(genPrefix_glsl() + "Geometry_Advance.glsl"){}
};

struct Drx11_Advance_GeometryShader : public Advance_GeometryShader {
	Drx11_Advance_GeometryShader() : Advance_GeometryShader(genPrefix_hlsl() + "Geometry_Advance.hlsl"){}
};