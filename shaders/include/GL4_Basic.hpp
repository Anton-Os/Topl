#include "Topl_Shader.hpp"

struct Basic_VertexShader : public Topl_Shader {
	Basic_VertexShader(const char* filePath)
		: Topl_Shader(
			SHDR_Vertex, filePath,
			{ Shader_Type("pos", SHDR_float_vec3), 
            Shader_Type("texcoord", SHDR_float_vec2) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override {
		bytes->clear(); // Make sure there is no preexisting data

		const uint8_t* rotation_bptr = reinterpret_cast<const uint8_t*>(component->getAngles()->data());
		const uint8_t* offset_bptr = reinterpret_cast<const uint8_t*>(component->getPos()->data());

		ValueGen::appendDataToBytes(rotation_bptr, component->getAngles()->size() * sizeof(float), 2 * sizeof(float), bytes);
		ValueGen::appendDataToBytes(offset_bptr, component->getPos()->size() * sizeof(float), 1 * sizeof(float), bytes);
		return true;
	}

	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const {
		const uint8_t* matrixBytes = reinterpret_cast<const uint8_t*>(scene->getCamera()->getProjMatrix()->data());
		ValueGen::assignDataToBytes(matrixBytes, scene->getCamera()->getProjMatrix()->size() * sizeof(float), bytes);
		return true;
	}
};

struct Basic_FragmentShader : public Topl_Shader {
	Basic_FragmentShader(const char* filePath)
		: Topl_Shader(
			SHDR_Fragment, filePath,
			{ Shader_Type("texcoord", SHDR_float_vec2) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const override { return false; }                                                                       
};