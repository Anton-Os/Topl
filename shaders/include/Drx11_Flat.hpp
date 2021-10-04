#include "Topl_Shader.hpp"

struct Flat_PixelShader : public Topl_PrimaryShader {
	Flat_PixelShader()
		: Topl_PrimaryShader(
			SHDR_Fragment, 
			genPrefix_hlsl() + "Flat_Pixel.hlsl",
			{ 
				Shader_Type("pos", "POSITION", SHDR_float_vec3), 
				Shader_Type("flatColor", "COLOR0", SHDR_uint) 
			} // Inputs
		) { }

	virtual bool genGeoBlock(const Geo_Actor* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, std::vector<uint8_t>* bytes) const { return false; }
};