#include "renderer/directx/Topl_Renderer_DX11.hpp"

namespace DX11 {
	static ID3D11SamplerState* texSamplers[MAX_TEX_BINDINGS + 1];
	static ID3D11ShaderResourceView* texResources[MAX_TEX_BINDINGS + 1];

	bool createBuff(ID3D11Device** device, ID3D11Buffer** buffer, UINT byteWidth, D3D11_USAGE usage, UINT bindFlags, UINT cpuFlags, const void* data) {
		// TODO: Ensure that data and size are valid!!!
		
		D3D11_BUFFER_DESC buffDesc;
		ZeroMemory(&buffDesc, sizeof(buffDesc));
		buffDesc.Usage = usage;
		buffDesc.ByteWidth = byteWidth;
		buffDesc.BindFlags = bindFlags;
		buffDesc.CPUAccessFlags = cpuFlags;
		buffDesc.MiscFlags = 0;
		buffDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA buffData;
		ZeroMemory(&buffData, sizeof(buffData));
		buffData.pSysMem = data;

		HRESULT result = (*(device))->CreateBuffer(&buffDesc, &buffData, buffer);
		if (FAILED(result)) return false;

		return true;
	}

	bool createVertexBuff(ID3D11Device** device, ID3D11Buffer** vBuff, vertex_cptr_t vData, unsigned vCount) {
		return createBuff(device, vBuff, sizeof(Geo_Vertex) * vCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, vData);
	}

	bool createIndexBuff(ID3D11Device** device, ID3D11Buffer** iBuff, DWORD* iData, unsigned iCount) {
		return createBuff(device, iBuff, sizeof(DWORD) * iCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, iData);
	}

	bool createBlockBuff(ID3D11Device** device, ID3D11Buffer** cBuff, const blockBytes_t* const blockBytes) {
		return createBuff(device, cBuff, sizeof(uint8_t) * blockBytes->size(), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, blockBytes->data());
	}

