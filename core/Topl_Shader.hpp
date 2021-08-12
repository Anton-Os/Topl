#ifndef TOPL_SHADER_H

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "Platform.hpp"
#include "ValueGen.hpp"

#include "Topl_Scene.hpp"
#include "Geo_Component.hpp"

enum SHDR_Type {
    SHDR_Vertex,
    SHDR_Fragment,
    SHDR_Compute, // Compute shader, implement later
    SHDR_Geom, // Geometry shader, implement later
    SHDR_TessCtrl,
    SHDR_TessEval
};

enum SHDR_ValueType {
    SHDR_float_vec4,
    SHDR_float_vec3,
    SHDR_float_vec2,
    SHDR_float,
    SHDR_double_vec4,
    SHDR_double_vec3,
    SHDR_double_vec2,
    SHDR_double,
    SHDR_int_vec4,
    SHDR_int_vec3,
    SHDR_int_vec2,
    SHDR_int,
    SHDR_uint_vec4,
    SHDR_uint_vec3,
    SHDR_uint_vec2,
    SHDR_uint,
    // Skip the boolean vector types for now
    SHDR_bool,
    SHDR_matrix_2x2, // 2 by matrices
    SHDR_matrix_2x3,
    SHDR_matrix_2x4,
    SHDR_matrix_3x2, // 3 by matrices
    SHDR_matrix_3x3,
    SHDR_matrix_3x4,
    SHDR_matrix_4x2, // 4 by matrices
    SHDR_matrix_4x3,
    SHDR_matrix_4x4,
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
    // Basic Input Value Constructor
    Topl_Shader(
		enum SHDR_Type type, 
		std::string fileSrc, 
		std::initializer_list<Shader_Type> inputs){
            
        _shaderType = type;
        _shaderFileSrc = fileSrc;
		_shaderFileSrc = SHADERS_DIR + fileSrc;
		std::replace(_shaderFileSrc.begin(), _shaderFileSrc.end(), '/', '\\');
        for(std::initializer_list<Shader_Type>::iterator currentInput = inputs.begin(); currentInput < inputs.end(); currentInput++)
            _inputs.push_back(*currentInput);
    }
    // Platform support variable constructor
	Topl_Shader(
		const Platform* platform,
		enum SHDR_Type type, 
		std::string fileSrc,
		std::initializer_list<Shader_Type> inputs){

		_platform_cptr = platform;
        _shaderType = type;
        _shaderFileSrc = SHADERS_DIR + fileSrc;
		std::replace(_shaderFileSrc.begin(), _shaderFileSrc.end(), '/', '\\');
        for(std::initializer_list<Shader_Type>::iterator currentInput = inputs.begin(); currentInput < inputs.end(); currentInput++)
            _inputs.push_back(*currentInput);
    }

	virtual bool genPerGeoDataBlock(const Geo_Component *const component, std::vector<uint8_t>* bytes) const = 0;
    virtual bool genPerSceneDataBlock(const Topl_Scene *const scene, std::vector<uint8_t>* bytes) const = 0;

    // static ValueGen mValueGen; // Utility for converting from eigen specific types
    const Shader_Type* getInputAtIndex(unsigned index) const { return (index < _inputs.size()) ? &_inputs.at(index) : nullptr; }
    unsigned short getInputCount() const { return _inputs.size(); }
    enum SHDR_Type getType() const { return _shaderType; }
    const char* getFilePath() const { return _shaderFileSrc.c_str(); }
protected:
	const Platform* _platform_cptr = nullptr;
	std::string genPrefix_glsl() { return "glsl/"; }
	std::string genPrefix_hlsl() { return "hlsl/"; }
private:
    std::vector<Shader_Type> _inputs;
    std::vector<Shader_Type> _blockUniforms;
    enum SHDR_Type _shaderType;
    std::string _shaderFileSrc; // make into const type!
};

#define TOPL_SHADER_H
#endif