#include "Topl_Pipeline.hpp"

#define BEAMS_MODE_LIGHT 0
#define BEAMS_MODE_AMBIENT 1
#define BEAMS_MODE_DIFFUSE 2
#define BEAMS_MODE_SPECULAR 3
#define BEAMS_MODE_DEPTH 4
#define BEAMS_MODE_REF 5
#define BEAMS_MODE_TRIG 6
// TODO: Include mode 7
// TODO: Include mode 8

// Light

enum LIGHT_Type { 
	LIGHT_Sky, // uniform direction
	LIGHT_Flash, // pointing relative respect to camera
	LIGHT_Lamp // hovers in scene
};

struct Topl_Light {
	Topl_Light(const Vec3f& p) { pos = p; } // Simple Constructor
	Topl_Light(const Vec3f& p, const Vec3f& v) { // Extended Constructor
		pos = p;
		value = v;
	}
	Vec3f pos;
	Vec3f value = Vec3f({ 1.0f, 1.0f, 1.0f }); // indicates both color and intensity
};

// Vertex Shaders

struct Beams_VertexShader : public Topl_EntryShader {
	Beams_VertexShader() : Topl_EntryShader(){}
	Beams_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Beams_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

	virtual void genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		Topl_EntryShader::genSceneBlock(scene, camera, bytes);
		sendLightData(&skyLight, bytes);
		sendLightData(&flashLight, bytes);
		sendLightData(&lampLight, bytes);
	}

	void setLight(LIGHT_Type type, const Topl_Light& light) {
		switch (type) {
		case LIGHT_Sky: skyLight = light; break;
		case LIGHT_Flash: flashLight = light; break;
		case LIGHT_Lamp: lampLight = light; break;
		default: break; // not supported
		}
	}
protected:
	Topl_Light skyLight = Topl_Light({ 0.0, 1.0f, 0.0 }, { 1.0, 1.0, 0.0 });
	Topl_Light flashLight = Topl_Light({ 0.0, 0.0f, -1.0 }, { 1.0, 0.0, 1.0 });
	Topl_Light lampLight = Topl_Light({ 0.0, 0.0f, 1.0 }, { 0.0, 1.0, 1.0 });
private:
	static void sendLightData(const Topl_Light* light, blockBytes_t* bytes) {
		appendDataToBytes((uint8_t*)&light->pos, sizeof(Vec3f), bytes);
		appendDataToBytes((uint8_t*)&light->value, sizeof(Vec3f), bytes);
	}
};

struct Beams_VertexShader_GL4 : public Beams_VertexShader {
	Beams_VertexShader_GL4() : Beams_VertexShader(genPrefix_glsl() + "Beams_Vertex.glsl") {}
	Beams_VertexShader_GL4(unsigned mode) : Beams_VertexShader(genPrefix_glsl() + "Beams_Vertex.glsl", mode) {}
};

struct Beams_VertexShader_DX11 : public Beams_VertexShader {
	Beams_VertexShader_DX11() : Beams_VertexShader(genPrefix_hlsl() + "Beams_Vertex.hlsl") {}
	Beams_VertexShader_DX11(unsigned mode) : Beams_VertexShader(genPrefix_hlsl() + "Beams_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Beams_PixelShader : public Topl_Shader {
	Beams_PixelShader() : Topl_Shader(){}
	Beams_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name){ }
};

struct Beams_PixelShader_GL4 : public Beams_PixelShader {
	Beams_PixelShader_GL4() : Beams_PixelShader(genPrefix_glsl() + "Beams_Frag.glsl") {}
};

struct Beams_PixelShader_DX11 : public Beams_PixelShader {
	Beams_PixelShader_DX11() : Beams_PixelShader(genPrefix_hlsl() + "Beams_Pixel.hlsl") {}
};