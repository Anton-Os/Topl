#include "Topl_Shader.hpp"

struct PassThru_TessCtrlShader : public Topl_Shader {
	PassThru_TessCtrlShader()
		: Topl_Shader(
			SHDR_TessCtrl,
			genPrefix_glsl() + "TessCtrl_PassThru.glsl",
			{ Shader_Type({}) }// Inputs
		){ }
};

struct PassThru_TessEvalShader : public Topl_Shader {
	PassThru_TessEvalShader()
		: Topl_Shader(
			SHDR_TessEval,
			genPrefix_glsl() + "TessEval_PassThru.glsl",
			{ Shader_Type({}) }// Inputs
		){ }
};

struct PassThru_GeometryShader : public Topl_Shader {
	PassThru_GeometryShader()
		: Topl_Shader(
			SHDR_Geom,
			genPrefix_glsl() + "Geometry_PassThru.glsl",
			{ Shader_Type({}) }// Inputs
		){ }
};