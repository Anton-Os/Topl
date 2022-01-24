#include "Topl_Renderer_Drx11.hpp"

namespace _Drx11 {
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
}

void Topl_Renderer_Drx11::setPipeline(Topl_Pipeline_Drx11* pipeline) {
	_pipeline = pipeline;
	Topl_Renderer::setPipeline((Topl_Pipeline*)pipeline);

	if (_isPipelineReady) {
		if (pipeline->vertexShader != nullptr) _deviceCtx->VSSetShader(pipeline->vertexShader, 0, 0);
		if (pipeline->hullShader != nullptr) _deviceCtx->HSSetShader(pipeline->hullShader, 0, 0);
		if (pipeline->domainShader != nullptr) _deviceCtx->DSSetShader(pipeline->domainShader, 0, 0);
		if (pipeline->geomShader != nullptr) _deviceCtx->GSSetShader(pipeline->geomShader, 0, 0);
		if (pipeline->pixelShader != nullptr) _deviceCtx->PSSetShader(pipeline->pixelShader, 0, 0);
	}
}

void Topl_Renderer_Drx11::genPipeline(Topl_Pipeline_Drx11* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader){
	HRESULT hr; // error checking variable

	// vertex shader compilation and creation code
	if(!_Drx11::compileShader(vertexShader->getFilePath(), "vs_5_0", &pipeline->vsBlob));
	hr = _device->CreateVertexShader(
		pipeline->vsBlob->GetBufferPointer(), pipeline->vsBlob->GetBufferSize(),
		NULL, &pipeline->vertexShader
	);
	if (FAILED(hr)) { pipeline->isReady = false; return; }

	// pixel shader compilation and creation code
	if(!_Drx11::compileShader(pixelShader->getFilePath(), "ps_5_0", &pipeline->psBlob));
	hr = _device->CreatePixelShader(
		pipeline->psBlob->GetBufferPointer(), pipeline->psBlob->GetBufferSize(),
		NULL, &pipeline->pixelShader
	);
	if (FAILED(hr)) { pipeline->isReady = false; return; }

	pipeline->entryShader = vertexShader;
	pipeline->isReady = true;
	setPipeline(pipeline);
}

void Topl_Renderer_Drx11::genPipeline(Topl_Pipeline_Drx11* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, shader_cptr tessCtrlShader, shader_cptr tessEvalShader, shader_cptr geomShader){
	HRESULT hr; // error checking variable

	// vertex shader compilation and creation code
	if(!_Drx11::compileShader(vertexShader->getFilePath(), "vs_5_0", &pipeline->vsBlob));
	hr = _device->CreateVertexShader(
		pipeline->vsBlob->GetBufferPointer(), pipeline->vsBlob->GetBufferSize(),
		NULL, &pipeline->vertexShader
	);
	if (FAILED(hr)) { pipeline->isReady = false; return; }

	// hull shader compilation and creation code
	if(!_Drx11::compileShader(tessCtrlShader->getFilePath(), "hs_5_0", &pipeline->hsBlob));
	hr = _device->CreateHullShader(
		pipeline->hsBlob->GetBufferPointer(), pipeline->hsBlob->GetBufferSize(),
		NULL, &pipeline->hullShader);
	if (FAILED(hr)) { pipeline->isReady = false; return; }

	// domain shader compilation and creation code
	if(!_Drx11::compileShader(tessEvalShader->getFilePath(), "ds_5_0", &pipeline->dsBlob));
	hr = _device->CreateDomainShader(
		pipeline->dsBlob->GetBufferPointer(), pipeline->dsBlob->GetBufferSize(),
		NULL, &pipeline->domainShader
	);
	if (FAILED(hr)) { pipeline->isReady = false; return; }

	// geometry shader compilation and creation code
	if(!_Drx11::compileShader(geomShader->getFilePath(), "gs_5_0", &pipeline->gsBlob));
	hr = _device->CreateGeometryShader(
		pipeline->gsBlob->GetBufferPointer(), pipeline->gsBlob->GetBufferSize(),
		NULL, &pipeline->geomShader
	);
	if (FAILED(hr)) { pipeline->isReady = false; return; }

	// pixel shader compilation and creation code
	if(!_Drx11::compileShader(pixelShader->getFilePath(), "ps_5_0", &pipeline->psBlob));
	hr = _device->CreatePixelShader(
		pipeline->psBlob->GetBufferPointer(), pipeline->psBlob->GetBufferSize(),
		NULL, &pipeline->pixelShader
	);
	if (FAILED(hr)) { pipeline->isReady = false; return; }

	pipeline->entryShader = vertexShader;
	pipeline->isReady = true;
	setPipeline(pipeline);
}
