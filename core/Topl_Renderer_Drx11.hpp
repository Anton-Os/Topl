#include "Topl_Renderer.hpp"

class Topl_Renderer_Drx11 : Topl_Renderer {
public:
    void buildScene(Topl_SceneGraph sceneGraph) override;
    void render(void) override;
private:
    void init(NATIVE_WINDOW hwnd) override;

    IDXGISwapChain* swapChain;
    ID3D11Device* device;
    ID3D11DeviceContext* deviceCtx;
    ID3D11RenderTargetView* rtv;
}