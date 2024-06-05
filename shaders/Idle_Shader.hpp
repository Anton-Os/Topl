#include "Topl_Pipeline.hpp"

// Vertex Shaders

struct Idle_VertexShader : public Topl_EntryShader {
	Idle_VertexShader() : Topl_EntryShader(){}
	Idle_VertexShader(std::string name)
		: Topl_EntryShader( // Idle Shader can be used for testing new inputs
			name,
			{ 
				Shader_Type("pos", "POSITION", SHDR_float_vec3),
				// Shader_Type("normal", "NORMAL", SHDR_float_vec3),
				Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) 
			} // Inputs
		) {  }

	virtual void genRenderBlock(const Geo_Actor* const actor, blockBytes_t* bytes) const override {
		Topl_EntryShader::genRenderBlock(actor, bytes);
	}

	virtual void genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		Topl_EntryShader::genSceneBlock(scene, camera, bytes);
	}
};

struct Idle_VertexShader_GL4 : public Idle_VertexShader {
    Idle_VertexShader_GL4() : Idle_VertexShader(genPrefix_glsl() + "Idle_Vertex.glsl") {}
};

struct Idle_VertexShader_DX11 : public Idle_VertexShader {
	Idle_VertexShader_DX11() : Idle_VertexShader(genPrefix_hlsl() + "Idle_Vertex.hlsl") {}
};

// Pixel Shaders

struct Idle_PixelShader : public Topl_Shader {
	Idle_PixelShader() : Topl_Shader(){}
	Idle_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name) { }
};

struct Idle_PixelShader_GL4 : public Idle_PixelShader {
    Idle_PixelShader_GL4() : Idle_PixelShader(genPrefix_glsl() + "Idle_Frag.glsl") {}
};

struct Idle_PixelShader_DX11 : public Idle_PixelShader {
	Idle_PixelShader_DX11() : Idle_PixelShader(genPrefix_hlsl() + "Idle_Pixel.hlsl") { }
};
