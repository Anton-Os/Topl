#include "Topl_Shader.hpp"

struct PassThru_TessCtrlShader : public Topl_PrimaryShader {
	PassThru_TessCtrlShader()
		: Topl_PrimaryShader(
			SHDR_TessCtrl,
			genPrefix_glsl() + "TessCtrl_PassThru.glsl",
			{ Shader_Type({}) }// Inputs
		){ }

	virtual bool genGeoBlock(const Geo_Actor* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, std::vector<uint8_t>* bytes) const { return false; }
};

struct PassThru_TessEvalShader : public Topl_PrimaryShader {
	PassThru_TessEvalShader()
		: Topl_PrimaryShader(
			SHDR_TessEval,
			genPrefix_glsl() + "TessEval_PassThru.glsl",
			{ Shader_Type({}) }// Inputs
		){ }

	virtual bool genGeoBlock(const Geo_Actor* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, std::vector<uint8_t>* bytes) const { return false; }
};

struct PassThru_GeometryShader : public Topl_PrimaryShader {
	PassThru_GeometryShader()
		: Topl_PrimaryShader(
			SHDR_Geom,
			genPrefix_glsl() + "Geometry_PassThru.glsl",
			{ Shader_Type({}) }// Inputs
		){ }

	virtual bool genGeoBlock(const Geo_Actor* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, std::vector<uint8_t>* bytes) const { return false; }
};