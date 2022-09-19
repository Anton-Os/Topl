#include "directx/Topl_Renderer_Drx11.hpp"

namespace Renderer {
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
		inputElementDesc.Format = Renderer::getShaderFormat(input->type);
		inputElementDesc.InputSlot = 0;
		inputElementDesc.AlignedByteOffset = offset;
		inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputElementDesc.InstanceDataStepRate = 0;

		return inputElementDesc;
	}

	static enum D3D11_TEXTURE_ADDRESS_MODE getTexMode(enum TEX_Mode mode) {
		switch (mode) {
		case(TEX_Wrap): return D3D11_TEXTURE_ADDRESS_WRAP;
		case(TEX_Mirror): return D3D11_TEXTURE_ADDRESS_MIRROR;
		case(TEX_Clamp): return D3D11_TEXTURE_ADDRESS_CLAMP;
		}
	}
}

Topl_Renderer_Drx11::~Topl_Renderer_Drx11() {
	_swapChain->Release();
	_device->Release();
	_deviceCtx->Release();

	_vertexDataLayout->Release();
	_resourceView->Release();
	_rtView->Release();
	_dsView->Release();
	_blendState->Release();
	_rasterizerState->Release();
}

void Topl_Renderer_Drx11::init(NATIVE_WINDOW window) {
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

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Height = TOPL_WIN_HEIGHT;
	depthStencilDesc.Width = TOPL_WIN_WIDTH;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	// depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	ID3D11Texture2D* depthStencilTex;
	_device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilTex);
	_device->CreateDepthStencilView(depthStencilTex, NULL, &_dsView);

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

	/* D3D11_RASTERIZER_DESC rasterizerStateDesc;
	ZeroMemory(&rasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
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

	// Viewport Creation

	if (_viewports == nullptr) {
		_isBuilt = false;
		return; // Error // Viewports Dont Exist
	}
	if (_viewportCount <= 1) { // singular viewport
		D3D11_VIEWPORT viewport = Renderer::createViewport(_viewports); // use first viewport
		_deviceCtx->RSSetViewports(1, &viewport);
	}
	else { // multiple viewports
		D3D11_VIEWPORT viewports[MAX_VIEWPORTS];
		// for (unsigned v = 0; v < _viewportCount; v++) viewports[v] = Renderer::createViewport(_viewports + v);
		for (unsigned v = 0; v < _viewportCount; v++) viewports[_viewportCount - v - 1] = Renderer::createViewport(_viewports + v); // reverse order
		_deviceCtx->RSSetViewports(_viewportCount, &viewports[0]);
	}
}

void Topl_Renderer_Drx11::clearView() {
	const float clearColor[] = { CLEAR_COLOR_RGB, CLEAR_COLOR_RGB, CLEAR_COLOR_RGB, CLEAR_COLOR_ALPHA };
	// const float clearColor[] = { 0.0f, 1.0f, 1.0f, 1.0f }; // cyan
	_deviceCtx->ClearRenderTargetView(_rtView, clearColor);
	_deviceCtx->ClearDepthStencilView(_dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);
}

void Topl_Renderer_Drx11::switchFramebuff() {
	_swapChain->Present(0, 0);
	_isDrawn = false; // awaiting another draw call
}

void Topl_Renderer_Drx11::build(const Topl_Scene* scene) {
	blockBytes_t blockBytes;
	// generating an input layout based on Vertex Shader Inputs
	D3D11_INPUT_ELEMENT_DESC* layout_ptr = (D3D11_INPUT_ELEMENT_DESC*)malloc(sizeof(D3D11_INPUT_ELEMENT_DESC) * _entryShader->getInputCount());
	unsigned inputElementOffset = 0;
	for (unsigned i = 0; i < _entryShader->getInputCount(); i++) {
		*(layout_ptr + i) = Renderer::getElementDescFromInput(_entryShader->getInputAtIndex(i), inputElementOffset);
		inputElementOffset += Topl_Pipeline::getOffset(_entryShader->getInputAtIndex(i)->type);
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

	// scene uniform block buffer generation
	if (_entryShader->genSceneBlock(scene, _activeCamera, &blockBytes)) {
		_isBuilt = Renderer::createBlockBuff(&_device, &_sceneBlockBuff, &blockBytes);
		_buffers.push_back(Buffer_Drx11(_sceneBlockBuff));
	}

	for (unsigned g = 0; g < scene->getActorCount(); g++) {
		_renderIDs++;
		_renderTargets_map.insert({ _renderIDs, scene->getGeoActor(g) });
		actor_cptr actor = scene->getGeoActor(g);
		unsigned renderID = getRenderID(actor);
		Geo_RenderObj* actor_renderObj = (Geo_RenderObj*)actor->getRenderObj();

		vertex_cptr_t actor_vData = actor_renderObj->getVertices();
		ui_cptr_t actor_iData = actor_renderObj->getIndices();

		// component block buffer generation
		if (_entryShader->genGeoBlock(actor, &blockBytes)) {
			ID3D11Buffer* renderBlockBuff = nullptr;
			_isBuilt = Renderer::createBlockBuff(&_device, &renderBlockBuff, &blockBytes);
			_buffers.push_back(Buffer_Drx11(renderID, BUFF_Render_Block, renderBlockBuff));
		}
		if (!_isBuilt) return; // Error

		// index creation
		ID3D11Buffer* indexBuff = nullptr;
		if (actor_iData != nullptr) { // Checks if index data exists for render object
			_isBuilt = Renderer::createIndexBuff(&_device, &indexBuff, (DWORD*)actor_iData, actor_renderObj->getIndexCount());
			_buffers.push_back(Buffer_Drx11(renderID, BUFF_Index_UI, indexBuff, actor_renderObj->getIndexCount()));
		}
		else _buffers.push_back(Buffer_Drx11(renderID, BUFF_Index_UI, indexBuff, 0));
		if (!_isBuilt) return; // Error

		ID3D11Buffer* vertexBuff = nullptr;
		_isBuilt = Renderer::createVertexBuff(&_device, &vertexBuff, actor_vData, actor_renderObj->getVertexCount());
		_buffers.push_back(Buffer_Drx11(renderID, BUFF_Vertex_Type, vertexBuff, actor_renderObj->getVertexCount()));
		if (!_isBuilt) return; // Error
	}

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
		srcOffset += pitch; // resource.RowPitch;
		dstOffset += image->width;
	}

	_deviceCtx->Unmap(framebuffTex, 0);
	return image;
}

void Topl_Renderer_Drx11::texturize(const Topl_Scene* scene) {
#ifdef RASTERON_H // Rasteron dependency required for updating textures
	// Need to clear saved textures entirely for texture update
	for (unsigned t = 0; t < _textures.size(); t++) {
		// _textures.at(t).texture->Release();
		_textures.at(t).sampler->Release();
		_textures.at(t).resView->Release();
	}
	_textures.clear();

	for (unsigned g = 0; g < scene->getActorCount(); g++) {
		actor_cptr actor = scene->getGeoActor(g);
		unsigned renderID = getRenderID(actor);

		const Rasteron_Image* texture = scene->getTexture(actor->getName());
		if (texture != nullptr) attachTexture(texture, renderID);

		// const Topl_Material* material = scene->getMaterial(actor->getName());
		// if (material != nullptr) attachMaterial(texture, renderID);
	}
#endif
}

void Topl_Renderer_Drx11::attachTexture(const Rasteron_Image* image, unsigned actorID) {
	HRESULT result;

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = Renderer::getTexMode(_texMode);
	samplerDesc.AddressV = Renderer::getTexMode(_texMode);
	samplerDesc.AddressW = Renderer::getTexMode(_texMode);
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

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

	_textures.push_back(Texture_Drx11(actorID, TEX_2D, _texMode, sampler, resView));
}

void Topl_Renderer_Drx11::attachMaterial(const Topl_Material* material, unsigned actorID) {
	HRESULT result;

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = Renderer::getTexMode(_texMode);
	samplerDesc.AddressV = Renderer::getTexMode(_texMode);
	samplerDesc.AddressW = Renderer::getTexMode(_texMode);
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* sampler;
	result = _device->CreateSamplerState(&samplerDesc, &sampler);

	D3D11_TEXTURE3D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = material->getLayer(MATERIAL_Albedo)->height;
	texDesc.Height = material->getLayer(MATERIAL_Albedo)->width;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	D3D11_SUBRESOURCE_DATA texData;
	texData.pSysMem = 0; // Generate Data Here!!!
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
	// _deviceCtx->UpdateSubresource(texture, 0, 0, material->getData, texData.SysMemPitch, 0); // Update Data Here!!!
	_deviceCtx->GenerateMips(resView);

	_textures.push_back(Texture_Drx11(actorID, TEX_3D, _texMode, sampler, resView));
}

#endif

void Topl_Renderer_Drx11::update(const Topl_Scene* scene) {
	blockBytes_t blockBytes;
	Buffer_Drx11* renderBlockBuff = nullptr;

	if (_entryShader->genSceneBlock(scene, _activeCamera, &blockBytes) && _buffers.front().renderID == SPECIAL_SCENE_RENDER_ID)
		Renderer::createBlockBuff(&_device, &_buffers.front().buffer, &blockBytes); // Update code should work

	for (unsigned g = 0; g < scene->getActorCount(); g++) {
		actor_cptr actor = scene->getGeoActor(g);
		unsigned renderID = getRenderID(actor);

		if (_entryShader->genGeoBlock(actor, &blockBytes)) {
			for (std::vector<Buffer_Drx11>::iterator buff = _buffers.begin(); buff < _buffers.end(); buff++)
				if (buff->renderID == renderID && buff->type == BUFF_Render_Block) {
					renderBlockBuff = &(*buff);
					break;
				}

			if (renderBlockBuff != nullptr) _isBuilt = Renderer::createBlockBuff(&_device, &renderBlockBuff->buffer, &blockBytes);
			if (!_isBuilt) return; // Error
		}
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

/* void Topl_Renderer_Drx11::render(const Topl_Scene* scene) {
	if (_buffers.front().renderID == SPECIAL_SCENE_RENDER_ID) { // passing scene block buffer to shader
		Buffer_Drx11* sceneBlockBuff = &_buffers.front();
		if (sceneBlockBuff != nullptr)
			_deviceCtx->VSSetConstantBuffers(SCENE_BLOCK_BINDING, 1, &sceneBlockBuff->buffer);
	}

	// Rendering Loop!
	for (unsigned g = 0; g < scene->getActorCount(); g++) { // Implement this!
		actor_cptr actor = scene->getGeoActor(scene->getActorCount() - g - 1);
		unsigned renderID = getRenderID(actor);
		if (renderID == 0) {
			_isDrawn = false;
			return logMessage(MESSAGE_Exclaim, "renderID not found!");
		}

		Buffer_Drx11* vertexBuff = findBuffer(BUFF_Vertex_Type, renderID);
		Buffer_Drx11* indexBuff = findBuffer(BUFF_Index_UI, renderID);
		Buffer_Drx11* renderBlockBuff = findBuffer(BUFF_Render_Block, renderID);

		if (renderBlockBuff != nullptr)
			_deviceCtx->VSSetConstantBuffers(RENDER_BLOCK_BINDING, 1, &renderBlockBuff->buffer);

		UINT stride = sizeof(Geo_Vertex);
		UINT offset = 0;

		if (vertexBuff == nullptr || vertexBuff->count == 0)
			return logMessage(MESSAGE_Exclaim, "Corrupted Vertex Buffer!");
		else _deviceCtx->IASetVertexBuffers(0, 1, &vertexBuff->buffer, &stride, &offset);

		if (indexBuff != nullptr && indexBuff->count > 0)
			_deviceCtx->IASetIndexBuffer(indexBuff->buffer, DXGI_FORMAT_R32_UINT, 0);

		for (unsigned t = 0; t < _textures.size(); t++) {
			if (_textures.at(t).renderID > renderID) break; // Geometry actor is passed in sequence 
			else if (_textures.at(t).renderID == renderID) {
				ID3D11SamplerState* sampler = _textures.at(t).sampler;
				ID3D11ShaderResourceView* resView = _textures.at(t).resView;

				_deviceCtx->PSSetShaderResources(0, 1, &resView);
				_deviceCtx->PSSetSamplers(0, 1, &sampler);
				break;
			}
		}

		// Draw Call!
		if (indexBuff != nullptr && indexBuff->count != 0) _deviceCtx->DrawIndexed(indexBuff->count, 0, 0); // indexed draw
		else _deviceCtx->Draw(vertexBuff->count, 0); // non-indexed draw
	}

	_isDrawn = true;
} */

 void Topl_Renderer_Drx11::renderTarget(unsigned long renderID) {
	if(renderID == SPECIAL_SCENE_RENDER_ID && _buffers.front().renderID == SPECIAL_SCENE_RENDER_ID) {
		Buffer_Drx11* sceneBlockBuff = &_buffers.front();
		if (sceneBlockBuff != nullptr)
			_deviceCtx->VSSetConstantBuffers(SCENE_BLOCK_BINDING, 1, &sceneBlockBuff->buffer);
	}
	else {
		Buffer_Drx11* vertexBuff = findBuffer(BUFF_Vertex_Type, renderID);
		Buffer_Drx11* indexBuff = findBuffer(BUFF_Index_UI, renderID);
		Buffer_Drx11* renderBlockBuff = findBuffer(BUFF_Render_Block, renderID);

		if (renderBlockBuff != nullptr)
			_deviceCtx->VSSetConstantBuffers(RENDER_BLOCK_BINDING, 1, &renderBlockBuff->buffer);

		UINT stride = sizeof(Geo_Vertex);
		UINT offset = 0;

		if (vertexBuff == nullptr || vertexBuff->count == 0)
			return logMessage(MESSAGE_Exclaim, "Corrupted Vertex Buffer!");
		else _deviceCtx->IASetVertexBuffers(0, 1, &vertexBuff->buffer, &stride, &offset);

		if (indexBuff != nullptr && indexBuff->count > 0)
			_deviceCtx->IASetIndexBuffer(indexBuff->buffer, DXGI_FORMAT_R32_UINT, 0);

		for (unsigned t = 0; t < _textures.size(); t++) {
			if (_textures.at(t).renderID > renderID) break; // Geometry actor is passed in sequence 
			else if (_textures.at(t).renderID == renderID) {
				ID3D11SamplerState* sampler = _textures.at(t).sampler;
				ID3D11ShaderResourceView* resView = _textures.at(t).resView;

				_deviceCtx->PSSetShaderResources(0, 1, &resView);
				_deviceCtx->PSSetSamplers(0, 1, &sampler);
				break;
			}
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