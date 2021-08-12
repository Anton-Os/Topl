#ifndef TOPL_RENDERER_H

#include "native_os_def.h"

#include "Rasteron.h" // For texturing support, should be conditional

#include "Topl_Shader.hpp"
#include "Topl_Scene.hpp"

typedef const Topl_Shader* topl_shader_cptr;

#define SPECIAL_SCENE_RENDER_ID 0

struct RenderableTarget {
	RenderableTarget() { targetID = SPECIAL_SCENE_RENDER_ID; }
	RenderableTarget(unsigned id) { targetID = id; }
	unsigned targetID;
};

#define MAX_BUFFERS_PER_TARGET 3 // KEEP THIS UPDATED ALWAYS!

enum BUFF_Type {
    BUFF_Vertex_Type = 0, // custom vertex format
    BUFF_Index_UI = 1, // unsigned int Index Type
    BUFF_Renderable_Block = 2, // stores constants per draw object
};

struct Buffer : public RenderableTarget {
    Buffer() : RenderableTarget(SPECIAL_SCENE_RENDER_ID){}
    Buffer(unsigned id, enum BUFF_Type t) : RenderableTarget(id){ type = t; }
    Buffer(unsigned id, enum BUFF_Type t, unsigned c) : RenderableTarget(id){ type = t; count = c; }

    enum BUFF_Type type; // Type of buffer 
    unsigned count = 1; // No. of primitives
};

#define TOPL_SINGLE_BLOCK_COUNT 1 // For singly supported block, no scene uniform data
#define TOPL_FULL_BLOCK_COUNT 2 // Number of fully supported uniform blocks
#define RENDER_BLOCK_INDEX 0 // Uniform block index for geometry updates // hard-coded value
#define RENDER_BLOCK_BINDING 0 // Uniform block binding to for geometry updates
#define SCENE_BLOCK_INDEX 1 // Uniform block index for scene updates // hard-coded value
#define SCENE_BLOCK_BINDING 1 // Uniform block binding to for updates

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

struct Texture : public RenderableTarget {
	Texture() : RenderableTarget(){}
	Texture(unsigned id, enum TEX_Frmt f, enum TEX_Mode m) : RenderableTarget(id) {
		format = f; mode = m;
	}
	// Additional data fields when needed and Derived texture object types
    enum TEX_Frmt format;
	enum TEX_Mode mode;
};

enum DRAW_Type {
	DRAW_Points,
	DRAW_Lines,
	DRAW_Triangles, // Default
	DRAW_Fan,
	DRAW_Strip
};

class Topl_Renderer {
public:
	virtual ~Topl_Renderer() {};

    // Basic pipeline creation
    void setPipeline(topl_shader_cptr vertexShader, topl_shader_cptr fragShader){
        _shaders.clear(); // Reset the pipeline values
        if(vertexShader->getType() != SHDR_Vertex || fragShader->getType() != SHDR_Fragment) return;

        _shaders.push_back(vertexShader);
        _shaders.push_back(fragShader);
        pipeline(vertexShader, fragShader);
    }
    // Extended Pipeline Creation
    void setPipeline(topl_shader_cptr vertexShader, topl_shader_cptr fragShader, topl_shader_cptr tessCtrlShader, topl_shader_cptr tessEvalShader, topl_shader_cptr geomShader){
        _shaders.clear(); // Reset the pipeline values
        if(vertexShader->getType() != SHDR_Vertex || fragShader->getType() != SHDR_Fragment || tessCtrlShader->getType() != SHDR_TessCtrl || tessEvalShader->getType() != SHDR_TessEval || geomShader->getType() != SHDR_Geom) return;

        _shaders.push_back(vertexShader);
        _shaders.push_back(fragShader);
        _shaders.push_back(tessCtrlShader);
        _shaders.push_back(tessEvalShader);
        _shaders.push_back(geomShader);
        pipeline(vertexShader, fragShader, tessCtrlShader, tessEvalShader, geomShader);
    }
    // void setTexMode(enum TEX_Mode mode){ mTexMode = mode; }
    bool updateScene(const Topl_Scene* scene){
        if(!_isPipelineReady) puts("Pipeline not set for update call");
        if(!_isSceneReady) puts("Scene not built for update call!");
        if(!_isPipelineReady || !_isSceneReady) return false;

        update(scene);
    }
    bool renderScene(enum DRAW_Type drawType){
        if(!_isPipelineReady) puts("Pipeline not set for draw call!");
        if(!_isSceneReady) puts("Scene not built for draw call!");
        if(!_isPipelineReady || !_isSceneReady) return false; // Rendering failed

        _drawType = drawType;
        render(); // Call virtual method
		return true; // Randering success
    }
    virtual void clearView()  = 0;
    virtual void buildScene(const Topl_Scene* scene) = 0;
	
	NATIVE_PLATFORM_CONTEXT _nativeContext; // Contains system specific information

#ifdef RASTERON_H
    virtual Rasteron_Image* getFrame() = 0;
    // May need a renderer specific texture type here // Texture should be linked to graphics object id!!!
    virtual void genTexture(const Rasteron_Image* image, unsigned id) = 0;
#endif

protected:
    topl_shader_cptr findShader(SHDR_Type type){
        for(std::vector<topl_shader_cptr>::iterator currentShader = _shaders.begin(); currentShader < _shaders.end(); currentShader++)
            if((*currentShader)->getType() == type) return *currentShader;
        return nullptr; // If shader is not found return null pointer
    }
    enum SHDR_Type _primaryShaderType = SHDR_Vertex; // Shader that contains relevant uniform blocks and associated virtual functions
    std::vector<topl_shader_cptr> _shaders;
    enum DRAW_Type _drawType = DRAW_Triangles; // Primitive to use to draw standard scene objects
    bool _isPipelineReady = false; // Switch to true when graphics pipeline is ready
    bool _isSceneReady = false; // Switch to true when elements of the scene are built
	unsigned _mainRenderIDs = 1; // Indicator for number of drawable graphics objects
private:
    virtual void init(NATIVE_WINDOW hwnd) = 0;
    virtual void pipeline(topl_shader_cptr vertexShader, topl_shader_cptr fragShader) = 0;
    virtual void pipeline(topl_shader_cptr vertexShader, topl_shader_cptr fragShader, topl_shader_cptr tessCtrlShader, topl_shader_cptr tessEvalShader, topl_shader_cptr geomShader) = 0;
    virtual void update(const Topl_Scene* scene) = 0;
	virtual void render(void) = 0;
};

#define TOPL_RENDERER_H
#endif