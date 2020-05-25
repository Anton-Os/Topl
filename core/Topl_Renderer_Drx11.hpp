#include "Topl_Renderer.hpp"


struct Topl_Pipeline_Drx11 {
    std::vector<ID3D11Buffer*> vertexBuffs_3f;
    std::vector<ID3D11Buffer*> indexBuffs_ui;
    std::vector<ID3D11Buffer*> constBuffs_vec3f;
	
    ID3D11Buffer* vertexRectBuff;
    ID3D11Buffer* indexRectBuff;
    ID3D11Buffer* constPosBuff;

	ID3D11InputLayout* vertexDataLayout; // Move out of here, NEXT IMPLEMENTATION
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3DBlob* vsBuff;
	ID3DBlob* psBuff;
};

class Topl_Renderer_Drx11 : Topl_Renderer {
public:
    Topl_Renderer_Drx11(HWND hwnd){ init(hwnd); }
	~Topl_Renderer_Drx11();

    //void buildScene(const Topl_SceneGraph* sceneGraph) override;
    // void buildScene(void) override;
	void buildScene(const Topl_SceneGraph* sceneGraph) override;
	void render(void) override;
private:
    void init(NATIVE_WINDOW hwnd) override;
    void createPipeline(void) override;
    // void cleanup(void) override;

    bool m_pipelineReady = false;
    Topl_Pipeline_Drx11 m_pipeline;
    // unsigned m_pipelineCount // NEXT IMPLEMENTATION
    // Topl_Pipeline_Drx11* m_pipelines(unsigned count) // NEXT IMPLEMENTATION

    bool m_sceneReady = false;
    // Build up using the build scene method

    IDXGISwapChain* m_swapChain;
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_deviceCtx;
    ID3D11RenderTargetView* m_rtv;
};