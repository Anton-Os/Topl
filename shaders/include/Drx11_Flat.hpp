#include "Topl_Shader.hpp"

struct Flat_PixelShader : public Topl_Shader {
	Flat_PixelShader()
		: Topl_Shader(
			SHDR_Fragment, 
			genPrefix_hlsl() + "Flat_Pixel.hlsl",
			{ 
				Shader_Type("pos", "POSITION", SHDR_float_vec3), 
				Shader_Type("flatColor", "COLOR0", SHDR_uint) 
			} // Inputs
		) { }
};