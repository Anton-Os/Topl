#include "Topl_Pipeline.hpp"

#define FLAT_MODE_SOLID 0 // for semi-transparent white
#define FLAT_MODE_ALTERNATE 1 // for alternating vertex colors
#define FLAT_MODE_DIRECTION 2 // for interpolating along xyz

#define FLAT_COLOR_INC 0.002f // value for generating color id

// Vertex Shaders

struct Flat_VertexShader : public Topl_EntryShader {
	Flat_VertexShader() : Topl_EntryShader(){}
	Flat_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Flat_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

	virtual void genRenderBlock(const Geo_Actor* const actor, blockBytes_t* bytes) const override {
		Vec4f color = getColor(actor);
		alignDataToBytes((uint8_t*)&color, sizeof(Vec4f), NO_PADDING, bytes);
		Topl_EntryShader::genRenderBlock(actor, bytes);
	}

	virtual void genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		appendDataToBytes((uint8_t*)&_mode, sizeof(unsigned), bytes);
		Topl_EntryShader::genSceneBlock(scene, camera, bytes);
	}
protected:
	Vec4f getColor(const Geo_Actor* const actor) const {
		unsigned colorID = actor->getId();
		return Vec4f({ ((colorID & 0xFF0000) >> 16) / 255.0f, ((colorID & 0xFF00) >> 8) / 255.0f, (colorID & 0xFF) / 255.0f, _alphaVal });
	}

	unsigned _mode = FLAT_MODE_SOLID;
	float _alphaVal = 1.0f;
};

struct Flat_VertexShader_GL4 : public Flat_VertexShader {
	Flat_VertexShader_GL4() : Flat_VertexShader(genPrefix_glsl() + "Flat_Vertex.glsl") {}
	Flat_VertexShader_GL4(unsigned mode) : Flat_VertexShader(genPrefix_glsl() + "Flat_Vertex.glsl", mode) {}
};

struct Flat_VertexShader_Drx11 : public Flat_VertexShader {
	Flat_VertexShader_Drx11() : Flat_VertexShader(genPrefix_hlsl() + "Flat_Vertex.hlsl") {}
	Flat_VertexShader_Drx11(unsigned mode) : Flat_VertexShader(genPrefix_hlsl() + "Flat_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Flat_PixelShader : public Topl_Shader {
	Flat_PixelShader() : Topl_Shader(){} // Blank Constructor
	Flat_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name){ }
};

struct Flat_PixelShader_GL4 : public Flat_PixelShader {
	Flat_PixelShader_GL4() : Flat_PixelShader(genPrefix_glsl() + "Flat_Frag.glsl") {}
};

struct Flat_PixelShader_Drx11 : public Flat_PixelShader {
	Flat_PixelShader_Drx11() : Flat_PixelShader(genPrefix_hlsl() + "Flat_Pixel.hlsl") {}
};