#include <cstdio>

#include "native_os_def.h"

#include "Topl_Shader.hpp"
#include "Topl_SceneGraph.hpp"

enum DRAW_Type {
    DRAW_Points,
    DRAW_Lines,
    DRAW_Triangles, // Default
    DRAW_Fan,
    DRAW_Strip
};

#define MAX_BUFFER_TYPES 3

enum BUFF_Type {
    BUFF_Vertex_3F,
    BUFF_Index_UI,
    BUFF_Const_vec3f
};

struct Buffer {
    unsigned targetID; // Multiple buffers share a target for binding before draw
    enum BUFF_Type type; // Type of buffer 
    unsigned count = 1; // No. of primitives
};

class Topl_Renderer {
public:
	virtual ~Topl_Renderer() {};

    virtual void createPipeline(const Topl_Shader* vertexShader, const Topl_Shader* fragShader) = 0;
    virtual void buildScene(const Topl_SceneGraph* sceneGraph) = 0;
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
    virtual void createPipeline(void) = 0; // Mark for deletion!
	virtual void render(void) = 0;
};
