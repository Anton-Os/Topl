#include "Topl_Shader.hpp"

struct Flat_FragmentShader : public Topl_Shader {
	Flat_FragmentShader()
		: Topl_Shader(
			SHDR_Fragment,
			genPrefix_glsl() + "Flat_Frag.glsl",
			{ Shader_Type("flatColor", SHDR_float) } // Inputs
		) { }
};