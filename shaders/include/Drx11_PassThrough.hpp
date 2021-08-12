#include "Topl_Shader.hpp"

struct PassThru_TessHullShader : public Topl_Shader {
    PassThru_TessHullShader()
		: Topl_Shader(
			SHDR_TessCtrl,
			genPrefix_hlsl() + "TessHull_PassThru.hlsl",
			{ Shader_Type({}) }// Inputs
		){ }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const { return false; }
};

struct PassThru_TessDomainShader : public Topl_Shader {
    PassThru_TessDomainShader()
		: Topl_Shader(
			SHDR_TessEval,
			genPrefix_hlsl() + "TessDomain_PassThru.hlsl",
			{ Shader_Type({}) }// Inputs
		){ }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const { return false; }
};

struct PassThru_GeometryShader : public Topl_Shader {
    PassThru_GeometryShader()
		: Topl_Shader(
			SHDR_Geom,
			genPrefix_hlsl() + "Geometry_PassThru.hlsl",
			{ Shader_Type({}) }// Inputs
		){ }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const { return false; }
};