#include <d3d11_1.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "Topl_Renderer.hpp"

// Buffer

struct Buffer_DX11 : public Buffer {
	Buffer_DX11(ID3D11Buffer* b) : Buffer(){ buffer = b; }
	Buffer_DX11(unsigned id, enum BUFF_Type t, ID3D11Buffer* b) : Buffer(id, t) { buffer = b; }
	Buffer_DX11(unsigned id, enum BUFF_Type t, ID3D11Buffer* b, unsigned c) : Buffer(id, t, c) { buffer = b; }

	ID3D11Buffer* buffer; // DirectX buffer
};

// Texture

struct Texture_DX11 : public Texture {
	Texture_DX11() : Texture() {}
	Texture_DX11(unsigned id, enum TEX_Frmt f, enum TEX_Mode m, ID3D11SamplerState* s, ID3D11ShaderResourceView* r)
	: Texture(id, f, m) {
		sampler = s;
		resource = r;
	}

	Texture_DX11(unsigned id, unsigned short b, enum TEX_Frmt f, enum TEX_Mode m, ID3D11SamplerState* s, ID3D11ShaderResourceView* r)
	: Texture(id, b, f, m) {
		sampler = s;
		resource = r;
	}

	ID3D11SamplerState* sampler = nullptr;
	ID3D11ShaderResourceView* resource = nullptr;
};

// Pipeline

struct Topl_Pipeline_DX11 : public Topl_Pipeline {
	Topl_Pipeline_DX11() : Topl_Pipeline(){}
	~Topl_Pipeline_DX11(){
		if(vertexShader != nullptr) vertexShader->Release(); if(vsBlob != nullptr) vsBlob->Release();
		if(pixelShader != nullptr) pixelShader->Release(); if(psBlob != nullptr) psBlob->Release();
		if(hullShader != nullptr) hullShader->Release(); if(hsBlob != nullptr) hsBlob->Release();
		if(domainShader != nullptr) domainShader->Release(); if(dsBlob != nullptr) dsBlob->Release();
		if(geomShader != nullptr) geomShader->Release(); if(gsBlob != nullptr) gsBlob->Release();
	}

	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3D11GeometryShader* geomShader = nullptr;
	ID3D11HullShader* hullShader = nullptr;
	ID3D11DomainShader* domainShader = nullptr;
	
	ID3DBlob *vsBlob, *psBlob, *hsBlob, *dsBlob, *gsBlob = nullptr;
};

// Remderer

class Topl_Renderer_DX11 : public Topl_Renderer {
public:
	Topl_Renderer_DX11(HWND window) : Topl_Renderer(window){
		_isDrawInOrder = INVERSE_DRAW_ORDER;
		init(window);
		
		setViewport(&_defaultViewport); // viewport creation
		setDrawMode(DRAW_Triangles);
	}
	~Topl_Renderer_DX11();

	void clearView() override;
	void setViewport(const Topl_Viewport* viewport) override;
	void swapBuffers(double frameTime) override;
	void setDrawMode(enum DRAW_Mode mode) override;

	void setPipeline(Topl_Pipeline_DX11* pipeline);
	void genPipeline(Topl_Pipeline_DX11* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader);
	void genPipeline(Topl_Pipeline_DX11* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr geomShader, shader_cptr hullShader, shader_cptr domainShader);
#ifdef RASTERON_H
    Img_Base frame() override;
#endif
protected:
	void init(NATIVE_WINDOW window) override;
	void build(const Topl_Scene* scene) override;
	void update(const Topl_Scene* scene) override;
	void renderTarget(unsigned long renderID) override;
#ifdef RASTERON_H
	// void attachTexAt(const Rasteron_Image* image, unsigned id) override;
	void attachTexAt(const Rasteron_Image* image, unsigned renderID, unsigned binding) override;
	void attachTex3D(const Img_Volume* volumeTex, unsigned id) override;
#endif

	const float _clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	Topl_Pipeline_DX11* _pipeline = nullptr;

	ID3D11Device* _device;
	IDXGISwapChain* _swapChain;
	ID3D11DeviceContext* _deviceCtx;
	ID3D11InputLayout* _vertexDataLayout = nullptr;
	ID3D11RenderTargetView* _rtView;
	ID3D11DepthStencilView* _dsView;
	ID3D11BlendState* _blendState;
	ID3D11RasterizerState* _rasterizerState;

	ID3D11Buffer* _sceneBlockBuff = nullptr; // buffer target for scene block data
	std::vector<Buffer_DX11> _buffers;
	std::vector<Texture_DX11> _textures;

	const UINT _vertexStride = sizeof(Geo_Vertex);
	const UINT _vertexOffset = 0;
};