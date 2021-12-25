#include "Topl_Shader.hpp"

struct Textured_VertexShader : public Topl_EntryShader {
	Textured_VertexShader()
		: Topl_EntryShader(
			SHDR_Vertex, 
			genPrefix_glsl() + "Textured_Vertex.glsl",
			{ 
				Shader_Type("pos", SHDR_float_vec3), 
				Shader_Type("texcoord", SHDR_float_vec2) 
			} // Inputs
		) { }

	virtual bool genGeoBlock(const Geo_Actor* const component, std::vector<uint8_t>* bytes) const override {
		bytes->clear(); // Make sure there is no preexisting data

		const uint8_t* rotation_bptr = reinterpret_cast<const uint8_t*>(component->getAngles()->data());
		const uint8_t* offset_bptr = reinterpret_cast<const uint8_t*>(component->getPos()->data());

		ValueGen::appendDataToBytes(rotation_bptr, component->getAngles()->size() * sizeof(float), bytes);
		ValueGen::appendDataToBytes(offset_bptr, component->getPos()->size() * sizeof(float), bytes);
		return true;
	}

	virtual bool genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, std::vector<uint8_t>* bytes) const {
		const uint8_t* matrixBytes = reinterpret_cast<const uint8_t*>(camera->getProjMatrix()->data());
		ValueGen::assignDataToBytes(matrixBytes, camera->getProjMatrix()->size() * sizeof(float), bytes);
		return true;
	}
};

struct Textured_FragmentShader : public Topl_Shader {
	Textured_FragmentShader()
		: Topl_Shader(
			SHDR_Fragment,
			genPrefix_glsl() + "Textured_Frag.glsl",
			{ Shader_Type("texcoord", SHDR_float_vec2) } // Inputs
		) { }                                                                      
};