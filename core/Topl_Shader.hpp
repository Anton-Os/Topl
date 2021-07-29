#ifndef TOPL_SHADER_H

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "ValueGen.hpp"

#include "Topl_Scene.hpp"
#include "Geo_Component.hpp"

enum SHDR_Type {
    SHDR_Vertex,
    SHDR_Fragment,
    SHDR_Compute, // Compute shader, implement later
    SHDR_Geom, // Geometry shader, implement later
    SHDR_Tess // Tesselation shader, implement later
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
    std::string semantic = ""; // DirectX11 extensible 
    SHDR_ValueType type;
};

class Topl_Shader {
public:
    // Basic constructor, DEPRICATE!
    Topl_Shader(enum SHDR_Type type, const char* filePath){
        mShaderType = type;
        mShaderSrcPath = filePath;
    }
    // Extensible constructor w input values
    Topl_Shader(enum SHDR_Type type, const char* filePath, std::initializer_list<Shader_Type> inputs){
        mShaderType = type;
        mShaderSrcPath = filePath;
        for(std::initializer_list<Shader_Type>::iterator currentInput = inputs.begin(); currentInput < inputs.end(); currentInput++)
            mInputs.push_back(*currentInput);
    }

	virtual bool genPerGeoDataBlock(const Geo_Component *const component, std::vector<uint8_t>* bytes) const = 0;
    virtual bool genPerSceneDataBlock(const Topl_Scene *const scene, std::vector<uint8_t>* bytes) const = 0;

    // static ValueGen mValueGen; // Utility for converting from eigen specific types
    const Shader_Type* getInputAtIndex(unsigned index) const { return (index < mInputs.size()) ? &mInputs.at(index) : nullptr; }
    unsigned short getInputCount() const { return mInputs.size(); }
    enum SHDR_Type getType() const { return mShaderType; }
    const char* getFilePath() const { return mShaderSrcPath; }
private:
    std::vector<Shader_Type> mInputs;
    std::vector<Shader_Type> mBlockUniforms;
    enum SHDR_Type mShaderType;
    const char* mShaderSrcPath;
};

#define TOPL_SHADER_H
#endif