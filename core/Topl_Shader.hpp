#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "composites/Geo_Component.hpp"

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

typedef void (*bytesFromBlockCallback)(const Geo_Component*, std::vector<uint8_t>*);

struct Shader_Input {
    Shader_Input(const std::string& n, SHDR_ValueType t){
        name = n;
        type = t;
    }
    std::string name;
    SHDR_ValueType type;
};

class Topl_Shader {
public:
    // Basic constructor, DEPRICATE!
    Topl_Shader(enum SHDR_Type type, const char* filePath){
        mShaderType = type;
        mShaderSrcPath = filePath;
    }
    // Extensible constructor
    Topl_Shader(enum SHDR_Type type, const char* filePath, std::initializer_list<Shader_Input> inputs){
        mShaderType = type;
        mShaderSrcPath = filePath;
        for(std::initializer_list<Shader_Input>::iterator currentInput = inputs.begin(); currentInput < inputs.end(); currentInput++)
            mInputs.push_back(*currentInput);
    }
    // Extensible constructor, with custom block callback
    Topl_Shader(enum SHDR_Type type, const char* filePath, std::initializer_list<Shader_Input> inputs, bytesFromBlockCallback callback){
        mShaderType = type;
        mShaderSrcPath = filePath;
        mCallback = callback; 
        for(std::initializer_list<Shader_Input>::iterator currentInput = inputs.begin(); currentInput < inputs.end(); currentInput++)
            mInputs.push_back(*currentInput);
    }
    bool getIsCallback() const { return (mCallback != nullptr) ? true : false; }
    void execCallback(const Geo_Component* component, std::vector<uint8_t>* bytes) const { mCallback(component, bytes); }
    enum SHDR_Type getType() const { return mShaderType; }
    const char* getFilePath() const { return mShaderSrcPath; }
private:
    std::vector<Shader_Input> mInputs;
    enum SHDR_Type mShaderType;
    const char* mShaderSrcPath;
    bytesFromBlockCallback mCallback = nullptr; // Set to callback if shader recives info between draw calls
};