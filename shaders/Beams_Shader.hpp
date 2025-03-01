#include "Topl_Pipeline.hpp"

#define BEAMS_FULL 0
#define BEAMS_AMBIENT 1
#define BEAMS_DIFFUSE 2
#define BEAMS_SPECULAR 3
#define BEAMS_HIGHLIGHT 4
#define BEAMS_SPOT 5
#define BEAMS_DEPTH 6
#define BEAMS_DISTANCE 7
#define BEAMS_TRAJECTORY 8
#define BEAMS_TRIAL 9

#define BEAMS_SKY_LIGHT Topl_Light({ 0.0, 1.0f, 0.0 }, { 1.0, 1.0, 0.0 })
#define BEAMS_FLASH_LIGHT Topl_Light({ 0.0, 1.0f, 0.0 }, { 1.0, 0.0, 1.0 })
#define BEAMS_LAMP_LIGHT Topl_Light({ 0.0, 0.0f, 1.0 }, { 0.0, 1.0, 1.0 })

// Light

enum LIGHT_Type { 
	LIGHT_Sky, // uniform direction
	LIGHT_Flash, // pointing relative respect to camera
	LIGHT_Lamp // hovers in scene
};

// Vertex Shaders

struct Beams_VertexShader : public Topl_EntryShader {
	Beams_VertexShader() : Topl_EntryShader(){}
	Beams_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Beams_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

	void genActorBlock(actor_cptr actor, blockBytes_t* bytes) const override {
		// if(isAutoCtrl) setLight(LIGHT_Lamp, Topl_Light(*(actor->getPos()), lampLight.value));
		Topl_EntryShader::genActorBlock(actor, bytes);
	}

	void genSceneBlock(const Topl_Scene* const scene, blockBytes_t* bytes) const override {
		/* if(isAutoCtrl){
			setLight(LIGHT_Flash, Topl_Light(*(scene->camera->getPos()), flashLight.value));
			setLight(LIGHT_Sky, Topl_Light(Vec3f({ sin((*(scene->camera->getRot()))[0]), cos((*(scene->camera->getRot()))[0]), tan((*(scene->camera->getRot()))[1]) }), skyLight.value));
		} */

		Topl_EntryShader::genSceneBlock(scene, bytes);
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

	// bool isAutoCtrl = false;
protected:
	Topl_Light skyLight = BEAMS_SKY_LIGHT;
	Topl_Light flashLight = BEAMS_FLASH_LIGHT;
	Topl_Light lampLight = BEAMS_LAMP_LIGHT;
private:
	void sendLightData(const Topl_Light* light, blockBytes_t* bytes) const {
		appendDataToBytes((uint8_t*)&light->pos, sizeof(Vec3f), bytes);
		appendDataToBytes((uint8_t*)&light->value, sizeof(Vec3f), bytes);
	}
};

struct Beams_VertexShader_GL4 : public Beams_VertexShader {
	Beams_VertexShader_GL4() : Beams_VertexShader(genPrefix_glsl() + "vertex/" + "Beams_Vertex.glsl") {}
	Beams_VertexShader_GL4(unsigned mode) : Beams_VertexShader(genPrefix_glsl() + "vertex/" + "Beams_Vertex.glsl", mode) {}
};

struct Beams_VertexShader_DX11 : public Beams_VertexShader {
	Beams_VertexShader_DX11() : Beams_VertexShader(genPrefix_hlsl() + "vertex/" + "Beams_Vertex.hlsl") {}
	Beams_VertexShader_DX11(unsigned mode) : Beams_VertexShader(genPrefix_hlsl() + "vertex/" + "Beams_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Beams_PixelShader : public Topl_Shader {
	Beams_PixelShader() : Topl_Shader(){}
	Beams_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name){ }
};

struct Beams_PixelShader_GL4 : public Beams_PixelShader {
	Beams_PixelShader_GL4() : Beams_PixelShader(genPrefix_glsl() + "pixel/" + "Beams_Frag.glsl") {}
};

struct Beams_PixelShader_DX11 : public Beams_PixelShader {
	Beams_PixelShader_DX11() : Beams_PixelShader(genPrefix_hlsl() + "pixel/" + "Beams_Pixel.hlsl") {}
};