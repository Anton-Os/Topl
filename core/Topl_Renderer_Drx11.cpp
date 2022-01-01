#include "FileIO.hpp"

#include "Topl_Renderer_Drx11.hpp"

namespace _Drx11 {
	static DXGI_FORMAT getFormatFromShaderVal(enum SHDR_ValueType type){
		DXGI_FORMAT format;

		switch(type) {
		case SHDR_float_vec4: format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
		case SHDR_float_vec3: format = DXGI_FORMAT_R32G32B32_FLOAT; break;
		case SHDR_float_vec2: format = DXGI_FORMAT_R32G32_FLOAT; break;
		case SHDR_float: format = DXGI_FORMAT_R32_FLOAT; break;
		// DOUBLE SUPPORT PLACEHOLDER
		case SHDR_uint_vec4: format = DXGI_FORMAT_R32G32B32A32_UINT; break;
		case SHDR_uint_vec3: format = DXGI_FORMAT_R32G32B32_UINT; break;
		case SHDR_uint_vec2: format = DXGI_FORMAT_R32G32_UINT; break;
		case SHDR_uint: format = DXGI_FORMAT_R32_UINT; break;
		case SHDR_int_vec4: format = DXGI_FORMAT_R32G32B32A32_SINT; break;
		case SHDR_int_vec3: format = DXGI_FORMAT_R32G32B32_SINT; break;
		case SHDR_int_vec2: format = DXGI_FORMAT_R32G32_SINT; break;
		case SHDR_int: format = DXGI_FORMAT_R32_SINT; break;
		default:
			OutputDebugStringA("Drx11 Shader Input Type Not Supported!");
			break;
		}

		return format;
	}

	static unsigned getOffsetFromShaderVal(enum SHDR_ValueType type){
		unsigned offset = 0;

		switch(type) {
		case SHDR_float_vec4: offset = sizeof(float) * 4; break;
		case SHDR_float_vec3: offset = sizeof(float) * 3; break;
		case SHDR_float_vec2: offset = sizeof(float) * 2; break;
		case SHDR_float: offset = sizeof(float); break;
		case SHDR_double_vec4: offset = sizeof(double) * 4; break;
		case SHDR_double_vec3: offset = sizeof(double) * 3; break;
		case SHDR_double_vec2: offset = sizeof(double) * 2; break;
		case SHDR_double: offset = sizeof(double); break;
		case SHDR_uint_vec4: offset = sizeof(unsigned) * 4; break;
		case SHDR_uint_vec3: offset = sizeof(unsigned) * 3;  break;
		case SHDR_uint_vec2: offset = sizeof(unsigned) * 2; break;
		case SHDR_uint: offset = sizeof(unsigned); break;
		case SHDR_int_vec4: offset = sizeof(int) * 4; break;
		case SHDR_int_vec3: offset = sizeof(int) * 3; break;
		case SHDR_int_vec2: offset = sizeof(int) * 2; break;
		case SHDR_int: offset = sizeof(int); break;
		default:
			OutputDebugStringA("Drx11 Shader Input Type Not Supported!");
			break;
		}

		return offset;
	}

	bool compileShader(const char* shaderFilePath, LPCSTR shaderTarget, ID3DBlob** blob){
		ID3DBlob* errorBuff;
		size_t sourceSize;

		// Vertex shader compilation and creation code
		sourceSize = strlen(shaderFilePath) + 1;
		wchar_t* shaderFilePath_wchar = new wchar_t[sourceSize];
		mbstowcs(shaderFilePath_wchar, shaderFilePath, sourceSize); // need proper conversion to wcharhar_t
		if (FAILED(
			D3DCompileFromFile(shaderFilePath_wchar,
				nullptr, nullptr, "main", shaderTarget,
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 
				0, blob, &errorBuff
			)
		)) {
			OutputDebugStringA((char*)errorBuff->GetBufferPointer());
			delete shaderFilePath_wchar; // Proper deallocation of the source string
			return false;
		}
		delete shaderFilePath_wchar; // Proper deallocation of the source string
		return true;
	}

