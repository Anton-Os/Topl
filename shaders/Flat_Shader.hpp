#include "Topl_Pipeline.hpp"

#define FLAT_MODE_SOLID 0 // for semi-transparent white
#define FLAT_MODE_ALTERNATE 1 // for alternating vertex colors
#define FLAT_MODE_DIRECTION 2 // for interpolating along xyz

#define FLAT_COLOR_INC 0.002f // value for generating color id

// Vertex Shaders

struct Shape_VertexShader : public Topl_EntryShader {
	Shape_VertexShader() : Topl_EntryShader(){}
	Shape_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Shape_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

	virtual void genRenderBlock(const Geo_Actor* const actor, unsigned renderID, blockBytes_t* bytes) const override {
		Vec4f color = genShapeColor(renderID);

		alignDataToBytes((uint8_t*)&color, sizeof(Vec4f), NO_PADDING, bytes);
		Topl_EntryShader::genRenderBlock(actor, renderID, bytes);
	}

	virtual void genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		appendDataToBytes((uint8_t*)&_mode, sizeof(unsigned), bytes);
		Topl_EntryShader::genSceneBlock(scene, camera, bytes);
	}
protected:
	Vec4f genShapeColor(unsigned renderID) const {
		unsigned colorID = genColorID(renderID);
		return Vec4f({ ((colorID & 0xFF0000) >> 16) / 255.0f, ((colorID & 0xFF00) >> 8) / 255.0f, (colorID & 0xFF) / 255.0f, _alphaVal });
	}

	unsigned _mode = FLAT_MODE_SOLID;
	float _alphaVal = 1.0f;
};

struct GL4_Shape_VertexShader : public Shape_VertexShader {
	GL4_Shape_VertexShader() : Shape_VertexShader(genPrefix_glsl() + "Shape_Vertex.glsl") {}
	GL4_Shape_VertexShader(unsigned mode) : Shape_VertexShader(genPrefix_glsl() + "Shape_Vertex.glsl", mode) {}
};

struct Drx11_Shape_VertexShader : public Shape_VertexShader {
	Drx11_Shape_VertexShader() : Shape_VertexShader(genPrefix_hlsl() + "Shape_Vertex.hlsl") {}
	Drx11_Shape_VertexShader(unsigned mode) : Shape_VertexShader(genPrefix_hlsl() + "Shape_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Shape_PixelShader : public Topl_Shader {
	Shape_PixelShader() : Topl_Shader(){} // Blank Constructor
	Shape_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name){ }
};

struct GL4_Shape_PixelShader : public Shape_PixelShader {
	GL4_Shape_PixelShader() : Shape_PixelShader(genPrefix_glsl() + "Shape_Frag.glsl") {}
};

struct Drx11_Shape_PixelShader : public Shape_PixelShader {
	Drx11_Shape_PixelShader() : Shape_PixelShader(genPrefix_hlsl() + "Shape_Pixel.hlsl") {}
};