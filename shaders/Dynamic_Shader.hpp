#include "Topl_Pipeline.hpp"

#define DYNAMIC_MODE_RAINBOW 0

// Vertex Shaders

struct Dynamic_VertexShader : public Topl_EntryShader {
	Dynamic_VertexShader() : Topl_EntryShader(){ }
	Dynamic_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Dynamic_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

	void genSceneBlock(const Topl_Scene* const scene, blockBytes_t* bytes) const override {
		static Timer_Dynamic dynamic_timer = Timer_Dynamic(0.0);
        double relMillisecs = dynamic_timer.getRelMillisecs();
        double absMillisecs = dynamic_timer.getAbsMillisecs();

		Topl_EntryShader::genSceneBlock(scene, bytes);
        alignDataToBytes((uint8_t*)&relMillisecs, sizeof(relMillisecs), NO_PADDING, bytes);
        alignDataToBytes((uint8_t*)&absMillisecs, sizeof(absMillisecs), NO_PADDING, bytes);
	}
protected:
	// Timer_Dynamic dynamic_timer = Timer_Dynamic(0.0);

	std::string shaderRoutineStr; // TODO: Make this settable in the dynamic shader
};

struct Dynamic_VertexShader_GL4 : public Dynamic_VertexShader {
	Dynamic_VertexShader_GL4() : Dynamic_VertexShader(genPrefix_glsl() + "Dynamic_Vertex.glsl") {}
	Dynamic_VertexShader_GL4(unsigned mode) : Dynamic_VertexShader(genPrefix_glsl() + "Dynamic_Vertex.glsl", mode) {}
};

struct Dynamic_VertexShader_DX11 : public Dynamic_VertexShader {
	Dynamic_VertexShader_DX11() : Dynamic_VertexShader(genPrefix_hlsl() + "Dynamic_Vertex.hlsl") {}
	Dynamic_VertexShader_DX11(unsigned mode) : Dynamic_VertexShader(genPrefix_hlsl() + "Dynamic_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Dynamic_PixelShader : public Topl_Shader {
	Dynamic_PixelShader() : Topl_Shader(){} // Blank Constructor
	Dynamic_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name){ }
};

struct Dynamic_PixelShader_GL4 : public Dynamic_PixelShader {
	Dynamic_PixelShader_GL4() : Dynamic_PixelShader(genPrefix_glsl() + "Dynamic_Frag.glsl") {}
};

struct Dynamic_PixelShader_DX11 : public Dynamic_PixelShader {
	Dynamic_PixelShader_DX11() : Dynamic_PixelShader(genPrefix_hlsl() + "Dynamic_Pixel.hlsl") {}
};
