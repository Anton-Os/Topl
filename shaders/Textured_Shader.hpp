#include "Topl_Pipeline.hpp"

#define TEX_MODE_BASE 0 // default base texture
#define TEX_MODE_LAYER1 1
#define TEX_MODE_LAYER2 2
#define TEX_MODE_LAYER3 3
#define TEX_MODE_LAYER4 4
#define TEX_MODE_LAYER5 5
#define TEX_MODE_LAYER6 6
#define TEX_MODE_LAYER7 7
#define TEX_MODE_VOLUME 8 // volumetric texture

// TODO: Include more modes

// Vertex Shaders

struct Textured_VertexShader : public Topl_EntryShader {
	Textured_VertexShader() : Topl_EntryShader(){}
	Textured_VertexShader(std::string name) : Topl_EntryShader(name) {  }

	virtual void genActorBlock(const Geo_Actor* const actor, blockBytes_t* bytes) const override {
		Topl_EntryShader::genActorBlock(actor, bytes);
		appendDataToBytes((uint8_t*)&_texScroll, sizeof(Vec3f), bytes);
	}

	virtual void genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		// appendDataToBytes((uint8_t*)&_time, sizeof(double), bytes);
		Topl_EntryShader::genSceneBlock(scene, camera, bytes);
	}

	void setTexScroll(const Vec3f& s){ _texScroll = s; }
private:
	Vec3f _texScroll = Vec3f({ 0.0, 0.0, 0.0 });
	// Vec3f _texScale = Vec3f({ 1.0, 1.0, 1.0 });
};

struct Textured_VertexShader_GL4 : public Textured_VertexShader {
	Textured_VertexShader_GL4() : Textured_VertexShader(genPrefix_glsl() + "Textured_Vertex.glsl") {}
};

struct Textured_VertexShader_DX11 : public Textured_VertexShader {
	Textured_VertexShader_DX11() : Textured_VertexShader(genPrefix_hlsl() + "Textured_Vertex.hlsl") {}
};

// Pixel Shaders

struct Textured_PixelShader : public Topl_Shader {
	Textured_PixelShader() : Topl_Shader(){}
	Textured_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name) { }
};

struct Textured_PixelShader_GL4 : public Textured_PixelShader {
	Textured_PixelShader_GL4() : Textured_PixelShader(genPrefix_glsl() + "Textured_Frag.glsl") {}
};

struct Textured_PixelShader_DX11 : public Textured_PixelShader {
	Textured_PixelShader_DX11() : Textured_PixelShader(genPrefix_hlsl() + "Textured_Pixel.hlsl") { }
};