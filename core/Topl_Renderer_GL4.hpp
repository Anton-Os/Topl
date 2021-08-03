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

struct VertexArray_GL4 : public RenderableTarget {
	VertexArray_GL4() : RenderableTarget() {}
	VertexArray_GL4(unsigned id, GLuint v) : RenderableTarget(id){ vao = v; }
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

	// GLenum allocFrmt;
	GLuint slots[GL4_TEXTURE_BINDINGS_MAX];
	unsigned slotIndex = 0;
};

struct Topl_Pipeline_GL4 {
	GLuint vertexDataLayouts[GL4_VERTEX_ARRAY_MAX];
	unsigned layoutIndex = 0;

	GLuint vShader; // Vertex Shader
	GLuint fShader; // Fragment Shader
	GLuint shaderProg;
};

class Topl_Renderer_GL4 : public Topl_Renderer {
public:
	Topl_Renderer_GL4(NATIVE_WINDOW window){ init(window); }
	~Topl_Renderer_GL4();

	void clearView() override;
	void buildScene(const Topl_Scene* scene) override;

#ifdef RASTERON_H
	Rasteron_Image* getFrame() override;
	void genTexture(const Rasteron_Image* image, unsigned id) override;
#endif
private:
  	void init(NATIVE_WINDOW window) override;
	void pipeline(const Topl_Shader* vertexShader, const Topl_Shader* fragShader) override;
	void pipeline(topl_shader_cptr vertexShader, topl_shader_cptr fragShader, topl_shader_cptr tessCtrlShader, topl_shader_cptr tessEvalShader, topl_shader_cptr geomShader, topl_shader_cptr compShader) override;
	void update(const Topl_Scene* scene) override;
	void render(void) override;

  	Topl_Pipeline_GL4 _pipeline;

	Topl_BufferAlloc_GL4 _bufferAlloc; // Buffer allocator object
	std::vector<Buffer_GL4> _buffers;
	Topl_VertexArrayAlloc_GL4 _vertexArrayAlloc; // Vertex Array allocator object
	std::vector<VertexArray_GL4> _VAOs; // Vertex Array Objects

	Topl_TextureBindingAlloc_GL4 _textureBindingsAlloc; // Texture bindings allocator object
	std::vector<Texture_GL4> _textures;
};
