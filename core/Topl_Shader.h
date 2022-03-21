#ifndef TOPL_SHADER_H

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "Platform.hpp"
#include "ValueGen.hpp"

#include "Geo_Actor.hpp"
#include "Topl_Scene.hpp"

enum SHDR_Type {
    SHDR_Vertex,
    SHDR_Fragment,
    SHDR_Compute, // Compute shader, implement later
    SHDR_Geom, // Geometry shader, implement later
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

class Topl_Shader {
public:
    Topl_Shader(){} // Blank Constructor
    Topl_Shader( // Named Constructor
		enum SHDR_Type type, 
		std::string fileSrc
    ){    
        _shaderType = type;
        _shaderFileSrc = fileSrc;
		_shaderFileSrc = SHADERS_DIR + fileSrc;
		std::replace(_shaderFileSrc.begin(), _shaderFileSrc.end(), '/', '\\');
    }
    enum SHDR_Type getType() const { return _shaderType; }
    const char* getFilePath() const { return _shaderFileSrc.c_str(); }
protected:
    enum SHDR_Type _shaderType;
    std::string _shaderFileSrc; // make into const type!

	std::string genPrefix_glsl() { return "glsl/"; }
	std::string genPrefix_hlsl() { return "hlsl/"; }
};

// Primary shader contains virtual functions in order to pass uniforms
class Topl_EntryShader : public Topl_Shader {
public:
    Topl_EntryShader() : Topl_Shader(){} // Blank Constructor
    Topl_EntryShader( // Named Constructor
		enum SHDR_Type type, 
		std::string fileSrc, 
		std::initializer_list<Shader_Type> inputs
	) : Topl_Shader(type, fileSrc){ 
        for(std::initializer_list<Shader_Type>::iterator currentInput = inputs.begin(); currentInput < inputs.end(); currentInput++)
            _inputs.push_back(*currentInput); // fills input list with data
    }
    const Shader_Type* getInputAtIndex(unsigned index) const { return (index < _inputs.size()) ? &_inputs.at(index) : nullptr; }
    unsigned short getInputCount() const { return _inputs.size(); }
	virtual bool genGeoBlock(const Geo_Actor *const component, blockBytes_t* bytes) const = 0;
    virtual bool genSceneBlock(const Topl_Scene *const scene, const Topl_Camera *const camera, blockBytes_t* bytes) const = 0;

    std::vector<Shader_Type> _inputs; // inputs are required for vertex layout
};

typedef const Topl_EntryShader* entry_shader_cptr;
typedef const Topl_Shader* shader_cptr;

#define TOPL_SHADER_H
#endif