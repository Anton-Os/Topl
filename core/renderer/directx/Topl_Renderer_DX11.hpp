#include "_DX11.hpp"

// Remderer

class Topl_Renderer_DX11 : public Topl_Renderer {
public:
	Topl_Renderer_DX11(NATIVE_PLATFORM_CONTEXT* context) : Topl_Renderer(context){
		_flags[DRAW_ORDER_BIT] = DRAW_INVERSE;
		init(context->window);

		setViewport(&_activeViewport); // viewport creation
		setDrawMode(DRAW_Triangles);
	}
	~Topl_Renderer_DX11();

	void draw(const Geo_Actor* actor) override;
	void clear() override;
	void setViewport(const Topl_Viewport* viewport) override;
	void swapBuffers(double frameTime) override;
	void setDrawMode(enum DRAW_Mode mode) override;

	void setPipeline(DX11::Pipeline* pipeline);
	void genPipeline(DX11::Pipeline* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader);
	void genPipeline(DX11::Pipeline* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr geomShader, shader_cptr hullShader, shader_cptr domainShader);
#ifdef RASTERON_H
    Img_Base frame() override;
#endif
protected:
	void init(NATIVE_WINDOW window) override;
	void update(const Geo_Actor* actor) override;
	void build(const Geo_Actor* actor) override;
#ifdef RASTERON_H
    void attachTexAt(const Img_Base* imageTex, unsigned renderID, unsigned binding) override;
	void attachTex3D(const Img_Volume* volumeTex, unsigned id) override;
#endif
	void setConstBufferData(ID3D11Buffer* buffer, unsigned short binding);

	const float _clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	DX11::Pipeline* _pipeline = nullptr;

	ID3D11Buffer* _sceneBlockBuff = nullptr; // buffer target for scene block data
	std::map<unsigned long, DX11::Buffer> _vertexBufferMap, _indexBufferMap, _blockBufferMap, _extBlockBufferMap;
	std::vector<DX11::Texture> _textures;
	std::map<unsigned long, DX11::Texture[MAX_TEX_BINDINGS + 2]> _textureMap; // TODO: Change to this type
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
