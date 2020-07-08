#include "Topl_Renderer.hpp"

// Make this derived from Buffer within Renderer header

struct Buffer_Drx11 : public Buffer {
	Buffer_Drx11() : Buffer() {}
	Buffer_Drx11(unsigned id, enum BUFF_Type t, ID3D11Buffer* b) : Buffer(id, t) {
		buffer = b;
	}
	Buffer_Drx11(unsigned id, enum BUFF_Type t, ID3D11Buffer* b, unsigned c) : Buffer(id, t, c) {
		buffer = b;
	}

	ID3D11Buffer* buffer; // DirectX specific
};


struct Topl_Pipeline_Drx11 {
	ID3D11InputLayout* vertexDataLayout; // Move out of here, NEXT IMPLEMENTATION
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3DBlob* vsBuff;
	ID3DBlob* psBuff;

	// Texturing things
	// ID3D11ShaderResourceView* texture; // Expand this code
	// ID3D11SamplerState* samplerState;

	ID3D11Texture2D* texture = nullptr;
};

class Topl_Renderer_Drx11 : public Topl_Renderer {
public:
	Topl_Renderer_Drx11(HWND hwnd) { init(hwnd); }
	~Topl_Renderer_Drx11();

	void createPipeline(const Topl_Shader* vertexShader, const Topl_Shader* fragShader) override;
	void buildScene(const Topl_SceneManager* sceneGraph) override;

#ifdef RASTERON_H
    Rasteron_Image* getFrame() override;
    void genTexture(const Rasteron_Image* image) override;
#endif
private:
	void init(NATIVE_WINDOW hwnd) override;
	void update(const Topl_SceneManager* sceneGraph) override;
	void render(void) override;

	Topl_Pipeline_Drx11 m_pipeline;
	std::vector<Buffer_Drx11> mBuffers;
	unsigned mMaxBuffID = 1;

	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceCtx;
	ID3D11RenderTargetView* m_rtv;
};