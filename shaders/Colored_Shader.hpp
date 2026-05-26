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

struct Colored_VertexShader : public Topl_EntryShader {
	Colored_VertexShader() : Topl_EntryShader(){}
	Colored_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Colored_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

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

struct Colored_VertexShader_GL4 : public Colored_VertexShader {
	Colored_VertexShader_GL4() : Colored_VertexShader(genPrefix_glsl() + "colored/" + "Colored_Vertex.glsl") {}
	Colored_VertexShader_GL4(unsigned mode) : Colored_VertexShader(genPrefix_glsl() + "colored/" + "Colored_Vertex.glsl", mode) {}
};

struct Colored_VertexShader_DX11 : public Colored_VertexShader {
	Colored_VertexShader_DX11() : Colored_VertexShader(genPrefix_hlsl() + "colored/" + "Colored_Vertex.hlsl") {}
	Colored_VertexShader_DX11(unsigned mode) : Colored_VertexShader(genPrefix_hlsl() + "colored/" + "Colored_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Colored_PixelShader : public Topl_Shader {
	Colored_PixelShader() : Topl_Shader(){} // Blank Constructor
	Colored_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name){ }
};

struct Colored_PixelShader_GL4 : public Colored_PixelShader {
	Colored_PixelShader_GL4() : Colored_PixelShader(genPrefix_glsl() + "colored/" + "Colored_Frag.glsl") {}
};

struct Colored_PixelShader_DX11 : public Colored_PixelShader {
	Colored_PixelShader_DX11() : Colored_PixelShader(genPrefix_hlsl() + "colored/" + "Colored_Pixel.hlsl") {}
};