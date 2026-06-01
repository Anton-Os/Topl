#include "Topl_Pipeline.hpp"

// Vertex Shaders

struct Spectral_VertexShader : public Topl_EntryShader {
	Spectral_VertexShader() : Topl_EntryShader(){}
	Spectral_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Spectral_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

	void genSceneBlock(const Topl_Scene* const scene, blockBytes_t* bytes) const override {
		Topl_EntryShader::genSceneBlock(scene, bytes);
        appendDataToBytes((uint8_t*)&_texScroll, sizeof(Vec3f), bytes);
		appendDataToBytes((uint8_t*)&_texScale, sizeof(Vec3f), bytes);
		appendDataToBytes((uint8_t*)&light.pos, sizeof(Vec3f), bytes);
		appendDataToBytes((uint8_t*)&light.value, sizeof(Vec3f), bytes);
	}

	void setTexCoordParams(Vec3f scroll, Vec3f scale){
		_texScroll = scroll;
		_texScale = scale;
	}

	void setLight(const Topl_Light& l){ light = l; }
protected:
	Vec3f _texScroll = Vec3f({ 0.0, 0.0, 0.0 });
	Vec3f _texScale = Vec3f({ 1.0, 1.0, 1.0 });

	Topl_Light light = Topl_Light(Vec3f({ 0.0F, 0.0F, -1.0F }));
};

struct Spectral_VertexShader_GL4 : public Spectral_VertexShader {
	Spectral_VertexShader_GL4() : Spectral_VertexShader(genPrefix_glsl() + "spectral/" + "Vertex.glsl") {}
	Spectral_VertexShader_GL4(unsigned mode) : Spectral_VertexShader(genPrefix_glsl() + "spectral/" + "Vertex.glsl", mode) {}
};

struct Spectral_VertexShader_DX11 : public Spectral_VertexShader {
	Spectral_VertexShader_DX11() : Spectral_VertexShader(genPrefix_hlsl() + "spectral/" + "Vertex.hlsl") {}
	Spectral_VertexShader_DX11(unsigned mode) : Spectral_VertexShader(genPrefix_hlsl() + "spectral/" + "Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Spectral_PixelShader : public Topl_Shader {
	Spectral_PixelShader() : Topl_Shader(){}
	Spectral_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name){ }
};

struct Spectral_PixelShader_GL4 : public Spectral_PixelShader {
	Spectral_PixelShader_GL4() : Spectral_PixelShader(genPrefix_glsl() + "spectral/" + "Frag.glsl") {}
};

struct Spectral_PixelShader_DX11 : public Spectral_PixelShader {
	Spectral_PixelShader_DX11() : Spectral_PixelShader(genPrefix_hlsl() + "spectral/" + "Pixel.hlsl") {}
};