#include "_GL4.hpp"

// Renderer

class Topl_Renderer_GL4 : public Topl_Renderer {
public:
	Topl_Renderer_GL4(NATIVE_PLATFORM_CONTEXT* context) : Topl_Renderer(context) {
		_flags[DRAW_ORDER_BIT] = DRAW_NORMAL;
		init(context->window);
		setViewport(&_activeViewport); // viewport creation
		setDrawMode(DRAW_Triangles);
		_clearColors = Vec4f({ 0.25F, 1.0F, 0.25F, 1.0F });
	}
	~Topl_Renderer_GL4();

	void draw(const Geo_Actor* actor) override;
    void update(const Geo_Actor* actor) override;
    void build(const Geo_Actor* actor) override;
	void clear() override;
	void setViewport(const Topl_Viewport* viewport) override;
	void setDrawMode(enum DRAW_Mode mode) override;

	void setPipeline(GL4::Pipeline* pipeline);
	Topl_Pipeline* getPipeline() override { return _pipeline; }
	void genPipeline(GL4::Pipeline* pipeline, entry_shader_cptr vShader, shader_cptr pShader){ genPipeline(pipeline, vShader, pShader, {}); }
	void genPipeline(GL4::Pipeline* pipeline, entry_shader_cptr vShader, shader_cptr pShader, std::initializer_list<shader_cptr> shaders);
    void genComputePipeline(GL4::Pipeline* pipeline, shader_cptr cShader){ if(cShader->getType() == SHDR_Compute) genPipeline(pipeline, nullptr, nullptr, { cShader }); }
#ifdef RASTERON_H
	Sampler_2D frame() override;
#endif
#ifndef __ANDROID__
	void dispatch(std::vector<Vec3f>* data) override { glDispatchCompute(data->size(), data->size(), data->size()); }
#endif
protected:
    void init(NATIVE_WINDOW window) override;
    void swapBuffers(double frameTime) override;
#ifdef RASTERON_H
    void attachTexAt(const Sampler_2D* imageTex, unsigned renderID, unsigned binding) override;
	void attachTex3D(const Sampler_3D* volumeTex, unsigned id) override;
#endif
	void linkShaders(GL4::Pipeline* pipeline, entry_shader_cptr vShader, shader_cptr pShader, std::initializer_list<shader_cptr> shaders);
	
	GL4::Pipeline* _pipeline;

	std::map<unsigned long, GL4::VertexArray> _vertexArrayMap;
	std::map<unsigned long, GL4::Buffer> _vertexBufferMap, _indexBufferMap, _blockBufferMap, _extBlockBufferMap;
	std::map<unsigned long, GL4::Buffer> _storageBufferMap;
    std::map<unsigned long, GL4::Texture[MAX_TEX_BINDINGS + 2]> _textureMap; // TODO: Change to this type
    std::vector<GL4::Texture> _textures; // active textures
private:
	GLenum _drawMode_GL4; // OpenGL specific draw mode
	GLuint* _bufferSlots;
	unsigned _bufferIndex = 0; // increments to indicate next available buffer slot
	GLuint* _vertexArraySlots;
	unsigned _vertexArrayIndex = 0; // increments to indicate next available vertex array slot
	GLuint* _textureSlots;
	unsigned _textureIndex = 0; // increments to indicate next available texture slot
	GLuint* _storageSlots;
	unsigned _storageIndex = 0; // increments to indicate next available storage buffer
};
