#include "Topl_Pipeline.hpp"

// Vertex Shaders

struct Pattern_VertexShader : public Topl_EntryShader {
	Pattern_VertexShader() : Topl_EntryShader(){ }
	Pattern_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Pattern_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

	void genActorBlock(actor_cptr actor, blockBytes_t* bytes) const {
		Topl_EntryShader::genActorBlock(actor, bytes);
	}

	void genSceneBlock(const Topl_Scene* const scene, blockBytes_t* bytes) const override {
		static Timer_Dynamic dynamic_timer = Timer_Dynamic(0.0);
		
        double relMillisecs = dynamic_timer.getRelMillisecs();
        double absMillisecs = dynamic_timer.getAbsMillisecs();

		Topl_EntryShader::genSceneBlock(scene, bytes);
		alignDataToBytes((uint8_t*)&relMillisecs, sizeof(relMillisecs), NO_PADDING, bytes);
        alignDataToBytes((uint8_t*)&absMillisecs, sizeof(absMillisecs), NO_PADDING, bytes);
	}
protected:
	int width = TOPL_WIN_WIDTH;
	int height = TOPL_WIN_HEIGHT;
};

struct Pattern_VertexShader_GL4 : public Pattern_VertexShader {
	Pattern_VertexShader_GL4() : Pattern_VertexShader(genPrefix_glsl() + "pattern/" + "Pattern_Vertex.glsl") {}
	Pattern_VertexShader_GL4(unsigned mode) : Pattern_VertexShader(genPrefix_glsl() + "pattern/" + "Pattern_Vertex.glsl", mode) {}
};

struct Pattern_VertexShader_DX11 : public Pattern_VertexShader {
	Pattern_VertexShader_DX11() : Pattern_VertexShader(genPrefix_hlsl() + "pattern/" + "Pattern_Vertex.hlsl") {}
	Pattern_VertexShader_DX11(unsigned mode) : Pattern_VertexShader(genPrefix_hlsl() + "pattern/" + "Pattern_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Pattern_PixelShader : public Topl_Shader {
	Pattern_PixelShader() : Topl_Shader(){} // Blank Constructor
	Pattern_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name){ }
};

struct Pattern_PixelShader_GL4 : public Pattern_PixelShader {
	Pattern_PixelShader_GL4() : Pattern_PixelShader(genPrefix_glsl() + "pattern/" + "Pattern_Frag.glsl") {}
};

struct Pattern_PixelShader_DX11 : public Pattern_PixelShader {
	Pattern_PixelShader_DX11() : Pattern_PixelShader(genPrefix_hlsl() + "pattern/" + "Pattern_Pixel.hlsl") {
		_embedMap.insert({ "Custom_Pattern", 
			std::string("float4 customPattern(float3 coords){\n")
				+ "\tuint m = 1; // uint(abs(mode) / 10) + 1; \n"
				+ "\n"
				+ "\tfloat r = abs(sin(coords.x * m) + cos(coords.y * m) + tan(coords.z * m)); \n"
				+ "\tfloat g = abs(sin(coords.y * m) * cos(coords.z * m) * tan(coords.x * m)); \n"
				+ "\tfloat b = abs(pow(pow(sin(coords.z * m), cos(coords.x * m)), tan(coords.y * m))); \n"
				+ "\n"
				+ "\treturn float4(r, g, b, 1.0);"
			+ "}\n"
		});
	}
};
