#include "Topl_Shader.hpp"

#define EFFECT_MODE_DEFAULT 0

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

		bytes_cptr renderId_bytes = reinterpret_cast<bytes_cptr>(&renderId);

		ValueGen::appendDataToBytes(renderId_bytes, sizeof(unsigned), bytes);
		return true;
	}

	virtual bool genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		const Eigen::Vector2i screenRes = Eigen::Vector2i(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);
		Eigen::Vector2f cursorPos = Eigen::Vector2f(Platform::getCursorX(), Platform::getCursorY());

		bytes_cptr screenRes_bytes = reinterpret_cast<bytes_cptr>(screenRes.data());
		bytes_cptr cursorPos_bytes = reinterpret_cast<bytes_cptr>(cursorPos.data());

		ValueGen::appendDataToBytes(screenRes_bytes, screenRes.size() * sizeof(int), 0, bytes);
		ValueGen::appendDataToBytes(cursorPos_bytes, cursorPos.size() * sizeof(float), 0, bytes);
		return true;
	}
protected:
	unsigned renderMode = EFFECT_MODE_DEFAULT;
};

struct GL4_Effect_VertexShader : public Effect_VertexShader {
    GL4_Effect_VertexShader() : Effect_VertexShader(genPrefix_glsl() + "Effect_Vertex.glsl"){}
	GL4_Effect_VertexShader(unsigned rm) : Effect_VertexShader(genPrefix_glsl() + "Effect_Vertex.glsl"){
		renderMode = rm;
	}
};

struct Drx11_Effect_VertexShader : public Effect_VertexShader {
    Drx11_Effect_VertexShader() : Effect_VertexShader(genPrefix_hlsl() + "Effect_Vertex.hlsl"){}
	Drx11_Effect_VertexShader(unsigned rm) : Effect_VertexShader(genPrefix_hlsl() + "Effect_Vertex.hlsl"){
		renderMode = rm;
	}
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