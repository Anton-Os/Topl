#include "Topl_Renderer_Drx11.hpp"

void Topl_Renderer_Drx11::init(NATIVE_WINDOW hwnd){
	m_native.window = &hwnd; // Supplying platform specific stuff

    DXGI_MODE_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

    bufferDesc.Width = WIN_WIDTH;
    bufferDesc.Height = WIN_HEIGHT;
    bufferDesc.RefreshRate.Numerator = 60;
    bufferDesc.RefreshRate.Denominator = 1;
    bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SWAP_CHAIN_DESC swapChainDesc; 
    
    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

    swapChainDesc.BufferDesc = bufferDesc;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = *(m_native.window); 
    swapChainDesc.Windowed = TRUE; 
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    if(FAILED(   
        D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,  D3D11_SDK_VERSION,
                                      &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceCtx )   
    ))
        return; // Provide error handling code
    
    ID3D11Texture2D* backBuffer;
    if(FAILED(
        m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer)
    ))
        return; // Provide error handling code

    m_device->CreateRenderTargetView(backBuffer, NULL, &m_rtv);
    backBuffer->Release();

    m_deviceCtx->OMSetRenderTargets(1, &m_rtv, NULL);

    return;
}

void Topl_Renderer_Drx11::buildScene(Topl_SceneGraph sceneGraph){
    return;
}

void Topl_Renderer_Drx11::render(void){
    const float clearColor[] = { 0.4f, 0.4f, 0.9f, 1.0f };

    m_deviceCtx->ClearRenderTargetView(m_rtv, clearColor);

    m_swapChain->Present(0, 0);
}

void Topl_Renderer_Drx11::cleanup(void){
    m_swapChain->Release();
    m_device->Release();
    m_deviceCtx->Release();
}