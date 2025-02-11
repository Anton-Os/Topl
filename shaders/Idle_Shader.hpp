#include "Topl_Pipeline.hpp"

// Vertex Shaders

struct Idle_VertexShader : public Topl_EntryShader {
	Idle_VertexShader() : Topl_EntryShader(){}
	Idle_VertexShader(std::string name)
		: Topl_EntryShader( // Idle Shader can be used for testing new inputs
			name,
			{ 
                Shader_Type("pos", "POSITION", SHDR_float_vec3),
				Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) 
			} // Inputs
		) {  }

	void genActorBlock(const Geo_Actor* const actor, blockBytes_t* bytes) const override {
		Topl_EntryShader::genActorBlock(actor, bytes);
	}

	void genSceneBlock(const Topl_Scene* const scene, blockBytes_t* bytes) const override {
		Topl_EntryShader::genSceneBlock(scene, bytes);
	}
};

struct Idle_VertexShader_GL4 : public Idle_VertexShader {
	Idle_VertexShader_GL4() : Idle_VertexShader(genPrefix_glsl() + "vertex/" + "Idle_Vertex.glsl") {}
};

#ifdef __ANDROID__
struct Idle_VertexShader_GLES : public Idle_VertexShader {
    Idle_VertexShader_GLES() : Idle_VertexShader("") {} // TODO: Correctly route to local emulated path

    std::string getFileSource() const {
        return std::string("#version 440")
            + "\n" + "layout(location = 0) in vec3 pos;"
            + "\n" + "void main() {"
            + "\n" + "gl_Position = vec4(pos, 1.0f);"
            + "\n" + "}";
    }
};
#endif

struct Idle_VertexShader_DX11 : public Idle_VertexShader {
	Idle_VertexShader_DX11() : Idle_VertexShader(genPrefix_hlsl() + "vertex/" + "Idle_Vertex.hlsl") {}
};

// Pixel Shaders

struct Idle_PixelShader : public Topl_Shader {
	Idle_PixelShader() : Topl_Shader(){}
	Idle_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name) { }
};

struct Idle_PixelShader_GL4 : public Idle_PixelShader {
	Idle_PixelShader_GL4() : Idle_PixelShader(genPrefix_glsl() + "pixel/" + "Idle_Frag.glsl") {}
};

struct Idle_PixelShader_DX11 : public Idle_PixelShader {
	Idle_PixelShader_DX11() : Idle_PixelShader(genPrefix_hlsl() + "pixel/" + "Idle_Pixel.hlsl") { }
};


#ifdef __ANDROID__
struct Idle_PixelShader_GLES : public Idle_PixelShader {
    Idle_PixelShader_GLES() : Idle_PixelShader("") {} // TODO: Correctly route to local emulated path

    std::string getFileSource() const {
        return std::string("#version 440")
            + "\n" + "layout(location = 0) out vec4 color_out;"
            + "\n" + "void main() {"
            + "\n" + "color_out = vec4(1.0f, 1.0f, 0.0f, 1.0f);"
            + "\n" + "}";
    }
};
#endif