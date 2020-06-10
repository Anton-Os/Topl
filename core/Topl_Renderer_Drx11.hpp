#include "Topl_Renderer.hpp"


#define MAX_BUFFER_TYPES 3

// Make this derived from Buffer within Renderer header
struct Buffer_Drx11 {
    unsigned targetID;
    enum BUFF_Type type;
    ID3D11Buffer* buffer;
    unsigned count = 1;
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
    std::vector<Buffer_Drx11> mBuffers;
    unsigned mMaxBuffID = 1;

    IDXGISwapChain* m_swapChain;
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_deviceCtx;
    ID3D11RenderTargetView* m_rtv;
};