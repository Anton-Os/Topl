#include "Topl_Renderer_Drx11.hpp"

namespace _Drx11 {
	static bool createVertexBuff(ID3D11Device** device, ID3D11Buffer** vBuff, float* vData, unsigned vCount) {
		D3D11_BUFFER_DESC buffDesc;
		ZeroMemory(&buffDesc, sizeof(buffDesc));
		buffDesc.Usage = D3D11_USAGE_DEFAULT;
		buffDesc.ByteWidth = sizeof(float) * vCount;
		buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffDesc.CPUAccessFlags = 0;
		buffDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA buffData;
		ZeroMemory(&buffData, sizeof(buffData));
		buffData.pSysMem = vData;

		if (FAILED(
			(*(device))->CreateBuffer(&buffDesc, &buffData, vBuff)
		))
			return false; // Provide error handling code

		return true;
	}

    static bool createVertexBuff(ID3D11Device** device, ID3D11Buffer** vBuff, const Eigen::Vector3f *const vData, unsigned vCount) {
		D3D11_BUFFER_DESC buffDesc;
		ZeroMemory(&buffDesc, sizeof(buffDesc));
		buffDesc.Usage = D3D11_USAGE_DEFAULT;
		buffDesc.ByteWidth = sizeof(Eigen::Vector3f) * vCount;
		buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffDesc.CPUAccessFlags = 0;
		buffDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA buffData;
		ZeroMemory(&buffData, sizeof(buffData));
		buffData.pSysMem = vData;

		if (FAILED(
			(*(device))->CreateBuffer(&buffDesc, &buffData, vBuff)
		))
			return false; // Provide error handling code

		return true;
	}

	static bool createIndexBuff(ID3D11Device** device, ID3D11Buffer** iBuff, DWORD* iData, unsigned iCount) {
		D3D11_BUFFER_DESC buffDesc;
		ZeroMemory(&buffDesc, sizeof(buffDesc));
		buffDesc.Usage = D3D11_USAGE_DEFAULT;
		buffDesc.ByteWidth = sizeof(DWORD) * iCount;
		buffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		buffDesc.CPUAccessFlags = 0;
		buffDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA buffData;
		// ZeroMemory(&buffData, sizeof(buffData)); // Maybe solution??
		buffData.pSysMem = iData;
		buffData.SysMemPitch = 0;
		buffData.SysMemSlicePitch = 0;

		if (FAILED(
			(*(device))->CreateBuffer(&buffDesc, &buffData, iBuff)
		))
			return false; // Provide error handling code

		return true;
	}
}

void Topl_Renderer_Drx11::init(NATIVE_WINDOW hwnd) {
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

void Topl_Renderer_Drx11::buildScene(const Topl_SceneGraph* sceneGraph) {
    // Build a scene based on scene graph NEXT IMPLEMENTATION

    float verticesBox[] = {
		-0.5f, -0.5f, 0.5f, 
		-0.5f,  0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, 0.5f
    };

	DWORD indexBox[] = {
		0, 1, 2,
		0, 2, 3
	};

    const Topl_GeoEntity *const gRect1_ptr = sceneGraph->getGeoEntity("basicRect");
    const Eigen::Vector3f *const gRec1_vData = gRect1_ptr->mType.gRect->getVData(); // Fix this access
    const unsigned *const gRec1_iData = gRect1_ptr->mType.gRect->getIData();
	// Eigen::Vector3f* gRec1_vData = gRect1_ptr->mType.gRect->mVData;
	// const unsigned *const gRec1_iData = gRect1_ptr->mType.gRect->mIData;

	// Index creation procedures

	m_sceneReady = _Drx11::createIndexBuff(&m_device, &m_pipeline.indexBoxBuff, &indexBox[0], 6);
    m_sceneReady = _Drx11::createIndexBuff(&m_device, &m_pipeline.indexRectBuff, (DWORD*)gRec1_iData, 6); // Scared of casting

	// m_deviceCtx->IASetIndexBuffer(m_pipeline.indexBoxBuff, DXGI_FORMAT_R32_UINT, 0);
    m_deviceCtx->IASetIndexBuffer(m_pipeline.indexRectBuff, DXGI_FORMAT_R32_UINT, 0);

    // Vertex creation procedures

	// m_sceneReady = _Drx11::createVertexBuff(&m_device, &m_pipeline.vertexDataBuff, &verticesTest[0], 18);
	m_sceneReady = _Drx11::createVertexBuff(&m_device, &m_pipeline.vertexRectBuff, gRec1_vData, 4);

    UINT strideTest = sizeof(float) * 3;
    UINT stride = sizeof(Eigen::Vector3f);
    UINT offset = 0;
    m_deviceCtx->IASetVertexBuffers(0, 1, &m_pipeline.vertexRectBuff, &stride, &offset); // Use the correct vertex buff

	// Input assembler inputs to pipeline

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

	if (m_pipelineReady && m_sceneReady)
		m_deviceCtx->DrawIndexed(6, 0, 0);
        // m_deviceCtx->Draw(6, 0);

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