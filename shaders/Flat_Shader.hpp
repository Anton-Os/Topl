#include "Topl_Pipeline.hpp"

#define FLAT_ID 0
#define FLAT_DIRECTIONAL 1
#define FLAT_COORD 2
#define FLAT_VERTEX 3
#define FLAT_CAMERA 4
#define FLAT_ANGULAR 5
#define FLAT_TEXCOORD 6
#define FLAT_SECTIONED 7
#define FLAT_RANDOM 8
#define FLAT_TRIAL 9

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
protected:
	Vec4f getColor(const Geo_Actor* const actor) const {
		unsigned colorID = actor->getId();
		return Vec4f({ ((colorID & 0xFF0000) >> 16) / 255.0f, ((colorID & 0xFF00) >> 8) / 255.0f, (colorID & 0xFF) / 255.0f, _alphaVal });
	}

	float _alphaVal = 1.0f;
};

struct Flat_VertexShader_GL4 : public Flat_VertexShader {
	Flat_VertexShader_GL4() : Flat_VertexShader(genPrefix_glsl() + "vertex/" + "Flat_Vertex.glsl") {}
	Flat_VertexShader_GL4(unsigned mode) : Flat_VertexShader(genPrefix_glsl() + "vertex/" + "Flat_Vertex.glsl", mode) {}
};

struct Flat_VertexShader_DX11 : public Flat_VertexShader {
	Flat_VertexShader_DX11() : Flat_VertexShader(genPrefix_hlsl() + "vertex/" + "Flat_Vertex.hlsl") {}
	Flat_VertexShader_DX11(unsigned mode) : Flat_VertexShader(genPrefix_hlsl() + "vertex/" + "Flat_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Flat_PixelShader : public Topl_Shader {
	Flat_PixelShader() : Topl_Shader(){} // Blank Constructor
	Flat_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name){ }
};

struct Flat_PixelShader_GL4 : public Flat_PixelShader {
	Flat_PixelShader_GL4() : Flat_PixelShader(genPrefix_glsl() + "pixel/" + "Flat_Frag.glsl") {}
};

struct Flat_PixelShader_DX11 : public Flat_PixelShader {
	Flat_PixelShader_DX11() : Flat_PixelShader(genPrefix_hlsl() + "pixel/" + "Flat_Pixel.hlsl") {}
};