	D3D11_VIEWPORT createViewport(const Topl_Viewport* const vp) {
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

	D3D11_SAMPLER_DESC genSamplerDesc(enum TEX_Mode mode) {
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

Topl_Renderer_DX11::~Topl_Renderer_DX11() {
	_swapChain->Release();
	_device->Release();
	_deviceCtx->Release();

	if(_vertexDataLayout != nullptr) _vertexDataLayout->Release();
	if(_rtView != nullptr) _rtView->Release();
	if(_dsView != nullptr) _dsView->Release();
	if(_blendState != nullptr) _blendState->Release();
	if(_rasterizerState != nullptr) _rasterizerState->Release();

	if(_sceneBlockBuff != nullptr) _sceneBlockBuff->Release();
	// for(std::vector<DX11::Buffer>::iterator buff = _buffers.begin(); buff != _buffers.end(); buff++)
	//		buff->buffer->Release();
	
	for(std::vector<DX11::Texture>::iterator tex = _textures.begin(); tex != _textures.end(); tex++){
		if(tex->sampler != nullptr) tex->sampler->Release();
		if(tex->resource != nullptr) tex->resource->Release();
	}
}

void Topl_Renderer_DX11::init(NATIVE_WINDOW window) {
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
	swapChainDesc.OutputWindow = _platformCtx->window;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	HRESULT result; // error checking variable

	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION,
		&swapChainDesc, &_swapChain, &_device, NULL, &_deviceCtx);
	if (FAILED(result)) return logMessage(MESSAGE_Exclaim, "CreateAndSwapChain() failed");

	ID3D11Resource* backBuffer; // bgfx renderer_d3d11.cpp line 4660
	result = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (FAILED(result)) return logMessage(MESSAGE_Exclaim, "GetBuffer() from swapchain failed");

	_device->CreateRenderTargetView(backBuffer, NULL, &_rtView);
	backBuffer->Release();

	_deviceCtx->OMSetRenderTargets(1, &_rtView, NULL);

	// Depth Stencil View Creation

	D3D11_TEXTURE2D_DESC depthTexDesc;

	depthTexDesc.Height = Platform::getViewportHeight(_platformCtx);
	depthTexDesc.Width = Platform::getViewportWidth(_platformCtx);
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

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

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
	depthStencilState->Release();

	D3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc;
	dsViewDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsViewDesc.Texture2D.MipSlice = 0;

	_device->CreateDepthStencilView(depthTex, &dsViewDesc, &_dsView);
	_deviceCtx->OMSetRenderTargets(1, &_rtView, _dsView);

	// Blend State Creation

	D3D11_BLEND_DESC blendStateDesc = {};
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
}

void Topl_Renderer_DX11::clear() {
	const float clearColor[] = {_clearColors[0], _clearColors[1], _clearColors[2], _clearColors[3] };
	_deviceCtx->ClearRenderTargetView(_rtView, clearColor);
	_deviceCtx->ClearDepthStencilView(_dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);
}

void Topl_Renderer_DX11::setViewport(const Topl_Viewport* viewport) {
	if (viewport != nullptr) {
		D3D11_VIEWPORT vp = DX11::createViewport(viewport);
		_deviceCtx->RSSetViewports(1, &vp);
	}
	else _flags[BUILD_BIT] = false; // Error
}

void Topl_Renderer_DX11::swapBuffers(double frameTime) { _swapChain->Present(0, 0); }

void Topl_Renderer_DX11::build(const Geo_Actor* actor){
	if(actor == SCENE_RENDERID){
		_flags[BUILD_BIT] = DX11::createBlockBuff(&_device, &_sceneBlockBuff, &_sceneBlockData);
		_blockBufferMap.insert({ SCENE_RENDERID, DX11::Buffer(_sceneBlockBuff) });
	} else {
		Geo_Mesh* mesh = (Geo_Mesh*)actor->getMesh();
		unsigned long renderID = getRenderID(actor);

		// render block
		ID3D11Buffer* renderBlockBuff = nullptr;
		if(_blockBufferMap.find(renderID) != _blockBufferMap.end())
			_flags[BUILD_BIT] = DX11::createBlockBuff(&_device, &_blockBufferMap.at(renderID).buffer, &_actorBlockData);
		else {
			_flags[BUILD_BIT] = DX11::createBlockBuff(&_device, &renderBlockBuff, &_actorBlockData);
			_blockBufferMap.insert({ renderID, DX11::Buffer(renderID, BUFF_Render_Block, renderBlockBuff) });
		}
		if (!_flags[BUILD_BIT]) return logMessage(MESSAGE_Exclaim, "Buffer creation failed"); // Error

		// mesh block
		ID3D11Buffer* meshBlockBuff = nullptr;
		if(_extBlockBufferMap.find(renderID) != _extBlockBufferMap.end())
			_flags[BUILD_BIT] = DX11::createBlockBuff(&_device, &_extBlockBufferMap.at(renderID).buffer, &_meshBlockData);
		else {
			_flags[BUILD_BIT] = DX11::createBlockBuff(&_device, &meshBlockBuff, &_meshBlockData);
			_extBlockBufferMap.insert({ renderID, DX11::Buffer(renderID, BUFF_Ext_Block, meshBlockBuff) });
		}
		if (!_flags[BUILD_BIT]) return logMessage(MESSAGE_Exclaim, "Mesh Buffer creation failed"); // Error

		// indices generation
		ID3D11Buffer* indexBuff = nullptr;
		if(_indexBufferMap.find(renderID) != _indexBufferMap.end())
			_flags[BUILD_BIT] = DX11::createIndexBuff(&_device, &_indexBufferMap.at(renderID).buffer, (DWORD*)mesh->getIndices(), mesh->getIndexCount());
		else if (mesh->getIndices() != nullptr) { // checks if index data exists for render object
			_flags[BUILD_BIT] = DX11::createIndexBuff(&_device, &indexBuff, (DWORD*)mesh->getIndices(), mesh->getIndexCount());
			_indexBufferMap.insert({ renderID, DX11::Buffer(renderID, BUFF_Index_UI, indexBuff, mesh->getIndexCount()) });
		}
		else _indexBufferMap.insert({ renderID, DX11::Buffer(renderID, BUFF_Index_UI, indexBuff, 0) });
		if (!_flags[BUILD_BIT]) return logMessage(MESSAGE_Exclaim, "Buffer creation failed"); // Error

		// vertices generation
		ID3D11Buffer* vertexBuff = nullptr;
		if(_vertexBufferMap.find(renderID) != _vertexBufferMap.end())
			_flags[BUILD_BIT] = DX11::createVertexBuff(&_device, &_vertexBufferMap.at(renderID).buffer, mesh->getVertices(), mesh->getVertexCount());
		else {
			_flags[BUILD_BIT] = DX11::createVertexBuff(&_device, &vertexBuff, mesh->getVertices(), mesh->getVertexCount());
			_vertexBufferMap.insert({ renderID, DX11::Buffer(renderID, BUFF_Vertex_Type, vertexBuff, mesh->getVertexCount()) });
		}
		if (!_flags[BUILD_BIT]) return logMessage(MESSAGE_Exclaim, "Buffer creation failed"); // Error
	}
}

void Topl_Renderer_DX11::update(const Geo_Actor* actor){
	if(actor == SCENE_RENDERID) DX11::createBlockBuff(&_device, &_blockBufferMap.at(SCENE_RENDERID).buffer, &_sceneBlockData);
	else {
		unsigned long renderID = getRenderID(actor);

		if(_blockBufferMap.find(renderID) != _blockBufferMap.end())
			_flags[BUILD_BIT] = DX11::createBlockBuff(&_device, &_blockBufferMap.at(renderID).buffer, &_actorBlockData);

		if(_extBlockBufferMap.find(renderID) != _extBlockBufferMap.end())
			_flags[BUILD_BIT] = DX11::createBlockBuff(&_device, &_extBlockBufferMap.at(renderID).buffer, &_meshBlockData);
	}
}

void Topl_Renderer_DX11::setDrawMode(enum DRAW_Mode mode) {
	_drawMode = mode;

	switch (_drawMode) {
	case DRAW_Points: _deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST); break;
	case DRAW_Lines: _deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST); break;
	case DRAW_Triangles: _deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); break;
	case DRAW_Fan: _deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ); break; // not sure this is correct topology
	case DRAW_Strip: _deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); break;
	case DRAW_Patch: _deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST); break;
	}
}

