enum SHDR_Type {
    SHDR_Vertex,
    SHDR_Fragment,
    SHDR_Compute, // Compute shader, implement later
    SHDR_Tess // Tesselation shader, implement later
};


// Shaders need to be aware of their imput layouts and have some default value prepared

struct Topl_Shader {
    Topl_Shader(enum SHDR_Type type, const char* filePath){
        mShaderType = type;
        mShaderSrcPath = filePath;
    }
    enum SHDR_Type getType(){ return mShaderType; }
    const char* getFilePath(){ return mShaderSrcPath; }
private:
    enum SHDR_Type mShaderType;
    const char* mShaderSrcPath;
};