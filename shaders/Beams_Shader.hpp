#include "Topl_Shader_Pipeline.h"

#define BEAMS_MODE_LIGHT 0
#define BEAMS_MODE_ALTERNATE 1
#define BEAMS_MODE_DEPTH 2

// Vertex Shaders

struct Beams_VertexShader : public Topl_EntryShader {
	Beams_VertexShader() : Topl_EntryShader(){}
	Beams_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Beams_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

	virtual void genRenderBlock(const Geo_Actor* const actor, unsigned renderID, blockBytes_t* bytes) const override {
		bytes->clear(); // make sure there is no preexisting data

		appendDataToBytes((uint8*)actor->getPos(), sizeof(Vec3f), bytes);
		appendDataToBytes((uint8*)actor->getRot(), sizeof(Vec3f), bytes);
	}

	virtual void genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		bytes->clear(); // make sure there is no preexisting data
		
		appendDataToBytes((uint8*)&_mode, sizeof(unsigned), bytes);
		appendDataToBytes((uint8*)camera->getPos(), sizeof(Vec3f), bytes);
		appendDataToBytes((uint8*)camera->getLookPos(), sizeof(Vec2f), bytes);
		// appendDataToBytes((uint8*)camera->getProjMatrix(), camera->getProjMatrix()->size() * sizeof(float), bytes);

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
		appendDataToBytes((uint8*)&light->pos, sizeof(Vec3f), bytes);
		appendDataToBytes((uint8*)&light->value, sizeof(Vec3f), bytes);
	}
protected:
	unsigned _mode = BEAMS_MODE_ALTERNATE;

	Topl_Light skyLight = Topl_Light({ 0.0, 1.0f, 0.0 });
	Topl_Light flashLight = Topl_Light({ 0.0, 0.0f, 1.0 });
	Topl_Light lampLight = Topl_Light({ -1.0, 0.0f, 0.0 });
};

struct GL4_Beams_VertexShader : public Beams_VertexShader {
	GL4_Beams_VertexShader() : Beams_VertexShader(genPrefix_glsl() + "Beams_Vertex.glsl") {}
	GL4_Beams_VertexShader(unsigned mode) : Beams_VertexShader(genPrefix_glsl() + "Beams_Vertex.glsl", mode) {}
};

struct Drx11_Beams_VertexShader : public Beams_VertexShader {
	Drx11_Beams_VertexShader() : Beams_VertexShader(genPrefix_hlsl() + "Beams_Vertex.hlsl") {}
	Drx11_Beams_VertexShader(unsigned mode) : Beams_VertexShader(genPrefix_hlsl() + "Beams_Vertex.hlsl", mode) {}
};

// Fragment Shaders

struct Beams_FragmentShader : public Topl_Shader {
	Beams_FragmentShader() : Topl_Shader(){}
	Beams_FragmentShader(std::string name) : Topl_Shader(SHDR_Fragment, name){ }
};

struct GL4_Beams_FragmentShader : public Beams_FragmentShader {
	GL4_Beams_FragmentShader() : Beams_FragmentShader(genPrefix_glsl() + "Beams_Frag.glsl") {}
};

struct Drx11_Beams_FragmentShader : public Beams_FragmentShader {
	Drx11_Beams_FragmentShader() : Beams_FragmentShader(genPrefix_hlsl() + "Beams_Pixel.hlsl") {}
};