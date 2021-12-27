#include "Topl_Shader.hpp"

// Vertex Shaders

struct Textured_VertexShader : public Topl_EntryShader {
	Textured_VertexShader(std::string name)
		: Topl_EntryShader(
			SHDR_Vertex, name,
			{ 
				Shader_Type("pos", SHDR_float_vec3), 
				Shader_Type("texcoord", SHDR_float_vec2) 
			} // Inputs
		) {  }

	virtual bool genGeoBlock(const Geo_Actor* const component, std::vector<uint8_t>* bytes) const override {
		bytes->clear(); // Make sure there is no preexisting data

		const uint8_t* offset_bptr = reinterpret_cast<const uint8_t*>(component->getPos()->data());
		const uint8_t* rotation_bptr = reinterpret_cast<const uint8_t*>(component->getAngles()->data());
	
		ValueGen::appendDataToBytes(offset_bptr, component->getPos()->size() * sizeof(float), bytes);
		ValueGen::appendDataToBytes(rotation_bptr, component->getAngles()->size() * sizeof(float), bytes);
		return true;
	}

	virtual bool genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, std::vector<uint8_t>* bytes) const {
		const uint8_t* matrixBytes = reinterpret_cast<const uint8_t*>(camera->getProjMatrix()->data());
		ValueGen::assignDataToBytes(matrixBytes, camera->getProjMatrix()->size() * sizeof(float), bytes);
		return true;
	}
};

struct GL4_Textured_VertexShader : public Textured_VertexShader {
    GL4_Textured_VertexShader() : Textured_VertexShader(genPrefix_glsl() + "Textured_Vertex.glsl"){}
}

struct Drx11_Textured_VertexShader : public Textured_VertexShader {
    Drx11_Textured_VertexShader() : Textured_VertexShader(genPrefix_hlsl() + "Textured_Vertex.hlsl"){}
}

// Fragment Shaders

struct Textured_FragmentShader : public Topl_Shader {
	Textured_FragmentShader(std::string name) : Topl_Shader(SHDR_Fragment, name) { }
};

struct GL4_Textured_FragmentShader : public Textured_FragmentShader {
    GL4_Textured_FragmentShader() : Textured_FragmentShader(genPrefix_glsl() + "Textured_Frag.glsl"){}
}

struct Drx11_Textured_FragmentShader : public Textured_FragmentShader {
	Drx11_Textured_FragmentShader() : Textured_FragmentShader(genPrefix_hlsl() + "Textured_Pixel.hlsl") { }
};