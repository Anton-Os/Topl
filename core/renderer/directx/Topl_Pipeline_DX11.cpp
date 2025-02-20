#include "renderer/directx/Topl_Renderer_DX11.hpp"

namespace DX11 {
    bool compileShader(const char* shaderFilePath, LPCSTR shaderTarget, ID3DBlob** blob){
		ID3DBlob* errorBuff;
		size_t sourceSize;

		sourceSize = strlen(shaderFilePath) + 1;
		wchar_t* shaderFilePath_wchar = new wchar_t[sourceSize];
		mbstowcs(shaderFilePath_wchar, shaderFilePath, sourceSize); // need proper conversion to wcharhar_t
		if (FAILED(
			D3DCompileFromFile(shaderFilePath_wchar,
				nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", shaderTarget,
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 
				0, blob, &errorBuff
			)
		)) {
			OutputDebugStringA((char*)errorBuff->GetBufferPointer());
			printf("Debug error is %s", (char*)errorBuff->GetBufferPointer());
			delete shaderFilePath_wchar; // Proper deallocation of the source string
			return false;
		}

		delete shaderFilePath_wchar; // Proper deallocation of the source string
		return true;
	}

	bool compileShader(std::string shaderText, LPCSTR shaderTarget, ID3DBlob** blob){
		std::string tempFilePath = std::string(SHADERS_DIR) + "hlsl/" + "Shader.hlsl";
		std::replace(tempFilePath.begin(), tempFilePath.end(), '/', '\\'); // replacing forward and backward slashes
		std::ofstream shaderFile = std::ofstream(tempFilePath.c_str(), std::ios::out);
		if(!shaderFile.is_open()) return false;
		shaderFile.write(shaderText.c_str(), shaderText.size());
		shaderFile.flush();
		shaderFile.close();

		if(compileShader(tempFilePath.c_str(), shaderTarget, blob)) std::remove(tempFilePath.c_str()); // remove file on success
		
		return true;
	}

	DXGI_FORMAT getShaderFormat(enum SHDR_ValueType type) {
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
			logMessage("DX11 shader input type not supported!");
			break;
		}

		DXGI_FORMAT format;
		return format;
	}

	D3D11_INPUT_ELEMENT_DESC getElementDescFromInput(const Shader_Type* input, UINT offset) {
		D3D11_INPUT_ELEMENT_DESC vertexElemDesc;
		vertexElemDesc.SemanticName = input->semantic.c_str();
		vertexElemDesc.SemanticIndex = 0;
		vertexElemDesc.Format = DX11::getShaderFormat(input->type);
		vertexElemDesc.InputSlot = 0;
		vertexElemDesc.AlignedByteOffset = offset;
		vertexElemDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		vertexElemDesc.InstanceDataStepRate = 0;

		return vertexElemDesc;
	}

	bool createVertexLayout(ID3D11Device** device, ID3D11DeviceContext** deviceCtx, ID3D11InputLayout** layout, DX11::Pipeline* pipeline, entry_shader_cptr entryShader){
		D3D11_INPUT_ELEMENT_DESC* vertexLayout_ptr = (D3D11_INPUT_ELEMENT_DESC*)malloc(sizeof(D3D11_INPUT_ELEMENT_DESC) * entryShader->getInputCount());
		unsigned vertexElemOffset = 0;
		for (unsigned inputNum = 0; inputNum < entryShader->getInputCount(); inputNum++) {
			*(vertexLayout_ptr + inputNum) = getElementDescFromInput(entryShader->getInputAtIndex(inputNum), vertexElemOffset);
			vertexElemOffset += Topl_Pipeline::getOffset(entryShader->getInputAtIndex(inputNum)->type);
		}

		(*device)->CreateInputLayout(
			vertexLayout_ptr, 
			entryShader->getInputCount(),
			pipeline->vsBlob->GetBufferPointer(), 
			pipeline->vsBlob->GetBufferSize(),
			layout
		);

		(*deviceCtx)->IASetInputLayout(*layout);
		free(vertexLayout_ptr); // deallocating vertexLayout_ptr and all associated data

		return true;
	}
}

void Topl_Renderer_DX11::setPipeline(DX11::Pipeline* pipeline) {
	if (pipeline == nullptr) 
		return logMessage(MESSAGE_Exclaim, "Pipeline cannot be null!");

	_pipeline = pipeline;
	Topl_Renderer::setPipeline((Topl_Pipeline*)pipeline);

	if (_flags[PIPELINE_BIT]) {
		_deviceCtx->VSSetShader(pipeline->vertexShader, 0, 0);
		_deviceCtx->HSSetShader(pipeline->hullShader, 0, 0);
		_deviceCtx->DSSetShader(pipeline->domainShader, 0, 0);
		_deviceCtx->GSSetShader(pipeline->geomShader, 0, 0);
		_deviceCtx->PSSetShader(pipeline->pixelShader, 0, 0);
		_deviceCtx->CSSetShader(pipeline->computeShader, 0, 0);
	}
}

