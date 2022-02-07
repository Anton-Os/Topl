#include "Topl_Shader.hpp"

// Vertex Shaders

struct Effect_VertexShader : public Topl_EntryShader {
	Effect_VertexShader(std::string name)
		: Topl_EntryShader(
			SHDR_Vertex, name,
			{ 
				Shader_Type("pos", "POSITION", SHDR_float_vec3), 
            	Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) 
			} // Inputs
		) { }

	virtual bool genGeoBlock(const Geo_Actor* const component, blockBytes_t* bytes) const override {
		bytes->clear(); // Make sure there is no preexisting data
		const unsigned renderId = component->getId();

		const uint8_t* renderId_bptr = reinterpret_cast<const uint8_t*>(&renderId);

		ValueGen::appendDataToBytes(renderId_bptr, sizeof(unsigned), bytes);
		return true;
	}

	virtual bool genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		const Eigen::Vector2i screenRes = Eigen::Vector2i(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);
		Eigen::Vector2f cursorPos = Eigen::Vector2f(Platform::getCursorX(), Platform::getCursorY());

		const uint8_t* screenRes_bptr = reinterpret_cast<const uint8_t*>(screenRes.data());
		const uint8_t* cursorPos_bptr = reinterpret_cast<const uint8_t*>(cursorPos.data());

		ValueGen::appendDataToBytes(screenRes_bptr, screenRes.size() * sizeof(int), 0, bytes);
		ValueGen::appendDataToBytes(cursorPos_bptr, cursorPos.size() * sizeof(float), 0, bytes);
		return true;
	}
};

struct GL4_Effect_VertexShader : public Effect_VertexShader {
    GL4_Effect_VertexShader() : Effect_VertexShader(genPrefix_glsl() + "Effect_Vertex.glsl"){}
};

struct Drx11_Effect_VertexShader : public Effect_VertexShader {
    Drx11_Effect_VertexShader() : Effect_VertexShader(genPrefix_hlsl() + "Effect_Vertex.hlsl"){}
};

// Fragment Shaders

struct Effect_FragmentShader : public Topl_Shader {
	Effect_FragmentShader(std::string name) : Topl_Shader(SHDR_Fragment, name) { }
};

struct GL4_Effect_FragmentShader : public Effect_FragmentShader {
	GL4_Effect_FragmentShader() : Effect_FragmentShader(genPrefix_glsl() + "Effect_Frag.glsl") { }
};

struct Drx11_Effect_FragmentShader : public Effect_FragmentShader {
	Drx11_Effect_FragmentShader() : Effect_FragmentShader(genPrefix_hlsl() + "Effect_Pixel.hlsl") { }
};