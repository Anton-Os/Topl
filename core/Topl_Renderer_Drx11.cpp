#include "Topl_Renderer_Drx11.hpp"


namespace _Drx11 {
    static bool createVertexBuff(ID3D11Device** device, ID3D11Buffer** vBuff, vec3f_cptr vData, unsigned vCount) {
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

		HRESULT hr = (*(device))->CreateBuffer(&buffDesc, &buffData, vBuff);
		if (FAILED(hr)) return false;

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

		HRESULT hr = (*(device))->CreateBuffer(&buffDesc, &buffData, iBuff);
		if (FAILED(hr)) return false;

		return true;
	}

    static bool createConstBuff_Vec3(ID3D11Device** device, ID3D11Buffer** cBuff, vec3f_cptr vector) {
        D3D11_BUFFER_DESC buffDesc;
		ZeroMemory(&buffDesc, sizeof(buffDesc));
		buffDesc.Usage = D3D11_USAGE_DYNAMIC; // Note that this may need frequent updating
		// buffDesc.ByteWidth = sizeof(Eigen::Vector3f);
		buffDesc.ByteWidth = 16 * sizeof(BYTE);
		buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffDesc.MiscFlags = 0;
		buffDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA buffData;
		ZeroMemory(&buffData, sizeof(buffData));
		buffData.pSysMem = vector;
		buffData.SysMemPitch = 0;
		buffData.SysMemSlicePitch = 0;

		HRESULT hr = (*(device))->CreateBuffer(&buffDesc, &buffData, cBuff);
		if (FAILED(hr)) return false;

		return true;
	}
}

Topl_Renderer_Drx11::~Topl_Renderer_Drx11() {
	for(unsigned b = 0; b < m_bufferData.vertexBuffs_3f.size(); b++) // Release all vertex buffers
		m_bufferData.vertexBuffs_3f.at(b)->Release(); 

	for(unsigned b = 0; b < m_bufferData.indexBuffs_ui.size(); b++) // Release all index buffers
		m_bufferData.indexBuffs_ui.at(b)->Release();
	
	for(unsigned b = 0; b < m_bufferData.constBuffs_vec3f.size(); b++) // Release all constant buffers
		m_bufferData.constBuffs_vec3f.at(b)->Release();

	m_swapChain->Release();
	m_device->Release();
	m_deviceCtx->Release();

	m_pipeline.vertexShader->Release();
	m_pipeline.pixelShader->Release();
	m_pipeline.vsBuff->Release();
	m_pipeline.psBuff->Release();
	m_pipeline.vertexDataLayout->Release();
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

	HRESULT hr; // Error handler

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION,
		&swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceCtx);
	if (FAILED(hr)) return;
    
    ID3D11Texture2D* backBuffer;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if(FAILED(hr)) return; // Provide error handling code

    m_device->CreateRenderTargetView(backBuffer, NULL, &m_rtv);
    backBuffer->Release();

    m_deviceCtx->OMSetRenderTargets(1, &m_rtv, NULL);

    // Creates pipeline items
    Topl_Renderer_Drx11::createPipeline();

    return;
}

