#include "Topl_Renderer.hpp"


enum BUFF_Type {
    BUFF_Vertex_3F,
    BUFF_Index_UI,
    BUFF_Const_vec3f
};

struct Buffer_Drx11 {
    enum BUFF_Type type;
    ID3D11Buffer* buffer;
    unsigned count = 1;
};

struct Topl_Data_Drx11 {
    std::vector<Buffer_Drx11> vertexBuffs_3f;
    std::vector<Buffer_Drx11> indexBuffs_ui;
    std::vector<Buffer_Drx11> constBuffs_vec3f;
};

struct Topl_Pipeline_Drx11 {
	ID3D11InputLayout* vertexDataLayout; // Move out of here, NEXT IMPLEMENTATION
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3DBlob* vsBuff;
	ID3DBlob* psBuff;
};

class Topl_Renderer_Drx11 : public Topl_Renderer {
public:
    Topl_Renderer_Drx11(HWND hwnd){ init(hwnd); }
	~Topl_Renderer_Drx11();

    void createPipeline(const Topl_Shader* vertexShader, const Topl_Shader* fragShader) override;
	void buildScene(const Topl_SceneGraph* sceneGraph) override;
private:
    void init(NATIVE_WINDOW hwnd) override;
    void createPipeline(void) override; // Marked for removal
    void render(void) override;

    Topl_Pipeline_Drx11 m_pipeline;
    Topl_Data_Drx11 m_bufferData;

    IDXGISwapChain* m_swapChain;
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_deviceCtx;
    ID3D11RenderTargetView* m_rtv;
};