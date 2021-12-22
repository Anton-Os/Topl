#include "Topl_Shader.hpp"

struct Effect_VertexShader : public Topl_EntryShader {
	Effect_VertexShader(const Platform* platform)
		: Topl_EntryShader(
			platform, 
			SHDR_Vertex, 
			genPrefix_hlsl() + "Effect_Vertex.hlsl",
			{ 
				Shader_Type("pos", "POSITION", SHDR_float_vec3), 
            	Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) 
			} // Inputs
		) {  }

	virtual bool genGeoBlock(const Geo_Actor* const component, std::vector<uint8_t>* bytes) const override {
		bytes->clear(); // Make sure there is no preexisting data
		const unsigned renderId = component->getId();

		const uint8_t* renderId_bptr = reinterpret_cast<const uint8_t*>(&renderId);

		ValueGen::appendDataToBytes(renderId_bptr, sizeof(unsigned), bytes);
		return true;
	}

	virtual bool genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, std::vector<uint8_t>* bytes) const {
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

struct Effect_PixelShader : public Topl_Shader {
	Effect_PixelShader()
		: Topl_Shader(
			SHDR_Fragment,
			genPrefix_hlsl() + "Effect_Pixel.hlsl",
			{ 
				Shader_Type("screenRes", "RESOLUTION", SHDR_uint_vec2), 
				Shader_Type("cursorPos", "CURSOR", SHDR_float_vec2) 
			} // Inputs
		) { }
};