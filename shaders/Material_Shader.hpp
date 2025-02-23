#include "Topl_Pipeline.hpp"

// Vertex Shaders

struct Material_VertexShader : public Topl_EntryShader {
	Material_VertexShader() : Topl_EntryShader(){}
	Material_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Material_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

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

struct Material_VertexShader_GL4 : public Material_VertexShader {
	Material_VertexShader_GL4() : Material_VertexShader(genPrefix_glsl() + "vertex/" + "Material_Vertex.glsl") {}
	Material_VertexShader_GL4(unsigned mode) : Material_VertexShader(genPrefix_glsl() + "vertex/" + "Material_Vertex.glsl", mode) {}
};

struct Material_VertexShader_DX11 : public Material_VertexShader {
	Material_VertexShader_DX11() : Material_VertexShader(genPrefix_hlsl() + "vertex/" + "Material_Vertex.hlsl") {}
	Material_VertexShader_DX11(unsigned mode) : Material_VertexShader(genPrefix_hlsl() + "vertex/" + "Material_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Material_PixelShader : public Topl_Shader {
	Material_PixelShader() : Topl_Shader(){}
	Material_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name){ }
};

struct Material_PixelShader_GL4 : public Material_PixelShader {
	Material_PixelShader_GL4() : Material_PixelShader(genPrefix_glsl() + "pixel/" + "Material_Frag.glsl") {}
};

struct Material_PixelShader_DX11 : public Material_PixelShader {
	Material_PixelShader_DX11() : Material_PixelShader(genPrefix_hlsl() + "pixel/" + "Material_Pixel.hlsl") {}
};