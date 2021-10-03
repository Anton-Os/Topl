#include "Topl_Shader.hpp"

struct Flat_FragmentShader : public Topl_PrimaryShader {
	Flat_FragmentShader()
		: Topl_PrimaryShader(
			SHDR_Fragment,
			genPrefix_glsl() + "Flat_Frag.glsl",
			{ Shader_Type("flatColor", SHDR_float) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Actor* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const { return false; }
};