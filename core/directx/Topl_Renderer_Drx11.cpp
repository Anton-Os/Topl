#include "directx/Topl_Renderer_Drx11.hpp"

namespace Drx11 {
	// Shader Functions

	static DXGI_FORMAT getShaderFormat(enum SHDR_ValueType type) {
		switch (type) {
		case SHDR_float_vec4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case SHDR_float_vec3: return DXGI_FORMAT_R32G32B32_FLOAT;
		case SHDR_float_vec2: return DXGI_FORMAT_R32G32_FLOAT;
		case SHDR_float: return DXGI_FORMAT_R32_FLOAT;
		case SHDR_uint_vec4: return DXGI_FORMAT_R32G32B32A32_UINT;
		case SHDR_uint_vec3: return DXGI_FORMAT_R32G32B32_UINT;
		case SHDR_uint_vec2: return DXGI_FORMAT_R32G32_UINT;
		case SHDR_uint: return DXGI_FORMAT_R32_UINT;
		default:
			logMessage("Drx11 shader input type not supported!");
			break;
		}

		DXGI_FORMAT format;
		return format;
	}

	// Buffer Functions

	static bool createBuff(ID3D11Device** device, ID3D11Buffer** buffer, UINT byteWidth, D3D11_USAGE usage, UINT bindFlags, UINT cpuAccessFlags, const void* data) {
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

		HRESULT result = (*(device))->CreateBuffer(&buffDesc, &buffData, buffer);
		if (FAILED(result)) return false;

		return true;
	}

	static bool createVertexBuff(ID3D11Device** device, ID3D11Buffer** vBuff, vertex_cptr_t pvData, unsigned vCount) {
		return createBuff(device, vBuff, sizeof(Geo_Vertex) * vCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, pvData);
	}

	static bool createIndexBuff(ID3D11Device** device, ID3D11Buffer** iBuff, DWORD* iData, unsigned iCount) {
		return createBuff(device, iBuff, sizeof(DWORD) * iCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, iData);
	}

	static bool createBlockBuff(ID3D11Device** device, ID3D11Buffer** cBuff, const blockBytes_t* const blockBytes) {
		return createBuff(device, cBuff, sizeof(uint8_t) * blockBytes->size(), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, blockBytes->data());
	}

	// Additional Functions

	static D3D11_VIEWPORT createViewport(const Topl_Viewport* const vp) {
		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

		viewport.TopLeftX = vp->xOffset;
		viewport.TopLeftY = vp->yOffset;
		viewport.Height = vp->height;
		viewport.Width = vp->width;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		return viewport;
	}

	static D3D11_INPUT_ELEMENT_DESC getElementDescFromInput(const Shader_Type* input, UINT offset) {
		D3D11_INPUT_ELEMENT_DESC inputElementDesc;
		inputElementDesc.SemanticName = input->semantic.c_str();
		inputElementDesc.SemanticIndex = 0;
		inputElementDesc.Format = Drx11::getShaderFormat(input->type);
		inputElementDesc.InputSlot = 0;
		inputElementDesc.AlignedByteOffset = offset;
		inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputElementDesc.InstanceDataStepRate = 0;

		return inputElementDesc;
	}

	static D3D11_SAMPLER_DESC genSamplerDesc(enum TEX_Mode mode) {
		D3D11_TEXTURE_ADDRESS_MODE texMode;

		switch (mode) {
		case(TEX_Wrap): texMode = D3D11_TEXTURE_ADDRESS_WRAP; break;
		case(TEX_Mirror): texMode = D3D11_TEXTURE_ADDRESS_MIRROR; break;
		case(TEX_Clamp): texMode = D3D11_TEXTURE_ADDRESS_CLAMP; break;
		}

		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = texMode;
		samplerDesc.AddressV = texMode;
		samplerDesc.AddressW = texMode;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		return samplerDesc;
	}
}

Topl_Renderer_Drx11::~Topl_Renderer_Drx11() {
	_swapChain->Release();
	_device->Release();
	_deviceCtx->Release();

	if(_vertexDataLayout != nullptr) _vertexDataLayout->Release();
	_rtView->Release();
	if(_dsView != nullptr) _dsView->Release();
	_blendState->Release();
	_rasterizerState->Release();
}

