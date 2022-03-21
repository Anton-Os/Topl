#include "Topl_Shader.h"

// Vertex Shaders

struct Layered_VertexShader : public Topl_EntryShader {
	Layered_VertexShader() : Topl_EntryShader(){}
	Layered_VertexShader(std::string name)
		: Topl_EntryShader(
			SHDR_Vertex, name,
			{ 
				Shader_Type("pos", "POSITION", SHDR_float_vec3), 
				Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) 
			} // Inputs
		) {  }

	virtual bool genGeoBlock(const Geo_Actor* const component, blockBytes_t* bytes) const override {
		bytes->clear(); // Make sure there is no preexisting data

		bytes_cptr offset_bytes = reinterpret_cast<bytes_cptr>(component->getPos()->data());
		bytes_cptr rotation_bytes = reinterpret_cast<bytes_cptr>(component->getAngles()->data());
	
		appendDataToBytes(offset_bytes, component->getPos()->size() * sizeof(float), bytes);
		appendDataToBytes(rotation_bytes, component->getAngles()->size() * sizeof(float), bytes);
		return true;
	}

	virtual bool genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		return false;
	}
};

struct GL4_Layered_VertexShader : public Layered_VertexShader {
	GL4_Layered_VertexShader() : Layered_VertexShader(genPrefix_glsl() + "Layered_Vertex.glsl") {}
};

struct Drx11_Layered_VertexShader : public Layered_VertexShader {
	Drx11_Layered_VertexShader() : Layered_VertexShader(genPrefix_hlsl() + "Layered_Vertex.hlsl") {}
};

// Fragment Shaders

struct Layered_FragmentShader : public Topl_Shader {
	Layered_FragmentShader() : Topl_Shader(){}
	Layered_FragmentShader(std::string name) : Topl_Shader(SHDR_Fragment, name) { }
};

struct GL4_Layered_FragmentShader : public Layered_FragmentShader {
	GL4_Layered_FragmentShader() : Layered_FragmentShader(genPrefix_glsl() + "Layered_Frag.glsl") {}
};

struct Drx11_Layered_FragmentShader : public Layered_FragmentShader {
	Drx11_Layered_FragmentShader() : Layered_FragmentShader(genPrefix_hlsl() + "Layered_Pixel.hlsl") { }
};