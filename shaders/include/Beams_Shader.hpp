#include "Topl_Shader.hpp"

// Vertex Shaders

struct Beams_VertexShader : public Topl_EntryShader {
	Beams_VertexShader(std::string name)
		: Topl_EntryShader(
			SHDR_Vertex, name,
			{ 
				Shader_Type("pos", "POSITION", SHDR_float_vec3), 
				Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) 
			} // Inputs
		) { }

	virtual bool genGeoBlock(const Geo_Actor* const component, blockBytes_t* bytes) const override {
		bytes->clear(); // Make sure there is no preexisting data

		bytes_cptr offset_bytes = reinterpret_cast<bytes_cptr>(component->getPos()->data());
		bytes_cptr rotation_bytes = reinterpret_cast<bytes_cptr>(component->getAngles()->data());
		// Eigen::Vector4f color = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 0.8f);
		// bytes_cptr color_bytes = reinterpret_cast<bytes_cptr>(color.data());
	
		appendDataToBytes(offset_bytes, component->getPos()->size() * sizeof(float), bytes);
		appendDataToBytes(rotation_bytes, component->getAngles()->size() * sizeof(float), bytes);
		// appendDataToBytes(color_bytes, color.size() * sizeof(float), bytes);
		
		return true;
	}

	virtual bool genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		bytes_cptr cameraPos_bytes = reinterpret_cast<bytes_cptr>(camera->getPos()->data());
		bytes_cptr cameraRot_bytes = reinterpret_cast<bytes_cptr>(camera->getLookPos()->data());
		bytes_cptr matrix_bytes = reinterpret_cast<bytes_cptr>(camera->getProjMatrix()->data());

		appendDataToBytes(cameraPos_bytes, camera->getPos()->size() * sizeof(float), bytes);
		appendDataToBytes(cameraRot_bytes, camera->getLookPos()->size() * sizeof(float), bytes);
		// appendDataToBytes(matrix_bytes, camera->getProjMatrix()->size() * sizeof(float), bytes);

		const unsigned lightCount = scene->getLightCount();
        if(lightCount > 0) appendLight(scene, 0, bytes); // 1st light source (sky light)
        if(lightCount > 1) appendLight(scene, 1, bytes); // 2nd light source (flash light)
        if(lightCount > 2) appendLight(scene, 2, bytes); // 3rd light source (lamp light)
		
        return true;
	}

private:
    static void appendLight(const Topl_Scene *const scene, unsigned i, blockBytes_t* bytes){
        bytes_cptr pos_bytes = reinterpret_cast<bytes_cptr>(scene->getLight(i)->pos.data());
        bytes_cptr value_bytes = reinterpret_cast<bytes_cptr>(scene->getLight(i)->value.data());
        // bytes_cptr intensity_bytes = reinterpret_cast<bytes_cptr>(&scene->getLight(i)->intensity);

        appendDataToBytes(pos_bytes, scene->getLight(i)->pos.size() * sizeof(float), bytes);
        appendDataToBytes(value_bytes, scene->getLight(i)->value.size() * sizeof(float), bytes);
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
	Beams_FragmentShader(std::string name) : Topl_Shader(SHDR_Fragment, name){ }
};

struct GL4_Beams_FragmentShader : public Beams_FragmentShader {
	GL4_Beams_FragmentShader() : Beams_FragmentShader(genPrefix_glsl() + "Beams_Frag.glsl") {}
};

struct Drx11_Beams_FragmentShader : public Beams_FragmentShader {
	Drx11_Beams_FragmentShader() : Beams_FragmentShader(genPrefix_hlsl() + "Beams_Pixel.hlsl") {}
};