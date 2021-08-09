#include "Topl_Shader.hpp"

struct FrameFx_VertexShader : public Topl_Shader {
	FrameFx_VertexShader(const Platform* platform, const char* filePath)
		: Topl_Shader(
			platform, SHDR_Vertex, filePath,
			{ Shader_Type("pos", SHDR_float_vec3), Shader_Type("texcoord", SHDR_float_vec2) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override {
		bytes->clear(); // Make sure there is no preexisting data
		const unsigned renderId = component->getId();

		const uint8_t* renderId_bptr = reinterpret_cast<const uint8_t*>(&renderId);

		ValueGen::appendDataToBytes(renderId_bptr, sizeof(unsigned), bytes);
		return true;
	}

	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const {
		const Eigen::Vector2i screenRes = Eigen::Vector2i(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);
		Eigen::Vector2f cursorPos;
		bool isOnScreen = _platform_cptr->getCursorCoords(&cursorPos.x(), &cursorPos.y());
		if (!isOnScreen) cursorPos = Eigen::Vector2f(0.0f, 0.0f);

		const uint8_t* screenRes_bptr = reinterpret_cast<const uint8_t*>(screenRes.data());
		const uint8_t* cursorPos_bptr = reinterpret_cast<const uint8_t*>(cursorPos.data());

		ValueGen::appendDataToBytes(screenRes_bptr, screenRes.size() * sizeof(int), 0, bytes);
		ValueGen::appendDataToBytes(cursorPos_bptr, cursorPos.size() * sizeof(float), 0, bytes);
		return true;
	}
};

struct FrameFx_FragmentShader : public Topl_Shader {
	FrameFx_FragmentShader(const char* filePath)
		: Topl_Shader(
			SHDR_Fragment, filePath,
			{ Shader_Type("screenRes", SHDR_uint_vec2), 
            Shader_Type("cursorPos", SHDR_float_vec2) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const { return false; }
};