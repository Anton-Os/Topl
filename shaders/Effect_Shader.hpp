#include "Topl_Pipeline.hpp"

#define EFFECT_MODE_CURSOR 0
#define EFFECT_MODE_FRACTAL 1

// Vertex Shaders

struct Effect_VertexShader : public Topl_EntryShader {
	Effect_VertexShader() : Topl_EntryShader(){}
	Effect_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Effect_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

	virtual void genRenderBlock(const Geo_Actor* const actor, blockBytes_t* bytes) const override {
		appendDataToBytes((uint8_t*)&renderID, sizeof(unsigned), bytes); // renderID
	}

	virtual void genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		Vec2i screenRes = Vec2i({ width, height });
		Vec2f cursorPos = Vec2f({ Platform::getCursorX(), Platform::getCursorY() });
	
		alignDataToBytes((uint8_t*)&screenRes.data[0], sizeof(screenRes), NO_PADDING, bytes);
		alignDataToBytes((uint8_t*)&cursorPos.data[0], sizeof(cursorPos), NO_PADDING, bytes);
		appendDataToBytes((uint8_t*)&_mode, sizeof(unsigned), bytes);
	}

	void setWidth(int w) { if(w > 0) width = w; }
	void setHeight(int h) { if(h > 0) height = h; }
protected:
	unsigned _mode = EFFECT_MODE_CURSOR;
	int width = TOPL_WIN_WIDTH;
	int height = TOPL_WIN_HEIGHT;
};

struct Effect_VertexShader_GL4 : public Effect_VertexShader {
    Effect_VertexShader_GL4() : Effect_VertexShader(genPrefix_glsl() + "Effect_Vertex.glsl"){}
	Effect_VertexShader_GL4(unsigned mode) : Effect_VertexShader(genPrefix_glsl() + "Effect_Vertex.glsl", mode){}
};

struct Effect_VertexShader_Drx11 : public Effect_VertexShader {
    Effect_VertexShader_Drx11() : Effect_VertexShader(genPrefix_hlsl() + "Effect_Vertex.hlsl"){}
	Effect_VertexShader_Drx11(unsigned mode) : Effect_VertexShader(genPrefix_hlsl() + "Effect_Vertex.hlsl", mode){}
};

// Pixel Shaders

struct Effect_PixelShader : public Topl_Shader {
	Effect_PixelShader() : Topl_Shader(){}
	Effect_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name) { }
};

struct Effect_PixelShader_GL4 : public Effect_PixelShader {
	Effect_PixelShader_GL4() : Effect_PixelShader(genPrefix_glsl() + "Effect_Frag.glsl") { }
};

struct Effect_PixelShader_Drx11 : public Effect_PixelShader {
	Effect_PixelShader_Drx11() : Effect_PixelShader(genPrefix_hlsl() + "Effect_Pixel.hlsl") { }
};