#include "_DX11.hpp"

// Remderer

class Topl_Renderer_DX11 : public Topl_Renderer {
public:
	Topl_Renderer_DX11(NATIVE_PLATFORM_CONTEXT* context) : Topl_Renderer(context){
		_flags[DRAW_ORDER_BIT] = DRAW_INVERSE;
		init(context->window);
		setViewport(&_activeViewport); // viewport creation
		setDrawMode(DRAW_Triangles);
		_clearColors = Vec4f({ 0.25F, 0.25F, 1.0F, 1.0F });
	}
	~Topl_Renderer_DX11();

    void draw(const Geo_Actor* actor) override;
    void update(const Geo_Actor* actor) override;
    void build(const Geo_Actor* actor) override;
	void clear() override;
	void setViewport(const Topl_Viewport* viewport) override;
	void setDrawMode(enum DRAW_Mode mode) override;

	void setPipeline(DX11::Pipeline* pipeline);
	Topl_Pipeline* getPipeline() override { return _pipeline; }
	void genPipeline(DX11::Pipeline* pipeline, entry_shader_cptr vShader, shader_cptr pShader){ genPipeline(pipeline, vShader, pShader, {}); }
	void genPipeline(DX11::Pipeline* pipeline, entry_shader_cptr vShader, shader_cptr pShader, std::initializer_list<shader_cptr> shaders);
    void genComputePipeline(DX11::Pipeline* pipeline, shader_cptr cShader){ if(cShader->getType() == SHDR_Compute) genPipeline(pipeline, nullptr, nullptr, { cShader }); }
#ifdef RASTERON_H
    Img_Base frame() override;
#endif
	void dispatch(std::vector<Vec3f>* data) override { _deviceCtx->Dispatch(data->size(), data->size(), data->size()); }
protected:
	void init(NATIVE_WINDOW window) override;
    void swapBuffers(double frameTime) override;
#ifdef RASTERON_H
    void attachTexAt(const Img_Base* imageTex, unsigned renderID, unsigned binding) override;
	void attachTex3D(const Img_Volume* volumeTex, unsigned id) override;
#endif
	void setConstBufferData(ID3D11Buffer* buffer, unsigned short binding);

	DX11::Pipeline* _pipeline = nullptr;

	ID3D11Buffer* _sceneBlockBuff = nullptr; // buffer target for scene block data
	std::map<unsigned long, DX11::Buffer> _vertexBufferMap, _indexBufferMap, _blockBufferMap, _extBlockBufferMap;
    // DX11::Buffer _feedBuffers[2] = { DX11::Buffer(0), DX11::Buffer(0) }; // for shader storage blocks
	std::map<unsigned long, DX11::Texture[MAX_TEX_BINDINGS + 2]> _textureMap; // TODO: Change to this type
    std::vector<DX11::Texture> _textures;
private:
	ID3D11Device* _device;
	IDXGISwapChain* _swapChain;
	ID3D11DeviceContext* _deviceCtx;
	ID3D11InputLayout* _vertexDataLayout = nullptr;
	ID3D11RenderTargetView* _rtView;
	ID3D11DepthStencilView* _dsView;
	ID3D11BlendState* _blendState;
	ID3D11RasterizerState* _rasterizerState;

	const UINT _vertexStride = sizeof(Geo_Vertex);
	const UINT _vertexOffset = 0;
};
