#ifndef TOPL_RENDERER_H

#include "Platform.hpp"

#include "Topl_Pipeline.hpp"

#include "Topl_Scene.hpp"

// RenderTarget

#define SCENE_RENDER_ID 0

struct RenderTarget {
	RenderTarget() { renderID = SCENE_RENDER_ID; }
	RenderTarget(unsigned id) { renderID = id; }
	unsigned renderID;
};

// Buffer

#define BUFFERS_PER_RENDERTARGET 3

enum BUFF_Type {
    BUFF_Vertex_Type = 0, // vertex buffer type
    BUFF_Index_UI = 1, // index buffer type
    BUFF_Render_Block = 2, // render block buffer type
};

struct Buffer : public RenderTarget {
    Buffer() : RenderTarget(SCENE_RENDER_ID){}
    Buffer(unsigned id, enum BUFF_Type t) : RenderTarget(id){ type = t; }
    Buffer(unsigned id, enum BUFF_Type t, unsigned c) : RenderTarget(id){ type = t; count = c; }

    enum BUFF_Type type; // type of buffer 
    unsigned count = 1; // no. of meshes
};

// Texture

enum TEX_Frmt { TEX_2D, TEX_3D };
enum TEX_Mode { TEX_Wrap, TEX_Mirror, TEX_Clamp };

struct Texture : public RenderTarget {
	Texture() : RenderTarget(){}
	Texture(unsigned id, enum TEX_Frmt f, enum TEX_Mode m) : RenderTarget(id) {
		format = f; 
        mode = m;
		binding = 0;
	}

	Texture(unsigned id, unsigned short b, enum TEX_Frmt f, enum TEX_Mode m) : RenderTarget(id) {
		format = f;
		mode = m;
		binding = b;
	}

    enum TEX_Frmt format;
	enum TEX_Mode mode;
	unsigned binding;
};

// Viewport

struct Topl_Viewport {
    Topl_Viewport(){} // Full-Screen Constructor
    Topl_Viewport(int x, int y, int w, int h){
        xOffset = x; yOffset = y;
        width = w; height = h;
    }

    int xOffset = 0; 
	int yOffset = 0;
	int width = TOPL_WIN_WIDTH;
	int height = TOPL_WIN_HEIGHT;
};

// Renderer

enum DRAW_Mode { DRAW_Points, DRAW_Lines, DRAW_Triangles, DRAW_Fan, DRAW_Strip };

#define REGULAR_DRAW_ORDER true
#define INVERSE_DRAW_ORDER false

#define RENDER_BLOCK_INDEX 0 // uniform block index for geometry updates // hard-coded value
#define RENDER_BLOCK_BINDING 0 // uniform block binding to for geometry updates
#define SCENE_BLOCK_INDEX 1 // uniform block index for scene updates // hard-coded value
#define SCENE_BLOCK_BINDING 1 // uniform block binding to for updates

#define CLEAR_COLOR_ALPHA 0.98f // used for alpha channel clear color
#define CLEAR_COLOR_RGB 0.1f // used for rgb channels for clear color
#define CLEAR_COLOR_CODE 0xFA191919 // hexadecimal version of clear color
#define MAX_PIPELINES 24 // limits number of unique pipelines
#define MAX_SHADERS 24 * 5  // limits number of unique shaders
#define FRAME_CACHE_COUNT 32 // sets number of frames that are cached
#define FIRST_BUILD_CALL 0
#define FRAME_RATE_SECS 1.0 / 60.0
#define FRAME_RATE_MILLISECS (1.0 / 60.0) * 1000

class Topl_Renderer {
public:
	Topl_Renderer(NATIVE_WINDOW window) { _platformCtx.window = window; }

    void setCamera(const Topl_Camera* camera);
    void setPipeline(const Topl_Pipeline* pipeline);
    bool buildScene(const Topl_Scene* scene);
    bool updateScene(const Topl_Scene* scene);
	void setTexMode(enum TEX_Mode mode) { _texMode = mode; }
	bool renderAll();
    bool renderScene(const Topl_Scene* scene);
	void present(); // present scene by swapping front and back buffers
    virtual void clearView() = 0; // clears view to predefined background color
	virtual void setViewport(const Topl_Viewport* viewport) = 0; // creates a viewport
	virtual void setDrawMode(enum DRAW_Mode mode) { _drawMode = mode; }
    unsigned long getFrameCount(){ return _frameIDs; } // gets the frame count
    void texturize(const Topl_Scene* scene); // reloads all textures
#ifdef RASTERON_H
    virtual Img_Base frame() = 0;
	unsigned getPixelAt(float x, float y);
#endif
    unsigned long getRenderID(const Geo_Actor* actor);
protected:
	NATIVE_PLATFORM_CONTEXT _platformCtx; // system specific context
    entry_shader_cptr _entryShader;
    enum DRAW_Mode _drawMode = DRAW_Triangles; // mode used to draw standard scene objects
    enum TEX_Mode _texMode = TEX_Wrap; // switching texturing mode switches way textures are drawn
    Topl_Camera _defaultCamera; // identity matrix by default, no transformation
    const Topl_Camera* _activeCamera = &_defaultCamera; // camera supplied by user
    Topl_Viewport _defaultViewport = Topl_Viewport();
    bool _isBuilt = false; // switch to true when elements of the scene are built
    bool _isPipelineReady = false; // switch to true when graphics pipeline is ready
    bool _isPresented = false; // true after draw call, false after buffer swap
	bool _isDrawInOrder = REGULAR_DRAW_ORDER; // defines order of render targets during draw call
    unsigned long _renderIDs = 0; // id for each render target
    std::map<unsigned long, const Geo_Actor*> _renderTargets_map; // maps each render target to unique id
    unsigned long _frameIDs = 0; // increments with each frame drawn

	Img_Base _frameImage; // internal frame container
private:
    virtual void init(NATIVE_WINDOW window) = 0;
    virtual void build(const Topl_Scene* scene) = 0;
    virtual void update(const Topl_Scene* scene) = 0;
	virtual void renderTarget(unsigned long renderID) = 0; // draw call per render target
	virtual void swapBuffers(double frameTime) = 0;
#ifdef RASTERON_H
	virtual void attachTexture(const Rasteron_Image* image, unsigned renderID) { attachTexture(image, renderID, 0); } // attaches to default binding
	virtual void attachTexture(const Rasteron_Image* image, unsigned renderID, unsigned binding) = 0;
	virtual void attachVolume(const Img_Volume* volume, unsigned renderID) = 0;
#endif
};

#define TOPL_RENDERER_H
#endif