#include "Topl_Shader.hpp"

struct Flat_FragmentShader : public Topl_Shader {
	Flat_FragmentShader()
		: Topl_Shader(
			SHDR_Fragment,
			genPrefix_glsl() + "Frag_Flat.glsl",
			{ Shader_Type("flatColor", SHDR_float) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const { return false; }
};