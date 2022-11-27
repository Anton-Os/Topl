#include "Topl_Shader_Pipeline.h"

#define EFFECT_MODE_CURSOR 0
#define EFFECT_MODE_SPECIAL 1

// Vertex Shaders

struct Effect_VertexShader : public Topl_EntryShader {
	Effect_VertexShader() : Topl_EntryShader(){}
	Effect_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Effect_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

	virtual void genRenderBlock(const Geo_Actor* const actor, unsigned renderID, blockBytes_t* bytes) const override {
		bytes->clear(); // make sure there is no preexisting data
		appendDataToBytes((uint8_t*)&renderID, sizeof(unsigned), bytes); // renderID
	}

	virtual void genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		bytes->clear(); // make sure there is no preexisting data

		Vec2i screenRes = Vec2i({ width, height });
		Vec2f cursorPos = Vec2f({ Platform::getCursorX(), Platform::getCursorY() });
	
		alignDataToBytes((uint8*)&screenRes.data[0], sizeof(screenRes), NO_PADDING, bytes);
		alignDataToBytes((uint8*)&cursorPos.data[0], sizeof(cursorPos), NO_PADDING, bytes);
		appendDataToBytes((uint8*)&_mode, sizeof(unsigned), bytes);
	}

	void setWidth(int w) { if(w > 0) width = w; }
	void setHeight(int h) { if(h > 0) height = h; }
protected:
	unsigned _mode = EFFECT_MODE_CURSOR;
	int width = TOPL_WIN_WIDTH;
	int height = TOPL_WIN_HEIGHT;
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