	static bool createBuff(ID3D11Device** device, ID3D11Buffer** buffer, UINT byteWidth, D3D11_USAGE usage, UINT bindFlags, UINT cpuAccessFlags, const void* data){
		D3D11_BUFFER_DESC buffDesc;
		ZeroMemory(&buffDesc, sizeof(buffDesc));
		buffDesc.Usage = usage;
		buffDesc.ByteWidth = byteWidth;
		buffDesc.BindFlags = bindFlags;
		buffDesc.CPUAccessFlags = cpuAccessFlags;
		buffDesc.MiscFlags = 0;
		buffDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA buffData;
		ZeroMemory(&buffData, sizeof(buffData));
		buffData.pSysMem = data;

		HRESULT hr = (*(device))->CreateBuffer(&buffDesc, &buffData, buffer);
		if (FAILED(hr)) return false;

		return true;
	}

	static bool createVertexBuff(ID3D11Device** device, ID3D11Buffer** vBuff, geoVertex_cptr pvData, unsigned vCount) {
		return createBuff(device, vBuff, sizeof(Geo_Vertex) * vCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, pvData);
	}

	static bool createIndexBuff(ID3D11Device** device, ID3D11Buffer** iBuff, DWORD* iData, unsigned iCount) {
		return createBuff(device, iBuff, sizeof(DWORD) * iCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, iData);
	}

	static bool createBlockBuff(ID3D11Device** device, ID3D11Buffer** cBuff, const std::vector<uint8_t> *const blockBytes) {
		return createBuff(device, cBuff, sizeof(uint8_t) * blockBytes->size(), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, blockBytes->data());
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
		// TODO No error checks for duplicate buffers are provided, bufferVector needs to be vetted first
		for (std::vector<Buffer_Drx11>::iterator currentBuff = bufferVector->begin(); currentBuff < bufferVector->end(); currentBuff++)
			if (currentBuff->targetID == id)
				*(dBuffers + currentBuff->type) = &(*currentBuff); // Type indicates 
	}
}

