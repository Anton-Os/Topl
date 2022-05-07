// OpenGL Specific Inclusions

#define GLEW_STATIC
#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/glu.h>

// Renderer Implementation

#include "Topl_Renderer.hpp"

#define GL4_BUFFER_OFFSET(i) ((void*)(i))

#define RENDER_BLOCK_SUPPORT 1 // render block support only
#define SCENE_BLOCK_SUPPORT 2 // render block and scene block support

#define GL4_BUFFER_MAX 1024
struct Buffer_GL4 : public Buffer {
	Buffer_GL4(GLuint b){ buffer = b; }
	Buffer_GL4(unsigned id, enum BUFF_Type t, GLuint b) : Buffer(id, t){ buffer = b; }
	Buffer_GL4(unsigned id, enum BUFF_Type t, GLuint b, unsigned c) : Buffer(id, t, c) { buffer = b; }
	GLuint buffer; // OpenGL buffer
};

#define GL4_VERTEX_ARRAY_MAX 1024
struct VertexArray_GL4 : public RenderTarget {
	VertexArray_GL4() : RenderTarget() {}
	VertexArray_GL4(unsigned id, GLuint v) : RenderTarget(id){ vao = v; }
	GLuint vao;
};

#define GL4_TEXTURE_BINDINGS_MAX 1024
struct Texture_GL4 : public Texture {
	Texture_GL4() : Texture() {}
	Texture_GL4(unsigned id, enum TEX_Frmt f, enum TEX_Mode m, GLuint t) : Texture(id, f, m) { texture = t; }
	GLuint texture;
};

struct Topl_Pipeline_GL4 : public Topl_Pipeline {
	Topl_Pipeline_GL4() : Topl_Pipeline(){}

	GLuint shaderProg; // Linked Shader Program

	GLuint vShader; // Vertex Shader
	GLuint fShader; // Fragment Shader
	GLuint tcShader; // Tesselation Control Shader
	GLuint teShader; // Tesselation Evaluation Shader
	GLuint gShader; // Geometry Shader
};

class Topl_Renderer_GL4 : public Topl_Renderer {
public:
	Topl_Renderer_GL4(NATIVE_WINDOW window) : Topl_Renderer(window) { 
		init(window);
	}
	Topl_Renderer_GL4(NATIVE_WINDOW window, std::initializer_list<Topl_Viewport> viewports) : Topl_Renderer(window, viewports) { 
		init(window);
	}
	~Topl_Renderer_GL4();

	void clearView() override;
	void switchFramebuff() override;
	void build(const Topl_Scene* scene) override;
	void texturize(const Topl_Scene* scene) override;

	void setPipeline(Topl_Pipeline_GL4* pipeline); // replace this!
	// void switchPipeline(const Topl_Pipeline* pipeline) override;
	void genPipeline(Topl_Pipeline_GL4* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader);
	void genPipeline(Topl_Pipeline_GL4* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader, shader_cptr geomShader);
#ifdef RASTERON_H
	Rasteron_Image* frame() override;
#endif
private:
  	void init(NATIVE_WINDOW window) override;
	// void init(NATIVE_WINDOW window, std::initializer_list<Topl_Viewport> viewports) override;
	void update(const Topl_Scene* scene) override;
	void drawMode(void) override;
	// void render(const Topl_Scene* scene) override;
	void renderTarget(unsigned long renderID) override;
#ifdef RASTERON_H
	void attachTexture(const Rasteron_Image* image, unsigned id) override;
	void attachMaterial(const Topl_Material* material, unsigned id) override;
#endif
	Buffer_GL4* findBuffer(BUFF_Type type, unsigned long renderID);

	Topl_Pipeline_GL4* _pipeline;

	GLenum _drawMode_GL4; // OpenGL specific draw mode
	GLuint _bufferSlots[GL4_BUFFER_MAX]; // stores all buffers
	unsigned _bufferIndex = 0; // increments to indicate next available buffer slot
	std::vector<Buffer_GL4> _buffers; // active buffers
	GLuint _vertexArraySlots[GL4_VERTEX_ARRAY_MAX]; // stores all vertex arrays
	unsigned _vertexArrayIndex = 0; // increments to indicate next available vertex array slot
	std::vector<VertexArray_GL4> _vertexArrays; // active vertex arrays
	GLuint _textureSlots[GL4_TEXTURE_BINDINGS_MAX]; // stores all textures
	unsigned _textureIndex = 0; // increments to indicate next available texture slot
	std::vector<Texture_GL4> _textures; // active textures
};
