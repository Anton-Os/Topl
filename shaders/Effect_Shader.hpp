#include "Platform.hpp"

#include "Topl_Pipeline.hpp"

#define EFFECT_SIZE 10.0
#define EFFECT_ITER 15

#define EFFECT_MODES_MANDLEBROT 0
#define EFFECT_MODES_JULIA 10

// Vertex Shaders

struct Effect_VertexShader : public Topl_EntryShader {
	Effect_VertexShader() : Topl_EntryShader(){}
	Effect_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Effect_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

	void genActorBlock(const Geo_Actor* const actor, blockBytes_t* bytes) const override {
		Topl_EntryShader::genActorBlock(actor, bytes);
	}

	void genSceneBlock(const Topl_Scene* const scene, blockBytes_t* bytes) const override {
		Vec2i screenRes = Vec2i({ width, height });
		Vec2f cursorPos = Vec2f({ Platform::getCursorX(), Platform::getCursorY() });
		Topl_EntryShader::genSceneBlock(scene, bytes);
		alignDataToBytes((uint8_t*)&screenRes.data[0], sizeof(screenRes), NO_PADDING, bytes);
		alignDataToBytes((uint8_t*)&cursorPos.data[0], sizeof(cursorPos), NO_PADDING, bytes);
        alignDataToBytes((uint8_t*)&effectSize, sizeof(effectSize), NO_PADDING, bytes);
        alignDataToBytes((uint8_t*)&effectIters, sizeof(effectIters), sizeof(unsigned) * 2, bytes); // TODO: See if you can remove padding!
	}

	void setWidth(int w) { if(w > 0) width = w; }
	void setHeight(int h) { if(h > 0) height = h; }

    void setEffect(float s, unsigned i){
        effectSize = s;
        effectIters = i;
    }
protected:
	int width = TOPL_WIN_WIDTH;
	int height = TOPL_WIN_HEIGHT;

    float effectSize = EFFECT_SIZE;
    unsigned effectIters = EFFECT_ITER;
};

struct Effect_VertexShader_GL4 : public Effect_VertexShader {
    Effect_VertexShader_GL4() : Effect_VertexShader(genPrefix_glsl() + "vertex/" + "Effect_Vertex.glsl"){}
	Effect_VertexShader_GL4(unsigned mode) : Effect_VertexShader(genPrefix_glsl() + "vertex/" + "Effect_Vertex.glsl", mode){}
};

struct Effect_VertexShader_DX11 : public Effect_VertexShader {
    Effect_VertexShader_DX11() : Effect_VertexShader(genPrefix_hlsl() + "vertex/" + "Effect_Vertex.hlsl"){}
	Effect_VertexShader_DX11(unsigned mode) : Effect_VertexShader(genPrefix_hlsl() + "vertex/" + "Effect_Vertex.hlsl", mode){}
};

// Pixel Shaders

struct Effect_PixelShader : public Topl_Shader {
	Effect_PixelShader() : Topl_Shader(){}
	Effect_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name) { }
};

struct Effect_PixelShader_GL4 : public Effect_PixelShader {
	Effect_PixelShader_GL4() : Effect_PixelShader(genPrefix_glsl() + "pixel/" + "Effect_Frag.glsl") { }
};

struct Effect_PixelShader_DX11 : public Effect_PixelShader {
	Effect_PixelShader_DX11() : Effect_PixelShader(genPrefix_hlsl() + "pixel/" + "Effect_Pixel.hlsl") { }
};
