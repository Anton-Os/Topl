#ifndef TOPL_RENDERER_H

#include "Platform.hpp"

#include "Topl_Pipeline.hpp"

#include "Topl_Scene.hpp"

// RenderObj

#define SCENE_RENDERID 0

struct RenderObj {
	RenderObj() { renderID = SCENE_RENDERID; }
	RenderObj(unsigned id) { renderID = id; }
	unsigned renderID;
};

// Buffer

#define BUFFERS_PER_TARGET 3

enum BUFF_Type {
    BUFF_Vertex_Type = 0, // vertex buffer type
    BUFF_Index_UI = 1, // index buffer type
    BUFF_Render_Block = 2, // render block buffer type
};

struct Buffer : public RenderObj {
    Buffer() : RenderObj(SCENE_RENDERID){}
    Buffer(unsigned id, enum BUFF_Type t) : RenderObj(id){ type = t; }
    Buffer(unsigned id, enum BUFF_Type t, unsigned c) : RenderObj(id){ type = t; count = c; }

    enum BUFF_Type type; // type of buffer 
    unsigned count = 1; // no. of meshes
};

// Texture

enum TEX_Frmt { TEX_2D, TEX_3D };
enum TEX_Mode { TEX_Wrap, TEX_Mirror, TEX_Clamp };

struct Texture : public RenderObj {
	Texture() : RenderObj(){}
	Texture(unsigned id, enum TEX_Frmt f, enum TEX_Mode m) : RenderObj(id) {
		format = f; mode = m; binding = 0;
	}

	Texture(unsigned id, unsigned short b, enum TEX_Frmt f, enum TEX_Mode m) : RenderObj(id) {
		format = f; mode = m; binding = b;
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

#define DRAW_NORMAL true
#define DRAW_INVERSE false

#define RENDER_BLOCK_BINDING 0 // uniform block binding to for geometry updates
#define SCENE_BLOCK_BINDING 1 // uniform block binding to for updates
#define EXT_BLOCK_BINDING 2 // uniform block for extended functionality and geometry

#define ALL_SCENES nullptr // works for updating everything
#define MAX_RENDERID 256 * 256 // * 256
#define MAX_PIPELINES 24 // limits number of unique pipelines
#define MAX_SHADERS 24 * 5  // limits number of unique shaders
#define FRAME_CACHE_COUNT 32 // sets number of frames that are cached
#define FRAME_RATE_SECS 1.0 / 60.0
#define FRAME_RATE_MILLISECS (1.0 / 60.0) * 1000
#define INVALID_RENDERID (unsigned long)- 1

#define CLEAR_COLOR_CODE 0xFF000000// 0xFF4A412A // hexadecimal version of clear color
#define CLEAR_R 0.0F // 0.290196F // red clear color code
#define CLEAR_G 0.0F // 0.254902F // green clear color code
#define CLEAR_B 0.0F // 0.164706F // blue clear color code
#define CLEAR_A 1.0F // used for alpha channel clear color

#define BUILD_BIT 0  // switch to true when build operation succeeds in _flags
#define PIPELINE_BIT 1 // switch to true when pipeline creation succeeds in _flags
#define DRAWN_BIT 2 // switch to true after draw call and false after front/back buffer swap in _flags
#define DRAW_ORDER_BIT 3 // defines order to render targets during draw call in _flags

class Topl_Renderer {
public:
	Topl_Renderer(NATIVE_PLATFORM_CONTEXT* context){ _platformCtx = context; }

    void setCamera(const Topl_Camera* camera);
    void setPipeline(const Topl_Pipeline* pipeline);
    bool buildScene(const Topl_Scene* scene);
    bool updateScene(const Topl_Scene* scene);
	void setTexMode(enum TEX_Mode mode) { _texMode = mode; }
    bool drawScene(const Topl_Scene* scene);
	void present(); // present scene by swapping front and back buffers
    virtual void draw(const Geo_Actor* actor) = 0; // draw call per render target
    virtual void clear() = 0; // clears view to predefined background color
	virtual void setViewport(const Topl_Viewport* viewport) = 0; // creates a viewport
	virtual void setDrawMode(enum DRAW_Mode mode) { _drawMode = mode; }
    unsigned long getFrameCount(){ return _frameIDs; } // gets the frame count
    void texturizeScene(const Topl_Scene* scene); // reloads textures from scene
#ifdef RASTERON_H
    virtual Img_Base frame() = 0;
	unsigned getPixelAt(float x, float y);
#endif
    unsigned long getRenderID(const Geo_Actor* actor);
protected:
    unsigned long _renderIDs = 0; // id for each render target
    unsigned long _frameIDs = 0; // increments with each frame drawn
    std::map<unsigned long, const Geo_Actor*> _renderObjMap; // maps each render target to unique id
    std::map<const Geo_Actor*, unsigned long> _renderTargetMap; // maps each object to renderID
    std::map<const TaggedImg*, std::string> _texTagMap; // tracks textures to refresh

    std::bitset<4> _flags; // tracks important states within renderer
    std::thread _threads[4]; // worker threads for allowing concurrent operations
	NATIVE_PLATFORM_CONTEXT* _platformCtx; // system specific context
    entry_shader_cptr _entryShader; // entry shader specifies vertex layout and uniforms passed to shader
    blockBytes_t _sceneBlockData, _actorBlockData, _meshBlockData; // shader block data contains bytes passed to GPU
    enum DRAW_Mode _drawMode = DRAW_Triangles; // mode used to draw standard scene objects
    enum TEX_Mode _texMode = TEX_Wrap; // switching texturing mode switches way textures are drawn
    Topl_Camera _defaultCamera; // identity matrix by default, no transformation
    const Topl_Camera* _activeCamera = &_defaultCamera; // camera supplied by userunsigned int numThreads = std::thread::hardware_concurrency();
    Topl_Viewport _activeViewport = Topl_Viewport();

	Img_Base _frameImage; // internal frame container
private:
    virtual void init(NATIVE_WINDOW window) = 0;
    virtual void build(const Geo_Actor* actor) = 0;
    virtual void update(const Geo_Actor* actor) = 0;
	virtual void swapBuffers(double frameTime) = 0;
#ifdef RASTERON_H
	virtual void attachTex(const Rasteron_Image* image, unsigned renderID) { attachTexAt(image, renderID, 0); } // attaches to default binding
	virtual void attachTexAt(const Rasteron_Image* image, unsigned renderID, unsigned binding) = 0;
	virtual void attachTex3D(const Img_Volume* volumeTex, unsigned renderID) = 0;
#endif
};

#define TOPL_RENDERER_H
#endif
