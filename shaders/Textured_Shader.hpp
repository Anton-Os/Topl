#include "Topl_Shader_Pipeline.h"

// Vertex Shaders

struct Textured_VertexShader : public Topl_EntryShader {
	Textured_VertexShader() : Topl_EntryShader(){}
	Textured_VertexShader(std::string name) : Topl_EntryShader(name) {  }

	virtual void genRenderBlock(const Geo_Actor* const actor, unsigned renderID, blockBytes_t* bytes) const override {
		bytes->clear(); // make sure there is no preexisting data

		appendDataToBytes((uint8*)actor->getPos(), sizeof(Vec3f), bytes);
		appendDataToBytes((uint8*)actor->getRot(), sizeof(Vec2f), bytes);
	}

	virtual void genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		bytes->clear(); // make sure there is no preexisting data

		appendDataToBytes((uint8*)camera->getPos(), sizeof(Vec3f), bytes);
		appendDataToBytes((uint8*)camera->getLookPos(), sizeof(Vec3f), bytes);
		appendDataToBytes((uint8*)camera->getProjMatrix(), sizeof(Mat4x4), bytes);
	}
};

struct GL4_Textured_VertexShader : public Textured_VertexShader {
	GL4_Textured_VertexShader() : Textured_VertexShader(genPrefix_glsl() + "Textured_Vertex.glsl") {}
};

struct Drx11_Textured_VertexShader : public Textured_VertexShader {
	Drx11_Textured_VertexShader() : Textured_VertexShader(genPrefix_hlsl() + "Textured_Vertex.hlsl") {}
};

// Fragment Shaders

struct Textured_FragmentShader : public Topl_Shader {
	Textured_FragmentShader() : Topl_Shader(){}
	Textured_FragmentShader(std::string name) : Topl_Shader(SHDR_Fragment, name) { }
};

struct GL4_Textured_FragmentShader : public Textured_FragmentShader {
	GL4_Textured_FragmentShader() : Textured_FragmentShader(genPrefix_glsl() + "Textured_Frag.glsl") {}
};

struct Drx11_Textured_FragmentShader : public Textured_FragmentShader {
	Drx11_Textured_FragmentShader() : Textured_FragmentShader(genPrefix_hlsl() + "Textured_Pixel.hlsl") { }
};