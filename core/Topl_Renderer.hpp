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

#define BUFFERS_PER_RENDERTARGET 3 // Each render target has fixed number buffers, see BUFF_Type below

enum BUFF_Type {
    BUFF_Vertex_Type = 0, // vertex buffer type
    BUFF_Index_UI = 1, // index buffer type
    BUFF_Render_Block = 2, // render block buffer types
};

struct Buffer : public RenderTarget {
    Buffer() : RenderTarget(SPECIAL_SCENE_RENDER_ID){}
    Buffer(unsigned id, enum BUFF_Type t) : RenderTarget(id){ type = t; }
    Buffer(unsigned id, enum BUFF_Type t, unsigned c) : RenderTarget(id){ type = t; count = c; }

    enum BUFF_Type type; // type of buffer 
    unsigned count = 1; // no. of primitives
};

#define RENDER_BLOCK_INDEX 0 // uniform block index for geometry updates // hard-coded value
#define RENDER_BLOCK_BINDING 0 // uniform block binding to for geometry updates
#define SCENE_BLOCK_INDEX 1 // uniform block index for scene updates // hard-coded value
#define SCENE_BLOCK_BINDING 1 // uniform block binding to for updates

#define MAX_TEXTURES_PER_TARGET 12 // corresponds to MAX_TEXTURES_PER_ACTOR in Topl_Scene.hpp
#define MAX_RENDERER_CONTEXTS 24 // limits number of unique scenes and render contexts to 24

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
		format = f; 
        mode = m;
	}
	// Additional data fields when needed and Derived texture object types
    enum TEX_Frmt format;
	enum TEX_Mode mode;
};

enum DRAW_Mode {
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

struct Topl_RenderContext {
    Topl_RenderContext(const Topl_Scene *const s) : scene(s) { // Rigid Scene Constructor
        renderIDs = (unsigned long*)malloc(scene->getActorCount());
    }
    Topl_RenderContext(const Topl_Scene *const s, unsigned idCount) : scene(s) { // Dynamic Scene Constructor
        renderIDs = (unsigned long*)malloc(idCount);
    }
    ~Topl_RenderContext(){
        if(renderIDs != nullptr) free(renderIDs);
        renderIDs = nullptr;
    }
    const Topl_Scene *const scene;
    unsigned long* renderIDs = nullptr; // render ids associated with scene object
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
        if(!_isPipelineReady){
            perror("Pipeline not set for build call!");
            _isSceneReady = false;
            return false; // failure
        }
        if(_renderCtxIndex >= MAX_RENDERER_CONTEXTS) {
            perror("Too many render contexts!");
            // _renderCtxIndex = 0;
            _isSceneReady = false;
            return false;
        }

        build(scene);
        texturize(scene);
        _renderCtxIndex++;
        return _isSceneReady;
    }
    bool buildScene(const Topl_Scene* scene, const Topl_Camera* camera){
		_activeCamera = camera; // switch to new camera
		return (buildScene(scene)) ? true : false;
    }
    bool updateScene(const Topl_Scene* scene){
        if(!_isPipelineReady) perror("Pipeline not set for update call!");
        if(!_isSceneReady) perror("Scene not built for update call!");
        if(!_isPipelineReady || !_isSceneReady) return false; // failure

        update(scene);
        return _isSceneReady;
    }
    bool updateScene(const Topl_Scene* scene, const Topl_Camera* camera){
        _activeCamera = camera; // switch to new camera
		return (updateScene(scene)) ? true : false;
    }
    void setDrawMode(enum DRAW_Mode mode){
        _drawMode = mode;
        drawMode(); // sets the proper draw mode
    }
	void setTexMode(enum TEX_Mode mode) { _texMode = mode; }
    bool renderScene(Topl_Scene* scene){
		if (!_isPipelineReady) perror("Pipeline not set for draw call!");
		if (!_isSceneReady) perror("Scene not built for draw call!");
		if (_renderIDs == 0) perror("No render targets for draw call!");
		if (!_isPipelineReady || !_isSceneReady || _renderIDs == 0) {
			_isSceneDrawn = false;
			return false; // failure
		}

		render(scene);
		_frameIDs++; // increment frame counter
		return _isSceneDrawn; // render call sets variable to true on success
    }
    /* bool renderAll(){ // draws all render objects
        if(!_isPipelineReady) perror("Pipeline not set for draw call!");
        if(!_isSceneReady) perror("Scene not built for draw call!");
        if(_renderIDs == 0) perror("No render targets for draw call!");
        if(!_isPipelineReady || !_isSceneReady || _renderIDs == 0){
            _isSceneDrawn = false;
            return false; // failure
        }

		render(nullptr);
        _frameIDs++; // increment frame counter
		return _isSceneDrawn; // render call sets variable to true on success
    } */
    virtual void clearView() = 0; // clears view to predefined background color
    virtual void switchFramebuff() = 0; // switches front and back buffers
	virtual void texturize(const Topl_Scene* scene) = 0; // loads all textures
#ifdef RASTERON_H
    virtual Rasteron_Image* frame() = 0;
	unsigned getPixColor(float x, float y) {
		if (x < -1.0) x = -1.0; else if (x > 1.0) x = 1.0; // clamping x
		if (y < -1.0) y = -1.0; else if (y > 1.0) y = 1.0; // clamping y

		Rasteron_Image* image = frame();
		/* unsigned xOffset = (unsigned)(x * (float)image->width);
		unsigned yOffset = (unsigned)(y * (float)image->height);
		unsigned color = *(image->data + (yOffset * image->width) + xOffset); */
		unsigned offset = (image->width * image->height) / 2; // get offset halfway for testing
		unsigned color = *(image->data + offset);
		deleteImg(image);
		return color; // return color computed at offsets
	}
#endif
protected:
	NATIVE_PLATFORM_CONTEXT _platformCtx; // system specific variables
    // const Topl_Pipeline* _pipeline;
    entry_shader_cptr _entryShader;
    enum DRAW_Mode _drawMode = DRAW_Triangles; // mode used to draw standard scene objects
    enum TEX_Mode _texMode = TEX_Wrap; // switching texturing mode switches way textures are drawn
    unsigned _renderCtxIndex = 0; // tracks the render context in use
    Topl_Camera _defaultCamera; // identity matrix by default, no transformation
    const Topl_Camera* _activeCamera = &_defaultCamera; // updated by user
    bool _isPipelineReady = false; // switch to true when graphics pipeline is ready
    bool _isSceneReady = false; // switch to true when elements of the scene are built
    bool _isSceneDrawn = false; // true after draw call, false after framebuffer swap
    unsigned long _renderIDs = 0; // indicator for number of drawable graphics objects
    unsigned long _frameIDs = 0; // increments with each frame drawn
	// TODO: Add method for fetching render context based on scene
private:
    virtual void init(NATIVE_WINDOW hwnd) = 0;
    virtual void build(const Topl_Scene* scene) = 0;
    virtual void update(const Topl_Scene* scene) = 0;
    virtual void drawMode() = 0;
	virtual void render(const Topl_Scene* scene) = 0;
#ifdef RASTERON_H
	virtual void attachTexture(const Rasteron_Image* image, unsigned id) = 0;
	virtual void attachMaterial(const Topl_Material* material, unsigned id) = 0;
#endif
};

#define TOPL_RENDERER_H
#endif