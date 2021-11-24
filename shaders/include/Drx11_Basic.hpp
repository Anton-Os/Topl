#include "Topl_Shader.hpp"

struct Basic_VertexShader : public Topl_EntryShader {
	Basic_VertexShader()
		: Topl_EntryShader(
			SHDR_Vertex, 
			genPrefix_hlsl() + "Basic_Vertex.hlsl",
			{ 
				Shader_Type("pos", "POSITION", SHDR_float_vec3), 
				Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) 
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

struct Basic_PixelShader : public Topl_Shader {
	Basic_PixelShader()
		: Topl_Shader(
			SHDR_Fragment, 
			genPrefix_hlsl() + "Basic_Pixel.hlsl",
			{ 
				Shader_Type("pos", "POSITION", SHDR_float_vec3), 
				Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) 
			} // Inputs
		) { }
};
