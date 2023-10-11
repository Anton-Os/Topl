#include "Topl_Pipeline.hpp"

// Vertex Shaders

struct Textured_VertexShader : public Topl_EntryShader {
	Textured_VertexShader() : Topl_EntryShader(){}
	Textured_VertexShader(std::string name) : Topl_EntryShader(name) {  }

	virtual void genRenderBlock(const Geo_Actor* const actor, unsigned renderID, blockBytes_t* bytes) const override {
		Topl_EntryShader::genRenderBlock(actor, renderID, bytes);
		appendDataToBytes((uint8_t*)&_texScroll, sizeof(Vec3f), bytes);
	}

	virtual void genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		appendDataToBytes((uint8_t*)&_time, sizeof(double), bytes);
		Topl_EntryShader::genSceneBlock(scene, camera, bytes);
	}

	void setTime(double t) { _time = t; }
private:
	double _time = 0.0;
	Vec3f _texScroll = Vec3f({ 0.0, 0.0, 0.0 });
	// Vec3f _texScale = Vec3f({ 1.0, 1.0, 1.0 });
};

struct GL4_Textured_VertexShader : public Textured_VertexShader {
	GL4_Textured_VertexShader() : Textured_VertexShader(genPrefix_glsl() + "Textured_Vertex.glsl") {}
};

struct Drx11_Textured_VertexShader : public Textured_VertexShader {
	Drx11_Textured_VertexShader() : Textured_VertexShader(genPrefix_hlsl() + "Textured_Vertex.hlsl") {}
};

// Pixel Shaders

struct Textured_PixelShader : public Topl_Shader {
	Textured_PixelShader() : Topl_Shader(){}
	Textured_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name) { }
};

struct GL4_Textured_PixelShader : public Textured_PixelShader {
	GL4_Textured_PixelShader() : Textured_PixelShader(genPrefix_glsl() + "Textured_Frag.glsl") {}
};

struct Drx11_Textured_PixelShader : public Textured_PixelShader {
	Drx11_Textured_PixelShader() : Textured_PixelShader(genPrefix_hlsl() + "Textured_Pixel.hlsl") { }
};