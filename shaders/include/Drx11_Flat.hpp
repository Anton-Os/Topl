#include "Topl_Shader.hpp"

struct Flat_VertexShader : public Topl_EntryShader {
	Flat_VertexShader()
		: Topl_EntryShader(
			SHDR_Vertex, 
			genPrefix_hlsl() + "Flat_Vertex.hlsl",
			{ 
				Shader_Type("pos", "POSITION", SHDR_float_vec3), 
				Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) 
			} // Inputs
		) {  }

	virtual bool genGeoBlock(const Geo_Actor* const component, std::vector<uint8_t>* bytes) const override {
		bytes->clear(); // Make sure there is no preexisting data

		const uint8_t* offset_bptr = reinterpret_cast<const uint8_t*>(component->getPos()->data());
		const uint8_t* rotation_bptr = reinterpret_cast<const uint8_t*>(component->getAngles()->data());
		Eigen::Vector4f color = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 0.8f);
		const uint8_t* color_bptr = reinterpret_cast<const uint8_t*>(color.data());
	
		ValueGen::appendDataToBytes(offset_bptr, component->getPos()->size() * sizeof(float), bytes);
		ValueGen::appendDataToBytes(rotation_bptr, component->getAngles()->size() * sizeof(float), bytes);
		ValueGen::appendDataToBytes(color_bptr, color.size() * sizeof(float), bytes);
		return true;
	}

	virtual bool genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, std::vector<uint8_t>* bytes) const {
		const uint8_t* cameraPos_bptr = reinterpret_cast<const uint8_t*>(camera->getPos()->data());
		const uint8_t* cameraRot_bptr = reinterpret_cast<const uint8_t*>(camera->getDirection()->data());
		const uint8_t* matrix_bptr = reinterpret_cast<const uint8_t*>(camera->getProjMatrix()->data());
		// const uint8_t* lightSrc_bptr = reinterpret_cast<const uint8_t*>(scene->getLightSource(0)->pos->data());

		ValueGen::appendDataToBytes(cameraPos_bptr, camera->getPos()->size() * sizeof(float), bytes);
		ValueGen::appendDataToBytes(cameraRot_bptr, camera->getDirection()->size() * sizeof(float), bytes);
		ValueGen::appendDataToBytes(matrix_bptr, camera->getProjMatrix()->size() * sizeof(float), bytes);
		// ValueGen::appendDataToBytes(lightSrc_bptr, 4 * sizeof(float), bytes);

		return true;
	}
};

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