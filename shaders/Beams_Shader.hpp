#include "Topl_Pipeline.hpp"

#define BEAMS_MODE_LIGHT 0
#define BEAMS_MODE_ALTERNATE 1
#define BEAMS_MODE_DEPTH 2

// Vertex Shaders

struct Beams_VertexShader : public Topl_EntryShader {
	Beams_VertexShader() : Topl_EntryShader(){}
	Beams_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Beams_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

	virtual void genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		appendDataToBytes((uint8_t*)&_mode, sizeof(unsigned), bytes);
		appendDataToBytes((uint8_t*)camera->getPos(), sizeof(Vec3f), bytes);
		appendDataToBytes((uint8_t*)camera->getRot(), sizeof(Vec3f), bytes);
		// appendDataToBytes((uint8_t*)camera->getProjMatrix(), camera->getProjMatrix()->size() * sizeof(float), bytes);

		appendLight(&skyLight, bytes);
		appendLight(&flashLight, bytes);
		appendLight(&lampLight, bytes);
	}

	void setLight(LIGHT_Type type, const Topl_Light& light) {
		switch (type) {
		case LIGHT_Sky: skyLight = light; break;
		case LIGHT_Flash: flashLight = light; break;
		case LIGHT_Lamp: lampLight = light; break;
		default: break; // not supported
		}
	}
private:
	static void appendLight(const Topl_Light* light, blockBytes_t* bytes) {
		appendDataToBytes((uint8_t*)&light->pos, sizeof(Vec3f), bytes);
		appendDataToBytes((uint8_t*)&light->value, sizeof(Vec3f), bytes);
	}
protected:
	unsigned _mode = BEAMS_MODE_LIGHT;

	Topl_Light skyLight = Topl_Light({ 0.0, 1.0f, 0.0 });
	Topl_Light flashLight = Topl_Light({ 0.0, 0.0f, -1.0 });
	Topl_Light lampLight = Topl_Light({ 0.0, 0.0f, 1.0 });
};

struct GL4_Beams_VertexShader : public Beams_VertexShader {
	GL4_Beams_VertexShader() : Beams_VertexShader(genPrefix_glsl() + "Beams_Vertex.glsl") {}
	GL4_Beams_VertexShader(unsigned mode) : Beams_VertexShader(genPrefix_glsl() + "Beams_Vertex.glsl", mode) {}
};

struct Drx11_Beams_VertexShader : public Beams_VertexShader {
	Drx11_Beams_VertexShader() : Beams_VertexShader(genPrefix_hlsl() + "Beams_Vertex.hlsl") {}
	Drx11_Beams_VertexShader(unsigned mode) : Beams_VertexShader(genPrefix_hlsl() + "Beams_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Beams_PixelShader : public Topl_Shader {
	Beams_PixelShader() : Topl_Shader(){}
	Beams_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name){ }
};

struct GL4_Beams_PixelShader : public Beams_PixelShader {
	GL4_Beams_PixelShader() : Beams_PixelShader(genPrefix_glsl() + "Beams_Frag.glsl") {}
};

struct Drx11_Beams_PixelShader : public Beams_PixelShader {
	Drx11_Beams_PixelShader() : Beams_PixelShader(genPrefix_hlsl() + "Beams_Pixel.hlsl") {}
};