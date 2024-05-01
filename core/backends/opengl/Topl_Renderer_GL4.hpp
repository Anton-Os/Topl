#include "Topl_Renderer.hpp"

#define GLEW_STATIC
#include "GL/glew.h"
#include <GL/gl.h>

// Buffer

struct Buffer_GL4 : public Buffer {
	Buffer_GL4(GLuint b){ buffer = b; }
	Buffer_GL4(unsigned id, enum BUFF_Type t, GLuint b) : Buffer(id, t){ buffer = b; }
	Buffer_GL4(unsigned id, enum BUFF_Type t, GLuint b, unsigned c) : Buffer(id, t, c) { buffer = b; }
	
	GLuint buffer; // OpenGL buffer
};

// Vertex Array Object

struct VertexArray_GL4 : public RenderObj {
	VertexArray_GL4() : RenderObj() {}
	VertexArray_GL4(unsigned id, GLuint v) : RenderObj(id){ vao = v; }

	GLuint vao;
};

// Texture

struct Texture_GL4 : public Texture {
	Texture_GL4() : Texture() {}
	Texture_GL4(unsigned id, enum TEX_Frmt f, enum TEX_Mode m, GLuint t) : Texture(id, f, m) { texture = t; }
	Texture_GL4(unsigned id, unsigned short b, enum TEX_Frmt f, enum TEX_Mode m, GLuint t) : Texture(id, b, f, m) { texture = t; }

	GLuint texture;
};

// Pipeline

struct Topl_Pipeline_GL4 : public Topl_Pipeline {
	Topl_Pipeline_GL4() : Topl_Pipeline(){}

	GLuint shaderProg; // Linked Shader Program

	GLuint vertexShader;
	GLuint pixelShader;
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
		_flags[DRAW_ORDER_BIT] = DRAW_NORMAL;
		init(window);

		setViewport(&_defaultViewport); // viewport creation
		setDrawMode(DRAW_Triangles);
	}
	~Topl_Renderer_GL4();

	void draw(const Geo_Actor* actor) override;
	void clear() override;
	void setViewport(const Topl_Viewport* viewport) override;
	void swapBuffers(double frameTime) override;
	void setDrawMode(enum DRAW_Mode mode) override;

	void setPipeline(Topl_Pipeline_GL4* pipeline);
	void genPipeline(Topl_Pipeline_GL4* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader);
	void genPipeline(Topl_Pipeline_GL4* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr geomShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader);
#ifdef RASTERON_H
	Img_Base frame() override;
#endif
protected:
  	void init(NATIVE_WINDOW window) override;
	void update(const Topl_Scene* scene) override;
	void update(const Geo_Actor* actor) override;
	// void build(const Topl_Scene* scene) override;
	void build(const Geo_Actor* actor) override;
#ifdef RASTERON_H
	// void attachTexAt(const Rasteron_Image* image, unsigned id) override;
	void attachTexAt(const Rasteron_Image* image, unsigned renderID, unsigned binding) override;
	void attachTex3D(const Img_Volume* volumeTex, unsigned id) override;
#endif
	void linkShaders(Topl_Pipeline_GL4* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr geomShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader);
	
	Topl_Pipeline_GL4* _pipeline;

	std::map<unsigned long, VertexArray_GL4> _vertexArrayMap;
	std::map<unsigned long, Buffer_GL4> _vertexBufferMap, _indexBufferMap, _blockBufferMap;
	std::vector<Texture_GL4> _textures; // active textures
	std::map<unsigned long, Texture_GL4[MAX_TEX_BINDINGS + 2]> _textureMap; // TODO: Change to this type
private:
	GLenum _drawMode_GL4; // OpenGL specific draw mode
	GLuint* _bufferSlots;
	unsigned _bufferIndex = 0; // increments to indicate next available buffer slot
	GLuint* _vertexArraySlots;
	unsigned _vertexArrayIndex = 0; // increments to indicate next available vertex array slot
	GLuint* _textureSlots;
	unsigned _textureIndex = 0; // increments to indicate next available texture slot
};
