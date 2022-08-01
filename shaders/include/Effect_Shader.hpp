#include "Topl_Shader_Pipeline.h"

#define EFFECT_MODE_CURSOR 0
#define EFFECT_MODE_FRACTAL 1

// Vertex Shaders

struct Effect_VertexShader : public Topl_EntryShader {
	Effect_VertexShader() : Topl_EntryShader(){}
	Effect_VertexShader(std::string name)
		: Topl_EntryShader(
			name,
			{ 
				Shader_Type("pos", "POSITION", SHDR_float_vec3), 
            	Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) 
			} // Inputs
		) { }
	
	Effect_VertexShader(std::string name, unsigned mode)
		: Topl_EntryShader(
			name,
			{ 
				Shader_Type("pos", "POSITION", SHDR_float_vec3), 
            	Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) 
			} // Inputs
		) { _mode = mode; }

	virtual bool genGeoBlock(const Geo_Actor* const actor, blockBytes_t* bytes) const override {
		return true; // make this false
	}

	virtual bool genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		bytes->clear(); // make sure there is no preexisting data

		Vec2i screenRes = Vec2i({ TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT });
		Vec2f cursorPos = Vec2f((Vec2f){ Platform::getCursorX(), Platform::getCursorY() });

		bytes_cptr screenRes_bytes = reinterpret_cast<bytes_cptr>(&screenRes.data[0]);
		bytes_cptr cursorPos_bytes = reinterpret_cast<bytes_cptr>(&cursorPos.data[0]);
		bytes_cptr mode_bytes = reinterpret_cast<bytes_cptr>(&_mode);
	
		alignDataToBytes(screenRes_bytes, sizeof(screenRes), NO_PADDING, bytes);
		alignDataToBytes(cursorPos_bytes, sizeof(cursorPos), NO_PADDING, bytes);
		appendDataToBytes(mode_bytes, sizeof(unsigned), bytes);

		return true;
	}
protected:
	unsigned _mode = EFFECT_MODE_CURSOR;
};

struct GL4_Effect_VertexShader : public Effect_VertexShader {
    GL4_Effect_VertexShader() : Effect_VertexShader(genPrefix_glsl() + "Effect_Vertex.glsl"){}
	GL4_Effect_VertexShader(unsigned mode) : Effect_VertexShader(genPrefix_glsl() + "Effect_Vertex.glsl", mode){}
};

struct Drx11_Effect_VertexShader : public Effect_VertexShader {
    Drx11_Effect_VertexShader() : Effect_VertexShader(genPrefix_hlsl() + "Effect_Vertex.hlsl"){}
	Drx11_Effect_VertexShader(unsigned mode) : Effect_VertexShader(genPrefix_hlsl() + "Effect_Vertex.hlsl", mode){}
};

// Fragment Shaders

struct Effect_FragmentShader : public Topl_Shader {
	Effect_FragmentShader() : Topl_Shader(){}
	Effect_FragmentShader(std::string name) : Topl_Shader(SHDR_Fragment, name) { }
};

struct GL4_Effect_FragmentShader : public Effect_FragmentShader {
	GL4_Effect_FragmentShader() : Effect_FragmentShader(genPrefix_glsl() + "Effect_Frag.glsl") { }
};

struct Drx11_Effect_FragmentShader : public Effect_FragmentShader {
	Drx11_Effect_FragmentShader() : Effect_FragmentShader(genPrefix_hlsl() + "Effect_Pixel.hlsl") { }
};