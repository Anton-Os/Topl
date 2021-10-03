#include "Topl_Shader.hpp"

struct Volumes_VertexShader : public Topl_PrimaryShader {
	Volumes_VertexShader()
		: Topl_PrimaryShader(
			SHDR_Vertex, 
			genPrefix_hlsl() + "Volumes_Vertex.hlsl",
			{ 
				Shader_Type("pos", "POSITION", SHDR_float_vec3), 
				Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) 
			} // Inputs
		) {  }

	virtual bool genPerGeoDataBlock(const Geo_Actor* const component, std::vector<uint8_t>* bytes) const override {
		bytes->clear(); // Make sure there is no preexisting data

		const uint8_t* offset_bptr = reinterpret_cast<const uint8_t*>(component->getPos()->data());
		const uint8_t* rotation_bptr = reinterpret_cast<const uint8_t*>(component->getAngles()->data());
	
		ValueGen::appendDataToBytes(offset_bptr, component->getPos()->size() * sizeof(float), bytes);
		ValueGen::appendDataToBytes(rotation_bptr, component->getAngles()->size() * sizeof(float), bytes);
		return true;
	}

	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const {
		const uint8_t* cameraPos_bptr = reinterpret_cast<const uint8_t*>(scene->getCamera()->getPos()->data());
		const uint8_t* cameraRot_bptr = reinterpret_cast<const uint8_t*>(scene->getCamera()->getDirection()->data());
		const uint8_t* matrix_bptr = reinterpret_cast<const uint8_t*>(scene->getCamera()->getProjMatrix()->data());

		ValueGen::appendDataToBytes(cameraPos_bptr, scene->getCamera()->getPos()->size() * sizeof(float), bytes);
		ValueGen::appendDataToBytes(cameraRot_bptr, scene->getCamera()->getDirection()->size() * sizeof(float), bytes);
		ValueGen::appendDataToBytes(matrix_bptr, scene->getCamera()->getProjMatrix()->size() * sizeof(float), bytes);
		// ValueGen::assignDataToBytes(matrixBytes, scene->getCamera()->getProjMatrix()->size() * sizeof(float), bytes);
		return true;
	}
};