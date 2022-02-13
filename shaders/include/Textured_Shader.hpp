#include "Topl_Shader.hpp"

// Vertex Shaders

struct Textured_VertexShader : public Topl_EntryShader {
	Textured_VertexShader(std::string name)
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
	
		ValueGen::appendDataToBytes(offset_bytes, component->getPos()->size() * sizeof(float), bytes);
		ValueGen::appendDataToBytes(rotation_bytes, component->getAngles()->size() * sizeof(float), bytes);
		return true;
	}

	virtual bool genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		bytes_cptr matrixBytes = reinterpret_cast<bytes_cptr>(camera->getProjMatrix()->data());
		ValueGen::assignDataToBytes(matrixBytes, camera->getProjMatrix()->size() * sizeof(float), bytes);
		return true;
	}
};

struct GL4_Textured_VertexShader : public Textured_VertexShader {
	GL4_Textured_VertexShader() : Textured_VertexShader(genPrefix_glsl() + "Textured_Vertex.glsl") {}
};

struct Drx11_Textured_VertexShader : public Textured_VertexShader {
	Drx11_Textured_VertexShader() : Textured_VertexShader(genPrefix_hlsl() + "Textured_Vertex.hlsl") {}
};

// Fragment Shaders

struct Textured_FragmentShader : public Topl_Shader {
	Textured_FragmentShader(std::string name) : Topl_Shader(SHDR_Fragment, name) { }
};

struct GL4_Textured_FragmentShader : public Textured_FragmentShader {
	GL4_Textured_FragmentShader() : Textured_FragmentShader(genPrefix_glsl() + "Textured_Frag.glsl") {}
};

struct Drx11_Textured_FragmentShader : public Textured_FragmentShader {
	Drx11_Textured_FragmentShader() : Textured_FragmentShader(genPrefix_hlsl() + "Textured_Pixel.hlsl") { }
};