void Topl_Renderer_Drx11::init(NATIVE_WINDOW window) {
	// Swap Chain Creation
	
	DXGI_MODE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = TOPL_WIN_WIDTH;
	bufferDesc.Height = TOPL_WIN_HEIGHT;
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
	swapChainDesc.OutputWindow = _platformCtx.window;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	HRESULT result; // Error handler

	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION,
		&swapChainDesc, &_swapChain, &_device, NULL, &_deviceCtx);
	if (FAILED(result)) return;

	// ID3D11Texture2D* backBuffer;
	ID3D11Resource* backBuffer; // bgfx renderer_d3d11.cpp line 4660
	result = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (FAILED(result)) return; // Provide error handling code

	_device->CreateRenderTargetView(backBuffer, NULL, &_rtView);
	backBuffer->Release();

	_deviceCtx->OMSetRenderTargets(1, &_rtView, NULL);

	// Depth Stencil View Creation

	D3D11_TEXTURE2D_DESC depthTexDesc;

	depthTexDesc.Height = TOPL_WIN_HEIGHT;
	depthTexDesc.Width = TOPL_WIN_WIDTH;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	// depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;

	ID3D11Texture2D* depthTex = NULL;
	_device->CreateTexture2D(&depthTexDesc, NULL, &depthTex);

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	ID3D11DepthStencilState* depthStencilState;
	_device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	_deviceCtx->OMSetDepthStencilState(depthStencilState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc;
	dsViewDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsViewDesc.Texture2D.MipSlice = 0;

	_device->CreateDepthStencilView(depthTex, &dsViewDesc, &_dsView);
	_deviceCtx->OMSetRenderTargets(1, &_rtView, _dsView);

	// Blend State Creation

	D3D11_BLEND_DESC blendStateDesc;
	ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));

	blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	_device->CreateBlendState(&blendStateDesc, &_blendState);

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT blendMask = 0xffffffff;

	_deviceCtx->OMSetBlendState(_blendState, blendFactor, blendMask);

	// Rasterizer State creation

	D3D11_RASTERIZER_DESC rasterizerStateDesc;
	ZeroMemory(&rasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerStateDesc.CullMode = D3D11_CULL_NONE; // disable culling

	_device->CreateRasterizerState(&rasterizerStateDesc, &_rasterizerState);
	_deviceCtx->RSSetState(_rasterizerState);

	setViewport(&_defaultViewport); // Viewport Creation
}

void Topl_Renderer_Drx11::clearView() {
	const float clearColor[] = { CLEAR_COLOR_RGB, CLEAR_COLOR_RGB, CLEAR_COLOR_RGB, CLEAR_COLOR_ALPHA };
	_deviceCtx->ClearRenderTargetView(_rtView, clearColor);
	_deviceCtx->ClearDepthStencilView(_dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);
}

void Topl_Renderer_Drx11::setViewport(const Topl_Viewport* viewport) {
	if (viewport != nullptr) {
		D3D11_VIEWPORT vp = Drx11::createViewport(viewport);
		_deviceCtx->RSSetViewports(1, &vp);
	}
	else _isBuilt = false; // Error
}

void Topl_Renderer_Drx11::switchFramebuff() {
	if (_isDrawn) {
		_swapChain->Present(0, 0);
		_isDrawn = false; // await future draw call
	}
}

