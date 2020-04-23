#include "Topl_Renderer.hpp"

/* typedef enum {

} DRX11_Init_Status; */

class Topl_Renderer_Drx11 : Topl_Renderer {
public:
    Topl_Renderer_Drx11(HWND hwnd){ init(hwnd); }
    ~Topl_Renderer_Drx11(){ cleanup() }

    void buildScene(Topl_SceneGraph sceneGraph) override;
    void render(void) override;
private:
    void init(NATIVE_WINDOW hwnd) override;
    void cleanup(void) override;

    IDXGISwapChain* m_swapChain;
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_deviceCtx;
    ID3D11RenderTargetView* m_rtv;
}