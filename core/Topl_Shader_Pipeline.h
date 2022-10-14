#ifndef TOPL_SHADER_H

#include <cstring>
#include <vector>

#include "Platform.hpp"
#include "ValueGen.hpp"

#include "Geo_Actor.hpp"

#include "Topl_Scene.hpp"

// Values and Types

enum SHDR_Type {
    SHDR_Vertex,
    SHDR_Fragment,
    SHDR_Geom,
    SHDR_TessCtrl,
    SHDR_TessEval
};

enum SHDR_ValueType {
    SHDR_float, SHDR_float_vec2, SHDR_float_vec3, SHDR_float_vec4,
    SHDR_double, SHDR_double_vec2, SHDR_double_vec3, SHDR_double_vec4,
    SHDR_int, SHDR_int_vec2, SHDR_int_vec3, SHDR_int_vec4,
    SHDR_uint, SHDR_uint_vec2, SHDR_uint_vec3, SHDR_uint_vec4,
    // Skip the boolean vector types for now
    SHDR_bool,
    SHDR_matrix_2x2, SHDR_matrix_2x3, SHDR_matrix_2x4, // 2 by matrices
    SHDR_matrix_3x2, SHDR_matrix_3x3, SHDR_matrix_3x4, // 3 by matrices
    SHDR_matrix_4x2, SHDR_matrix_4x3, SHDR_matrix_4x4, // 4 by matrices
};

struct Shader_Type {
    Shader_Type(){}
    Shader_Type(const std::string& n, SHDR_ValueType t){
        name = n;
        type = t;
    }
    Shader_Type(const std::string& n, const std::string& s, SHDR_ValueType t){
        name = n;
        semantic = s;
        type = t;
    }
    std::string name;
    std::string semantic = ""; // Relevant for DirectX
    SHDR_ValueType type;
};

// Shader

class Topl_Shader {
public:
    Topl_Shader(){} // Blank Constructor
    Topl_Shader( // Filename Constructor
		enum SHDR_Type type, 
		std::string fileSrc
    ){    
        _shaderType = type;
        _shaderFileSrc = fileSrc;
		_shaderFileSrc = SHADERS_DIR + fileSrc;
		std::replace(_shaderFileSrc.begin(), _shaderFileSrc.end(), '/', '\\');
    }
	/* Topl_Shader( // Renderer Instance Contructor
		enum SHDR_Type type,
		std::string fileSrc,
		const Topl_Renderer *const renderer
	) {
		_shaderType = type;
		_shaderFileSrc = fileSrc;
		_shaderFileSrc = SHADERS_DIR + fileSrc;
		std::replace(_shaderFileSrc.begin(), _shaderFileSrc.end(), '/', '\\');
	} */
    enum SHDR_Type getType() const { return _shaderType; }
    const char* getFilePath() const { return _shaderFileSrc.c_str(); }
protected:
    enum SHDR_Type _shaderType;
    std::string _shaderFileSrc; // make into const type!

	std::string genPrefix_glsl() { return "glsl/"; }
	std::string genPrefix_hlsl() { return "hlsl/"; }
};

// Entry shader contains inputs and functionality to pass uniform blocks
class Topl_EntryShader : public Topl_Shader {
public:
    Topl_EntryShader() : Topl_Shader(){} // Blank Constructor
    Topl_EntryShader( // Filename Constructor
		std::string fileSrc, 
		std::initializer_list<Shader_Type> inputs
	) : Topl_Shader(SHDR_Vertex, fileSrc){ 
        for(std::initializer_list<Shader_Type>::iterator currentInput = inputs.begin(); currentInput < inputs.end(); currentInput++)
            _inputs.push_back(*currentInput); // fills input list with data
    }
    const Shader_Type* getInputAtIndex(unsigned index) const { return (index < _inputs.size()) ? &_inputs.at(index) : nullptr; }
    unsigned short getInputCount() const { return _inputs.size(); }

	virtual bool genGeoBlock(const Geo_Actor *const component, blockBytes_t* bytes) const = 0;
    virtual bool genSceneBlock(const Topl_Scene *const scene, const Topl_Camera *const camera, blockBytes_t* bytes) const = 0;

private:
    std::vector<Shader_Type> _inputs; // inputs are required for vertex layout
};

/* struct Topl_ExitShader : public Topl_Shader {
    Topl_ExitShader() // Blank Constructor
		: Topl_Shader(), isTexSupport(false), isMatSupport(false) {}
    Topl_ExitShader(std::string& fileSrc) // Named Constructor
        : Topl_Shader(SHDR_Fragment, fileSrc), isTexSupport(false), isMatSupport(false){}
    Topl_ExitShader(std::string& fileSrc, bool tex, bool mat) // Extended Constructor
        : Topl_Shader(SHDR_Fragment, fileSrc), isTexSupport(tex), isMatSupport(mat){}

    const bool isTexSupport; // enable for texture support
    const bool isMatSupport; // enable for material support
}; */

typedef const Topl_EntryShader* entry_shader_cptr;
typedef const Topl_Shader* shader_cptr;

// Pipeline

struct Topl_Pipeline {
    Topl_Pipeline(){}
    Topl_Pipeline(entry_shader_cptr entry){ entryShader = entry; }

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

    static unsigned getSize(enum SHDR_ValueType type){
        switch(type){
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

    entry_shader_cptr entryShader = nullptr; // entryShader is saved internally
	bool isReady; // check for compilation and link status
};

#define TOPL_SHADER_H
#endif