#include "Topl_Renderer.hpp"

#define GLEW_STATIC // OpenGL Renderer Specific
#include "GL/glew.h" // OpenGL Renderer Specific

#include <GL/gl.h> // OpenGL Renderer Specific
 #include <GL/glu.h> // OpenGL Renderer Specific

#define GL4_BUFFER_OFFSET(i) ((void*)(i))

class Topl_DataAlloc_GL4 {
public:
	Topl_DataAlloc_GL4() {}
	virtual GLuint getAvailable() = 0;
protected:
	bool mIsInit = false;
	virtual void init() = 0;
};

// Buffer Object Allocation Helpers

struct Buffer_GL4 : public Buffer {
	Buffer_GL4(GLuint b){ buffer = b; }
	Buffer_GL4(unsigned id, enum BUFF_Type t, GLuint b) : Buffer(id, t){ buffer = b; }
	Buffer_GL4(unsigned id, enum BUFF_Type t, GLuint b, unsigned c) : Buffer(id, t, c) { buffer = b; }

	GLuint buffer; // OpenGL specific
};

#define GL4_BUFFER_MAX 1024

class Topl_BufferAlloc_GL4 : public Topl_DataAlloc_GL4 { // derived class
public:
	Topl_BufferAlloc_GL4(){}
	GLuint getAvailable() override;
private:
	void init() override { glGenBuffers(GL4_BUFFER_MAX, &slots[0]); mIsInit = true; }

	GLuint slots[GL4_BUFFER_MAX];
	unsigned slotIndex = 0; // May need to be zero
};

// Vertex Array Object Allocation Helpers

struct VertexArray_GL4 : public RenderTarget {
	VertexArray_GL4() : RenderTarget() {}
	VertexArray_GL4(unsigned id, GLuint v) : RenderTarget(id){ vao = v; }
	GLuint vao;
};

#define GL4_VERTEX_ARRAY_MAX 1024

class Topl_VertexArrayAlloc_GL4 : public Topl_DataAlloc_GL4 { // derived class
public:
	Topl_VertexArrayAlloc_GL4(){}
	GLuint getAvailable() override;
private:
	void init() override { glGenVertexArrays(GL4_VERTEX_ARRAY_MAX, &slots[0]); mIsInit = true; }

	GLuint slots[GL4_VERTEX_ARRAY_MAX];
	unsigned slotIndex = 0; // May need to be zero

};

// Texture Objects

struct Texture_GL4 : public Texture {
	Texture_GL4() : Texture() {}
	Texture_GL4(unsigned id, enum TEX_Frmt f, enum TEX_Mode m, GLuint t)
		: Texture(id, f, m) { texture = t; }

	GLuint texture;
};

#define GL4_TEXTURE_BINDINGS_MAX 1024

class Topl_TextureBindingAlloc_GL4 :  public Topl_DataAlloc_GL4 { // derived class
public:
	Topl_TextureBindingAlloc_GL4(){}
	GLuint getAvailable() override;
	enum TEX_Frmt getFormat(){ return TEX_2D; } // Set to default, use switch case statement
private:
	void init() override { glGenTextures(GL4_TEXTURE_BINDINGS_MAX, &slots[0]); mIsInit = true;}

	GLuint slots[GL4_TEXTURE_BINDINGS_MAX];
	unsigned slotIndex = 0;
};

// Instance Specific Containers

struct Topl_Pipeline_GL4 {
	GLuint vertexDataLayouts[GL4_VERTEX_ARRAY_MAX];
	unsigned layoutIndex = 0;

	GLuint shaderProg; // Linked Shader Program
	GLuint vShader; // Vertex Shader
	GLuint fShader; // Fragment Shader
	GLuint tcShader; // Tesselation Control Shader
	GLuint teShader; // Tesselation Evaluation Shader
	GLuint gShader; // Geometry Shader
};

struct Topl_RenderContext_GL4 { // Groups items together to allow for switching between multiple scenes and pipelines
	const Topl_Scene* scenePtr;
	Topl_Pipeline_GL4 pipeline;
	std::vector<Buffer_GL4> buffers;
	std::vector<VertexArray_GL4> VAOs; // Vertex Array Objects
	std::vector<Texture_GL4> textures;
};

class Topl_Renderer_GL4 : public Topl_Renderer {
public:
	Topl_Renderer_GL4(NATIVE_WINDOW window){ init(window); }
	~Topl_Renderer_GL4();

	void clearView() override;
	void buildScene(const Topl_Scene* scene) override;
	void buildScene(const Topl_Scene* scene, const Topl_Camera* camera) override;
#ifdef RASTERON_H
	Rasteron_Image* getFrame() override;
	void genTexture(const Rasteron_Image* image, unsigned id) override;
#endif
private:
  	void init(NATIVE_WINDOW window) override;
	void pipeline(const Topl_PrimaryShader* vertexShader, const Topl_PrimaryShader* fragShader) override;
	void pipeline(topl_shader_cptr vertexShader, topl_shader_cptr fragShader, topl_shader_cptr tessCtrlShader, topl_shader_cptr tessEvalShader, topl_shader_cptr geomShader) override;
	void update(const Topl_Scene* scene) override;
	void update(const Topl_Scene* scene, const Topl_Camera* camera) override;
	void render(void) override;

	std::vector<Topl_RenderContext_GL4> _renderCtx; // drawable render contexts
	Topl_RenderContext_GL4* _currentRenderCtx; // current render context to be used for drawing
  	Topl_Pipeline_GL4 _pipeline;

	Topl_BufferAlloc_GL4 _bufferAlloc; // Buffer allocator object
	Topl_VertexArrayAlloc_GL4 _vertexArrayAlloc; // Vertex Array allocator object
	Topl_TextureBindingAlloc_GL4 _textureBindingsAlloc; // Texture bindings allocator object
};
