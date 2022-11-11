#include "Diagnostic.hpp"

// #define APP_BACKEND APP_OpenGL_4
#define APP_BACKEND APP_DirectX_11

Geo_FlatTriangle triangle = Geo_FlatTriangle(1.0f);

// OpenGL Test Renderer
void Barebones_Renderer_GL4::config() {
	Geo_RenderObj* renderObj = (Geo_RenderObj*)&triangle;

	// indices generation
	_buffers.push_back(Buffer_GL4(1, BUFF_Index_UI, _bufferSlots[0], renderObj->getIndexCount()));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers.back().buffer); // gets the latest buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, renderObj->getIndexCount() * sizeof(unsigned), renderObj->getIndices(), GL_STATIC_DRAW);

	// vertices generation
	_buffers.push_back(Buffer_GL4(1, BUFF_Vertex_Type, _bufferSlots[1], renderObj->getVertexCount()));
	glBindBuffer(GL_ARRAY_BUFFER, _buffers.back().buffer); // gets the latest buffer
	glBufferData(GL_ARRAY_BUFFER, renderObj->getVertexCount() * sizeof(Geo_Vertex), renderObj->getVertices(), GL_STATIC_DRAW);

	// setting vertex input layout
	_vertexArrays.push_back(VertexArray_GL4(1, _vertexArraySlots[0]));
	glBindVertexArray(_vertexArrays.back().vao);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, sizeof(float) * 3, GL_FLOAT, GL_FALSE, sizeof(Geo_Vertex), NULL); // position data
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, sizeof(float) * 2, GL_FLOAT, GL_FALSE, sizeof(Geo_Vertex), (void*)1); // texture coordinate data
}

#ifdef _WIN32
// DirectX Test Renderer
void Barebones_Renderer_Drx11::config(){
	Geo_RenderObj* renderObj = (Geo_RenderObj*)&triangle;

	// setting vertex input layout

	D3D11_INPUT_ELEMENT_DESC vertexPosDesc; D3D11_INPUT_ELEMENT_DESC texCoordDesc;
	vertexPosDesc.SemanticName = "POSITION"; texCoordDesc.SemanticName = "TEXCOORD";
	vertexPosDesc.SemanticIndex = 0; texCoordDesc.SemanticName = 0;
	vertexPosDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT; texCoordDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexPosDesc.InputSlot = 0; texCoordDesc.InputSlot = 0;
	vertexPosDesc.AlignedByteOffset = sizeof(float) * 3; texCoordDesc.AlignedByteOffset = sizeof(float) * 2;
	vertexPosDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; texCoordDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexPosDesc.InstanceDataStepRate = 0; texCoordDesc.InstanceDataStepRate = 0;

	D3D11_INPUT_ELEMENT_DESC layoutDesc[2] = { vertexPosDesc, texCoordDesc };
	if(_pipeline != nullptr)
		_device->CreateInputLayout(&layoutDesc[0], 2,
			_pipeline->vsBlob->GetBufferPointer(),
			_pipeline->vsBlob->GetBufferSize(),
			&_vertexDataLayout
		);
	_deviceCtx->IASetInputLayout(_vertexDataLayout);

	// indices generation

	D3D11_BUFFER_DESC indexBuffDesc;
	ZeroMemory(&indexBuffDesc, sizeof(indexBuffDesc));
	indexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBuffDesc.ByteWidth = sizeof(DWORD) * renderObj->getIndexCount();
	indexBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBuffDesc.CPUAccessFlags = 0;
	indexBuffDesc.MiscFlags = 0;
	indexBuffDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexBuffData;
	ZeroMemory(&indexBuffData, sizeof(indexBuffData));
	indexBuffData.pSysMem = renderObj->getIndices();

	ID3D11Buffer* indexBuff = nullptr;
	_device->CreateBuffer(&indexBuffDesc, &indexBuffData, &indexBuff);
	_buffers.push_back(Buffer_Drx11(1, BUFF_Index_UI, indexBuff, renderObj->getIndexCount()));

	// vertices generation

	D3D11_BUFFER_DESC vertexBuffDesc;
	ZeroMemory(&vertexBuffDesc, sizeof(vertexBuffDesc));
	vertexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBuffDesc.ByteWidth = sizeof(DWORD) * renderObj->getVertexCount();
	vertexBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBuffDesc.CPUAccessFlags = 0;
	vertexBuffDesc.MiscFlags = 0;
	vertexBuffDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBuffData;
	ZeroMemory(&vertexBuffData, sizeof(vertexBuffData));
	vertexBuffData.pSysMem = renderObj->getVertices();

	ID3D11Buffer* vertexBuff = nullptr;
	_device->CreateBuffer(&vertexBuffDesc, &vertexBuffData, &vertexBuff);
	_buffers.push_back(Buffer_Drx11(1, BUFF_Vertex_Type, vertexBuff, renderObj->getVertexCount()));
}
#endif


// Diagnose Main Loop

int main(int argc, char** argv) {
	Platform platform(argv[0], "Diagnostic");
	platform.createWindow(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);

	// Constructor and Destructor Testing
	{
		Topl_Renderer_GL4 renderer_GL = Topl_Renderer_GL4(platform.getParentWindow());
#ifdef _WIN32
		Topl_Renderer_Drx11 renderer_Drx = Topl_Renderer_Drx11(platform.getParentWindow());
#endif
	}

	// Performance Testing

	Topl_Renderer* renderer = nullptr;

	if (APP_BACKEND == APP_OpenGL_4) renderer = new Barebones_Renderer_GL4(platform.getParentWindow());
	else if(APP_BACKEND == APP_DirectX_11) renderer = new Barebones_Renderer_Drx11(platform.getParentWindow());

	Timer_Ticker _ticker;
	while(1){
		double frameTime = _ticker.getRelMillisecs();
		platform.handleEvents(DISABLE_CURSOR_UPDATE);
		
		renderer->clearView();
		renderer->renderScene(nullptr);
		renderer->switchFramebuff();
	}

	if(renderer != nullptr) delete(renderer);
	return 0;
}