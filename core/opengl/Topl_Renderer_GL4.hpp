#include "Topl_Renderer.hpp"

// Buffer

#define GL4_BUFFER_MAX 10000
struct Buffer_GL4 : public Buffer {
	Buffer_GL4(GLuint b){ buffer = b; }
	Buffer_GL4(unsigned id, enum BUFF_Type t, GLuint b) : Buffer(id, t){ buffer = b; }
	Buffer_GL4(unsigned id, enum BUFF_Type t, GLuint b, unsigned c) : Buffer(id, t, c) { buffer = b; }
	GLuint buffer; // OpenGL buffer
};

// Vertex Array Object

#define GL4_VERTEX_ARRAY_MAX 10000
struct VertexArray_GL4 : public RenderTarget {
	VertexArray_GL4() : RenderTarget() {}
	VertexArray_GL4(unsigned id, GLuint v) : RenderTarget(id){ vao = v; }

	GLuint vao;
};

// Texture

#define GL4_TEXTURE_BINDINGS_MAX 10000
struct Texture_GL4 : public Texture {
	Texture_GL4() : Texture() {}
	Texture_GL4(unsigned id, enum TEX_Frmt f, enum TEX_Mode m, GLuint t) : Texture(id, f, m) { texture = t; }
	Texture_GL4(unsigned id, MATERIAL_Property b, enum TEX_Frmt f, enum TEX_Mode m, GLuint t) : Texture(id, b, f, m) { texture = t; }

	GLuint texture;
};

// Pipeline

struct Topl_Pipeline_GL4 : public Topl_Pipeline {
	Topl_Pipeline_GL4() : Topl_Pipeline(){}

	GLuint shaderProg; // Linked Shader Program

	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint geomShader;
	GLuint tessCtrlShader;
	GLuint tessEvalShader;
};

// Renderer

#define RENDER_BLOCK_SUPPORT 1 // render block support only
#define SCENE_BLOCK_SUPPORT 2 // render block and scene block support

class Topl_Renderer_GL4 : public Topl_Renderer {
public:
	Topl_Renderer_GL4(NATIVE_WINDOW window) : Topl_Renderer(window) { 
		_isDrawInOrder = REGULAR_DRAW_ORDER;
		init(window);
	}
	~Topl_Renderer_GL4();

	void clearView() override;
	void setViewport(const Topl_Viewport* viewport) override;
	void swapBuffers(double frameTime) override;
	void setDrawMode(enum DRAW_Mode mode) override;

	void setPipeline(Topl_Pipeline_GL4* pipeline);
	void genPipeline(Topl_Pipeline_GL4* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader);
	void genPipeline(Topl_Pipeline_GL4* pipeline, entry_shader_cptr vertexShader, shader_cptr geomShader, shader_cptr pixelShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader);
#ifdef RASTERON_H
	Img_Base frame() override;
#endif
protected:
  	void init(NATIVE_WINDOW window) override;
	void build(const Topl_Scene* scene) override;
	void update(const Topl_Scene* scene) override;
	void renderTarget(unsigned long renderID) override;
#ifdef RASTERON_H
	// void attachTexture(const Rasteron_Image* image, unsigned id) override;
	void attachTextureUnit(const Rasteron_Image* image, unsigned renderID, unsigned binding) override;
	void attachVolume(const Img_Volume* material, unsigned id) override;
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
