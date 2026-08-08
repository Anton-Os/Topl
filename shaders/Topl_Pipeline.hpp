#ifndef TOPL_PIPELINE_H

#include "Topl_Shader.hpp"

#define MAX_PIPELINE_STAGES 6
#define MAX_PIPELINES 24 // limits number of unique pipelines
#define MAX_SHADERS 24 * 6  // limits number of unique shaders

struct Topl_Light {
	Topl_Light(const Vec3f& p) { pos = p; } // Simple Constructor
	Topl_Light(const Vec3f& p, const Vec3f& v) { // Extended Constructor
		pos = p;
		value = v;
	}
	Vec3f pos;
	Vec3f value = Vec3f({ 1.0f, 1.0f, 1.0f }); // indicates both color and intensity
};

// TODO: Include other data that can be sent to pipeline

struct Topl_Pipeline {
	Topl_Pipeline() {}
	Topl_Pipeline(entry_shader_cptr entry) { _vertexShader = entry; }

	static unsigned getOffset(enum SHDR_ValueType type) {
		switch (type) {
		case SHDR_float_vec4: return sizeof(float) * 4;
		case SHDR_float_vec3: return sizeof(float) * 3;
		case SHDR_float_vec2: return sizeof(float) * 2;
		case SHDR_float: return sizeof(float);
		case SHDR_uint_vec4: return sizeof(unsigned) * 4;
		case SHDR_uint_vec3: return sizeof(unsigned) * 3;
		case SHDR_uint_vec2: return sizeof(unsigned) * 2;
		case SHDR_uint: return sizeof(unsigned);
		default:
			logMessage(MESSAGE_Exclaim, "Shader input type not supported!");
			break;
		}
		return 0;
	}

	static unsigned getSize(enum SHDR_ValueType type) {
		switch (type) {
		case SHDR_float_vec4: case SHDR_double_vec4: case SHDR_uint_vec4: case SHDR_int_vec4:
			return 4;
		case SHDR_float_vec3: case SHDR_double_vec3: case SHDR_uint_vec3: case SHDR_int_vec3:
			return 3;
		case SHDR_float_vec2: case SHDR_double_vec2: case SHDR_uint_vec2: case SHDR_int_vec2:
			return 2;
		case SHDR_float: case SHDR_double: case SHDR_uint: case SHDR_int:
			return 1;
		default: return 0; // error
		}
	}

	void setShaders(entry_shader_cptr vShader, shader_cptr pShader, std::initializer_list<shader_cptr> shaders){
		_vertexShader = vShader;
		_pixelShader = pShader;
		for(auto s = shaders.begin(); s != shaders.end(); s++)
			switch((*s)->getType()){
				case SHDR_Geom: _geomShader = *s; break;
				case SHDR_TessCtrl: _tessCtrlShader = *s; break;
				case SHDR_TessEval: _tessEvalShader = *s; break;
				case SHDR_Compute: _computeShader = *s; break;
			}
		// isReady = (vShader == nullptr || pShader == nullptr) && shaders.size() == 0;
		isReady = true;
	}

    entry_shader_cptr getEntryShader() const { return _vertexShader; }

    bool isReady = true; // value for compilation and link status // TODO: Should be conditional?
private:
	entry_shader_cptr _vertexShader = nullptr;
	shader_cptr _geomShader = nullptr;
	shader_cptr _tessCtrlShader = nullptr;
	shader_cptr _tessEvalShader = nullptr;
	shader_cptr _pixelShader = nullptr;
	shader_cptr _computeShader = nullptr;
};

#define TOPL_PIPELINE_H
#endif
