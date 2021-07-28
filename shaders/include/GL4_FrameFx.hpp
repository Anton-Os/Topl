#include "Topl_Shader.hpp"

struct FrameFx_VertexShader : public Topl_Shader {
	FrameFx_VertexShader(const char* filePath)
		: Topl_Shader(
			SHDR_Vertex, filePath,
			{ Shader_Type("pos", SHDR_float_vec3), Shader_Type("texcoord", SHDR_float_vec2) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override {
		bytes->clear(); // Make sure there is no preexisting data

		const uint8_t* renderId_bptr = reinterpret_cast<const uint8_t*>(&component->getId());

		ValueGen::appendDataToBytes(renderId_bptr, sizeof(unsigned), 0, bytes);
		return true;
	}

	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const {
		// Send window width and cursor position here!

		return false;
	}
};

struct FrameFx_FragmentShader : public Topl_Shader {
	FragmentShader(const char* filePath)
		: Topl_Shader(
			SHDR_Fragment, filePath,
			{ Shader_Type("screenRes", SHDR_uint_vec2), 
            Shader_Type("cursorPos", SHDR_uint_vec2) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const { return false; }
};