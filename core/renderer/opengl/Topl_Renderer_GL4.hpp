#include "Topl_GL4.hpp"

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

class Topl_Renderer_GL4 : public Topl_Renderer {
public:
	Topl_Renderer_GL4(NATIVE_PLATFORM_CONTEXT* context) : Topl_Renderer(context) {
		_flags[DRAW_ORDER_BIT] = DRAW_NORMAL;
		init(context->window);
		setViewport(&_activeViewport); // viewport creation
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
	void update(const Geo_Actor* actor) override;
	void build(const Geo_Actor* actor) override;
#ifdef RASTERON_H
	void attachTexAt(const Rasteron_Image* image, unsigned renderID, unsigned binding) override;
	void attachTex3D(const Img_Volume* volumeTex, unsigned id) override;
#endif
	void linkShaders(Topl_Pipeline_GL4* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr geomShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader);
	
	Topl_Pipeline_GL4* _pipeline;

	std::map<unsigned long, GL4::VertexArray> _vertexArrayMap;
	std::map<unsigned long, GL4::Buffer> _vertexBufferMap, _indexBufferMap, _blockBufferMap;
	std::vector<GL4::Texture> _textures; // active textures
	std::map<unsigned long, GL4::Texture[MAX_TEX_BINDINGS + 2]> _textureMap; // TODO: Change to this type
private:
	GLenum _drawMode_GL4; // OpenGL specific draw mode
	GLuint* _bufferSlots;
	unsigned _bufferIndex = 0; // increments to indicate next available buffer slot
	GLuint* _vertexArraySlots;
	unsigned _vertexArrayIndex = 0; // increments to indicate next available vertex array slot
	GLuint* _textureSlots;
	unsigned _textureIndex = 0; // increments to indicate next available texture slot
};