void Topl_Renderer_DX11::draw(const Geo_Actor* actor) {
	unsigned long renderID = _renderTargetMap[actor];

	if(renderID == SCENE_RENDERID && _blockBufferMap.at(SCENE_RENDERID).renderID == SCENE_RENDERID) { // Scene Target
		if(_blockBufferMap.find(SCENE_RENDERID) != _blockBufferMap.end()) setConstBufferData(_blockBufferMap.at(SCENE_RENDERID).buffer, SCENE_BLOCK_BINDING);
		for(unsigned b = 0; b < MAX_TEX_BINDINGS - 1; b++){
			auto tex2D = std::find_if(_textures.begin(), _textures.end(), [renderID, b](const DX11::Texture& t){ return t.renderID == renderID && t.format == TEX_2D && t.binding == b + 1; });
			if (tex2D != _textures.end()){
				DX11::texSamplers[b + 1] = tex2D->sampler;
				DX11::texResources[b + 1] = tex2D->resource;
			}
		}
	}
	else if(actor->getMesh() != nullptr && actor->isShown) { // Drawable Target
		if(actor->getMesh()->drawMode != DRAW_Default) setDrawMode(actor->getMesh()->drawMode);
		// Data & Buffer Updates

		if(_blockBufferMap.find(renderID) != _blockBufferMap.end()) setConstBufferData(_blockBufferMap.at(renderID).buffer, RENDER_BLOCK_BINDING);
		if(_extBlockBufferMap.find(renderID) != _extBlockBufferMap.end()) setConstBufferData(_extBlockBufferMap.at(renderID).buffer, EXT_BLOCK_BINDING);

		if(_vertexBufferMap.find(renderID) != _vertexBufferMap.end())
			_deviceCtx->IASetVertexBuffers(0, 1, &_vertexBufferMap.at(renderID).buffer, &_vertexStride, &_vertexOffset);
		if (_indexBufferMap.find(renderID) != _indexBufferMap.end())
			_deviceCtx->IASetIndexBuffer(_indexBufferMap.at(renderID).buffer, DXGI_FORMAT_R32_UINT, 0);

		// Texture Updates

		auto tex2D = std::find_if(_textures.begin(), _textures.end(), [renderID](const DX11::Texture& t){ return t.renderID == renderID && t.format == TEX_2D && t.binding == 0; });
		if (tex2D != _textures.end()){
			DX11::texSamplers[DEFAULT_TEX_BINDING] = tex2D->sampler;
			DX11::texResources[DEFAULT_TEX_BINDING] = tex2D->resource;
		}
		auto tex3D = std::find_if(_textures.begin(), _textures.end(), [renderID](const DX11::Texture& t){ return t.renderID == renderID && t.format == TEX_3D; });
		if(tex3D != _textures.end()){
			DX11::texSamplers[MAX_TEX_BINDINGS] = tex3D->sampler;
			DX11::texResources[MAX_TEX_BINDINGS] = tex3D->resource;
		}

		_deviceCtx->VSSetSamplers(0, MAX_TEX_BINDINGS + 1, &DX11::texSamplers[0]);
		_deviceCtx->VSSetShaderResources(0, MAX_TEX_BINDINGS + 1, &DX11::texResources[0]);
		_deviceCtx->PSSetSamplers(0, MAX_TEX_BINDINGS + 1, &DX11::texSamplers[0]);
		_deviceCtx->PSSetShaderResources(0, MAX_TEX_BINDINGS + 1, &DX11::texResources[0]);

        if (_vertexBufferMap.find(renderID) != _vertexBufferMap.end()){ // DRAW CALL
			unsigned drawCount = (actor->getMesh()->drawMax != 0)? actor->getMesh()->drawMax : 0;
			if(drawCount == 0) drawCount = (_indexBufferMap.find(renderID) != _indexBufferMap.end())? _indexBufferMap.at(renderID).count : _vertexBufferMap.at(renderID).count;

            if(actor->getMesh()->getInstanceCount() <= 1){
                if (_indexBufferMap.find(renderID) != _indexBufferMap.end()) _deviceCtx->DrawIndexed(drawCount, actor->getMesh()->drawMin, 0); // indexed draw
                else _deviceCtx->Draw(drawCount, actor->getMesh()->drawMin); // non-indexed draw
            } else {
                if (_indexBufferMap.find(renderID) != _indexBufferMap.end()) _deviceCtx->DrawIndexedInstanced(drawCount, actor->getMesh()->getInstanceCount(), actor->getMesh()->drawMin, 0, 0); // instanced indexed draw
                else _deviceCtx->DrawInstanced(drawCount, actor->getMesh()->getInstanceCount(), actor->getMesh()->drawMin, 0); // instanced non-indexed draw
            }
        } else logMessage(MESSAGE_Exclaim, "Corrupted Vertex Buffer!");

		// TODO: Conditionally restore previous draw mode
	}
} 

