#include "FileIO.hpp"

#include "Topl_Renderer_Drx11.hpp"

namespace _Drx11 {
	static DXGI_FORMAT getFormatFromShaderVal(enum SHDR_ValueType type){
		DXGI_FORMAT format;

		switch(type) {
		case SHDR_float_vec4:
			format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
		case SHDR_float_vec3:
			format = DXGI_FORMAT_R32G32B32_FLOAT; break;
		case SHDR_float_vec2:
			format = DXGI_FORMAT_R32G32_FLOAT; break;
		case SHDR_float:
			format = DXGI_FORMAT_R32_FLOAT; break;
		// DOUBLE SUPPORT PLACEHOLDER
		case SHDR_uint_vec4:
			format = DXGI_FORMAT_R32G32B32A32_UINT; break;
		case SHDR_uint_vec3:
			format = DXGI_FORMAT_R32G32B32_UINT; break;
		case SHDR_uint_vec2:
			format = DXGI_FORMAT_R32G32_UINT; break;
		case SHDR_uint:
			format = DXGI_FORMAT_R32_UINT; break;
		case SHDR_int_vec4:
			format = DXGI_FORMAT_R32G32B32A32_SINT; break;
		case SHDR_int_vec3:
			format = DXGI_FORMAT_R32G32B32_SINT; break;
		case SHDR_int_vec2:
			format = DXGI_FORMAT_R32G32_SINT; break;
		case SHDR_int:
			format = DXGI_FORMAT_R32_SINT; break;
		default:
			OutputDebugStringA("Drx11 Shader Input Type Not Supported!");
			break;
		}

		return format;
	}

	// Shared Renderer Code!
	static unsigned getOffsetFromShaderVal(enum SHDR_ValueType type){
		unsigned offset = 0;

		switch(type) {
		case SHDR_float_vec4:
			offset = sizeof(float) * 4; break;
		case SHDR_float_vec3:
			offset = sizeof(float) * 3; break;
		case SHDR_float_vec2:
			offset = sizeof(float) * 2; break;
		case SHDR_float:
			offset = sizeof(float); break;
		case SHDR_double_vec4:
			offset = sizeof(double) * 4; break;
		case SHDR_double_vec3:
			offset = sizeof(double) * 3; break;
		case SHDR_double_vec2:
			offset = sizeof(double) * 2; break;
		case SHDR_double:
			offset = sizeof(double); break;
		case SHDR_uint_vec4:
			offset = sizeof(unsigned) * 4; break;
		case SHDR_uint_vec3:
			offset = sizeof(unsigned) * 3;  break;
		case SHDR_uint_vec2:
			offset = sizeof(unsigned) * 2; break;
		case SHDR_uint:
			offset = sizeof(unsigned); break;
		case SHDR_int_vec4:
			offset = sizeof(int) * 4; break;
		case SHDR_int_vec3:
			offset = sizeof(int) * 3; break;
		case SHDR_int_vec2:
			offset = sizeof(int) * 2; break;
		case SHDR_int:
			offset = sizeof(int); break;
		default:
			OutputDebugStringA("Drx11 Shader Input Type Not Supported!");
			break;
		}

		return offset;
	}

