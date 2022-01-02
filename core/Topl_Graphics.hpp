#ifndef TOPL_RENDERER_H

#include "native_os_def.h"

#include "Rasteron.h" // For texturing support, should be conditional

#include "Topl_Shader.hpp"
#include "Topl_Scene.hpp"

#define SPECIAL_SCENE_RENDER_ID 0

struct RenderTarget {
	RenderTarget() { targetID = SPECIAL_SCENE_RENDER_ID; }
	RenderTarget(unsigned id) { targetID = id; }
	unsigned targetID;
};

#define MAX_BUFFERS_PER_TARGET 3 // KEEP THIS UPDATED ALWAYS!

enum BUFF_Type {
    BUFF_Vertex_Type = 0, // custom vertex format
    BUFF_Index_UI = 1, // unsigned int Index Type
    BUFF_Render_Block = 2, // stores constants per draw object
};

struct Buffer : public RenderTarget {
    Buffer() : RenderTarget(SPECIAL_SCENE_RENDER_ID){}
    Buffer(unsigned id, enum BUFF_Type t) : RenderTarget(id){ type = t; }
    Buffer(unsigned id, enum BUFF_Type t, unsigned c) : RenderTarget(id){ type = t; count = c; }

    enum BUFF_Type type; // type of buffer 
    unsigned count = 1; // no. of primitives
};

#define TOPL_SINGLE_BLOCK_COUNT 1 // for singly supported block, no scene uniform data
#define TOPL_FULL_BLOCK_COUNT 2 // number of fully supported uniform blocks
#define RENDER_BLOCK_INDEX 0 // uniform block index for geometry updates // hard-coded value
#define RENDER_BLOCK_BINDING 0 // uniform block binding to for geometry updates
#define SCENE_BLOCK_INDEX 1 // uniform block index for scene updates // hard-coded value
#define SCENE_BLOCK_BINDING 1 // uniform block binding to for updates

#define MAX_TEXTURES_PER_TARGET 12
#define MAX_PIPELINE_COUNT 12

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

struct Texture : public RenderTarget {
	Texture() : RenderTarget(){}
	Texture(unsigned id, enum TEX_Frmt f, enum TEX_Mode m) : RenderTarget(id) {
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

struct Topl_Pipeline {
    Topl_Pipeline(){}
    Topl_Pipeline(entry_shader_cptr entry){
        entryShader = entry;
    }
    entry_shader_cptr entryShader = nullptr; // saves entryShader internally
	bool isReady; // internal check for compilation and link status
};

class Topl_Renderer {
public:
	virtual ~Topl_Renderer() {};

    void setCamera(const Topl_Camera* camera){ _activeCamera = camera; }
    // basic pipeline creation
    void setPipeline(entry_shader_cptr vertexShader, shader_cptr fragShader){
        if(vertexShader->getType() != SHDR_Vertex || fragShader->getType() != SHDR_Fragment) return;

        _entryShader = vertexShader;
        pipeline(vertexShader, fragShader);
    }
    // extended pipeline creation
    void setPipeline(entry_shader_cptr vertexShader, shader_cptr fragShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader, shader_cptr geomShader){
        if(vertexShader->getType() != SHDR_Vertex || fragShader->getType() != SHDR_Fragment || tessCtrlShader->getType() != SHDR_TessCtrl || tessEvalShader->getType() != SHDR_TessEval || geomShader->getType() != SHDR_Geom) return;

        _entryShader = vertexShader;
        pipeline(vertexShader, fragShader, tessCtrlShader, tessEvalShader, geomShader);
    }
    // void setTexMode(enum TEX_Mode mode){ mTexMode = mode; }
    bool buildScene(const Topl_Scene* scene){
        if(!_isPipelineReady) puts("Pipeline not set for build call!");
        if(!_isPipelineReady) return false; // failure

        build(scene);
        return true; // success
    }
    bool buildScene(const Topl_Scene* scene, const Topl_Camera* camera){
        if(!_isPipelineReady) puts("Pipeline not set for build call!");
        if(!_isPipelineReady) return false; // failure

        // _activeCamera = camera;
        build(scene, camera);
        return true; // success
    }
    bool updateScene(const Topl_Scene* scene){
        if(!_isPipelineReady) puts("Pipeline not set for update call!");
        if(!_isSceneReady) puts("Scene not built for update call!");
        if(!_isPipelineReady || !_isSceneReady) return false; // failure

        update(scene);
        return true; // success
    }
    bool updateScene(const Topl_Scene* scene, const Topl_Camera* camera){
        if(!_isPipelineReady) puts("Pipeline not set for update call!");
        if(!_isSceneReady) puts("Scene not built for update call!");
        if(!_isPipelineReady || !_isSceneReady) return false; // failure

        update(scene, camera);
        return true; // success
    }
    bool renderScene(enum DRAW_Type drawType){
        if(!_isPipelineReady) puts("Pipeline not set for draw call!");
        if(!_isSceneReady) puts("Scene not built for draw call!");
        if(!_isPipelineReady || !_isSceneReady) return false; // failure

        _drawType = drawType;
        render(); // call virtual method
		return true; // success
    }
    virtual void clearView()  = 0;
#ifdef RASTERON_H
    Rasteron_Image* getFrame(){
        Rasteron_Image* frameImg = frame();
        _frameCapID++;
        return frameImg;
    }
    virtual Rasteron_Image* frame() = 0;
    // May need a renderer specific texture type here // Texture should be linked to graphics object id!!!
    virtual void assignTexture(const Rasteron_Image* image, unsigned id) = 0;
#endif
    NATIVE_PLATFORM_CONTEXT _nativeContext; // Contains system specific information
protected:
    entry_shader_cptr _entryShader;
    enum DRAW_Type _drawType = DRAW_Triangles; // primitive to use to draw standard scene objects
    bool _isPipelineReady = false; // switch to true when graphics pipeline is ready
    bool _isSceneReady = false; // switch to true when elements of the scene are built
	unsigned _mainRenderIDs = 1; // indicator for number of drawable graphics objects
    unsigned _frameCapID = 1; // increments as more frames are captured
    Topl_Camera _defaultCamera; // identity matrix by default, no transformation
    const Topl_Camera* _activeCamera = &_defaultCamera; // needs to be updated by user
private:
    virtual void init(NATIVE_WINDOW hwnd) = 0;
    virtual void pipeline(entry_shader_cptr vertexShader, shader_cptr fragShader) = 0;
    virtual void pipeline(entry_shader_cptr vertexShader, shader_cptr fragShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader, shader_cptr geomShader) = 0;
    virtual void build(const Topl_Scene* scene) = 0;
    virtual void build(const Topl_Scene* scene, const Topl_Camera* camera) = 0; // for custom camera control
    virtual void update(const Topl_Scene* scene) = 0;
    virtual void update(const Topl_Scene* scene, const Topl_Camera* camera) = 0; // for custom camera control
	virtual void render(void) = 0;
};

#define TOPL_RENDERER_H
#endif