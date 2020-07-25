#include "FileIO.hpp"

#include "Topl_Renderer_Drx11.hpp"

namespace _Drx11 {
	static bool createVertexBuff(ID3D11Device** device, ID3D11Buffer** vBuff, perVertex_cptr pvData, unsigned vCount) {
		D3D11_BUFFER_DESC buffDesc;
		ZeroMemory(&buffDesc, sizeof(buffDesc));
		buffDesc.Usage = D3D11_USAGE_DEFAULT;
		buffDesc.ByteWidth = sizeof(Geo_PerVertexData) * vCount;
		// buffDesc.ByteWidth = 28 * vCount;
		buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffDesc.CPUAccessFlags = 0;
		buffDesc.MiscFlags = 0;
		buffDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA buffData;
		ZeroMemory(&buffData, sizeof(buffData));
		buffData.pSysMem = pvData;

		HRESULT hr = (*(device))->CreateBuffer(&buffDesc, &buffData, vBuff);
		if (FAILED(hr)) return false;

		return true;
	}

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

	static bool createVertexBuff(ID3D11Device** device, ID3D11Buffer** vBuff, vec2f_cptr vData, unsigned vCount) {
		D3D11_BUFFER_DESC buffDesc;
		ZeroMemory(&buffDesc, sizeof(buffDesc));
		buffDesc.Usage = D3D11_USAGE_DEFAULT;
		buffDesc.ByteWidth = sizeof(Eigen::Vector2f) * vCount; // Code could be more reusable
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

	static Buffer_Drx11* findBuffer(enum BUFF_Type type, Buffer_Drx11** dBuffers, unsigned short count) {
		return *(dBuffers + type); // We know the offset with the type argument
	}

	static void discoverBuffers(Buffer_Drx11** dBuffers, std::vector<Buffer_Drx11>* bufferVector, unsigned id) {
		//TODO No error checks for duplicate buffers are provided, bufferVector needs to be vetted first
		for (std::vector<Buffer_Drx11>::iterator currentBuff = bufferVector->begin(); currentBuff < bufferVector->end(); currentBuff++)
			if (currentBuff->targetID == id)
				*(dBuffers + currentBuff->type) = &(*currentBuff); // Type indicates 
	}
}

Topl_Renderer_Drx11::~Topl_Renderer_Drx11() {
	for(unsigned b = 0; b < mBuffers.size(); b++)
		mBuffers.at(b).buffer->Release();

	for (unsigned t = 0; t < mTextures.size(); t++) {
		mTextures.at(t).texture->Release();
		mTextures.at(t).sampler->Release();
	}

	m_swapChain->Release();
	m_device->Release();
	m_deviceCtx->Release();

	m_pipeline.vertexShader->Release();
	m_pipeline.pixelShader->Release();
	m_pipeline.vsBuff->Release();
	m_pipeline.psBuff->Release();
	m_pipeline.vertexDataLayout->Release();
	m_pipeline.resourceView->Release();
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

	// Blend State Functions

	D3D11_BLEND_DESC blendStateDesc;
	ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));

	blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	m_device->CreateBlendState(&blendStateDesc, &m_pipeline.blendState);
    
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT blendMask = 0xffffffff;

	m_deviceCtx->OMSetBlendState(m_pipeline.blendState, blendFactor, blendMask);

	return;
}