void Topl_Renderer_DX11::setConstBufferData(ID3D11Buffer* buffer, unsigned short binding){
	_deviceCtx->VSSetConstantBuffers(binding, 1, &buffer);
	_deviceCtx->PSSetConstantBuffers(binding, 1, &buffer);
	if(_pipeline->geomShader != nullptr) _deviceCtx->GSSetConstantBuffers(binding, 1, &buffer);
	if(_pipeline->domainShader != nullptr) _deviceCtx->DSSetConstantBuffers(binding, 1, &buffer);
	if(_pipeline->hullShader != nullptr) _deviceCtx->HSSetConstantBuffers(binding, 1, &buffer);
}

#ifdef RASTERON_H

Sampler_2D Topl_Renderer_DX11::frame() {
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

	Rasteron_Image* stageImage = RASTERON_ALLOC("stage", TOPL_WIN_HEIGHT, TOPL_WIN_WIDTH);
	unsigned srcOffset = 0; unsigned dstOffset = 0;
	unsigned pitch = resource.RowPitch / 4; // << 2;
	for (unsigned r = 0; r < stageImage->height - 1; r++) {
		for (unsigned c = 0; c < stageImage->width; c++)
			*(stageImage->data + dstOffset + c) = *(sourceData + srcOffset + c);
		srcOffset += pitch;
		dstOffset += stageImage->width;
	}
	_deviceCtx->Unmap(framebuffTex, 0);
	bitSwitch_RB(stageImage->data, TOPL_WIN_WIDTH * TOPL_WIN_HEIGHT); // flipping red and blue bits

	_frameImage = Sampler_2D();
	_frameImage.setImage(stageImage);

	RASTERON_DEALLOC(stageImage);
	framebuffTex->Release();
	framebuff->Release();

	return _frameImage;
}

