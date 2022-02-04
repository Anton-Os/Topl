#include "Topl_Renderer.hpp"

#include <d3dx10.h> // DirectX Renderer Specific
#include <d3dx11.h> // DirectX Renderer Specific
#include <d3dx10math.h> // DirectX Renderer Specific

// see Topl_Renderer.h
#define DRAW_ORDER 1 // last render target is drawn first, reverse order

struct Buffer_Drx11 : public Buffer {
	// Buffer_Drx11() : Buffer() {}
	Buffer_Drx11(ID3D11Buffer* b) : Buffer(){ buffer = b; }
	Buffer_Drx11(unsigned id, enum BUFF_Type t, ID3D11Buffer* b) : Buffer(id, t) { buffer = b; }
	Buffer_Drx11(unsigned id, enum BUFF_Type t, ID3D11Buffer* b, unsigned c) : Buffer(id, t, c) { buffer = b; }

	ID3D11Buffer* buffer; // DirectX specific
};

struct Texture_Drx11 : public Texture {
	Texture_Drx11() : Texture() {}
	Texture_Drx11(unsigned id, enum TEX_Frmt f, enum TEX_Mode m, ID3D11Texture2D* t, ID3D11SamplerState* s, ID3D11ShaderResourceView* r)
		: Texture(id, f, m) {
			texture = t;
			sampler = s;
			resView = r;
		}

	ID3D11Texture2D* texture = nullptr;
	ID3D11SamplerState* sampler = nullptr;
	ID3D11ShaderResourceView* resView = nullptr;
};

struct Topl_Pipeline_Drx11 : public Topl_Pipeline {
	Topl_Pipeline_Drx11() : Topl_Pipeline(){}
	~Topl_Pipeline_Drx11(){
		if(vertexShader != nullptr) vertexShader->Release();
		if(pixelShader != nullptr) pixelShader->Release();
		if(hullShader != nullptr) hullShader->Release();
		if(domainShader != nullptr) domainShader->Release();
		if(geomShader != nullptr) geomShader->Release();
		if(vsBlob != nullptr) vsBlob->Release();
		if(psBlob != nullptr) psBlob->Release();
		if(hsBlob != nullptr) hsBlob->Release();
		if(dsBlob != nullptr) dsBlob->Release();
		if(gsBlob != nullptr) gsBlob->Release();
	}

	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3D11HullShader* hullShader = nullptr;
	ID3D11DomainShader* domainShader = nullptr;
	ID3D11GeometryShader* geomShader = nullptr;
	
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* hsBlob = nullptr;
	ID3DBlob* dsBlob = nullptr;
	ID3DBlob* gsBlob = nullptr;
private:
	friend class Topl_Renderer_Drx11;
};

struct Topl_RenderContext_Drx11 { // groups together data for rendering
	ID3D11Buffer* sceneBlockBuff = nullptr; // Drx11 buffer target for scene block data
	std::vector<Buffer_Drx11> buffers;
	std::vector<Texture_Drx11> textures;
};

class Topl_Renderer_Drx11 : public Topl_Renderer {
public:
	Topl_Renderer_Drx11(HWND hwnd) { init(hwnd); }
	~Topl_Renderer_Drx11();

	void clearView() override;
	void switchFramebuff() override;
	void setPipeline(Topl_Pipeline_Drx11* pipeline);
	void genPipeline(Topl_Pipeline_Drx11* pipeline, entry_shader_cptr vertexShader, shader_cptr fragShader);
	void genPipeline(Topl_Pipeline_Drx11* pipeline, entry_shader_cptr vertexShader, shader_cptr fragShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader, shader_cptr geomShader);
	void build(const Topl_Scene* scene) override;
#ifdef RASTERON_H
    Rasteron_Image* frame() override;
    void assignTexture(const Rasteron_Image* image, unsigned id) override;
#endif
private:
	void init(NATIVE_WINDOW hwnd) override;
	void update(const Topl_Scene* scene) override;
	void updateTex(const Topl_Scene* scene) override;
	void render(void) override;

	Topl_RenderContext_Drx11 _renderCtx;
	Topl_Pipeline_Drx11* _pipeline = nullptr;
	ID3D11InputLayout* _vertexDataLayout;

	ID3D11Device* _device;
	IDXGISwapChain* _swapChain; // used for switching between views used for rendering
	ID3D11DeviceContext* _deviceCtx;
	ID3D11RenderTargetView* _rtView;
	ID3D11DepthStencilView* _dsView;
	ID3D11ShaderResourceView* _resourceView;
	ID3D11BlendState* _blendState;
	ID3D11RasterizerState* _rasterizerState;
};