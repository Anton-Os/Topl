#include "Topl_Pipeline.hpp"

// Vertex Shaders

struct Material_VertexShader : public Topl_EntryShader {
	Material_VertexShader() : Topl_EntryShader(){}
	Material_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Material_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

	void genSceneBlock(const Topl_Scene* const scene, blockBytes_t* bytes) const override {
		Topl_EntryShader::genSceneBlock(scene, bytes);
        // TODO: Set texture properties
		appendDataToBytes((uint8_t*)&light.pos, sizeof(Vec3f), bytes);
		appendDataToBytes((uint8_t*)&light.value, sizeof(Vec3f), bytes);
	}
protected:
	Topl_Light light = Topl_Light(Vec3f({ 0.0F, 0.0F, 0.0F }));
};

struct Material_VertexShader_GL4 : public Material_VertexShader {
	Material_VertexShader_GL4() : Material_VertexShader(genPrefix_glsl() + "Material_Vertex.glsl") {}
	Material_VertexShader_GL4(unsigned mode) : Material_VertexShader(genPrefix_glsl() + "Material_Vertex.glsl", mode) {}
};

struct Material_VertexShader_DX11 : public Material_VertexShader {
	Material_VertexShader_DX11() : Material_VertexShader(genPrefix_hlsl() + "Material_Vertex.hlsl") {}
	Material_VertexShader_DX11(unsigned mode) : Material_VertexShader(genPrefix_hlsl() + "Material_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Material_PixelShader : public Topl_Shader {
	Material_PixelShader() : Topl_Shader(){}
	Material_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name){ }
};

struct Material_PixelShader_GL4 : public Material_PixelShader {
	Material_PixelShader_GL4() : Material_PixelShader(genPrefix_glsl() + "Material_Frag.glsl") {}
};

struct Material_PixelShader_DX11 : public Material_PixelShader {
	Material_PixelShader_DX11() : Material_PixelShader(genPrefix_hlsl() + "Material_Pixel.hlsl") {}
};