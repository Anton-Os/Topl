#include <cstdio>

#include "native_os_def.h"

#include "Topl_Shader.hpp"
#include "Topl_SceneManager.hpp"

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

#define MAX_BUFFERS_PER_TARGET 4 // KEEP THIS UPDATED ALWAYS!

enum BUFF_Type {
    BUFF_Vertex_Type = 0, // custom vertex format
    BUFF_Index_UI = 1, // unsigned int Index Type
    BUFF_Const_off_3F = 2, // stores 3F offset
	BUFF_Const_rot_2F = 3 // stores 2F rotation
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

    virtual void createPipeline(const Topl_Shader* vertexShader, const Topl_Shader* fragShader) = 0;
    virtual void buildScene(const Topl_SceneManager* sMan) = 0;
    void updateScene(const Topl_SceneManager* sMan){
        // if(!mPipelineReady) puts("Pipeline not ready");
        if(!mSceneReady){
            puts("Scene not built for update call!");
            return;
        }
        update(sMan);
    }
    bool renderScene(enum DRAW_Type drawType){
        if(!mPipelineReady) puts("Pipeline not ready for draw call!");
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
    bool mPipelineReady = false; // Switch to true when graphics pipeline is ready
    bool mSceneReady = false; // Switch to true when elements of the scene are built
	unsigned mMainGraphicsIDs = 1; // Indicator for number of graphics objects
    bool mDrawSupports = true; // Draws physics components and additional supports when true
    unsigned mSupportsGraphicsIDs = 0; // Reserved graphics ID's for physics components and additional supports
    enum DRAW_Type mDrawType = DRAW_Triangles; // Primitive to use to draw standard scene objects
private:
    virtual void init(NATIVE_WINDOW hwnd) = 0;
    virtual void update(const Topl_SceneManager* sMan) = 0;
	virtual void render(void) = 0;
};
