#include "Topl_Shader.hpp"

struct PassThru_TessCtrlShader : public Topl_Shader {
	Flat_FragmentShader()
		: Topl_Shader(
			SHDR_Fragment,
			genPrefix_glsl() + "TessCtrl_PassThru.glsl",
			{ Shader_Type({}) }// Inputs
		){ }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const { return false; }
};

struct PassThru_TessEvalShader : public Topl_Shader {
	Flat_FragmentShader()
		: Topl_Shader(
			SHDR_Fragment,
			genPrefix_glsl() + "TessEval_PassThru.glsl",
			{ Shader_Type({}) }// Inputs
		){ }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const { return false; }
};

struct Geometry_TessEvalShader : public Topl_Shader {
	Flat_FragmentShader()
		: Topl_Shader(
			SHDR_Fragment,
			genPrefix_glsl() + "Geometry_PassThru.glsl",
			{ Shader_Type({}) }// Inputs
		){ }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const { return false; }
};