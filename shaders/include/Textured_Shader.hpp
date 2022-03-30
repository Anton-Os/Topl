#include "Topl_Pipeline.h"

// Vertex Shaders

struct Textured_VertexShader : public Topl_EntryShader {
	Textured_VertexShader() : Topl_EntryShader(){}
	Textured_VertexShader(std::string name)
		: Topl_EntryShader(
			SHDR_Vertex, name,
			{ 
				Shader_Type("pos", "POSITION", SHDR_float_vec3), 
				Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) 
			} // Inputs
		) {  }

	virtual bool genGeoBlock(const Geo_Actor* const actor, blockBytes_t* bytes) const override {
		bytes->clear(); // Make sure there is no preexisting data

		bytes_cptr offset_bytes = reinterpret_cast<bytes_cptr>(actor->getPos());
		bytes_cptr rotation_bytes = reinterpret_cast<bytes_cptr>(actor->getRot());
	
		appendDataToBytes(offset_bytes, sizeof(Vec3f), bytes);
		appendDataToBytes(rotation_bytes, sizeof(Vec2f), bytes);
		return true;
	}

	virtual bool genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		bytes_cptr matrixBytes = reinterpret_cast<bytes_cptr>(camera->getProjMatrix());
		assignDataToBytes(matrixBytes, sizeof(Mat4x4), bytes);
		return true;
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