#include <cstdio>

#include "native_os_def.h"

#include "Topl_Shader.hpp"
#include "Topl_SceneManager.hpp"

typedef const Topl_Shader* const topl_shader_cptr;

enum DRAW_Type {
    DRAW_Points,
    DRAW_Lines,
    DRAW_Triangles, // Default
    DRAW_Fan,
    DRAW_Strip
};

struct GraphicsTargetObject {
	GraphicsTargetObject() {}
	GraphicsTargetObject(unsigned id) { targetID = id; }
	unsigned targetID;
};

#define MAX_BUFFERS_PER_TARGET 3 // KEEP THIS UPDATED ALWAYS!

enum BUFF_Type {
    BUFF_Vertex_Type = 0, // custom vertex format
    BUFF_Index_UI = 1, // unsigned int Index Type
    BUFF_Const_Block = 2, // stores 3F offset
};

struct Buffer : public GraphicsTargetObject {
    Buffer() : GraphicsTargetObject(){}
    Buffer(unsigned id, enum BUFF_Type t) : GraphicsTargetObject(id){
        type = t;
    }
    Buffer(unsigned id, enum BUFF_Type t, unsigned c) : GraphicsTargetObject(id){
        type = t; count = c;
    }
    enum BUFF_Type type; // Type of buffer 
    unsigned count = 1; // No. of primitives
};

#define MAX_TEXTURES_PER_TARGET 12

enum TEX_Frmt {
    TEX_1D,
    TEX_2D,
    TEX_3D
};

enum TEX_Mode {
	TEX_Wrap,
	TEX_Mirror,
	TEX_Clamp
};

struct Texture : public GraphicsTargetObject {
	Texture() : GraphicsTargetObject(){}
	Texture(unsigned id, enum TEX_Frmt f, enum TEX_Mode m) : GraphicsTargetObject(id) {
		format = f; mode = m;
	}
		// Additional data fields when needed and Derived texture object types
    enum TEX_Frmt format;
	enum TEX_Mode mode;
};

class Topl_Renderer {
public:
	virtual ~Topl_Renderer() {};

    void setPipeline(const Topl_Shader* vertexShader, const Topl_Shader* fragShader){
        // TODO: Improve code to perform checks on mShaders
        // TODO: Complete code here to be efficient

        mShaders.push_back(vertexShader);
        mShaders.push_back(fragShader);
        pipeline(vertexShader, fragShader);
    }
    virtual void buildScene(const Topl_SceneManager* sMan) = 0;
    bool updateScene(const Topl_SceneManager* sMan){
        if(!mPipelineReady) puts("Pipeline not set for update call");
        if(!mSceneReady) puts("Scene not built for update call!");
        if(!mPipelineReady || !mSceneReady) return false;

        update(sMan);
    }
    bool renderScene(enum DRAW_Type drawType){
        if(!mPipelineReady) puts("Pipeline not set for draw call!");
        if(!mSceneReady) puts("Scene not built for draw call!");
        if(!mPipelineReady || !mSceneReady) return false; // Rendering failed

        mDrawType = drawType;
        render(); // Call virtual method
		return true; // Randering success
    }
	
	NATIVE_PLATFORM_ELEM m_native; // Native Platform Element required to create a renderer

#ifdef RASTERON_H
    virtual Rasteron_Image* getFrame() = 0;
    // May need a renderer specific texture type here // Texture should be linked to graphics object id!!!
    virtual void genTexture(const Rasteron_Image* image, unsigned id) = 0;
#endif

protected:
    std::vector<const Topl_Shader*> mShaders;
    bool mPipelineReady = false; // Switch to true when graphics pipeline is ready
    bool mSceneReady = false; // Switch to true when elements of the scene are built
	unsigned mMainGraphicsIDs = 1; // Indicator for number of drawable graphics objects
    enum DRAW_Type mDrawType = DRAW_Triangles; // Primitive to use to draw standard scene objects
private:
    virtual void init(NATIVE_WINDOW hwnd) = 0;
    virtual void pipeline(const Topl_Shader* vertexShader, const Topl_Shader* fragShader) = 0;
    virtual void update(const Topl_SceneManager* sMan) = 0;
	virtual void render(void) = 0;
};
