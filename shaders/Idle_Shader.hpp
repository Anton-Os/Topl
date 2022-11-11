#include "Topl_Shader_Pipeline.h"

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

	virtual void genRenderBlock(const Geo_Actor* const actor, unsigned renderID, blockBytes_t* bytes) const override {}

	virtual void genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {}
};

struct GL4_Idle_VertexShader : public Idle_VertexShader {
	GL4_Idle_VertexShader() : Idle_VertexShader(genPrefix_glsl() + "Idle_Vertex.glsl") {}
};

struct Drx11_Idle_VertexShader : public Idle_VertexShader {
	Drx11_Idle_VertexShader() : Idle_VertexShader(genPrefix_hlsl() + "Idle_Vertex.hlsl") {}
};

// Fragment Shaders

struct Idle_FragmentShader : public Topl_Shader {
	Idle_FragmentShader() : Topl_Shader(){}
	Idle_FragmentShader(std::string name) : Topl_Shader(SHDR_Fragment, name) { }
};

struct GL4_Idle_FragmentShader : public Idle_FragmentShader {
	GL4_Idle_FragmentShader() : Idle_FragmentShader(genPrefix_glsl() + "Idle_Frag.glsl") {}
};

struct Drx11_Idle_FragmentShader : public Idle_FragmentShader {
	Drx11_Idle_FragmentShader() : Idle_FragmentShader(genPrefix_hlsl() + "Idle_Pixel.hlsl") { }
};