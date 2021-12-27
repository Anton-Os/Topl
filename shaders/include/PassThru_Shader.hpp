#include "Topl_Shader.hpp"

// Control Shaders

struct PassThru_TessCtrlShader : public Topl_Shader {
	PassThru_TessCtrlShader(std::string name) : Topl_Shader(SHDR_TessCtrl, name){ }
};

struct GL4_PassThru_TessCtrlShader : public PassThru_TessCtrlShader {
	GL4_PassThru_TessCtrlShader() : PassThru_TessCtrlShader(genPrefix_glsl() + "TessCtrl_PassThru.glsl"){ }
};

struct Drx11_PassThru_TessCtrlShader : public PassThru_TessCtrlShader {
	Drx11_PassThru_TessCtrlShader() : PassThru_TessCtrlShader(genPrefix_hlsl() + "TessHull_PassThru.hlsl"){ }
};

// Evaluation Shaders

struct PassThru_TessEvalShader : public Topl_Shader {
	PassThru_TessEvalShader(std::string name) : Topl_Shader(SHDR_TessEval, name){ }
};

struct GL4_PassThru_TessEvalShader : public PassThru_TessEvalShader {
	GL4_PassThru_TessEvalShader() : PassThru_TessEvalShader(genPrefix_glsl() + "TessEval_PassThru.glsl"){ }
};

struct Drx11_PassThru_TessEvalShader : public PassThru_TessEvalShader {
	Drx11_PassThru_TessEvalShader() : PassThru_TessEvalShader(genPrefix_hlsl() + "TessDomain_PassThru.hlsl"){ }
};

// Geometry Shaders

struct PassThru_GeometryShader : public Topl_Shader {
	PassThru_GeometryShader(std::string name) : Topl_Shader(SHDR_Geom, name){ }
};

struct GL4_PassThru_GeometryShader : public PassThru_GeometryShader {
    GL4_PassThru_GeometryShader() : PassThru_GeometryShader(genPrefix_glsl() + "Geometry_PassThru.glsl");
}

struct Drx11_PassThru_GeometryShader : public PassThru_GeometryShader {
    Drx11_PassThru_GeometryShader() : PassThru_GeometryShader(genPrefix_hlsl() + "Geometry_PassThru.hlsl");
}