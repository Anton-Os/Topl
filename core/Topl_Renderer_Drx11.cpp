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

    // Creates pipeline items
    Topl_Renderer_Drx11::createPipeline();

    return;
}

void Topl_Renderer_Drx11::createPipeline(void){
	ID3DBlob* errorBuff;

	if (FAILED(
		D3DCompileFromFile(L"C:\\AntonDocs\\Codex\\Ao-Project\\Topl-Skeleton\\MSVC_BUILD_2\\Debug\\Vertex_MostBasic.hlsl", 
		nullptr, nullptr, "main", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &m_pipeline.vsBuff, &errorBuff)
	)) {
		m_pipelineReady = false;
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		return; // Provide error handling code
	}

	if (FAILED(
		D3DCompileFromFile(L"C:\\AntonDocs\\Codex\\Ao-Project\\Topl-Skeleton\\MSVC_BUILD_2\\Debug\\Pixel_MostBasic.hlsl", 
			nullptr, nullptr, "main", "ps_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &m_pipeline.psBuff, &errorBuff)
	)) {
		m_pipelineReady = false;
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		return; // Provide error handling code
	} 

    if(FAILED(
        m_device->CreateVertexShader(
            m_pipeline.vsBuff->GetBufferPointer(), 
            m_pipeline.vsBuff->GetBufferSize(),
            NULL, &m_pipeline.vertexShader
        )
    )) {
        m_pipelineReady = false;
        return; // Provide error handling code
    }

    if(FAILED(
        m_device->CreatePixelShader(
            m_pipeline.psBuff->GetBufferPointer(), 
            m_pipeline.psBuff->GetBufferSize(),
            NULL, &m_pipeline.pixelShader
        )
    )) {
        m_pipelineReady = false;
        return; // Provide error handling code
    }

    m_deviceCtx->VSSetShader(m_pipeline.vertexShader, 0, 0);
    m_deviceCtx->PSSetShader(m_pipeline.pixelShader, 0, 0);

    m_pipelineReady = true;
    // Think that is all for now...
}

//void Topl_Renderer_Drx11::buildScene(const Topl_SceneGraph* sceneGraph){
void Topl_Renderer_Drx11::buildScene(void){
    // Build a scene based on scene graph NEXT IMPLEMENTATION
    
	float verticesTest[3][3] = {
		{0.1f, 0.7f, 0.5f},
		{0.4f, 0.7f, 0.5f},
		{0.9f, 0.5f, 0.5f}
	}; 

    D3D11_BUFFER_DESC vertexBuffDesc;
    ZeroMemory(&vertexBuffDesc, sizeof(vertexBuffDesc));

    vertexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBuffDesc.ByteWidth = sizeof(float) * 9;
    vertexBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBuffDesc.CPUAccessFlags = 0;
    vertexBuffDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexBuffData;
    ZeroMemory(&vertexBuffData, sizeof(vertexBuffData));
    vertexBuffData.pSysMem = verticesTest;

	if (FAILED(
		m_device->CreateBuffer(&vertexBuffDesc, &vertexBuffData, &m_pipeline.vertexDataBuff)
    )) {
        m_sceneReady = false;
        return; // Provide error handling code
    }

    UINT stride = sizeof(float) * 3;
    UINT offset = 0;
    m_deviceCtx->IASetVertexBuffers(0, 1, &m_pipeline.vertexDataBuff, &stride, &offset);

    D3D11_INPUT_ELEMENT_DESC layoutTest[] ={
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
    };
    UINT layoutElemCount = ARRAYSIZE(layoutTest); // REFINE THIS

    m_device->CreateInputLayout(
        layoutTest, layoutElemCount,
        m_pipeline.vsBuff->GetBufferPointer(), m_pipeline.vsBuff->GetBufferSize(), 
        &m_pipeline.vertexDataLayout
    );

    m_deviceCtx->IASetInputLayout(m_pipeline.vertexDataLayout);
    m_deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Viewport Creation

    RECT windowRect;
    GetWindowRect(*(m_native.window), &windowRect);

    D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Height = 500; // REPLACE WITH DETERMINED HEIGHT
    viewport.Width = 500; // REPLACE WITH DETERMINED WIDTH

	m_deviceCtx->RSSetViewports(1, &viewport);

    m_sceneReady = true;
    return;
}

void Topl_Renderer_Drx11::render(void){
    const float clearColor[] = { 0.4f, 0.4f, 0.9f, 1.0f };

    m_deviceCtx->ClearRenderTargetView(m_rtv, clearColor);

    if(m_pipelineReady && m_sceneReady)
        m_deviceCtx->Draw(3, 0);

    m_swapChain->Present(0, 0);
}

void Topl_Renderer_Drx11::cleanup(void){
    m_swapChain->Release();
    m_device->Release();
    m_deviceCtx->Release();

	m_pipeline.vertexDataBuff->Release();
	m_pipeline.vertexShader->Release();
	m_pipeline.pixelShader->Release();
	m_pipeline.vsBuff->Release();
	m_pipeline.psBuff->Release();
	m_pipeline.vertexDataLayout->Release();
}