void Topl_Renderer_Drx11::createPipeline(void){
	ID3DBlob* errorBuff;
	HRESULT hr;

	if (FAILED(
		D3DCompileFromFile(L"C:\\AntonDocs\\Codex\\Ao-Project\\Topl-Skeleton\\MSVC_BUILD_2\\Debug\\Vertex_MostBasic.hlsl", 
		nullptr, nullptr, "main", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &m_pipeline.vsBuff, &errorBuff)
	)) {
		mPipelineReady = false;
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		return;
	}

	if (FAILED(
		D3DCompileFromFile(L"C:\\AntonDocs\\Codex\\Ao-Project\\Topl-Skeleton\\MSVC_BUILD_2\\Debug\\Pixel_MostBasic.hlsl", 
			nullptr, nullptr, "main", "ps_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &m_pipeline.psBuff, &errorBuff)
	)) {
		mPipelineReady = false;
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		return;
	} 

	hr = m_device->CreateVertexShader( m_pipeline.vsBuff->GetBufferPointer(), m_pipeline.vsBuff->GetBufferSize(),
		NULL, &m_pipeline.vertexShader);
    if(FAILED(hr)){
        mPipelineReady = false;
        return;
    }

	hr = m_device->CreatePixelShader(m_pipeline.psBuff->GetBufferPointer(), m_pipeline.psBuff->GetBufferSize(),
		NULL, &m_pipeline.pixelShader);
	if (FAILED(hr)){
        mPipelineReady = false;
        return;
    }

    m_deviceCtx->VSSetShader(m_pipeline.vertexShader, 0, 0);
    m_deviceCtx->PSSetShader(m_pipeline.pixelShader, 0, 0);

    mPipelineReady = true;
    // Think that is all for now...
}

void Topl_Renderer_Drx11::createPipeline(const Topl_Shader* vertexShader, const Topl_Shader* fragShader){
	ID3DBlob* errorBuff;
	HRESULT hr;

	// Check for correct type
	if (vertexShader->getType() != SHDR_Vertex) {
		mPipelineReady = false;
		OutputDebugStringA("Incorrect shader type provided for vertex shader");
		return;
	}

	// Check for valid source
	LPCWSTR vertexShaderSrc = (LPCWSTR)vertexShader->getFilePath();
	if (FAILED(
		D3DCompileFromFile(vertexShaderSrc,
			nullptr, nullptr, "main", "vs_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &m_pipeline.vsBuff, &errorBuff)
	)) {
		mPipelineReady = false;
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		return;
	}

	// Check for correct type
	if (fragShader->getType() != SHDR_Fragment) {
		mPipelineReady = false;
		OutputDebugStringA("Incorrect shader type provided for fragment shader");
		return;
	}

	// Check for valid source
	LPCWSTR fragShaderSrc = (LPCWSTR)fragShader->getFilePath();
	if (FAILED(
		D3DCompileFromFile(fragShaderSrc,
			nullptr, nullptr, "main", "ps_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &m_pipeline.psBuff, &errorBuff)
	)) {
		mPipelineReady = false;
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		return;
	}

	// Creation methods

	hr = m_device->CreateVertexShader(m_pipeline.vsBuff->GetBufferPointer(), m_pipeline.vsBuff->GetBufferSize(),
		NULL, &m_pipeline.vertexShader);
	if (FAILED(hr)) {
		mPipelineReady = false;
		return;
	}

	hr = m_device->CreatePixelShader(m_pipeline.psBuff->GetBufferPointer(), m_pipeline.psBuff->GetBufferSize(),
		NULL, &m_pipeline.pixelShader);
	if (FAILED(hr)) {
		mPipelineReady = false;
		return;
	}

	m_deviceCtx->VSSetShader(m_pipeline.vertexShader, 0, 0);
	m_deviceCtx->PSSetShader(m_pipeline.pixelShader, 0, 0);

	mPipelineReady = true;
}

void Topl_Renderer_Drx11::buildScene(const Topl_SceneGraph* sceneGraph) {


	for(unsigned g = 0; g < sceneGraph->getGeoCount(); g++) {
		unsigned lastIndex;

		tpl_gEntity_cptr gRect1_ptr = sceneGraph->getGeoNode(g + 1); // ids begin at 1 // Add safeguards!
		vec3f_cptr gRect1_vData = gRect1_ptr->mRenderObj->getVData();
		ui_cptr gRect1_iData = gRect1_ptr->mRenderObj->getIData();

		// Constant values procedures
		ID3D11Buffer* constBuff_vec3f; m_bufferData.constBuffs_vec3f.push_back(constBuff_vec3f);
		lastIndex = m_bufferData.constBuffs_vec3f.size() - 1;

		vec3f_cptr gRect1_position = gRect1_ptr->getPos();

		mSceneReady = _Drx11::createConstBuff_Vec3(&m_device, 
											&m_bufferData.constBuffs_vec3f.at(lastIndex),
											gRect1_position );
		if(!mSceneReady) return; // Error

		// Index creation procedures
		ID3D11Buffer* indexBuff; m_bufferData.indexBuffs_ui.push_back(indexBuff);
		lastIndex = m_bufferData.indexBuffs_ui.size() - 1;

		mSceneReady = _Drx11::createIndexBuff(&m_device, 
											&m_bufferData.indexBuffs_ui.at(lastIndex),
											(DWORD*)gRect1_iData, 
											gRect1_ptr->mRenderObj->getICount() ); 

		if(!mSceneReady) return; // Error

		// Vertex creation procedures
		ID3D11Buffer* vertexBuff; m_bufferData.vertexBuffs_3f.push_back(vertexBuff);
		lastIndex = m_bufferData.vertexBuffs_3f.size() - 1;

		mSceneReady = _Drx11::createVertexBuff(&m_device, 
												&m_bufferData.vertexBuffs_3f.at(lastIndex), // Last element
												gRect1_vData,
												gRect1_ptr->mRenderObj->getVCount());

		if(!mSceneReady) return;
	}

	// Input assembler inputs to pipeline
	// These procedures belong inside the pipeline creation

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

    mSceneReady = true;
    return;
}

void Topl_Renderer_Drx11::render(void){ // May need to pass scene graph?
    const float clearColor[] = { 0.4f, 0.4f, 0.9f, 1.0f };
    m_deviceCtx->ClearRenderTargetView(m_rtv, clearColor);

	// Vertex buffers are used as reference for loop, assumes all vectors have same number of buffers
	if (mPipelineReady && mSceneReady)
		for (unsigned b = 0; b < m_bufferData.vertexBuffs_3f.size(); b++) {
			m_deviceCtx->VSSetConstantBuffers(0, 1, &m_bufferData.constBuffs_vec3f.at(b));
			m_deviceCtx->IASetIndexBuffer(m_bufferData.indexBuffs_ui.at(b), DXGI_FORMAT_R32_UINT, 0);

			UINT strideTest = sizeof(float) * 3;
			UINT stride = sizeof(Eigen::Vector3f);
			UINT offset = 0;
			m_deviceCtx->IASetVertexBuffers(0, 1, &m_bufferData.vertexBuffs_3f.at(b), &stride, &offset);

			m_deviceCtx->DrawIndexed(6, 0, 0); // This is not yet scalable
		}
      

    m_swapChain->Present(0, 0);
}