	static bool createVertexBuff(ID3D11Device** device, ID3D11Buffer** vBuff, geoVertex_cptr pvData, unsigned vCount) {
		D3D11_BUFFER_DESC buffDesc;
		ZeroMemory(&buffDesc, sizeof(buffDesc));
		buffDesc.Usage = D3D11_USAGE_DEFAULT;
		buffDesc.ByteWidth = sizeof(Geo_Vertex) * vCount;
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

	static bool createBlockBuff(ID3D11Device** device, ID3D11Buffer** cBuff, const std::vector<uint8_t> *const blockBytes) {
		D3D11_BUFFER_DESC buffDesc;
		ZeroMemory(&buffDesc, sizeof(buffDesc));
		buffDesc.ByteWidth = sizeof(uint8_t) * blockBytes->size();
		buffDesc.Usage = D3D11_USAGE_DYNAMIC;
		buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffDesc.MiscFlags = 0;
		buffDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA buffData;
		ZeroMemory(&buffData, sizeof(buffData));
		buffData.pSysMem = (const void*)blockBytes->data();
		buffData.SysMemPitch = 0;
		buffData.SysMemSlicePitch = 0;

		HRESULT hr = (*(device))->CreateBuffer(&buffDesc, &buffData, cBuff);
		if (FAILED(hr)) return false;

		return true;
	}

	static D3D11_INPUT_ELEMENT_DESC getElementDescFromInput(const Shader_Type* input, UINT offset){
		D3D11_INPUT_ELEMENT_DESC inputElementDesc;
		inputElementDesc.SemanticName = input->semantic.c_str();
		inputElementDesc.SemanticIndex = 0;
		inputElementDesc.Format = _Drx11::getFormatFromShaderVal(input->type);
		inputElementDesc.InputSlot = 0;
		inputElementDesc.AlignedByteOffset = offset;
		inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputElementDesc.InstanceDataStepRate = 0;

		return inputElementDesc;
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
	for(unsigned b = 0; b < _buffers.size(); b++)
		_buffers.at(b).buffer->Release();

	for (unsigned t = 0; t < _textures.size(); t++) {
		_textures.at(t).texture->Release();
		_textures.at(t).sampler->Release();
		_textures.at(t).resView->Release();
	}

	_swapChain->Release();
	_device->Release();
	_deviceCtx->Release();

	_pipeline.vertexShader->Release();
	_pipeline.pixelShader->Release();
	_pipeline.hullShader->Release();
	_pipeline.domainShader->Release();
	_pipeline.geomShader->Release();
	_pipeline.vsBuff->Release();
	_pipeline.psBuff->Release();
	_pipeline.hsBuff->Release();
	_pipeline.dsBuff->Release();
	_pipeline.gsBuff->Release();

	_vertexDataLayout->Release();
	_resourceView->Release();
	_blendState->Release();
	_rasterizerState->Release();
}

void Topl_Renderer_Drx11::init(NATIVE_WINDOW hwnd) {
	_nativeContext.window = hwnd; // Supplying platform specific stuff

    DXGI_MODE_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

    bufferDesc.Width = TOPL_WIN_WIDTH;
    bufferDesc.Height = TOPL_WIN_HEIGHT;
    // bufferDesc.RefreshRate.Numerator = 60;
    // bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.RefreshRate.Numerator = 1;
	bufferDesc.RefreshRate.Denominator = 60;
    bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SWAP_CHAIN_DESC swapChainDesc; 
    
    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

    swapChainDesc.BufferDesc = bufferDesc;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    // swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferCount = 2; // bgfx dxgi.cpp line 398
	swapChainDesc.OutputWindow = _nativeContext.window; 
    swapChainDesc.Windowed = TRUE; 
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	HRESULT hr; // Error handler

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION,
		&swapChainDesc, &_swapChain, &_device, NULL, &_deviceCtx);
	if (FAILED(hr)) return;
    
    // ID3D11Texture2D* backBuffer;
	ID3D11Resource* backBuffer; // bgfx renderer_d3d11.cpp line 4660
	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if(FAILED(hr)) return; // Provide error handling code

    _device->CreateRenderTargetView(backBuffer, NULL, &_rtv);
    backBuffer->Release();

    _deviceCtx->OMSetRenderTargets(1, &_rtv, NULL);

	// Viewport Creation

    D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Height = TOPL_WIN_HEIGHT;
    viewport.Width = TOPL_WIN_WIDTH;

	_deviceCtx->RSSetViewports(1, &viewport);

	// Blend State creation

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

	_device->CreateBlendState(&blendStateDesc, &_blendState);
    
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT blendMask = 0xffffffff;

	_deviceCtx->OMSetBlendState(_blendState, blendFactor, blendMask);

	// Rasterizer State creation

	D3D11_RASTERIZER_DESC rasterizerStateDesc;
	ZeroMemory(&rasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));

