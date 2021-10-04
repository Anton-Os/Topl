#include "Topl_Shader.hpp"

struct PassThru_TessHullShader : public Topl_PrimaryShader {
    PassThru_TessHullShader()
		: Topl_PrimaryShader(
			SHDR_TessCtrl,
			genPrefix_hlsl() + "TessHull_PassThru.hlsl",
			{ Shader_Type({}) }// Inputs
		){ }

	virtual bool genGeoBlock(const Geo_Actor* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, std::vector<uint8_t>* bytes) const { return false; }
};

struct PassThru_TessDomainShader : public Topl_PrimaryShader {
    PassThru_TessDomainShader()
		: Topl_PrimaryShader(
			SHDR_TessEval,
			genPrefix_hlsl() + "TessDomain_PassThru.hlsl",
			{ Shader_Type({}) }// Inputs
		){ }

	virtual bool genGeoBlock(const Geo_Actor* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, std::vector<uint8_t>* bytes) const { return false; }
};

struct PassThru_GeometryShader : public Topl_PrimaryShader {
    PassThru_GeometryShader()
		: Topl_PrimaryShader(
			SHDR_Geom,
			genPrefix_hlsl() + "Geometry_PassThru.hlsl",
			{ Shader_Type({}) }// Inputs
		){ }

	virtual bool genGeoBlock(const Geo_Actor* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, std::vector<uint8_t>* bytes) const { return false; }
};