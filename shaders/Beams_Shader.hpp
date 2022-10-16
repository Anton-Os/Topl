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
		appendDataToBytes((uint8*)actor->getRot(), sizeof(Vec2f), bytes);
	}

	virtual void genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		bytes->clear(); // make sure there is no preexisting data
		
		appendDataToBytes((uint8*)&_mode, sizeof(unsigned), bytes);
		appendDataToBytes((uint8*)camera->getPos(), sizeof(Vec3f), bytes);
		appendDataToBytes((uint8*)camera->getLookPos(), sizeof(Vec2f), bytes);
		// appendDataToBytes((uint8*)camera->getProjMatrix(), camera->getProjMatrix()->size() * sizeof(float), bytes);

		const unsigned lightCount = scene->getLightCount();
        if(lightCount > 0) appendLight(scene, 0, bytes); // 1st light source (sky light)
        if(lightCount > 1) appendLight(scene, 1, bytes); // 2nd light source (flash light)
        if(lightCount > 2) appendLight(scene, 2, bytes); // 3rd light source (lamp light)
	}

private:
    static void appendLight(const Topl_Scene *const scene, unsigned i, blockBytes_t* bytes){
        appendDataToBytes((uint8*)&scene->getLight(i)->pos, sizeof(Vec3f), bytes);
        appendDataToBytes((uint8*)&scene->getLight(i)->value, sizeof(Vec2f), bytes);
    }
protected:
	unsigned _mode = BEAMS_MODE_LIGHT;
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