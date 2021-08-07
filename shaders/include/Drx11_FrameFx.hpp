#include "Topl_Shader.hpp"

// see native_os_def.h
#define SCREEN_RES_X 1100
#define SCREEN_RES_Y 1200

struct FrameFx_VertexShader : public Topl_Shader {
	FrameFx_VertexShader(const Platform* platform, const char* filePath)
		: Topl_Shader(
			platform, SHDR_Vertex, filePath,
			{ Shader_Type("pos", "POSITION", SHDR_float_vec3), 
            Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) } // Inputs
		) {  }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override {
		bytes->clear(); // Make sure there is no preexisting data
		const unsigned renderId = component->getId();

		const uint8_t* renderId_bptr = reinterpret_cast<const uint8_t*>(&renderId);

		ValueGen::appendDataToBytes(renderId_bptr, sizeof(unsigned), bytes);
		return true;
	}

	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const {
		const Eigen::Vector2i screenRes = Eigen::Vector2i(SCREEN_RES_X, SCREEN_RES_Y);
		Eigen::Vector2f cursorPos;
		bool isOnScreen = _platform_cptr->getCursorCoords(&cursorPos.x(), &cursorPos.y());

		const uint8_t* screenRes_bptr = reinterpret_cast<const uint8_t*>(screenRes.data());
		const uint8_t* cursorPos_bptr = reinterpret_cast<const uint8_t*>(cursorPos.data());

		ValueGen::appendDataToBytes(screenRes_bptr, screenRes.size() * sizeof(int), bytes);
		ValueGen::appendDataToBytes(cursorPos_bptr, cursorPos.size() * sizeof(float), bytes);
		return true;
	}
};

struct FrameFx_PixelShader : public Topl_Shader {
	FrameFx_PixelShader(const char* filePath)
		: Topl_Shader(
			SHDR_Fragment, filePath,
			{ Shader_Type("screenRes", "RESOLUTION", SHDR_uint_vec2), 
            Shader_Type("cursorPos", "CURSOR", SHDR_uint_vec2) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const { return false; }
};