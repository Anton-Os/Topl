#include "Topl_Pipeline.hpp"

#define COLORED_ID 0
#define COLORED_DIRECTIONAL 1
#define COLORED_COORD 2
#define COLORED_VERTEX 3
#define COLORED_CAMERA 4
#define COLORED_ANGULAR 5
#define COLORED_TEXCOORD 6
#define COLORED_SECTIONED 7
#define COLORED_RANDOM 8
#define COLORED_TRIAL 9

// Vertex Shaders

struct Colorcode_VertexShader : public Topl_EntryShader {
	Colorcode_VertexShader() : Topl_EntryShader(){}
	Colorcode_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Colorcode_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

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

struct Colorcode_VertexShader_GL4 : public Colorcode_VertexShader {
	Colorcode_VertexShader_GL4() : Colorcode_VertexShader(genPrefix_glsl() + "colorcode/" + "Vertex.glsl") {}
	Colorcode_VertexShader_GL4(unsigned mode) : Colorcode_VertexShader(genPrefix_glsl() + "colorcode/" + "Vertex.glsl", mode) {}
};

struct Colorcode_VertexShader_DX11 : public Colorcode_VertexShader {
	Colorcode_VertexShader_DX11() : Colorcode_VertexShader(genPrefix_hlsl() + "colorcode/" + "Vertex.hlsl") {}
	Colorcode_VertexShader_DX11(unsigned mode) : Colorcode_VertexShader(genPrefix_hlsl() + "colorcode/" + "Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Colorcode_PixelShader : public Topl_Shader {
	Colorcode_PixelShader() : Topl_Shader(){} // Blank Constructor
	Colorcode_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name){ }
};

struct Colorcode_PixelShader_GL4 : public Colorcode_PixelShader {
	Colorcode_PixelShader_GL4() : Colorcode_PixelShader(genPrefix_glsl() + "colorcode/" + "Frag.glsl") {}
};

struct Colorcode_PixelShader_DX11 : public Colorcode_PixelShader {
	Colorcode_PixelShader_DX11() : Colorcode_PixelShader(genPrefix_hlsl() + "colorcode/" + "Pixel.hlsl") {}
};