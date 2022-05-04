// DirectX Specific Inclusions

#include <d3d11_1.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

// Renderer Implementation

#include "Topl_Renderer.hpp"

struct Buffer_Drx11 : public Buffer {
	// Buffer_Drx11() : Buffer() {}
	Buffer_Drx11(ID3D11Buffer* b) : Buffer(){ buffer = b; }
	Buffer_Drx11(unsigned id, enum BUFF_Type t, ID3D11Buffer* b) : Buffer(id, t) { buffer = b; }
	Buffer_Drx11(unsigned id, enum BUFF_Type t, ID3D11Buffer* b, unsigned c) : Buffer(id, t, c) { buffer = b; }

	ID3D11Buffer* buffer; // DirectX specific
};

struct Texture_Drx11 : public Texture {
	Texture_Drx11() : Texture() {}
	Texture_Drx11(unsigned id, enum TEX_Frmt f, enum TEX_Mode m, ID3D11SamplerState* s, ID3D11ShaderResourceView* r)
		: Texture(id, f, m) {
			// texture = t;
			sampler = s;
			resView = r;
		}

	// ID3D11Texture2D* texture = nullptr;
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
};

class Topl_Renderer_Drx11 : public Topl_Renderer {
public:
	Topl_Renderer_Drx11(HWND window) : Topl_Renderer(window){ 
		init(window);
	}
	Topl_Renderer_Drx11(NATIVE_WINDOW window, std::initializer_list<Topl_Viewport> viewports) : Topl_Renderer(window, viewports){
		init(window);
	}
	~Topl_Renderer_Drx11();

	void clearView() override;
	void switchFramebuff() override;
	void build(const Topl_Scene* scene) override;
	void texturize(const Topl_Scene* scene) override;

	void setPipeline(Topl_Pipeline_Drx11* pipeline);
	void genPipeline(Topl_Pipeline_Drx11* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader);
	void genPipeline(Topl_Pipeline_Drx11* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader, shader_cptr geomShader);
#ifdef RASTERON_H
    Rasteron_Image* frame() override;
#endif
private:
	void init(NATIVE_WINDOW window) override;
	// void init(NATIVE_WINDOW window, std::initializer_list<Topl_Viewport> viewports) override;
	void update(const Topl_Scene* scene) override;
	void drawMode(void) override;
	void render(const Topl_Scene* scene) override;
	// void renderTarget(unsigned long renderID) override;
#ifdef RASTERON_H
	void attachTexture(const Rasteron_Image* image, unsigned id) override;
	void attachMaterial(const Topl_Material* material, unsigned id) override;
#endif

	const float _clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	Topl_Pipeline_Drx11* _pipeline = nullptr;

	ID3D11Device* _device;
	IDXGISwapChain* _swapChain;
	ID3D11DeviceContext* _deviceCtx;
	ID3D11InputLayout* _vertexDataLayout;
	ID3D11RenderTargetView* _rtView;
	ID3D11DepthStencilView* _dsView;
	ID3D11ShaderResourceView* _resourceView;
	ID3D11BlendState* _blendState;
	ID3D11RasterizerState* _rasterizerState;

	ID3D11Buffer* _sceneBlockBuff = nullptr; // buffer target for scene block data
	std::vector<Buffer_Drx11> _buffers;
	std::vector<Texture_Drx11> _textures;
};