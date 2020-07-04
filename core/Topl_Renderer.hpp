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

struct RenderTargetObject {
	RenderTargetObject() {}
	RenderTargetObject(unsigned id) { targetID = id; }
	unsigned targetID;
};

#define MAX_BUFFER_TYPES 3

enum BUFF_Type {
    BUFF_Vertex_3F,
    BUFF_Index_UI,
    BUFF_Const_vec3f
};

struct Buffer : public RenderTargetObject {
    Buffer() : RenderTargetObject(){}
    Buffer(unsigned id, enum BUFF_Type t) : RenderTargetObject(id){
        type = t;
    }
    Buffer(unsigned id, enum BUFF_Type t, unsigned c) : RenderTargetObject(id){
        type = t; count = c;
    }
    enum BUFF_Type type; // Type of buffer 
    unsigned count = 1; // No. of primitives
};

enum UNIFORM_Type {
    UNIFORM_F,
    UNIFORM_2F,
    UNIFORM_3F,
    UNIFORM_4F,
    UNIFORM_UI,
    UNIFORM_2UI,
    UNIFORM_3UI,
    UNIFORM_4UI,
    UNIFORM_I,
    UNIFORM_2I,
    UNIFORM_3I,
    UNIFORM_4I,
    // More complex
    UNIFORM_FV,
    UNIFORM_2FV,
    UNIFORM_3FV,
    UNIFORM_4FV,
    UNIFORM_2x2M,
    UNIFORM_3x3M,
    UNIFORM_4x4M,
};

struct Uniform : public RenderTargetObject {
	Uniform() : RenderTargetObject() {}
	/* Uniform(unsigned id, enum UNIFORM_Type t, void* d, size_t s) : RenderTargetObject(id) {
		data = malloc(s); // make it the size of target data d
		*(data) = *(d); // Copying step
	}
	Uniform(unsigned id, enum UNIFORM_Type t, unsigned c, void* d, size_t s) : RenderTargetObject(id) {
		data = malloc(s); // make it the size of target data d
		*(data) = *(d); // Copying step

	}
	~Uniform() { if (data != nullptr) free(data); } */
	enum UNIFORM_Type type;
	void* data = nullptr;
	unsigned count = 1;
};

class Topl_Renderer {
public:
	virtual ~Topl_Renderer() {};

    virtual void createPipeline(const Topl_Shader* vertexShader, const Topl_Shader* fragShader) = 0;
    // virtual void genTexture(const Rasteron_Image* rstn_image) // Needs to be renderer specific
    virtual void buildScene(const Topl_SceneManager* sceneGraph) = 0;
    void updateScene(const Topl_SceneManager* sceneGraph){
        // if(!mPipelineReady) puts("Pipeline not ready");
        if(!mSceneReady){
            puts("Scene not built for update call!");
            return;
        }
        update(sceneGraph);
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
protected:
    bool mPipelineReady = false;
    bool mSceneReady = false;
    enum DRAW_Type mDrawType = DRAW_Triangles;
private:
    virtual void init(NATIVE_WINDOW hwnd) = 0;
    virtual void update(const Topl_SceneManager* sceneGraph) = 0;
	virtual void render(void) = 0;

#ifdef RASTERON_H
    virtual Rasteron_Image* getFrame() = 0;
    // May need a renderer specific texture type here
    virtual void genTexture(const Rasteron_Image*) genTexture() = 0;
#endif
};
