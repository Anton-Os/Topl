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

struct Color_VertexShader : public Topl_EntryShader {
	Color_VertexShader() : Topl_EntryShader(){}
	Color_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Color_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

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

struct Color_VertexShader_GL4 : public Topl_Shader_GL4, Color_VertexShader {
	Color_VertexShader_GL4() : Topl_Shader_GL4(), Color_VertexShader("color/glsl/Color_Vertex.glsl") {}
	Color_VertexShader_GL4(unsigned mode) : Topl_Shader_GL4(), Color_VertexShader("color/glsl/Color_Vertex.glsl", mode) {}
};

struct Color_VertexShader_DX11 : public Topl_Shader_DX11, Color_VertexShader {
	Color_VertexShader_DX11() : Topl_Shader_DX11(), Color_VertexShader("color/hlsl/Color_Vertex.hlsl") {}
	Color_VertexShader_DX11(unsigned mode) : Topl_Shader_DX11(), Color_VertexShader("color/hlsl/Color_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Color_PixelShader : public Topl_Shader {
	Color_PixelShader() : Topl_Shader(){} // Blank Constructor
	Color_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name){ }
};

struct Color_PixelShader_GL4 : public Topl_Shader_GL4, Color_PixelShader {
	Color_PixelShader_GL4() : Topl_Shader_GL4(), Color_PixelShader("color/glsl/Color_Frag.glsl") { /* std::cout << "embeddings size is " << std::to_string(_embedMap.size()) << std::endl; */ }
};

struct Color_PixelShader_DX11 : public Topl_Shader_DX11, Color_PixelShader {
	Color_PixelShader_DX11() : Topl_Shader_DX11(), Color_PixelShader("color/hlsl/Color_Pixel.hlsl"){ /* std::cout << "embeddings size is " << std::to_string(_embedMap.size()) << std::endl; */ }
};