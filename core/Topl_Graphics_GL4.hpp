#include "Topl_Graphics.hpp"

#define GLEW_STATIC // OpenGL Renderer Specific
#include "GL/glew.h" // OpenGL Renderer Specific
#include <GL/gl.h> // OpenGL Renderer Specific
 #include <GL/glu.h> // OpenGL Renderer Specific

#define GL4_BUFFER_OFFSET(i) ((void*)(i))

// Buffer Object Allocation Helpers

#define GL4_BUFFER_MAX 1024

struct Buffer_GL4 : public Buffer {
	Buffer_GL4(GLuint b){ buffer = b; }
	Buffer_GL4(unsigned id, enum BUFF_Type t, GLuint b) : Buffer(id, t){ buffer = b; }
	Buffer_GL4(unsigned id, enum BUFF_Type t, GLuint b, unsigned c) : Buffer(id, t, c) { buffer = b; }
	GLuint buffer; // OpenGL buffer
};

// Vertex Array Object Allocation Helpers

#define GL4_VERTEX_ARRAY_MAX 1024

struct VertexArray_GL4 : public RenderTarget {
	VertexArray_GL4() : RenderTarget() {}
	VertexArray_GL4(unsigned id, GLuint v) : RenderTarget(id){ vao = v; }
	GLuint vao;
};

// Texture Objects

#define GL4_TEXTURE_BINDINGS_MAX 1024

struct Texture_GL4 : public Texture {
	Texture_GL4() : Texture() {}
	Texture_GL4(unsigned id, enum TEX_Frmt f, enum TEX_Mode m, GLuint t) : Texture(id, f, m) { texture = t; }
	GLuint texture;
};

// Instance Specific Containers

struct Topl_Pipeline_GL4 : public Topl_Pipeline {
	Topl_Pipeline_GL4() : Topl_Pipeline(){}
	Topl_Pipeline_GL4(entry_shader_cptr entryShader) : Topl_Pipeline(entryShader){}

	GLuint shaderProg; // Linked Shader Program

	GLuint vShader; // Vertex Shader
	GLuint fShader; // Fragment Shader
	GLuint tcShader; // Tesselation Control Shader
	GLuint teShader; // Tesselation Evaluation Shader
	GLuint gShader; // Geometry Shader
};

struct Topl_RenderContext_GL4 { // groups together data for rendering
	std::vector<Buffer_GL4> buffers;
	std::vector<VertexArray_GL4> VAOs; // vertex array objects
	std::vector<Texture_GL4> textures;
};

class Topl_Renderer_GL4 : public Topl_Renderer {
public:
	Topl_Renderer_GL4(NATIVE_WINDOW window){ init(window); }
	~Topl_Renderer_GL4();

	void clearView() override;
	void setPipeline(Topl_Pipeline_GL4* pipeline);
	void genPipeline(Topl_Pipeline_GL4* pipeline, entry_shader_cptr vertexShader, shader_cptr fragShader);
	void genPipeline(Topl_Pipeline_GL4* pipeline, entry_shader_cptr vertexShader, shader_cptr fragShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader, shader_cptr geomShader);
	void build(const Topl_Scene* scene) override;
	void build(const Topl_Scene* scene, const Topl_Camera* camera) override;
#ifdef RASTERON_H
	Rasteron_Image* frame() override;
	void assignTexture(const Rasteron_Image* image, unsigned id) override;
#endif
private:
  	void init(NATIVE_WINDOW window) override;
	void pipeline(entry_shader_cptr vertex, shader_cptr frag) override;
	void pipeline(entry_shader_cptr vertex, shader_cptr frag, shader_cptr tessCtrl, shader_cptr tessEval, shader_cptr geom) override;
	void update(const Topl_Scene* scene) override;
	void update(const Topl_Scene* scene, const Topl_Camera* camera) override;
	void render(void) override;

	Topl_RenderContext_GL4 _renderCtx;
  	Topl_Pipeline_GL4* _pipeline;
	// Topl_Pipeline_GL4 _pipeline;

	GLuint _bufferSlots[GL4_BUFFER_MAX];
	unsigned _bufferIndex = 0;
	GLuint _vertexArraySlots[GL4_VERTEX_ARRAY_MAX];
	unsigned _vertexArrayIndex = 0;
	GLuint _textureSlots[GL4_TEXTURE_BINDINGS_MAX];
	unsigned _textureIndex = 0;
};
