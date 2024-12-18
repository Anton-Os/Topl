#ifndef TOPL_PIPELINE_H

#include "Topl_Shader.hpp"

#define MAX_PIPELINE_STAGES 6
#define MAX_PIPELINES 24 // limits number of unique pipelines
#define MAX_SHADERS 24 * 6  // limits number of unique shaders

struct Topl_Pipeline {
	Topl_Pipeline() {}
	Topl_Pipeline(entry_shader_cptr entry) { entryShader = entry; }

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

	// void setMode(unsigned mode){ if(entryShader != nullptr) entryShader->setMode(mode); }

	entry_shader_cptr entryShader = nullptr; // entry shader stored internally
	bool isReady; // value for compilation and link status
};

#define TOPL_PIPELINE_H
#endif