void Topl_Renderer_DX11::genPipeline(DX11::Pipeline* pipeline, entry_shader_cptr vertexShader, shader_cptr pixelShader, std::initializer_list<shader_cptr> shaders){
	if (pipeline == nullptr || ((vertexShader == nullptr || pixelShader == nullptr) && shaders.size() == 0))
		return logMessage(MESSAGE_Exclaim, "Pipeline, vertex and pixel shaders cannot be null!");
	HRESULT hr; // error checking variable

	// Vertex Shader
    if(vertexShader != nullptr){
        if(!DX11::compileShader(vertexShader->getFileSource(), "vs_5_0", &pipeline->vsBlob)) return;
        hr = _device->CreateVertexShader(pipeline->vsBlob->GetBufferPointer(), pipeline->vsBlob->GetBufferSize(), NULL, &pipeline->vertexShader);
        if (FAILED(hr)) { pipeline->isReady = false; return; }
    }

	// Pixel Shader
    if(pixelShader != nullptr){
        if (!DX11::compileShader(pixelShader->getFileSource(), "ps_5_0", &pipeline->psBlob)) return;
        hr = _device->CreatePixelShader(pipeline->psBlob->GetBufferPointer(), pipeline->psBlob->GetBufferSize(),NULL, &pipeline->pixelShader);
        if (FAILED(hr)) { pipeline->isReady = false; return; }
    }

	auto geomShader = std::find_if(shaders.begin(), shaders.end(), [](const shader_cptr& s){ return s->getType() == SHDR_Geom; });
	auto hullShader = std::find_if(shaders.begin(), shaders.end(), [](const shader_cptr& s){ return s->getType() == SHDR_TessCtrl; });
	auto domainShader = std::find_if(shaders.begin(), shaders.end(), [](const shader_cptr& s){ return s->getType() == SHDR_TessEval; });
	auto computeShader = std::find_if(shaders.begin(), shaders.end(), [](const shader_cptr& s){ return s->getType() == SHDR_Compute; });

	// Geometry Shader
	if (geomShader != shaders.end()) { // optional stage
		if (!DX11::compileShader((*geomShader)->getFileSource(), "gs_5_0", &pipeline->gsBlob)) return;
		hr = _device->CreateGeometryShader(pipeline->gsBlob->GetBufferPointer(), pipeline->gsBlob->GetBufferSize(), NULL, &pipeline->geomShader);
		if (FAILED(hr)) { pipeline->isReady = false; return; }
	}

	// Hull Shader
	if (hullShader != shaders.end()) { // optional stage
		if (!DX11::compileShader((*hullShader)->getFileSource(), "hs_5_0", &pipeline->hsBlob)) return;
		hr = _device->CreateHullShader( pipeline->hsBlob->GetBufferPointer(), pipeline->hsBlob->GetBufferSize(), NULL, &pipeline->hullShader);
		if (FAILED(hr)) { pipeline->isReady = false; return; }
	}

	// Domain Shader
	if (domainShader != shaders.end()) { // optional stage
		if (!DX11::compileShader((*domainShader)->getFileSource(), "ds_5_0", &pipeline->dsBlob)) return;
		hr = _device->CreateDomainShader(pipeline->dsBlob->GetBufferPointer(), pipeline->dsBlob->GetBufferSize(), NULL, &pipeline->domainShader);
		if (FAILED(hr)) { pipeline->isReady = false; return; }
	}

	// Compute Shader
	if (computeShader != shaders.end()) { // optional stage
		if (!DX11::compileShader((*computeShader)->getFileSource(), "cs_5_0", &pipeline->csBlob)) return;
		hr = _device->CreateComputeShader(pipeline->csBlob->GetBufferPointer(), pipeline->csBlob->GetBufferSize(), NULL, &pipeline->computeShader);
		if (FAILED(hr)) { pipeline->isReady = false; return; }
	}

	pipeline->setShaders(vertexShader, pixelShader, shaders);
	setPipeline(pipeline);

	if(_vertexDataLayout == nullptr && pipeline->getEntryShader() != nullptr) 
		DX11::createVertexLayout(&_device, &_deviceCtx, &_vertexDataLayout, pipeline, pipeline->getEntryShader());
}
