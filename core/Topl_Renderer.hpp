#ifndef TOPL_RENDERER_H

#include "native_os_def.h"

#include "Rasteron.h" // For texturing support, should be conditional

#include "Topl_Shader_Pipeline.h"
#include "Topl_Scene.hpp"

#define SPECIAL_SCENE_RENDER_ID 0
#define RENDER_BLOCK_INDEX 0 // uniform block index for geometry updates // hard-coded value
#define RENDER_BLOCK_BINDING 0 // uniform block binding to for geometry updates
#define SCENE_BLOCK_INDEX 1 // uniform block index for scene updates // hard-coded value
#define SCENE_BLOCK_BINDING 1 // uniform block binding to for updates

#define CLEAR_COLOR_ALPHA 0.98f // used for alpha channel clear color
#define CLEAR_COLOR_RGB 0.1f // used for rgb channels for clear color
#define CLEAR_COLOR_HEX 0xFA191919 // hexadecimal version of clear color
#define MAX_PIPELINES 24 // limits number of unique pipelines
#define MAX_SHADERS 24 * 5  // limits number of unique shaders
#define FRAME_CACHE_COUNT 32 // sets number of frames that are cached

struct RenderTarget {
	RenderTarget() { targetID = SPECIAL_SCENE_RENDER_ID; }
	RenderTarget(unsigned id) { targetID = id; }
	unsigned targetID;
};

#define BUFFERS_PER_RENDERTARGET 3 // each render target has fixed number buffers, see BUFF_Type below

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

enum TEX_Frmt { TEX_1D, TEX_2D, TEX_3D };
enum TEX_Mode { TEX_Wrap, TEX_Mirror, TEX_Clamp };

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

#define MAX_RENDERER_CONTEXTS 24 // max number of unique render contexts
#define MAX_RENDER_IDS 8000 // max number of ids for each context

struct Topl_RenderContext { // stores a collection fo ids used to look for render targets
    Topl_RenderContext() : scene(nullptr){} // Empty Constructor
    Topl_RenderContext(const Topl_Scene *const s) : scene(s) {} // Rigid Constructor
    Topl_RenderContext(const Topl_RenderContext& renderContext) : scene(renderContext.scene) {
        targetCount = renderContext.targetCount;
        for(unsigned r = 0; r < targetCount; r++)
            targetIDs[r] = renderContext.targetIDs[r]; // copy each element
    }
    const Topl_Scene *const scene;
    unsigned targetIDs[MAX_RENDER_IDS]; // max render targets
    unsigned targetCount = 0; // increments as render targets added
};

#define MAX_VIEWPORTS 12 // max number of separate viewports

struct Topl_Viewport {
    Topl_Viewport(){} // Full-Screen Constructor
    Topl_Viewport(unsigned x, unsigned y, unsigned w, unsigned h){
        xOffset = x; yOffset = y;
        width = w; height = h;
    }

    unsigned xOffset = 0; 
    unsigned yOffset = 0;
    unsigned width = TOPL_WIN_WIDTH;
	unsigned height = TOPL_WIN_HEIGHT;
};

class Topl_Renderer {
public:
    Topl_Renderer(NATIVE_WINDOW window);
    Topl_Renderer(NATIVE_WINDOW window, std::initializer_list<Topl_Viewport> viewports);
	virtual ~Topl_Renderer(){
        if(_viewports != nullptr) free(_viewports);
    };

    void setCamera(const Topl_Camera* camera);
    void setPipeline(const Topl_Pipeline* pipeline);
    bool buildScene(const Topl_Scene* scene);
    // bool buildScene(const Topl_Scene* scene, const Topl_Camera* camera);
    bool updateScene(const Topl_Scene* scene);
    // bool updateScene(const Topl_Scene* scene, const Topl_Camera* camera);
    void setDrawMode(enum DRAW_Mode mode);
	void setTexMode(enum TEX_Mode mode) { _texMode = mode; }
    bool renderScene(Topl_Scene* scene);
    // bool renderAll(); // implement this
    virtual void clearView() = 0; // clears view to predefined background color
    virtual void switchFramebuff() = 0; // switches front and back buffers
	virtual void texturize(const Topl_Scene* scene) = 0; // loads all textures
    unsigned long getFrameCount(){ return _frameIDs; } // gets the frame count
#ifdef RASTERON_H
    virtual Rasteron_Image* frame() = 0;
	unsigned getPixelAt(float x, float y);
    // void frameCapture(Topl_Frames* frames);
#endif
protected:
	NATIVE_PLATFORM_CONTEXT _platformCtx; // system specific context
    Topl_RenderContext* _activeRenderCtx; // active render context
    entry_shader_cptr _entryShader;
    enum DRAW_Mode _drawMode = DRAW_Triangles; // mode used to draw standard scene objects
    enum TEX_Mode _texMode = TEX_Wrap; // switching texturing mode switches way textures are drawn
    Topl_Camera _defaultCamera; // identity matrix by default, no transformation
    const Topl_Camera* _activeCamera = &_defaultCamera; // supplied by user
    Topl_Viewport* _viewports = nullptr;
    unsigned short _viewportCount = 0;
    bool _isBuilt = false; // switch to true when elements of the scene are built
    bool _isPipelineReady = false; // switch to true when graphics pipeline is ready
    bool _isDrawn = false; // true after draw call, false after swap
    unsigned long _renderIDs = 0; // indicator for number of drawable graphics objects
    unsigned long _frameIDs = 0; // increments with each frame drawn
private:
    virtual void init(NATIVE_WINDOW window) = 0;
    virtual void build(const Topl_Scene* scene) = 0;
    virtual void update(const Topl_Scene* scene) = 0;
    virtual void drawMode() = 0;
	virtual void render(const Topl_Scene* scene) = 0;
#ifdef RASTERON_H
	virtual void attachTexture(const Rasteron_Image* image, unsigned id) = 0;
	virtual void attachMaterial(const Topl_Material* material, unsigned id) = 0;

    Topl_Frames frameCache = Topl_Frames("cache", TOPL_WIN_HEIGHT, TOPL_WIN_WIDTH, FRAME_CACHE_COUNT);
#endif
    Topl_RenderContext* getRenderContext(const Topl_Scene*const scene);

    Topl_RenderContext _renderContexts[MAX_RENDERER_CONTEXTS]; // stores all render contexts
    unsigned short _renderCtxIndex = 0; // tracks the render context in use
};

#define TOPL_RENDERER_H
#endif