void Topl_Renderer_Drx11::build(const Topl_Scene* scene) {
	static unsigned buildCount = FIRST_BUILD_CALL;
	blockBytes_t shaderBlockData;

	// setting vertex input layout
	if (buildCount == FIRST_BUILD_CALL) { // must be invoked only once!
		D3D11_INPUT_ELEMENT_DESC* layout_ptr = (D3D11_INPUT_ELEMENT_DESC*)malloc(sizeof(D3D11_INPUT_ELEMENT_DESC) * _entryShader->getInputCount());
		unsigned inputElementOffset = 0;
		for (unsigned inputNum = 0; inputNum < _entryShader->getInputCount(); inputNum++) {
			*(layout_ptr + inputNum) = Drx11::getElementDescFromInput(_entryShader->getInputAtIndex(inputNum), inputElementOffset);
			inputElementOffset += Topl_Pipeline::getOffset(_entryShader->getInputAtIndex(inputNum)->type);
		}
		UINT layoutElemCount = (unsigned)_entryShader->getInputCount();

		_device->CreateInputLayout(
			layout_ptr, layoutElemCount,
			// _pipeline.vsBlob->GetBufferPointer(), _pipeline.vsBlob->GetBufferSize(),
			_pipeline->vsBlob->GetBufferPointer(), _pipeline->vsBlob->GetBufferSize(),
			&_vertexDataLayout
		);

		_deviceCtx->IASetInputLayout(_vertexDataLayout);
		free(layout_ptr); // deallocating layout_ptr and all associated data
	}

	// scene block buffer generation
	_entryShader->genSceneBlock(scene, _activeCamera, &shaderBlockData); 
	_isBuilt = Drx11::createBlockBuff(&_device, &_sceneBlockBuff, &shaderBlockData);
	_buffers.push_back(Buffer_Drx11(_sceneBlockBuff));

	for (unsigned g = 0; g < scene->getActorCount(); g++) {
		_renderIDs++;
		_renderTargets_map.insert({ _renderIDs, scene->getGeoActor(g) });
		actor_cptr actor = scene->getGeoActor(g);
		unsigned renderID = getRenderID(actor);
		Geo_RenderObj* renderObj = (Geo_RenderObj*)actor->getRenderObj();

		// render block buffer generation
		_entryShader->genRenderBlock(actor, renderID, &shaderBlockData);
		ID3D11Buffer* renderBlockBuff = nullptr;
		_isBuilt = Drx11::createBlockBuff(&_device, &renderBlockBuff, &shaderBlockData);
		_buffers.push_back(Buffer_Drx11(renderID, BUFF_Render_Block, renderBlockBuff));
		if (!_isBuilt) return; // Error

		// indices generation
		ID3D11Buffer* indexBuff = nullptr;
		if (renderObj->getIndices() != nullptr) { // checks if index data exists for render object
			_isBuilt = Drx11::createIndexBuff(&_device, &indexBuff, (DWORD*)renderObj->getIndices(), renderObj->getIndexCount());
			_buffers.push_back(Buffer_Drx11(renderID, BUFF_Index_UI, indexBuff, renderObj->getIndexCount()));
		}
		else _buffers.push_back(Buffer_Drx11(renderID, BUFF_Index_UI, indexBuff, 0));
		if (!_isBuilt) return; // Error

		// vertices generation
		ID3D11Buffer* vertexBuff = nullptr;
		_isBuilt = Drx11::createVertexBuff(&_device, &vertexBuff, renderObj->getVertices(), renderObj->getVertexCount());
		_buffers.push_back(Buffer_Drx11(renderID, BUFF_Vertex_Type, vertexBuff, renderObj->getVertexCount()));
		if (!_isBuilt) return; // Error
	}

	buildCount++;
	_isBuilt = true;
}

#ifdef RASTERON_H

Rasteron_Image* Topl_Renderer_Drx11::frame() {
	HRESULT result;

	ID3D11Texture2D* framebuff;
	result = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&framebuff));

	D3D11_TEXTURE2D_DESC framebuffDesc;
	framebuff->GetDesc(&framebuffDesc);
	framebuffDesc.BindFlags = 0;
	framebuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	framebuffDesc.Usage = D3D11_USAGE_STAGING;

	ID3D11Texture2D* framebuffTex = NULL;
	result = _device->CreateTexture2D(&framebuffDesc, NULL, &framebuffTex);

	// Copying and Mapping

	_deviceCtx->CopyResource(framebuffTex, framebuff);
	D3D11_MAPPED_SUBRESOURCE resource;
	unsigned subresource = D3D11CalcSubresource(0, 0, 0);
	result = _deviceCtx->Map(framebuffTex, subresource, D3D11_MAP_READ_WRITE, 0, &resource);
	const unsigned int* sourceData = static_cast<const unsigned int*>(resource.pData);

	Rasteron_Image* image = allocNewImg("framebuff", TOPL_WIN_HEIGHT, TOPL_WIN_WIDTH);
	unsigned srcOffset = 0; unsigned dstOffset = 0;
	unsigned pitch = resource.RowPitch / 4; // << 2;
	for (unsigned r = 0; r < image->height - 1; r++) {
		for (unsigned c = 0; c < image->width; c++)
			*(image->data + dstOffset + c) = *(sourceData + srcOffset + c);
		srcOffset += pitch;
		dstOffset += image->width;
	}
	_deviceCtx->Unmap(framebuffTex, 0);

	switchRB(image->data, TOPL_WIN_WIDTH * TOPL_WIN_HEIGHT); // edits
	return image;
}