void Topl_Renderer_Drx11::createPipeline(const Topl_Shader* vertexShader, const Topl_Shader* fragShader){
	ID3DBlob* errorBuff;
	HRESULT hr;
	size_t sourceSize;

	// Check for correct type
	if (vertexShader->getType() != SHDR_Vertex) {
		mPipelineReady = false;
		OutputDebugStringA("Incorrect shader type provided for vertex shader");
		return;
	}

	// Check for valid source
	sourceSize = strlen(vertexShader->getFilePath()) + 1;
	wchar_t* vertexShaderSrc = new wchar_t[sourceSize];
	mbstowcs(vertexShaderSrc, vertexShader->getFilePath(), sourceSize); // need proper conversion to wchar_t first, sorry
	if (FAILED(
		D3DCompileFromFile(vertexShaderSrc,
			nullptr, nullptr, "main", "vs_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &m_pipeline.vsBuff, &errorBuff)
	)) {
		mPipelineReady = false;
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		delete vertexShaderSrc; // Proper deallocation of the source string
		return;
	}
	delete vertexShaderSrc; // Proper deallocation of the source string

	// Check for correct type
	if (fragShader->getType() != SHDR_Fragment) {
		mPipelineReady = false;
		OutputDebugStringA("Incorrect shader type provided for fragment shader");
		return;
	}

	// Check for valid source
	sourceSize = strlen(fragShader->getFilePath()) + 1;
	wchar_t* fragmentShaderSrc = new wchar_t[sourceSize];
	mbstowcs(fragmentShaderSrc, fragShader->getFilePath(), sourceSize); // need proper conversion to wchar_t first, sorry
	if (FAILED(
		D3DCompileFromFile(fragmentShaderSrc,
			nullptr, nullptr, "main", "ps_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &m_pipeline.psBuff, &errorBuff)
	)) {
		mPipelineReady = false;
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		delete fragmentShaderSrc; // Proper deallocation of the source string
		return;
	}
	delete fragmentShaderSrc; // Proper deallocation of the source string

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

void Topl_Renderer_Drx11::buildScene(const Topl_SceneManager* sMan) {

	for(unsigned g = 0; g < sMan->getGeoCount(); g++) {
		tpl_gEntity_cptr geoTarget_ptr = sMan->getGeoNode(g + 1); // ids begin at 1 // Add safeguards!
		Geo_RenderObj* geoTarget_renderObj = (Geo_RenderObj*)geoTarget_ptr->mRenderObj;
		
		perVertex_cptr geoTarget_perVertexData = geoTarget_renderObj->getPerVertexData();
		ui_cptr geoTarget_iData = geoTarget_renderObj->getIData(); // TODO: Keep these, remove other getters
		vec3f_cptr geoTarget_position = geoTarget_ptr->getPos(); // TODO: Keep these, remove other getters

		ID3D11Buffer* constBuff_vec3f;
		mSceneReady = _Drx11::createConstBuff_Vec3(&m_device, 
											&constBuff_vec3f,
											geoTarget_position );
		mBuffers.push_back(Buffer_Drx11(g + 1, BUFF_Const_vec3f, constBuff_vec3f));

		if(!mSceneReady) return; // Error

		// Index creation procedures
		ID3D11Buffer* indexBuff;
		if(geoTarget_iData != nullptr) { // Check if index data exists for render object
			mSceneReady = _Drx11::createIndexBuff(&m_device, &indexBuff,
												(DWORD*)geoTarget_iData, geoTarget_renderObj->getICount() );

			mBuffers.push_back(Buffer_Drx11(g + 1, BUFF_Index_UI, indexBuff, geoTarget_renderObj->getICount()));

			if(!mSceneReady) return; // Error
		}

		ID3D11Buffer* vertexBuff;

		mSceneReady = _Drx11::createVertexBuff(&m_device, &vertexBuff,
												geoTarget_perVertexData, geoTarget_renderObj->getVCount());

		mBuffers.push_back(Buffer_Drx11(g + 1, BUFF_Vertex_3F, vertexBuff, geoTarget_renderObj->getVCount()));

#ifdef RASTERON_H
		unsigned texCount = sMan->getTextures(g + 1, nullptr); 
		if (texCount > 0) {
			const Rasteron_Image* baseTex = sMan->getFirstTexture(g + 1);

			genTexture(baseTex, g + 1);
		}
#endif

		if(!mSceneReady) return;
		mMaxGraphicsID = g + 1; // Gives us the greatest buffer ID number
	}

	// Input assembler inputs to pipeline
	// These procedures belong inside the pipeline creation

    D3D11_INPUT_ELEMENT_DESC layoutTest[] ={
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    UINT layoutElemCount = ARRAYSIZE(layoutTest); // REFINE THIS

    m_device->CreateInputLayout(
        layoutTest, layoutElemCount,
        m_pipeline.vsBuff->GetBufferPointer(), m_pipeline.vsBuff->GetBufferSize(), 
        &m_pipeline.vertexDataLayout
    );

    m_deviceCtx->IASetInputLayout(m_pipeline.vertexDataLayout);
	m_deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    mSceneReady = true;
    return;
}


#ifdef RASTERON_H

Rasteron_Image* Topl_Renderer_Drx11::getFrame(){
	HRESULT hr;

	ID3D11Texture2D* framebuff;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&framebuff);

	D3D11_TEXTURE2D_DESC framebuffDesc;
	framebuff->GetDesc(&framebuffDesc);
	framebuffDesc.BindFlags = 0;
	framebuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	framebuffDesc.Usage = D3D11_USAGE_STAGING;

	ID3D11Texture2D* destTexture;
	hr = m_device->CreateTexture2D(&framebuffDesc, NULL, &destTexture);
	
	// Copying phase and subresource mapping
	m_deviceCtx->CopyResource(destTexture, framebuff);
	D3D11_MAPPED_SUBRESOURCE resource;
	unsigned subresource = D3D11CalcSubresource(0, 0, 0);
	hr = m_deviceCtx->Map(destTexture, subresource, D3D11_MAP_READ_WRITE, 0, &resource);

	// Custom Image format creation
	Rasteron_Image* rstn_image = (Rasteron_Image*)malloc(sizeof(Rasteron_Image));

	rstn_image->width = WIN_WIDTH; // defined in native_os_def
	rstn_image->height = WIN_HEIGHT; // defined in native_os_def
	rstn_image->name = "framebuff"; // TODO: Make this incremental, i.e framebuff1 framebuff2

	rstn_image->data = (uint32_t*)malloc(rstn_image->width * rstn_image->height * sizeof(uint32_t));
	// TODO: Perform memcpy instead of pointer assignment to resource data
	// rstn_image->data = static_cast<uint32_t*>(resource.pData); // Copies from resource

	return rstn_image;
}

void Topl_Renderer_Drx11::genTexture(const Rasteron_Image* image, unsigned id){
	HRESULT hrCode; // For viewing potential issues

	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* sampler;
	hrCode = m_device->CreateSamplerState(&sd, &sampler);
	
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
    texDesc.Width = image->width;
    texDesc.Height = image->height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0; 
	/* texDesc.Usage = D3D11_USAGE_DYNAMIC;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texDesc.MiscFlags = 0; */

    D3D11_SUBRESOURCE_DATA texData;
    texData.pSysMem = /* (const void*) */image->data;
	texData.SysMemPitch = sizeof(uint32_t) * image->width;
	texData.SysMemSlicePitch = 0;

	ID3D11Texture2D* texture;
    hrCode = m_device->CreateTexture2D( &texDesc, &texData, &texture);

	mTextures.push_back(Texture_Drx11(id, TEX_2D, TEX_Wrap, texture, sampler));
}

#endif

void Topl_Renderer_Drx11::update(const Topl_SceneManager* sMan){
	Buffer_Drx11* targetBuff = nullptr;

	for(unsigned g = 0; g < sMan->getGeoCount(); g++) {
		tpl_gEntity_cptr geoTarget_ptr = sMan->getGeoNode(g + 1); // ids begin at 1 // Add safeguards!
		vec3f_cptr geoTarget_position = geoTarget_ptr->getPos();

		// Interested in the constant pos buffer, we have to find it
		for (std::vector<Buffer_Drx11>::iterator currentBuff = mBuffers.begin(); currentBuff < mBuffers.end(); currentBuff++)
			if (currentBuff->targetID == g + 1 && currentBuff->type == BUFF_Const_vec3f){
				targetBuff = &(*currentBuff);
				break;
			}

		if(targetBuff == nullptr){
			OutputDebugStringA("Position const buffer could not be located! ");
			return;
		}

		mSceneReady = _Drx11::createConstBuff_Vec3(&m_device, &targetBuff->buffer, geoTarget_position );

		if(!mSceneReady) return;
	}

    mSceneReady = true;
	return;
}

void Topl_Renderer_Drx11::render(void){ // May need to pass scene graph?
    const float clearColor[] = { 0.4f, 0.4f, 0.9f, 1.0f };
    m_deviceCtx->ClearRenderTargetView(m_rtv, clearColor);

	switch(mDrawType) {
	case DRAW_Triangles:
		m_deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	default:
		OutputDebugStringA("Draw type not supported yet!");
		return;
	}

	Buffer_Drx11** dBuffers = (Buffer_Drx11**)malloc(MAX_BUFFERS_PER_TARGET * sizeof(Buffer_Drx11*));

	// Vertex buffers are used as reference for loop, assumes all vectors have same number of buffers
	if (mPipelineReady && mSceneReady)
		for (unsigned id = 1; id <= mMaxGraphicsID; id++) { // ID signifies graphics target id
		// for (unsigned id = mMaxGraphicsID; id >= 1; id--) {

			_Drx11::discoverBuffers(dBuffers, &mBuffers, id);

			Buffer_Drx11* posBuff = _Drx11::findBuffer(BUFF_Const_vec3f, dBuffers, MAX_BUFFERS_PER_TARGET);
			Buffer_Drx11* indexBuff = _Drx11::findBuffer(BUFF_Index_UI, dBuffers, MAX_BUFFERS_PER_TARGET);
			Buffer_Drx11* vertexBuff = _Drx11::findBuffer(BUFF_Vertex_3F, dBuffers, MAX_BUFFERS_PER_TARGET);
			if (posBuff == nullptr || indexBuff == nullptr || vertexBuff == nullptr) {
				OutputDebugStringA("One of the required buffers was not ready for drawing. Oops");
				return;
			}

			m_deviceCtx->VSSetConstantBuffers(0, 1, &posBuff->buffer);
			m_deviceCtx->IASetIndexBuffer(indexBuff->buffer, DXGI_FORMAT_R32_UINT, 0);

			UINT stride = sizeof(Geo_PerVertexData);
			UINT offset = 0;
			m_deviceCtx->IASetVertexBuffers(0, 1, &vertexBuff->buffer, &stride, &offset);

			for (unsigned t = 0; t < mTextures.size(); t++) {
				if (mTextures.at(t).targetID > id) break; // This means we have passed it in sequence
				else if (mTextures.at(t).targetID == id) {
					ID3D11Texture2D* baseTex = mTextures.at(t).texture;
					ID3D11SamplerState* baseSampler = mTextures.at(t).sampler;
					// TODO: Need a getter function for resource view as well

					D3D11_SHADER_RESOURCE_VIEW_DESC desc;
					ZeroMemory(&desc, sizeof(desc));
					desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					desc.Texture2D.MostDetailedMip = 0;
					desc.Texture2D.MipLevels = 1; // Customize these values if needed
					
					m_device->CreateShaderResourceView(baseTex, &desc, &m_pipeline.resourceView);

					m_deviceCtx->PSSetShaderResources(0, 1, &m_pipeline.resourceView);
					m_deviceCtx->PSSetSamplers(0, 1, &baseSampler);
					break;
				}
			}

			m_deviceCtx->DrawIndexed(indexBuff->count, 0, 0);
		}

	free(dBuffers);

    m_swapChain->Present(0, 0);
}