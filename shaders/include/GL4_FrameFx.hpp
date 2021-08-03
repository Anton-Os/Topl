#include "Topl_Shader.hpp"

// see native_os_def.h
#define SCREEN_RES_X 1100
#define SCREEN_RES_Y 1200

struct FrameFx_VertexShader : public Topl_Shader {
	FrameFx_VertexShader(const char* filePath)
		: Topl_Shader(
			SHDR_Vertex, filePath,
			{ Shader_Type("pos", SHDR_float_vec3), Shader_Type("texcoord", SHDR_float_vec2) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override {
		bytes->clear(); // Make sure there is no preexisting data
		const unsigned renderId = component->getId();

		const uint8_t* renderId_bptr = reinterpret_cast<const uint8_t*>(&renderId);

		ValueGen::appendDataToBytes(renderId_bptr, sizeof(unsigned), 3 * sizeof(unsigned), bytes);
		return true;
	}

	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const {
		const Eigen::Vector2i screenRes = Eigen::Vector2i(SCREEN_RES_X, SCREEN_RES_Y);

		const uint8_t* screenRes_bptr = reinterpret_cast<const uint8_t*>(screenRes.data());
		// TODO: add support for cursorPos and renderMode!

		ValueGen::appendDataToBytes(screenRes_bptr, screenRes.size() * sizeof(int), 2 * sizeof(int), bytes);
		return true;
	}
};

struct FrameFx_FragmentShader : public Topl_Shader {
	FrameFx_FragmentShader(const char* filePath)
		: Topl_Shader(
			SHDR_Fragment, filePath,
			{ Shader_Type("screenRes", SHDR_uint_vec2), 
            Shader_Type("cursorPos", SHDR_uint_vec2) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const { return false; }
};