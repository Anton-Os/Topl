#include "Topl_Renderer.hpp"

#include <d3dx10.h> // DirectX Renderer Specific
#include <d3dx11.h> // DirectX Renderer Specific
#include <d3dx10math.h> // DirectX Renderer Specific

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

struct Topl_Pipeline_Drx11 {
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11HullShader* hullShader;
	ID3D11DomainShader* domainShader;
	ID3D11GeometryShader* geomShader;
	ID3DBlob* vsBlob;
	ID3DBlob* psBlob;
	ID3DBlob* hsBlob;
	ID3DBlob* dsBlob;
	ID3DBlob* gsBlob;
};

struct Topl_RenderContext_Drx11 { // Groups items together to allow for switching between multiple scenes and pipelines
	const Topl_Scene* scenePtr;
	Topl_Pipeline_Drx11 pipeline;
	ID3D11Buffer* sceneBlockBuff = nullptr; // Drx11 buffer target for scene block data
	std::vector<Buffer_Drx11> buffers;
	std::vector<Texture_Drx11> textures;
};

class Topl_Renderer_Drx11 : public Topl_Renderer {
public:
	Topl_Renderer_Drx11(HWND hwnd) { init(hwnd); }
	~Topl_Renderer_Drx11();

	void clearView() override;
	void buildScene(const Topl_Scene* scene) override;
	void buildScene(const Topl_Scene* scene, const Topl_Camera* camera) override;
#ifdef RASTERON_H
    Rasteron_Image* frame() override;
    void genTexture(const Rasteron_Image* image, unsigned id) override;
#endif
private:
	void init(NATIVE_WINDOW hwnd) override;
	void pipeline(entry_shader_cptr vertexShader, shader_cptr fragShader) override;
	void pipeline(entry_shader_cptr vertexShader, shader_cptr fragShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader, shader_cptr geomShader) override;
	void update(const Topl_Scene* scene) override;
	void update(const Topl_Scene* scene, const Topl_Camera* camera) override;
	void render(void) override;

	Topl_RenderContext_Drx11 _renderCtx;
	Topl_Pipeline_Drx11 _pipeline;
	ID3D11InputLayout* _vertexDataLayout;

	IDXGISwapChain* _swapChain;
	ID3D11Device* _device;
	ID3D11DeviceContext* _deviceCtx;
	ID3D11RenderTargetView* _rtv;
	ID3D11ShaderResourceView* _resourceView;
	ID3D11BlendState* _blendState;
	ID3D11RasterizerState* _rasterizerState;
};