#include "Topl_Shader_Pipeline.h"

#define FLAT_MODE_SOLID 0 // for semi-transparent white
#define FLAT_MODE_ALTERNATE 1 // for alternating vertex colors

#define FLAT_COLOR_INC 0.002f // value for generating color id

// Vertex Shaders

struct Flat_VertexShader : public Topl_EntryShader {
	Flat_VertexShader() : Topl_EntryShader(){}
	Flat_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Flat_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

	virtual void genRenderBlock(const Geo_Actor* const actor, unsigned renderID, blockBytes_t* bytes) const override {
		bytes->clear(); // make sure there is no preexisting data

		Vec4f color = genFlatColor(renderID);
	
		alignDataToBytes((uint8*)&color, sizeof(Vec4f), NO_PADDING, bytes);
		appendDataToBytes((uint8*)actor->getPos(), sizeof(Vec3f), bytes);
		appendDataToBytes((uint8*)actor->getRot(), sizeof(Vec2f), bytes);
	}

	virtual void genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		bytes->clear(); // make sure there is no preexisting data

		appendDataToBytes((uint8*)&_mode, sizeof(unsigned), bytes);
		appendDataToBytes((uint8*)camera->getPos(), sizeof(Vec3f), bytes);
		appendDataToBytes((uint8*)camera->getLookPos(), sizeof(Vec3f), bytes);
		appendDataToBytes((uint8*)camera->getProjMatrix(), sizeof(Mat4x4), bytes);
	}
protected:
	Vec4f genFlatColor(unsigned renderID) const {
		unsigned colorID = genColorID(renderID);
		return Vec4f({ (colorID & 0xFF) / 255.0f, (colorID & 0xFF00) / 255.0f, (colorID & 0xFF00) / 255.0f, _alphaVal }); // default value
	}

	unsigned _mode = FLAT_MODE_SOLID;
	float _alphaVal = 1.0f;
};

struct GL4_Flat_VertexShader : public Flat_VertexShader {
	GL4_Flat_VertexShader() : Flat_VertexShader(genPrefix_glsl() + "Flat_Vertex.glsl") {}
	GL4_Flat_VertexShader(unsigned mode) : Flat_VertexShader(genPrefix_glsl() + "Flat_Vertex.glsl", mode) {}
};

struct Drx11_Flat_VertexShader : public Flat_VertexShader {
	Drx11_Flat_VertexShader() : Flat_VertexShader(genPrefix_hlsl() + "Flat_Vertex.hlsl") {}
	Drx11_Flat_VertexShader(unsigned mode) : Flat_VertexShader(genPrefix_hlsl() + "Flat_Vertex.hlsl", mode) {}
};

// Fragment Shaders

struct Flat_FragmentShader : public Topl_Shader {
	Flat_FragmentShader() : Topl_Shader(){} // Blank Constructor
	Flat_FragmentShader(std::string name) : Topl_Shader(SHDR_Fragment, name){ }
};

struct GL4_Flat_FragmentShader : public Flat_FragmentShader {
	GL4_Flat_FragmentShader() : Flat_FragmentShader(genPrefix_glsl() + "Flat_Frag.glsl") {}
};

struct Drx11_Flat_FragmentShader : public Flat_FragmentShader {
	Drx11_Flat_FragmentShader() : Flat_FragmentShader(genPrefix_hlsl() + "Flat_Pixel.hlsl") {}
};