void Topl_Renderer_DX11::attachTexAt(const Sampler_2D* image, unsigned renderID, unsigned binding) {
	HRESULT result;

	D3D11_SAMPLER_DESC samplerDesc = DX11::genSamplerDesc(image->mode);
	ID3D11SamplerState* sampler;
	result = _device->CreateSamplerState(&samplerDesc, &sampler);

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
    texDesc.Width = image->getImage()->height; // inverse
    texDesc.Height = image->getImage()->width; // inverse
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	// texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	D3D11_SUBRESOURCE_DATA texData;
    texData.pSysMem = image->getImage()->data;
    texData.SysMemPitch = sizeof(uint32_t) * image->getImage()->height;
	texData.SysMemSlicePitch = 0;

	ID3D11Texture2D* texture;
	result = _device->CreateTexture2D(&texDesc, &texData, &texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc;
	ZeroMemory(&resourceDesc, sizeof(resourceDesc));
	resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resourceDesc.Texture2D.MipLevels = -1;

	ID3D11ShaderResourceView* resource;
	_device->CreateShaderResourceView(texture, &resourceDesc, &resource);
    _deviceCtx->UpdateSubresource(texture, 0, 0, image->getImage()->data, texData.SysMemPitch, 0);
	_deviceCtx->GenerateMips(resource);

	for(std::vector<DX11::Texture>::iterator tex = _textures.begin(); tex != _textures.end(); tex++)
		if (tex->renderID == renderID && tex->binding == binding && tex->format == TEX_2D) { // multi-texture subsitution
			tex->resource->Release(); // erase old texture
			tex->sampler->Release(); // erase old sampler
            *tex = DX11::Texture(renderID, (unsigned short)binding, TEX_2D, image->mode, sampler, resource);
			return;
		}


    _textures.push_back(DX11::Texture(renderID, (unsigned short)binding, TEX_2D, image->mode, sampler, resource)); // multi-texture addition

	texture->Release();
}

void Topl_Renderer_DX11::attachTex3D(const Sampler_3D* volumeTex, unsigned renderID) {
	HRESULT result;

    D3D11_SAMPLER_DESC samplerDesc = DX11::genSamplerDesc(volumeTex->mode);
	ID3D11SamplerState* sampler;
	result = _device->CreateSamplerState(&samplerDesc, &sampler);

	D3D11_TEXTURE3D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = volumeTex->getWidth();
	texDesc.Height = volumeTex->getHeight();
	texDesc.Depth = volumeTex->getDepth();
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	// texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	const Sampler_2D* volumeTexImage = volumeTex->getVolumeImg();
	D3D11_SUBRESOURCE_DATA texData;
	texData.pSysMem = volumeTexImage->getImage()->data;
	texData.SysMemPitch = sizeof(uint32_t) * volumeTex->getWidth() * volumeTex->getDepth();
	texData.SysMemSlicePitch = sizeof(uint32_t) * volumeTex->getHeight();

	ID3D11Texture3D* texture;
	result = _device->CreateTexture3D(&texDesc, &texData, &texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc;
	ZeroMemory(&resourceDesc, sizeof(resourceDesc));
	resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	resourceDesc.Texture2D.MipLevels = -1;

	ID3D11ShaderResourceView* resource;
	_device->CreateShaderResourceView(texture, &resourceDesc, &resource);
	_deviceCtx->UpdateSubresource(texture, 0, 0, volumeTexImage->getImage()->data, texData.SysMemPitch, 0);

	for (std::vector<DX11::Texture>::iterator tex = _textures.begin(); tex != _textures.end(); tex++)
		if (tex->renderID == renderID && tex->format == TEX_3D) { // texture substitution
			tex->resource->Release(); // erase old texture
			tex->sampler->Release(); // erase old sampler
            *tex = DX11::Texture(renderID, TEX_3D, volumeTex->mode, sampler, resource);
			return;
		}
    _textures.push_back(DX11::Texture(renderID, TEX_3D, volumeTex->mode, sampler, resource)); // texture addition

	// texture->Release(); // test deallocation
}

#endif
