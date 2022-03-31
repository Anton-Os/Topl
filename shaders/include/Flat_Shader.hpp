#include "Topl_Shader_Pipeline.h"

#define FLAT_MODE_DEFAULT 0
#define FLAT_MODE_ALTERNATE 1

// Vertex Shaders

struct Flat_VertexShader : public Topl_EntryShader {
	Flat_VertexShader() : Topl_EntryShader(){} // Blank Constructor

	Flat_VertexShader(std::string name)
		: Topl_EntryShader(
			name,
			{ 
				Shader_Type("pos", "POSITION", SHDR_float_vec3), 
				Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) 
			} // Inputs
		) { }

	Flat_VertexShader(std::string name, unsigned mode)
		: Topl_EntryShader(
			name,
			{ 
				Shader_Type("pos", "POSITION", SHDR_float_vec3), 
				Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) 
			} // Inputs
		) { _mode = mode; }

	virtual bool genGeoBlock(const Geo_Actor* const actor, blockBytes_t* bytes) const override {
		bytes->clear(); // make sure there is no preexisting data

		bytes_cptr offset_bytes = reinterpret_cast<bytes_cptr>(actor->getPos());
		bytes_cptr rotation_bytes = reinterpret_cast<bytes_cptr>(actor->getRot());
		
		Vec4f color = genFlatColor();
		bytes_cptr color_bytes = reinterpret_cast<bytes_cptr>(&color);
		bytes_cptr mode_bytes = reinterpret_cast<bytes_cptr>(&_mode);
	
		appendDataToBytes(mode_bytes, sizeof(unsigned), bytes);
		appendDataToBytes(offset_bytes, sizeof(Vec3f), bytes);
		appendDataToBytes(rotation_bytes, sizeof(Vec2f), bytes);
		appendDataToBytes(color_bytes, sizeof(Vec4f), bytes);
		
		return true;
	}

	virtual bool genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		bytes->clear(); // make sure there is no preexisting data

		bytes_cptr cameraPos_bytes = reinterpret_cast<bytes_cptr>(camera->getPos());
		bytes_cptr cameraLookPos_bytes = reinterpret_cast<bytes_cptr>(camera->getLookPos());
		bytes_cptr matrix_bytes = reinterpret_cast<bytes_cptr>(camera->getProjMatrix());

		appendDataToBytes(cameraPos_bytes, sizeof(Vec3f), bytes);
		appendDataToBytes(cameraLookPos_bytes, sizeof(Vec3f), bytes);
		appendDataToBytes(matrix_bytes, sizeof(Mat4x4), bytes);
		return true;
	}
protected:
	Vec4f genFlatColor() const { // generates color based on mode
		return Vec4f({ 1.0f, 1.0f, 1.0f, 0.8f }); // simplified model
	}

	unsigned _mode = FLAT_MODE_DEFAULT;
};

struct GL4_Flat_VertexShader : public Flat_VertexShader {
	GL4_Flat_VertexShader() : Flat_VertexShader(genPrefix_glsl() + "Flat_Vertex.glsl") {}
	GL4_Flat_VertexShader(unsigned mode) : Flat_VertexShader(genPrefix_glsl() + "Flat_Vertex.glsl", mode) {}
};

struct Drx11_Flat_VertexShader : public Flat_VertexShader {
	Drx11_Flat_VertexShader() : Flat_VertexShader(genPrefix_hlsl() + "Flat_Vertex.hlsl") {}
	Drx11_Flat_VertexShader(unsigned mode) : Flat_VertexShader(genPrefix_hlsl() + "Flat_Vertex.hlsl", mode) {}
};

// Fragment Shaders

struct Flat_FragmentShader : public Topl_Shader {
	Flat_FragmentShader() : Topl_Shader(){} // Blank Constructor
	Flat_FragmentShader(std::string name) : Topl_Shader(SHDR_Fragment, name){ }
};

struct GL4_Flat_FragmentShader : public Flat_FragmentShader {
	GL4_Flat_FragmentShader() : Flat_FragmentShader(genPrefix_glsl() + "Flat_Frag.glsl") {}
};

struct Drx11_Flat_FragmentShader : public Flat_FragmentShader {
	Drx11_Flat_FragmentShader() : Flat_FragmentShader(genPrefix_hlsl() + "Flat_Pixel.hlsl") {}
};