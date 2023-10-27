#include "Topl_Pipeline.hpp"

// Control Shaders

struct Advance_TessCtrlShader : public Topl_Shader {
	Advance_TessCtrlShader() : Topl_Shader(){} // Blank Constructor
	Advance_TessCtrlShader(std::string fileName) : Topl_Shader(SHDR_TessCtrl, fileName){ }
};

struct Advance_TessCtrlShader_GL4 : public Advance_TessCtrlShader {
	Advance_TessCtrlShader_GL4() : Advance_TessCtrlShader(genPrefix_glsl() + "Advance_TessCtrl.glsl"){ }
};

struct Advance_TessCtrlShader_Drx11 : public Advance_TessCtrlShader {
	Advance_TessCtrlShader_Drx11() : Advance_TessCtrlShader(genPrefix_hlsl() + "Advance_Hull.hlsl"){ }
};

// Evaluation Shaders

struct Advance_TessEvalShader : public Topl_Shader {
	Advance_TessEvalShader() : Topl_Shader(){} // Blank Constructor
	Advance_TessEvalShader(std::string fileName) : Topl_Shader(SHDR_TessEval, fileName){ }
};

struct Advance_TessEvalShader_GL4 : public Advance_TessEvalShader {
	Advance_TessEvalShader_GL4() : Advance_TessEvalShader(genPrefix_glsl() + "Advance_TessEval.glsl"){ }
};

struct Advance_TessEvalShader_Drx11 : public Advance_TessEvalShader {
	Advance_TessEvalShader_Drx11() : Advance_TessEvalShader(genPrefix_hlsl() + "Advance_Domain.hlsl"){ }
};

// Geometry Shaders

struct Advance_GeometryShader : public Topl_Shader {
	Advance_GeometryShader() : Topl_Shader(){} // Blank Constructor
	Advance_GeometryShader(std::string fileName) : Topl_Shader(SHDR_Geom, fileName){ }
};

struct Advance_GeometryShader_GL4 : public Advance_GeometryShader {
	Advance_GeometryShader_GL4() : Advance_GeometryShader(genPrefix_glsl() + "Advance_Geometry.glsl"){}
};

struct Advance_GeometryShader_Drx11 : public Advance_GeometryShader {
	Advance_GeometryShader_Drx11() : Advance_GeometryShader(genPrefix_hlsl() + "Advance_Geometry.hlsl"){}
};