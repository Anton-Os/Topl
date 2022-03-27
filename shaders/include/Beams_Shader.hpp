#include "Topl_Shader.h"

// Vertex Shaders

struct Beams_VertexShader : public Topl_EntryShader {
	Beams_VertexShader() : Topl_EntryShader(){}
	Beams_VertexShader(std::string name)
		: Topl_EntryShader(
			SHDR_Vertex, name,
			{ 
				Shader_Type("pos", "POSITION", SHDR_float_vec3), 
				Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) 
			} // Inputs
		) { }

	virtual bool genGeoBlock(const Geo_Actor* const actor, blockBytes_t* bytes) const override {
		bytes->clear(); // make sure there is no preexisting data

		bytes_cptr offset_bytes = reinterpret_cast<bytes_cptr>(actor->getPos());
		bytes_cptr rotation_bytes = reinterpret_cast<bytes_cptr>(actor->getRot());
	
		appendDataToBytes(offset_bytes, sizeof(Vec3f), bytes);
		appendDataToBytes(rotation_bytes, sizeof(Vec2f), bytes);
		// appendDataToBytes(color_bytes, color.size() * sizeof(float), bytes);
		
		return true;
	}

	virtual bool genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		bytes_cptr cameraPos_bytes = reinterpret_cast<bytes_cptr>(camera->getPos());
		bytes_cptr cameraLookPos_bytes = reinterpret_cast<bytes_cptr>(camera->getLookPos());
		bytes_cptr matrix_bytes = reinterpret_cast<bytes_cptr>(camera->getProjMatrix());

		appendDataToBytes(cameraPos_bytes, sizeof(Vec3f), bytes);
		appendDataToBytes(cameraLookPos_bytes, sizeof(Vec2f), bytes);
		// appendDataToBytes(matrix_bytes, camera->getProjMatrix()->size() * sizeof(float), bytes);

		const unsigned lightCount = scene->getLightCount();
        if(lightCount > 0) appendLight(scene, 0, bytes); // 1st light source (sky light)
        if(lightCount > 1) appendLight(scene, 1, bytes); // 2nd light source (flash light)
        if(lightCount > 2) appendLight(scene, 2, bytes); // 3rd light source (lamp light)
		
        return true;
	}

private:
    static void appendLight(const Topl_Scene *const scene, unsigned i, blockBytes_t* bytes){
        bytes_cptr pos_bytes = reinterpret_cast<bytes_cptr>(&scene->getLight(i)->pos);
        bytes_cptr value_bytes = reinterpret_cast<bytes_cptr>(&scene->getLight(i)->value);
        // bytes_cptr intensity_bytes = reinterpret_cast<bytes_cptr>(&scene->getLight(i)->intensity);

        appendDataToBytes(pos_bytes, sizeof(Vec3f), bytes);
        appendDataToBytes(value_bytes, sizeof(Vec2f), bytes);
        // appendDataToBytes(intensity_bytes, sizeof(float), bytes);
    }
};

struct GL4_Beams_VertexShader : public Beams_VertexShader {
	GL4_Beams_VertexShader() : Beams_VertexShader(genPrefix_glsl() + "Beams_Vertex.glsl") {}
};

struct Drx11_Beams_VertexShader : public Beams_VertexShader {
	Drx11_Beams_VertexShader() : Beams_VertexShader(genPrefix_hlsl() + "Beams_Vertex.hlsl") {}
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