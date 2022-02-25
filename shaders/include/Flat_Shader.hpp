#include "Topl_Shader.hpp"

// Vertex Shaders

struct Flat_VertexShader : public Topl_EntryShader {
	Flat_VertexShader(std::string name)
		: Topl_EntryShader(
			SHDR_Vertex, name,
			{ 
				Shader_Type("pos", "POSITION", SHDR_float_vec3), 
				Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) 
			} // Inputs
		) { }

	virtual bool genGeoBlock(const Geo_Actor* const component, blockBytes_t* bytes) const override {
		bytes->clear(); // Make sure there is no preexisting data

		bytes_cptr offset_bytes = reinterpret_cast<bytes_cptr>(component->getPos()->data());
		bytes_cptr rotation_bytes = reinterpret_cast<bytes_cptr>(component->getAngles()->data());
		Eigen::Vector4f color = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 0.8f);
		bytes_cptr color_bytes = reinterpret_cast<bytes_cptr>(color.data());
	
		appendDataToBytes(offset_bytes, component->getPos()->size() * sizeof(float), bytes);
		appendDataToBytes(rotation_bytes, component->getAngles()->size() * sizeof(float), bytes);
		appendDataToBytes(color_bytes, color.size() * sizeof(float), bytes);
		
		return true;
	}

	virtual bool genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		bytes_cptr cameraPos_bytes = reinterpret_cast<bytes_cptr>(camera->getPos()->data());
		bytes_cptr cameraRot_bytes = reinterpret_cast<bytes_cptr>(camera->getLookPos()->data());
		bytes_cptr matrix_bytes = reinterpret_cast<bytes_cptr>(camera->getProjMatrix()->data());

		appendDataToBytes(cameraPos_bytes, camera->getPos()->size() * sizeof(float), bytes);
		appendDataToBytes(cameraRot_bytes, camera->getLookPos()->size() * sizeof(float), bytes);
		appendDataToBytes(matrix_bytes, camera->getProjMatrix()->size() * sizeof(float), bytes);
		return true;
	}
};

struct GL4_Flat_VertexShader : public Flat_VertexShader {
	GL4_Flat_VertexShader() : Flat_VertexShader(genPrefix_glsl() + "Flat_Vertex.glsl") {}
};

struct Drx11_Flat_VertexShader : public Flat_VertexShader {
	Drx11_Flat_VertexShader() : Flat_VertexShader(genPrefix_hlsl() + "Flat_Vertex.hlsl") {}
};

// Fragment Shaders

struct Flat_FragmentShader : public Topl_Shader {
	Flat_FragmentShader(std::string name) : Topl_Shader(SHDR_Fragment, name){ }
};

struct GL4_Flat_FragmentShader : public Flat_FragmentShader {
	GL4_Flat_FragmentShader() : Flat_FragmentShader(genPrefix_glsl() + "Flat_Frag.glsl") {}
};

struct Drx11_Flat_FragmentShader : public Flat_FragmentShader {
	Drx11_Flat_FragmentShader() : Flat_FragmentShader(genPrefix_hlsl() + "Flat_Pixel.hlsl") {}
};