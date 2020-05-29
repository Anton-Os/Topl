#include <cstdlib>
#include <cstring>

#include <vector>

enum SHDR_Type {
    SHDR_Vertex,
    SHDR_Fragment,
    SHDR_Compute, // Compute shader, implement later
    SHDR_Tess // Tesselation shader, implement later
};

enum SHDR_Input_Type {
    SHDR_In,
    SHDR_Out,
    SHDR_Const
};

typedef void* (*allocDefaultData)(void); // Default data needs memory allocation within the Shader_Input

struct Shader_Input {
    Shader_Input(enum SHDR_Input_Type i, char* t, char* n, allocDefaultData a){ 
        inputType = i;
        strcpy(varType, t);
        strcpy(name, n);
        mAllocDefaultFunc = a;
    
        defaultData = (*mAllocDefaultFunc)();
    }
    ~Shader_Input(){
        if(defaultData != nullptr) free(defaultData); // Free default data
    }
    enum SHDR_Input_Type inputType; // Specifies how the variable is processed
    char* varType; // Shader variable data type
    char* name;
    void* defaultData = nullptr;
private:
    allocDefaultData mAllocDefaultFunc;
};

// Shaders need to be aware of their imput layouts and have some default value prepared

struct Topl_Shader {
    Topl_Shader(enum SHDR_Type type, const char* filePath){
        mShaderType = type;
        mShaderSrcPath = filePath;
    }
    enum SHDR_Type getType() const{ return mShaderType; }
    const char* getFilePath() const{ return mShaderSrcPath; }
private:
    std::vector<Shader_Input> mInputs;
    enum SHDR_Type mShaderType;
    const char* mShaderSrcPath;
};