	/* rasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerStateDesc.CullMode = D3D11_CULL_FRONT;
	rasterizerStateDesc.FrontCounterClockwise = true;
	rasterizerStateDesc.DepthBias = false;
	rasterizerStateDesc.DepthBiasClamp = 0;
	rasterizerStateDesc.SlopeScaledDepthBias = 0;
	rasterizerStateDesc.DepthClipEnable = true;
	rasterizerStateDesc.ScissorEnable = true;
	rasterizerStateDesc.MultisampleEnable = true;
	rasterizerStateDesc.AntialiasedLineEnable = false;
	// rasterizerStateDesc.ForcedSampleCount = 0;

	_device->CreateRasterizerState(&rasterizerStateDesc, &_rasterizerState);
	_deviceCtx->RSSetState(_rasterizerState); */

	return;
}

void Topl_Renderer_Drx11::pipeline(const Topl_Shader* vertexShader, const Topl_Shader* fragShader){
	ID3DBlob* errorBuff;
	HRESULT hr;
	size_t sourceSize;

	// Vertex shader compilation and creation code
	sourceSize = strlen(vertexShader->getFilePath()) + 1;
	wchar_t* vertexShaderSrc_wchar = new wchar_t[sourceSize];
	mbstowcs(vertexShaderSrc_wchar, vertexShader->getFilePath(), sourceSize); // need proper conversion to wcharhar_t
	if (FAILED(
		D3DCompileFromFile(vertexShaderSrc_wchar,
			nullptr, nullptr, "main", "vs_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &_pipeline.vsBuff, &errorBuff)
	)) {
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		delete vertexShaderSrc_wchar; // Proper deallocation of the source string
		return;
	}
	delete vertexShaderSrc_wchar; // Proper deallocation of the source string

	hr = _device->CreateVertexShader(_pipeline.vsBuff->GetBufferPointer(), _pipeline.vsBuff->GetBufferSize(),
		NULL, &_pipeline.vertexShader);
	if (FAILED(hr)) {
		_isPipelineReady = false;
		return;
	}

	// Pixel shader compilation and creation code
	sourceSize = strlen(fragShader->getFilePath()) + 1;
	wchar_t* fragmentShaderSrc_wchar = new wchar_t[sourceSize];
	mbstowcs(fragmentShaderSrc_wchar, fragShader->getFilePath(), sourceSize); // need proper conversion to wcharhar_t first, sorry
	if (FAILED(
		D3DCompileFromFile(fragmentShaderSrc_wchar,
			nullptr, nullptr, "main", "ps_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &_pipeline.psBuff, &errorBuff)
	)) {
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		delete fragmentShaderSrc_wchar; // Proper deallocation of the source string
		return;
	}
	delete fragmentShaderSrc_wchar; // Proper deallocation of the source string

	hr = _device->CreatePixelShader(_pipeline.psBuff->GetBufferPointer(), _pipeline.psBuff->GetBufferSize(),
		NULL, &_pipeline.pixelShader);
	if (FAILED(hr)) {
		_isPipelineReady = false;
		return;
	}

	_deviceCtx->VSSetShader(_pipeline.vertexShader, 0, 0);
	_deviceCtx->PSSetShader(_pipeline.pixelShader, 0, 0);


	// Generating an input layout based on Vertex Shader Inputs

	D3D11_INPUT_ELEMENT_DESC* layout_ptr = (D3D11_INPUT_ELEMENT_DESC*)malloc(sizeof(D3D11_INPUT_ELEMENT_DESC) * vertexShader->getInputCount());
	unsigned inputElementOffset = 0;
	for(unsigned i = 0; i < vertexShader->getInputCount(); i++){
		*(layout_ptr + i) = _Drx11::getElementDescFromInput(vertexShader->getInputAtIndex(i), inputElementOffset);
		inputElementOffset += _Drx11::getOffsetFromShaderVal(vertexShader->getInputAtIndex(i)->type);
	}
    UINT layoutElemCount = (unsigned)vertexShader->getInputCount();

    _device->CreateInputLayout(
        layout_ptr, layoutElemCount,
        _pipeline.vsBuff->GetBufferPointer(), _pipeline.vsBuff->GetBufferSize(), 
        &_vertexDataLayout
    );

    _deviceCtx->IASetInputLayout(_vertexDataLayout);

	free(layout_ptr); // Deallocating layout_ptr and all associated data
	_isPipelineReady = true;
}

void Topl_Renderer_Drx11::pipeline(topl_shader_cptr vertexShader, topl_shader_cptr fragShader, topl_shader_cptr tessCtrlShader, topl_shader_cptr tessEvalShader, topl_shader_cptr geomShader){
	ID3DBlob* errorBuff;
	HRESULT hr;
	size_t sourceSize;

	// Vertex shader compilation and creation code
	sourceSize = strlen(vertexShader->getFilePath()) + 1;
	wchar_t* vertexShaderSrc_wchar = new wchar_t[sourceSize];
	mbstowcs(vertexShaderSrc_wchar, vertexShader->getFilePath(), sourceSize); // need proper conversion to wcharhar_t
	if (FAILED(
		D3DCompileFromFile(vertexShaderSrc_wchar,
			nullptr, nullptr, "main", "vs_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &_pipeline.vsBuff, &errorBuff)
	)) {
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		delete vertexShaderSrc_wchar; // Proper deallocation of the source string
		return;
	}
	delete vertexShaderSrc_wchar; // Proper deallocation of the source string

	hr = _device->CreateVertexShader(_pipeline.vsBuff->GetBufferPointer(), _pipeline.vsBuff->GetBufferSize(),
		NULL, &_pipeline.vertexShader);
	if (FAILED(hr)) {
		_isPipelineReady = false;
		return;
	}

	// Hull shader compilation and creation code
	sourceSize = strlen(tessCtrlShader->getFilePath()) + 1;
	wchar_t* tessCtrlShaderSrc_wchar = new wchar_t[sourceSize];
	mbstowcs(tessCtrlShaderSrc_wchar, tessCtrlShader->getFilePath(), sourceSize); // need proper conversion to wcharhar_t
	if (FAILED(
		D3DCompileFromFile(tessCtrlShaderSrc_wchar,
			nullptr, nullptr, "main", "hs_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &_pipeline.hsBuff, &errorBuff)
	)) {
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		delete tessCtrlShaderSrc_wchar; // Proper deallocation of the source string
		return;
	}
	delete tessCtrlShaderSrc_wchar; // Proper deallocation of the source string

	hr = _device->CreateHullShader(_pipeline.hsBuff->GetBufferPointer(), _pipeline.hsBuff->GetBufferSize(),
		NULL, &_pipeline.hullShader);
	if (FAILED(hr)) {
		_isPipelineReady = false;
		return;
	}

	// Domain shader compilation and creation code
	sourceSize = strlen(tessEvalShader->getFilePath()) + 1;
	wchar_t* tessEvalShaderSrc_wchar = new wchar_t[sourceSize];
	mbstowcs(tessEvalShaderSrc_wchar, tessEvalShader->getFilePath(), sourceSize); // need proper conversion to wcharhar_t
	if (FAILED(
		D3DCompileFromFile(tessEvalShaderSrc_wchar,
			nullptr, nullptr, "main", "ds_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &_pipeline.dsBuff, &errorBuff)
	)) {
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		delete tessEvalShaderSrc_wchar; // Proper deallocation of the source string
		return;
	}
	delete tessEvalShaderSrc_wchar; // Proper deallocation of the source string

	hr = _device->CreateDomainShader(_pipeline.dsBuff->GetBufferPointer(), _pipeline.dsBuff->GetBufferSize(),
		NULL, &_pipeline.domainShader);
	if (FAILED(hr)) {
		_isPipelineReady = false;
		return;
	}

	// Geometry shader compilation and creation code
	sourceSize = strlen(geomShader->getFilePath()) + 1;
	wchar_t* geomShaderSrc_wchar = new wchar_t[sourceSize];
	mbstowcs(geomShaderSrc_wchar, geomShader->getFilePath(), sourceSize); // need proper conversion to wcharhar_t
	if (FAILED(
		D3DCompileFromFile(geomShaderSrc_wchar,
			nullptr, nullptr, "main", "gs_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &_pipeline.gsBuff, &errorBuff)
	)) {
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		delete geomShaderSrc_wchar; // Proper deallocation of the source string
		return;
	}
	delete geomShaderSrc_wchar; // Proper deallocation of the source string

	hr = _device->CreateGeometryShader(_pipeline.gsBuff->GetBufferPointer(), _pipeline.gsBuff->GetBufferSize(),
		NULL, &_pipeline.geomShader);
	if (FAILED(hr)) {
		_isPipelineReady = false;
		return;
	}

	// Pixel shader compilation and creation code
	sourceSize = strlen(fragShader->getFilePath()) + 1;
	wchar_t* fragmentShaderSrc_wchar = new wchar_t[sourceSize];
	mbstowcs(fragmentShaderSrc_wchar, fragShader->getFilePath(), sourceSize); // need proper conversion to wcharhar_t first, sorry
	if (FAILED(
		D3DCompileFromFile(fragmentShaderSrc_wchar,
			nullptr, nullptr, "main", "ps_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &_pipeline.psBuff, &errorBuff)
	)) {
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		delete fragmentShaderSrc_wchar; // Proper deallocation of the source string
		return;
	}
	delete fragmentShaderSrc_wchar; // Proper deallocation of the source string

	hr = _device->CreatePixelShader(_pipeline.psBuff->GetBufferPointer(), _pipeline.psBuff->GetBufferSize(),
		NULL, &_pipeline.pixelShader);
	if (FAILED(hr)) {
		_isPipelineReady = false;
		return;
	}

	_deviceCtx->VSSetShader(_pipeline.vertexShader, 0, 0);
	_deviceCtx->HSSetShader(_pipeline.hullShader, 0, 0);
	_deviceCtx->DSSetShader(_pipeline.domainShader, 0, 0);
	_deviceCtx->GSSetShader(_pipeline.geomShader, 0, 0);
	_deviceCtx->PSSetShader(_pipeline.pixelShader, 0, 0);

	// Generating an input layout based on Vertex Shader Inputs

	D3D11_INPUT_ELEMENT_DESC* layout_ptr = (D3D11_INPUT_ELEMENT_DESC*)malloc(sizeof(D3D11_INPUT_ELEMENT_DESC) * vertexShader->getInputCount());
	unsigned inputElementOffset = 0;
	for(unsigned i = 0; i < vertexShader->getInputCount(); i++){
		*(layout_ptr + i) = _Drx11::getElementDescFromInput(vertexShader->getInputAtIndex(i), inputElementOffset);
		inputElementOffset += _Drx11::getOffsetFromShaderVal(vertexShader->getInputAtIndex(i)->type);
	}
    UINT layoutElemCount = (unsigned)vertexShader->getInputCount();

    _device->CreateInputLayout(
        layout_ptr, layoutElemCount,
        _pipeline.vsBuff->GetBufferPointer(), _pipeline.vsBuff->GetBufferSize(), 
        &_vertexDataLayout
    );

    _deviceCtx->IASetInputLayout(_vertexDataLayout);

	free(layout_ptr); // Deallocating layout_ptr and all associated data
	_isPipelineReady = true;
}

void Topl_Renderer_Drx11::clearView(){
	const float clearColor[] = { 0.4f, 0.4f, 0.9f, 1.0f };
    _deviceCtx->ClearRenderTargetView(_rtv, clearColor);
	// _deviceCtx->ClearDepthStencilView(_rtv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);
}

void Topl_Renderer_Drx11::buildScene(const Topl_Scene* scene) {
	_renderCtx.push_back(Topl_RenderContext_Drx11());
	_currentRenderCtx = &_renderCtx.back(); // gets the most recent render context

	const Topl_Shader* primaryShader = findShader(_primaryShaderType);
	std::vector<uint8_t> blockBytes; // For constant and uniform buffer updates

	// Generates object for single scene block buffer
	if (primaryShader->genPerSceneDataBlock(scene, &blockBytes)) {
		_isSceneReady = _Drx11::createBlockBuff(&_device, &_sceneBlockBuff, &blockBytes);
		_buffers.push_back(Buffer_Drx11(_sceneBlockBuff));
	}

	for(unsigned g = 0; g < scene->getGeoCount(); g++) {
		unsigned currentRenderID = g + 1;
		topl_geoComponent_cptr geoTarget_ptr = scene->getGeoComponent(currentRenderID - 1); // ids begin at 1, conversion is required
		Geo_RenderObj* geoTarget_renderObj = (Geo_RenderObj*)geoTarget_ptr->getRenderObj();
		
		geoVertex_cptr geoTarget_perVertex = geoTarget_renderObj->getVertices();
		ui_cptr geoTarget_iData = geoTarget_renderObj->getIndices();

		// Geo component block implementation
		if (primaryShader->genPerGeoDataBlock(geoTarget_ptr, &blockBytes)) {
			ID3D11Buffer* renderBlockBuff = nullptr;
			_isSceneReady = _Drx11::createBlockBuff(&_device, &renderBlockBuff, &blockBytes);
			_buffers.push_back(Buffer_Drx11(currentRenderID, BUFF_Render_Block, renderBlockBuff));
		}
		if (!_isSceneReady) return; // Error

		// Index creation procedures
		ID3D11Buffer* indexBuff = nullptr;
		if (geoTarget_iData != nullptr) { // Checks if index data exists for render object
			_isSceneReady = _Drx11::createIndexBuff(&_device, &indexBuff, (DWORD*)geoTarget_iData, geoTarget_renderObj->getIndexCount());
			_buffers.push_back(Buffer_Drx11(currentRenderID, BUFF_Index_UI, indexBuff, geoTarget_renderObj->getIndexCount()));
		} else _buffers.push_back(Buffer_Drx11(currentRenderID, BUFF_Index_UI, indexBuff, 0));
		if (!_isSceneReady) return; // Error

		ID3D11Buffer* vertexBuff = nullptr;
		_isSceneReady = _Drx11::createVertexBuff(&_device, &vertexBuff,
												geoTarget_perVertex, geoTarget_renderObj->getVerticesCount());

		_buffers.push_back(Buffer_Drx11(currentRenderID, BUFF_Vertex_Type, vertexBuff, geoTarget_renderObj->getVerticesCount()));
		if (!_isSceneReady) return;

#ifdef RASTERON_H
		const Rasteron_Image* baseTex = scene->getFirstTexture(geoTarget_ptr->getName());
		if(baseTex != nullptr) genTexture(baseTex, currentRenderID); // Add the method definition
#endif

		if(!_isSceneReady) return;
		_mainRenderIDs = currentRenderID; // Gives us the greatest buffer ID number
	}

    _isSceneReady = true;
    return;
}


#ifdef RASTERON_H

// EXPERIMENTAL SCREEN CAPTURE CODE!
Rasteron_Image* Topl_Renderer_Drx11::getFrame(){
	HRESULT hr;

	ID3D11Texture2D* framebuff;
	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&framebuff);

	D3D11_TEXTURE2D_DESC framebuffDesc;
	framebuff->GetDesc(&framebuffDesc);
	framebuffDesc.BindFlags = 0;
	framebuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	framebuffDesc.Usage = D3D11_USAGE_STAGING;

	ID3D11Texture2D* srcTexture;
	hr = _device->CreateTexture2D(&framebuffDesc, NULL, &srcTexture);
	
	// Copying phase and subresource mapping
	_deviceCtx->CopyResource(srcTexture, framebuff);
	D3D11_MAPPED_SUBRESOURCE resource;
	unsigned subresource = D3D11CalcSubresource(0, 0, 0);
	hr = _deviceCtx->Map(srcTexture, subresource, D3D11_MAP_READ_WRITE, 0, &resource);
	const uint32_t* srcTexData = static_cast<const uint32_t*>(resource.pData);

	// Custom Image format creation
	Rasteron_Image* rast_image = (Rasteron_Image*)malloc(sizeof(Rasteron_Image));

	rast_image->width = TOPL_WIN_WIDTH; // defined in native_os_def
	rast_image->height = TOPL_WIN_HEIGHT; // defined in native_os_def
	rast_image->name = "framebuff"; // TODO: Make this incremental, i.e framebuff1 framebuff2

	rast_image->data = (uint32_t*)malloc(rast_image->width * rast_image->height * sizeof(uint32_t));
	memcpy(rast_image->data, srcTexData, rast_image->width * rast_image->height * sizeof(uint32_t));

	return rast_image;
}

void Topl_Renderer_Drx11::genTexture(const Rasteron_Image* image, unsigned id){
	HRESULT hrCode; // For viewing potential issues

	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.MipLODBias = 0.0f;
	sd.MaxAnisotropy = 1;
	sd.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* sampler;
	hrCode = _device->CreateSamplerState(&sd, &sampler);
	
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
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    texDesc.CPUAccessFlags = 0;
    // texDesc.MiscFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    D3D11_SUBRESOURCE_DATA texData;
    texData.pSysMem = image->data;
	texData.SysMemPitch = sizeof(uint32_t) * image->width;
	texData.SysMemSlicePitch = 0;

	ID3D11Texture2D* texture;
    hrCode = _device->CreateTexture2D( &texDesc, &texData, &texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC resViewDesc;
	ZeroMemory(&resViewDesc, sizeof(resViewDesc));
	resViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	// resViewDesc.Texture2D.MostDetailedMip = 0;
	resViewDesc.Texture2D.MipLevels = -1;
	
	ID3D11ShaderResourceView* resView;
	_device->CreateShaderResourceView(texture, &resViewDesc, &resView);
	_deviceCtx->UpdateSubresource(texture, 0, 0, image->data, texData.SysMemPitch, 0);
	_deviceCtx->GenerateMips(resView);

	_textures.push_back(Texture_Drx11(id, TEX_2D, TEX_Wrap, texture, sampler, resView));
}

#endif

void Topl_Renderer_Drx11::update(const Topl_Scene* scene){
	const Topl_Shader* primaryShader = findShader(_primaryShaderType); // New Implementation
	std::vector<uint8_t> blockBytes; // New Implementation
	Buffer_Drx11* renderBlockBuff = nullptr;
	// Buffer_Drx11* sceneBlockBuff = nullptr;

	if (primaryShader->genPerSceneDataBlock(scene, &blockBytes) && _buffers.front().targetID == SPECIAL_SCENE_RENDER_ID)
		_Drx11::createBlockBuff(&_device, &_buffers.front().buffer, &blockBytes); // Update code should work

	for(unsigned g = 0; g < scene->getGeoCount(); g++) {
		unsigned currentRenderID = g + 1;
		topl_geoComponent_cptr geoTarget_ptr = scene->getGeoComponent(currentRenderID - 1); // ids begin at 1, conversion is required

		if (primaryShader->genPerGeoDataBlock(geoTarget_ptr, &blockBytes)) {
			for (std::vector<Buffer_Drx11>::iterator currentBuff = _buffers.begin(); currentBuff < _buffers.end(); currentBuff++)
				if (currentBuff->targetID == currentRenderID && currentBuff->type == BUFF_Render_Block) {
					renderBlockBuff = &(*currentBuff);
					break;
				}

			if (renderBlockBuff == nullptr) { // TODO: Replace this!
				OutputDebugStringA("Block buffer could not be located!");
				return;
			} else _isSceneReady = _Drx11::createBlockBuff(&_device, &renderBlockBuff->buffer, &blockBytes);

			if (!_isSceneReady) return; // Error
		}
	}

    _isSceneReady = true;
	return;
}

void Topl_Renderer_Drx11::render(void){
    /* const float clearColor[] = { 0.4f, 0.4f, 0.9f, 1.0f };
    _deviceCtx->ClearRenderTargetView(_rtv, clearColor); */

	switch(_drawType) { // Change draw type depending on what is configured
	case DRAW_Points:
		_deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		break;
	case DRAW_Lines:
		_deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		break;
	case DRAW_Triangles:
		_deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	case DRAW_Fan:
		_deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ); // not sure this is correct
		break;
	case DRAW_Strip:
		_deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		break;
	default:
		OutputDebugStringA("Draw type not supported yet!");
		return;
	}

	// Getting instance of scene block buffer at the very front of the buffer vector, if it exists
	if (_buffers.front().targetID == SPECIAL_SCENE_RENDER_ID) {
		Buffer_Drx11* sceneBlockBuff = &_buffers.front();
		_deviceCtx->VSSetConstantBuffers(SCENE_BLOCK_BINDING, 1, &sceneBlockBuff->buffer);
	}

	Buffer_Drx11** dBuffers = (Buffer_Drx11**)malloc(MAX_BUFFERS_PER_TARGET * sizeof(Buffer_Drx11*));

	// Rendering Loop!
	if (_isPipelineReady && _isSceneReady)
		for (unsigned id = _mainRenderIDs; id >= 1; id--) {
			_Drx11::discoverBuffers(dBuffers, &_buffers, id);

			Buffer_Drx11* vertexBuff = _Drx11::findBuffer(BUFF_Vertex_Type, dBuffers, MAX_BUFFERS_PER_TARGET);
			Buffer_Drx11* indexBuff = _Drx11::findBuffer(BUFF_Index_UI, dBuffers, MAX_BUFFERS_PER_TARGET);
			Buffer_Drx11* renderBlockBuff = _Drx11::findBuffer(BUFF_Render_Block, dBuffers, MAX_BUFFERS_PER_TARGET);
			if (indexBuff == nullptr || vertexBuff == nullptr) {
				OutputDebugStringA("One of the required buffers was not ready for drawing. Oops");
				return;
			}

			// TODO: Check for renderBlockBuff validity
			_deviceCtx->VSSetConstantBuffers(RENDER_BLOCK_BINDING, 1, &renderBlockBuff->buffer);

			UINT stride = sizeof(Geo_Vertex);
			UINT offset = 0;
			_deviceCtx->IASetVertexBuffers(0, 1, &vertexBuff->buffer, &stride, &offset);
			_deviceCtx->IASetIndexBuffer(indexBuff->buffer, DXGI_FORMAT_R32_UINT, 0);

			for (unsigned t = 0; t < _textures.size(); t++) {
				if (_textures.at(t).targetID > id) break; // This means we have passed it in sequence
				else if (_textures.at(t).targetID == id) {
					ID3D11SamplerState* baseSampler = _textures.at(t).sampler;
					ID3D11ShaderResourceView* resView = _textures.at(t).resView;

					_deviceCtx->PSSetShaderResources(0, 1, &resView);
					_deviceCtx->PSSetSamplers(0, 1, &baseSampler);
					break;
				}
			}

			// Draw Call!
			if (indexBuff != nullptr && indexBuff->count != 0) _deviceCtx->DrawIndexed(indexBuff->count, 0, 0);
			else _deviceCtx->Draw(vertexBuff->count, 0);
		}

	free(dBuffers);

    _swapChain->Present(0, 0);
}