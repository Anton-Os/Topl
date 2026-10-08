#include "Platform.hpp"

#include "Topl_Pipeline.hpp"

#define EFFECT_SIZE 10.0
#define EFFECT_ITER 15

#define EFFECT_MODES_MANDLEBROT 0
#define EFFECT_MODES_JULIA 10

// Vertex Shaders

struct Fractal_VertexShader : public Topl_EntryShader {
	Fractal_VertexShader() : Topl_EntryShader(){}
	Fractal_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Fractal_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

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

struct Fractal_VertexShader_GL4 : public Fractal_VertexShader {
    Fractal_VertexShader_GL4() : Fractal_VertexShader(std::string("legacy/fractal/glsl/") + "Fractal_Vertex.glsl"){}
	Fractal_VertexShader_GL4(unsigned mode) : Fractal_VertexShader(std::string("legacy/fractal/glsl/") + "Fractal_Vertex.glsl", mode){}
};

struct Fractal_VertexShader_DX11 : public Fractal_VertexShader {
    Fractal_VertexShader_DX11() : Fractal_VertexShader(std::string("legacy/fractal/hlsl/") + "Fractal_Vertex.hlsl"){}
	Fractal_VertexShader_DX11(unsigned mode) : Fractal_VertexShader(std::string("legacy/fractal/hlsl/") + "Fractal_Vertex.hlsl", mode){}
};

// Pixel Shaders

struct Fractal_PixelShader : public Topl_Shader {
	Fractal_PixelShader() : Topl_Shader(){}
	Fractal_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name) { }
};

struct Fractal_PixelShader_GL4 : public Fractal_PixelShader {
	Fractal_PixelShader_GL4() : Fractal_PixelShader(std::string("legacy/fractal/glsl/") + "Fractal_Frag.glsl") { }
};

struct Fractal_PixelShader_DX11 : public Fractal_PixelShader {
	Fractal_PixelShader_DX11() : Fractal_PixelShader(std::string("legacy/fractal/hlsl/") + "Fractal_Pixel.hlsl") { }
};
