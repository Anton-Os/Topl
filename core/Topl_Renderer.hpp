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
    Topl_Pipeline(entry_shader_cptr entry){ entryShader = entry; }

    entry_shader_cptr entryShader = nullptr; // entryShader needs to be saved internally
	bool isReady; // internal check for compilation and link status
};

class Topl_Renderer {
public:
	virtual ~Topl_Renderer() {};

    void setCamera(const Topl_Camera* camera){ 
        if(camera != nullptr) _activeCamera = camera;
        else _activeCamera = &_defaultCamera;
    }
    void setPipeline(Topl_Pipeline* pipeline){
        _entryShader = pipeline->entryShader;
        _isPipelineReady = pipeline->isReady;
    }
    bool buildScene(const Topl_Scene* scene){
        if(!_isPipelineReady) puts("Pipeline not set for build call!");
        if(!_isPipelineReady) return false; // failure

        build(scene);
        return true; // success
    }
    bool buildScene(const Topl_Scene* scene, const Topl_Camera* camera){
		_activeCamera = camera; // switch to new camera
		return (buildScene(scene)) ? true : false;
    }
    bool updateScene(const Topl_Scene* scene){
        if(!_isPipelineReady) puts("Pipeline not set for update call!");
        if(!_isSceneReady) puts("Scene not built for update call!");
        if(!_isPipelineReady || !_isSceneReady) return false; // failure

        update(scene);
        return true; // success
    }
    bool updateScene(const Topl_Scene* scene, const Topl_Camera* camera){
        _activeCamera = camera; // switch to new camera
		return (updateScene(scene)) ? true : false;
    }
    bool renderScene(enum DRAW_Type drawType){
        if(!_isPipelineReady) puts("Pipeline not set for draw call!");
        if(!_isSceneReady) puts("Scene not built for draw call!");
        if(_renderIDs == 0) puts("No render targets for draw call!");
        if(!_isPipelineReady || !_isSceneReady || _renderIDs == 0) return false; // failure

        _drawType = drawType;
        render(); // call virtual method
        _frameIDs++; // increment frame counter
		return true; // success
    }
    virtual void clearView() = 0;
    // virtual unsigned getPixColor(float x, float y) = 0; // takes mouse coordinates as inputs
	unsigned getPixColor(float x, float y) {
		if (x < 0.0) x = 0.0; else if (x > 1.0) x = 1.0; // clamping x
		if (y < 0.0) y = 0.0; else if (y > 1.0) y = 1.0; // clamping y

		Rasteron_Image* image = frame();
		unsigned xOffset = (unsigned)(x * (float)image->width);
		unsigned yOffset = (unsigned)(y * (float)image->height);
		unsigned color = *(image->data + (yOffset * image->width) + xOffset);
		deleteImg(image);
		return color; // return color computed at offsets
	}
    // void setTexMode(enum TEX_Mode mode){ mTexMode = mode; }
#ifdef RASTERON_H
    virtual Rasteron_Image* frame() = 0;
    // May need a renderer specific texture type here // Texture should be linked to graphics object id!!!
    virtual void assignTexture(const Rasteron_Image* image, unsigned id) = 0;
#endif
    NATIVE_PLATFORM_CONTEXT _nativeContext; // Contains system specific information
protected:
    // const Topl_Pipeline* _pipeline;
    entry_shader_cptr _entryShader;
    enum DRAW_Type _drawType = DRAW_Triangles; // primitive to use to draw standard scene objects
    bool _isPipelineReady = false; // switch to true when graphics pipeline is ready
    bool _isSceneReady = false; // switch to true when elements of the scene are built
	unsigned long _renderIDs = 0; // indicator for number of drawable graphics objects
    unsigned long _frameIDs = 0; // increments with each frame drawn
    Topl_Camera _defaultCamera; // identity matrix by default, no transformation
    const Topl_Camera* _activeCamera = &_defaultCamera; // needs to be updated by user
private:
    virtual void init(NATIVE_WINDOW hwnd) = 0;
    virtual void build(const Topl_Scene* scene) = 0;
    virtual void update(const Topl_Scene* scene) = 0;
	virtual void render(void) = 0;
};

#define TOPL_RENDERER_H
#endif