Topl_Renderer_Drx11::~Topl_Renderer_Drx11() {
	for(unsigned b = 0; b < _renderCtx.buffers.size(); b++)
		_renderCtx.buffers.at(b).buffer->Release();

	for (unsigned t = 0; t < _renderCtx.textures.size(); t++) {
		_renderCtx.textures.at(t).texture->Release();
		_renderCtx.textures.at(t).sampler->Release();
		_renderCtx.textures.at(t).resView->Release();
	}

	_swapChain->Release();
	_device->Release();
	_deviceCtx->Release();

	/* _pipeline.vertexShader->Release();
	_pipeline.pixelShader->Release();
	_pipeline.hullShader->Release();
	_pipeline.domainShader->Release();
	_pipeline.geomShader->Release();
	_pipeline.vsBlob->Release();
	_pipeline.psBlob->Release();
	_pipeline.hsBlob->Release();
	_pipeline.dsBlob->Release();
	_pipeline.gsBlob->Release(); */

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

Topl_Pipeline_Drx11 Topl_Renderer_Drx11::genPipeline(entry_shader_cptr vertexShader, shader_cptr pixelShader){
	Topl_Pipeline_Drx11 pipeline = Topl_Pipeline_Drx11();
	HRESULT hr; // error checking variable

	// vertex shader compilation and creation code
	if(!_Drx11::compileShader(vertexShader->getFilePath(), "vs_5_0", &pipeline.vsBlob));
	hr = _device->CreateVertexShader(
		pipeline.vsBlob->GetBufferPointer(), pipeline.vsBlob->GetBufferSize(),
		NULL, &pipeline.vertexShader
	);
	if (FAILED(hr)) { pipeline.isReady = false; return pipeline; }

	// pixel shader compilation and creation code
	if(!_Drx11::compileShader(pixelShader->getFilePath(), "ps_5_0", &pipeline.psBlob));
	hr = _device->CreatePixelShader(
		pipeline.psBlob->GetBufferPointer(), pipeline.psBlob->GetBufferSize(),
		NULL, &pipeline.pixelShader
	);
	if (FAILED(hr)) { pipeline.isReady = false; return pipeline; }

	pipeline.isReady = true;
	_entryShader = vertexShader;
	return pipeline;
}

void Topl_Renderer_Drx11::pipeline(entry_shader_cptr vertexShader, shader_cptr pixelShader){
	HRESULT hr; // error checking variable

	// Vertex shader compilation and creation code
	if(!_Drx11::compileShader(vertexShader->getFilePath(), "vs_5_0", &_pipeline.vsBlob));
	hr = _device->CreateVertexShader(
		_pipeline.vsBlob->GetBufferPointer(), _pipeline.vsBlob->GetBufferSize(),
		NULL, &_pipeline.vertexShader
	);
	if (FAILED(hr)) { _isPipelineReady = false; return; }

	// Pixel shader compilation and creation code
	if(!_Drx11::compileShader(pixelShader->getFilePath(), "ps_5_0", &_pipeline.psBlob));
	hr = _device->CreatePixelShader(
		_pipeline.psBlob->GetBufferPointer(), _pipeline.psBlob->GetBufferSize(),
		NULL, &_pipeline.pixelShader
	);
	if (FAILED(hr)) { _isPipelineReady = false; return; }

	_deviceCtx->VSSetShader(_pipeline.vertexShader, 0, 0);
	_deviceCtx->PSSetShader(_pipeline.pixelShader, 0, 0);

	_isPipelineReady = true;
}

Topl_Pipeline_Drx11 Topl_Renderer_Drx11::genPipeline(entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader, shader_cptr geomShader){
	Topl_Pipeline_Drx11 pipeline = Topl_Pipeline_Drx11();
	HRESULT hr; // error checking variable

	// vertex shader compilation and creation code
	if(!_Drx11::compileShader(vertexShader->getFilePath(), "vs_5_0", &pipeline.vsBlob));
	hr = _device->CreateVertexShader(
		pipeline.vsBlob->GetBufferPointer(), pipeline.vsBlob->GetBufferSize(),
		NULL, &pipeline.vertexShader
	);
	if (FAILED(hr)) { pipeline.isReady = false; return pipeline; }

	// hull shader compilation and creation code
	if(!_Drx11::compileShader(tessCtrlShader->getFilePath(), "hs_5_0", &pipeline.hsBlob));
	hr = _device->CreateHullShader(
		pipeline.hsBlob->GetBufferPointer(), pipeline.hsBlob->GetBufferSize(),
		NULL, &pipeline.hullShader);
	if (FAILED(hr)) { pipeline.isReady = false; return pipeline; }

	// domain shader compilation and creation code
	if(!_Drx11::compileShader(tessEvalShader->getFilePath(), "ds_5_0", &pipeline.dsBlob));
	hr = _device->CreateDomainShader(
		pipeline.dsBlob->GetBufferPointer(), pipeline.dsBlob->GetBufferSize(),
		NULL, &pipeline.domainShader
	);
	if (FAILED(hr)) { pipeline.isReady = false; return pipeline; }

	// geometry shader compilation and creation code
	if(!_Drx11::compileShader(geomShader->getFilePath(), "gs_5_0", &pipeline.gsBlob));
	hr = _device->CreateGeometryShader(
		pipeline.gsBlob->GetBufferPointer(), pipeline.gsBlob->GetBufferSize(),
		NULL, &pipeline.geomShader
	);
	if (FAILED(hr)) { pipeline.isReady = false; return pipeline; }

	// pixel shader compilation and creation code
	if(!_Drx11::compileShader(pixelShader->getFilePath(), "ps_5_0", &pipeline.psBlob));
	hr = _device->CreatePixelShader(
		pipeline.psBlob->GetBufferPointer(), pipeline.psBlob->GetBufferSize(),
		NULL, &pipeline.pixelShader
	);
	if (FAILED(hr)) { pipeline.isReady = false; return pipeline; }

	pipeline.isReady = true;
	_entryShader = vertexShader;
	return pipeline;
}

void Topl_Renderer_Drx11::pipeline(entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader, shader_cptr geomShader){
	HRESULT hr; // error checking variable

	// Vertex shader compilation and creation code
	if(!_Drx11::compileShader(vertexShader->getFilePath(), "vs_5_0", &_pipeline.vsBlob));
	hr = _device->CreateVertexShader(
		_pipeline.vsBlob->GetBufferPointer(), _pipeline.vsBlob->GetBufferSize(),
		NULL, &_pipeline.vertexShader
	);
	if (FAILED(hr)) { _isPipelineReady = false; return; }

	// Hull shader compilation and creation code
	if(!_Drx11::compileShader(tessCtrlShader->getFilePath(), "hs_5_0", &_pipeline.hsBlob));
	hr = _device->CreateHullShader(
		_pipeline.hsBlob->GetBufferPointer(), _pipeline.hsBlob->GetBufferSize(),
		NULL, &_pipeline.hullShader);
	if (FAILED(hr)) { _isPipelineReady = false; return; }

	// Domain shader compilation and creation code
	if(!_Drx11::compileShader(tessEvalShader->getFilePath(), "ds_5_0", &_pipeline.dsBlob));
	hr = _device->CreateDomainShader(
		_pipeline.dsBlob->GetBufferPointer(), _pipeline.dsBlob->GetBufferSize(),
		NULL, &_pipeline.domainShader
	);
	if (FAILED(hr)) { _isPipelineReady = false; return; }

	// Geometry shader compilation and creation code
	if(!_Drx11::compileShader(geomShader->getFilePath(), "gs_5_0", &_pipeline.gsBlob));
	hr = _device->CreateGeometryShader(
		_pipeline.gsBlob->GetBufferPointer(), _pipeline.gsBlob->GetBufferSize(),
		NULL, &_pipeline.geomShader
	);
	if (FAILED(hr)) { _isPipelineReady = false; return; }

	// Pixel shader compilation and creation code
	if(!_Drx11::compileShader(pixelShader->getFilePath(), "ps_5_0", &_pipeline.psBlob));
	hr = _device->CreatePixelShader(
		_pipeline.psBlob->GetBufferPointer(), _pipeline.psBlob->GetBufferSize(),
		NULL, &_pipeline.pixelShader
	);
	if (FAILED(hr)) { _isPipelineReady = false; return; }

	_deviceCtx->VSSetShader(_pipeline.vertexShader, 0, 0);
	_deviceCtx->HSSetShader(_pipeline.hullShader, 0, 0);
	_deviceCtx->DSSetShader(_pipeline.domainShader, 0, 0);
	_deviceCtx->GSSetShader(_pipeline.geomShader, 0, 0);
	_deviceCtx->PSSetShader(_pipeline.pixelShader, 0, 0);

	_isPipelineReady = true;
}

void Topl_Renderer_Drx11::clearView(){
	const float clearColor[] = { 0.0f, 1.0f, 1.0f, 1.0f }; // cyan
    _deviceCtx->ClearRenderTargetView(_rtv, clearColor);
	// _deviceCtx->ClearDepthStencilView(_rtv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);
}

void Topl_Renderer_Drx11::build(const Topl_Scene* scene) {
	build(scene, &_defaultCamera);
}

void Topl_Renderer_Drx11::build(const Topl_Scene* scene, const Topl_Camera* camera){
	std::vector<uint8_t> blockBytes; // container for constant and uniform buffer updates

	// generating an input layout based on Vertex Shader Inputs
	D3D11_INPUT_ELEMENT_DESC* layout_ptr = (D3D11_INPUT_ELEMENT_DESC*)malloc(sizeof(D3D11_INPUT_ELEMENT_DESC) * _entryShader->getInputCount());
	unsigned inputElementOffset = 0;
	for(unsigned i = 0; i < _entryShader->getInputCount(); i++){
		*(layout_ptr + i) = _Drx11::getElementDescFromInput(_entryShader->getInputAtIndex(i), inputElementOffset);
		inputElementOffset += _Drx11::getOffsetFromShaderVal(_entryShader->getInputAtIndex(i)->type);
	}
    UINT layoutElemCount = (unsigned)_entryShader->getInputCount();

    _device->CreateInputLayout(
        layout_ptr, layoutElemCount,
        _pipeline.vsBlob->GetBufferPointer(), _pipeline.vsBlob->GetBufferSize(), 
        &_vertexDataLayout
    );

    _deviceCtx->IASetInputLayout(_vertexDataLayout);

	free(layout_ptr); // deallocating layout_ptr and all associated data

	// scene uniform block buffer generation
	if (_entryShader->genSceneBlock(scene, camera, &blockBytes)) {
		_isSceneReady = _Drx11::createBlockBuff(&_device, &_renderCtx.sceneBlockBuff, &blockBytes);
		_renderCtx.buffers.push_back(Buffer_Drx11(_renderCtx.sceneBlockBuff));
	}

	for(unsigned g = 0; g < scene->getActorCount(); g++) {
		unsigned currentRenderID = g + 1;
		geo_cptr actor_ptr = scene->getGeoActor(currentRenderID - 1); // ids begin at 1, conversion is required
		Geo_RenderObj* actor_renderObj = (Geo_RenderObj*)actor_ptr->getRenderObj();
		
		geoVertex_cptr actor_perVertex = actor_renderObj->getVertices();
		ui_cptr actor_iData = actor_renderObj->getIndices();

		// component block buffer generation
		if (_entryShader->genGeoBlock(actor_ptr, &blockBytes)) {
			ID3D11Buffer* renderBlockBuff = nullptr;
			_isSceneReady = _Drx11::createBlockBuff(&_device, &renderBlockBuff, &blockBytes);
			_renderCtx.buffers.push_back(Buffer_Drx11(currentRenderID, BUFF_Render_Block, renderBlockBuff));
		}
		if (!_isSceneReady) return; // Error

		// index creation
		ID3D11Buffer* indexBuff = nullptr;
		if (actor_iData != nullptr) { // Checks if index data exists for render object
			_isSceneReady = _Drx11::createIndexBuff(&_device, &indexBuff, (DWORD*)actor_iData, actor_renderObj->getIndexCount());
			_renderCtx.buffers.push_back(Buffer_Drx11(currentRenderID, BUFF_Index_UI, indexBuff, actor_renderObj->getIndexCount()));
		} else _renderCtx.buffers.push_back(Buffer_Drx11(currentRenderID, BUFF_Index_UI, indexBuff, 0));
		if (!_isSceneReady) return; // Error

		ID3D11Buffer* vertexBuff = nullptr;
		_isSceneReady = _Drx11::createVertexBuff(&_device, &vertexBuff,
												actor_perVertex, actor_renderObj->getVerticesCount());

		_renderCtx.buffers.push_back(Buffer_Drx11(currentRenderID, BUFF_Vertex_Type, vertexBuff, actor_renderObj->getVerticesCount()));
		if (!_isSceneReady) return;

#ifdef RASTERON_H
		const Rasteron_Image* baseTex = scene->getFirstTexture(actor_ptr->getName());
		if(baseTex != nullptr) assignTexture(baseTex, currentRenderID); // Add the method definition
#endif
		if(!_isSceneReady) return;
		_mainRenderIDs = currentRenderID; // Gives us the greatest buffer ID number
	}

    _isSceneReady = true;
    return;
}

#ifdef RASTERON_H

// EXPERIMENTAL SCREEN CAPTURE CODE!
Rasteron_Image* Topl_Renderer_Drx11::frame(){
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
	Rasteron_Image* image = (Rasteron_Image*)malloc(sizeof(Rasteron_Image));
	image->width = TOPL_WIN_WIDTH;
	image->height = TOPL_WIN_HEIGHT;
	image->name = "framebuff"; // TODO: Make this incremental, i.e framebuff1 framebuff2

	image->data = (uint32_t*)malloc(image->width * image->height * sizeof(uint32_t));
	memcpy(image->data, srcTexData, image->width * image->height * sizeof(uint32_t));

	return image;
}

void Topl_Renderer_Drx11::assignTexture(const Rasteron_Image* image, unsigned id){
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
    texDesc.Width = image->height;
    texDesc.Height = image->width;
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
	texData.SysMemPitch = sizeof(uint32_t) * image->height;
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

	_renderCtx.textures.push_back(Texture_Drx11(id, TEX_2D, TEX_Wrap, texture, sampler, resView));
}

#endif

void Topl_Renderer_Drx11::update(const Topl_Scene* scene){
	update(scene, &_defaultCamera);
}

void Topl_Renderer_Drx11::update(const Topl_Scene* scene, const Topl_Camera* camera){
	std::vector<uint8_t> blockBytes; // New Implementation
	Buffer_Drx11* renderBlockBuff = nullptr;
	// Buffer_Drx11* sceneBlockBuff = nullptr;

	if (_entryShader->genSceneBlock(scene, camera, &blockBytes) && _renderCtx.buffers.front().targetID == SPECIAL_SCENE_RENDER_ID)
		_Drx11::createBlockBuff(&_device, &_renderCtx.buffers.front().buffer, &blockBytes); // Update code should work

	for(unsigned g = 0; g < scene->getActorCount(); g++) {
		unsigned currentRenderID = g + 1;
		geo_cptr actor_ptr = scene->getGeoActor(currentRenderID - 1); // ids begin at 1, conversion is required

		if (_entryShader->genGeoBlock(actor_ptr, &blockBytes)) {
			for (std::vector<Buffer_Drx11>::iterator currentBuff = _renderCtx.buffers.begin(); currentBuff < _renderCtx.buffers.end(); currentBuff++)
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
	switch(_drawType) { // Change draw type depending on what is configured
	case DRAW_Points: _deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST); break;
	case DRAW_Lines: _deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST); break;
	case DRAW_Triangles: _deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); break;
	case DRAW_Fan: _deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ); break; // not sure this is correct topology
	case DRAW_Strip: _deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); break;
	default:
		OutputDebugStringA("Draw type not supported yet!");
		return;
	}

	// getting instance of scene block buffer at the very front of the buffer vector, if it exists
	if (_renderCtx.buffers.front().targetID == SPECIAL_SCENE_RENDER_ID) {
		Buffer_Drx11* sceneBlockBuff = &_renderCtx.buffers.front();
		_deviceCtx->VSSetConstantBuffers(SCENE_BLOCK_BINDING, 1, &sceneBlockBuff->buffer);
	}

	Buffer_Drx11** dBuffers = (Buffer_Drx11**)malloc(MAX_BUFFERS_PER_TARGET * sizeof(Buffer_Drx11*));

	// rendering Loop!
	if (_isPipelineReady && _isSceneReady)
		for (unsigned id = _mainRenderIDs; id >= 1; id--) {
			_Drx11::discoverBuffers(dBuffers, &_renderCtx.buffers, id);

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

			for (unsigned t = 0; t < _renderCtx.textures.size(); t++) {
				if (_renderCtx.textures.at(t).targetID > id) break; // This means we have passed it in sequence
				else if (_renderCtx.textures.at(t).targetID == id) {
					ID3D11SamplerState* baseSampler = _renderCtx.textures.at(t).sampler;
					ID3D11ShaderResourceView* resView = _renderCtx.textures.at(t).resView;

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