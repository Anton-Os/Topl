#include "Topl_Pipeline.hpp"

#define MAX_FLAT_MODE 3

#define FLAT_MODE_SOLID 0 // default color from id
#define FLAT_MODE_DIRECTION 1 // determines position on screen
#define FLAT_MODE_COORD 2 // determines coordiates relative to object

// Vertex Shaders

struct Flat_VertexShader : public Topl_EntryShader {
	Flat_VertexShader() : Topl_EntryShader(){}
	Flat_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Flat_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

	void genActorBlock(const Geo_Actor* const actor, blockBytes_t* bytes) const override {
		Vec4f color = getColor(actor);
		alignDataToBytes((uint8_t*)&color, sizeof(Vec4f), NO_PADDING, bytes);
		Topl_EntryShader::genActorBlock(actor, bytes);
	}

	// void setMode(unsigned short m){ if(m < 3) _mode = m; }
protected:
	Vec4f getColor(const Geo_Actor* const actor) const {
		unsigned colorID = actor->getId();
		return Vec4f({ ((colorID & 0xFF0000) >> 16) / 255.0f, ((colorID & 0xFF00) >> 8) / 255.0f, (colorID & 0xFF) / 255.0f, _alphaVal });
	}

	float _alphaVal = 1.0f;
};

struct Flat_VertexShader_GL4 : public Flat_VertexShader {
	Flat_VertexShader_GL4() : Flat_VertexShader(genPrefix_glsl() + "Flat_Vertex.glsl") {}
	Flat_VertexShader_GL4(unsigned mode) : Flat_VertexShader(genPrefix_glsl() + "Flat_Vertex.glsl", mode) {}
};

struct Flat_VertexShader_DX11 : public Flat_VertexShader {
	Flat_VertexShader_DX11() : Flat_VertexShader(genPrefix_hlsl() + "Flat_Vertex.hlsl") {}
	Flat_VertexShader_DX11(unsigned mode) : Flat_VertexShader(genPrefix_hlsl() + "Flat_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Flat_PixelShader : public Topl_Shader {
	Flat_PixelShader() : Topl_Shader(){} // Blank Constructor
	Flat_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name){ }
};

struct Flat_PixelShader_GL4 : public Flat_PixelShader {
	Flat_PixelShader_GL4() : Flat_PixelShader(genPrefix_glsl() + "Flat_Frag.glsl") {}
};

struct Flat_PixelShader_DX11 : public Flat_PixelShader {
	Flat_PixelShader_DX11() : Flat_PixelShader(genPrefix_hlsl() + "Flat_Pixel.hlsl") {}
};