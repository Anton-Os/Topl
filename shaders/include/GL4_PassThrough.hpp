#include "Topl_Shader.hpp"

struct PassThru_TessCtrlShader : public Topl_Shader {
	PassThru_TessCtrlShader()
		: Topl_Shader(
			SHDR_TessCtrl,
			genPrefix_glsl() + "TessCtrl_PassThru.glsl",
			{ Shader_Type({}) }// Inputs
		){ }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const { return false; }
};

struct PassThru_TessEvalShader : public Topl_Shader {
	PassThru_TessEvalShader()
		: Topl_Shader(
			SHDR_TessEval,
			genPrefix_glsl() + "TessEval_PassThru.glsl",
			{ Shader_Type({}) }// Inputs
		){ }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const { return false; }
};

struct PassThru_GeometryShader : public Topl_Shader {
	PassThru_GeometryShader()
		: Topl_Shader(
			SHDR_Geom,
			genPrefix_glsl() + "Geometry_PassThru.glsl",
			{ Shader_Type({}) }// Inputs
		){ }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const { return false; }
};