void Topl_Renderer_Drx11::attachTexture(const Rasteron_Image* image, unsigned renderID) {
	HRESULT result;

	D3D11_SAMPLER_DESC samplerDesc = Drx11::genSamplerDesc(_texMode);
	ID3D11SamplerState* sampler;
	result = _device->CreateSamplerState(&samplerDesc, &sampler);

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
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	D3D11_SUBRESOURCE_DATA texData;
	texData.pSysMem = image->data;
	texData.SysMemPitch = sizeof(uint32_t) * image->height;
	texData.SysMemSlicePitch = 0;

	ID3D11Texture2D* texture;
	result = _device->CreateTexture2D(&texDesc, &texData, &texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC resViewDesc;
	ZeroMemory(&resViewDesc, sizeof(resViewDesc));
	resViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resViewDesc.Texture2D.MipLevels = -1;

	ID3D11ShaderResourceView* resView;
	_device->CreateShaderResourceView(texture, &resViewDesc, &resView);
	_deviceCtx->UpdateSubresource(texture, 0, 0, image->data, texData.SysMemPitch, 0);
	_deviceCtx->GenerateMips(resView);

	for(std::vector<Texture_Drx11>::iterator tex = _textures.begin(); tex != _textures.end(); tex++)
		if (tex->renderID == renderID) { // Texture Substitution
			tex->resView->Release(); // erase old texture
			tex->sampler->Release(); // erase old sampler
			*tex = Texture_Drx11(renderID, TEX_2D, _texMode, sampler, resView);
			return;
		}
	_textures.push_back(Texture_Drx11(renderID, TEX_2D, _texMode, sampler, resView)); // Texture Addition
}

void Topl_Renderer_Drx11::attachMaterial(const Img_Material* material, unsigned renderID) {
	HRESULT result;

	D3D11_SAMPLER_DESC samplerDesc = Drx11::genSamplerDesc(_texMode);
	ID3D11SamplerState* sampler;
	result = _device->CreateSamplerState(&samplerDesc, &sampler);

	D3D11_TEXTURE3D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = material->getLayer(MATERIAL_Albedo)->height;
	texDesc.Height = material->getLayer(MATERIAL_Albedo)->width;
	texDesc.Depth = MAX_MATERIAL_PROPERTIES;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	D3D11_SUBRESOURCE_DATA texData;
	texData.pSysMem = material->getLayer(MATERIAL_Albedo)->data; // TODO: Pass Full Material Data
	texData.SysMemPitch = sizeof(uint32_t) * material->getLayer(MATERIAL_Albedo)->height;
	texData.SysMemSlicePitch = 0;

	ID3D11Texture3D* texture;
	result = _device->CreateTexture3D(&texDesc, &texData, &texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC resViewDesc;
	ZeroMemory(&resViewDesc, sizeof(resViewDesc));
	resViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	resViewDesc.Texture2D.MipLevels = -1;

	ID3D11ShaderResourceView* resView;
	_device->CreateShaderResourceView(texture, &resViewDesc, &resView);
	_deviceCtx->UpdateSubresource(texture, 0, 0, material->getLayer(MATERIAL_Albedo)->data, texData.SysMemPitch, 0);
	_deviceCtx->GenerateMips(resView);

	for (std::vector<Texture_Drx11>::iterator tex = _textures.begin(); tex != _textures.end(); tex++)
		if (tex->renderID == renderID) { // Texture Substitution
			tex->resView->Release(); // erase old texture
			tex->sampler->Release(); // erase old sampler
			*tex = Texture_Drx11(renderID, TEX_3D, _texMode, sampler, resView);
			return;
		}
	_textures.push_back(Texture_Drx11(renderID, TEX_3D, _texMode, sampler, resView)); // Texture Addition
}
#endif


void Topl_Renderer_Drx11::update(const Topl_Scene* scene) {
	blockBytes_t blockBytes;
	Buffer_Drx11* renderBlockBuff = nullptr;

	if (_buffers.front().renderID == SPECIAL_SCENE_RENDER_ID) {
		_entryShader->genSceneBlock(scene, _activeCamera, &blockBytes); 
		Drx11::createBlockBuff(&_device, &_buffers.front().buffer, &blockBytes);
	}

	for (unsigned g = 0; g < scene->getActorCount(); g++) {
		actor_cptr actor = scene->getGeoActor(g);
		unsigned renderID = getRenderID(actor);

		_entryShader->genRenderBlock(actor, renderID, &blockBytes);
		for (std::vector<Buffer_Drx11>::iterator buff = _buffers.begin(); buff < _buffers.end(); buff++)
			if (buff->renderID == renderID && buff->type == BUFF_Render_Block) {
				renderBlockBuff = &(*buff);
				break;
			}

		if (renderBlockBuff != nullptr) _isBuilt = Drx11::createBlockBuff(&_device, &renderBlockBuff->buffer, &blockBytes);
		if (!_isBuilt) return; // Error
	}
}

void Topl_Renderer_Drx11::drawMode() {
	switch (_drawMode) {
	case DRAW_Points: _deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST); break;
	case DRAW_Lines: _deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST); break;
	case DRAW_Triangles: _deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); break;
	case DRAW_Fan: _deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ); break; // not sure this is correct topology
	case DRAW_Strip: _deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); break;
	default: return logMessage(MESSAGE_Exclaim, "Draw Type not supported!");
	}
}

 void Topl_Renderer_Drx11::renderTarget(unsigned long renderID) {
	if(renderID == SPECIAL_SCENE_RENDER_ID && _buffers.front().renderID == SPECIAL_SCENE_RENDER_ID) {
		Buffer_Drx11* sceneBlockBuff = &_buffers.front();
		if (sceneBlockBuff != nullptr) {
			_deviceCtx->VSSetConstantBuffers(SCENE_BLOCK_BINDING, 1, &sceneBlockBuff->buffer);
			_deviceCtx->PSSetConstantBuffers(SCENE_BLOCK_BINDING, 1, &sceneBlockBuff->buffer);
		}
	}
	else {
		Buffer_Drx11* vertexBuff = findBuffer(BUFF_Vertex_Type, renderID);
		Buffer_Drx11* indexBuff = findBuffer(BUFF_Index_UI, renderID);
		Buffer_Drx11* renderBlockBuff = findBuffer(BUFF_Render_Block, renderID);

		if (renderBlockBuff != nullptr) {
			_deviceCtx->VSSetConstantBuffers(RENDER_BLOCK_BINDING, 1, &renderBlockBuff->buffer);
			_deviceCtx->PSSetConstantBuffers(RENDER_BLOCK_BINDING, 1, &renderBlockBuff->buffer);
		}

		UINT stride = sizeof(Geo_Vertex);
		UINT offset = 0;

		if (vertexBuff == nullptr || vertexBuff->count == 0)
			return logMessage(MESSAGE_Exclaim, "Corrupted Vertex Buffer!");
		else _deviceCtx->IASetVertexBuffers(0, 1, &vertexBuff->buffer, &stride, &offset);

		if (indexBuff != nullptr && indexBuff->count > 0)
			_deviceCtx->IASetIndexBuffer(indexBuff->buffer, DXGI_FORMAT_R32_UINT, 0);

		for (unsigned t = 0; t < _textures.size(); t++)
			if (_textures.at(t).renderID == renderID) {
				ID3D11SamplerState* sampler = _textures.at(t).sampler;
				ID3D11ShaderResourceView* resView = _textures.at(t).resView;

				_deviceCtx->PSSetShaderResources(0, 1, &resView);
				_deviceCtx->PSSetSamplers(0, 1, &sampler);
				break;
			}

		// Draw Call!
		if (indexBuff != nullptr && indexBuff->count != 0) _deviceCtx->DrawIndexed(indexBuff->count, 0, 0); // indexed draw
		else _deviceCtx->Draw(vertexBuff->count, 0); // non-indexed draw
	}
} 

Buffer_Drx11* Topl_Renderer_Drx11::findBuffer(BUFF_Type type, unsigned long renderID){
	for (std::vector<Buffer_Drx11>::iterator buffer = _buffers.begin(); buffer < _buffers.end(); buffer++)
		if (buffer->type == type && buffer->renderID == renderID)
			return &(*buffer);
	return nullptr; // error
}