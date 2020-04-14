#include "Topl_Renderer_Drx12.hpp"

DRX12_INIT_STATUS Topl_Drx12::init(HWND hwnd){
	HRESULT hr;

	IDXGIFactory4* dxgiFactory;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(hr)) return DRX12_Init_Fail_CreateFactory;

	IDXGIAdapter1* drxAdapter;
	int drxAdapterIdx = 0;
	bool drxAdapterFound = false;

	while (dxgiFactory->EnumAdapters1(drxAdapterIdx, &drxAdapter) != DXGI_ERROR_NOT_FOUND) {
		DXGI_ADAPTER_DESC1 drxAdapterDesc;
		drxAdapter->GetDesc1(&drxAdapterDesc);
		
		if (drxAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			drxAdapterIdx++;
			continue; // Adapter not found
		} else {
			hr = D3D12CreateDevice(drxAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);
			// DOES NOT CREATE DEVICE, JUST TESTS FOR D3D12 SUPPORT
			if (SUCCEEDED(hr)) {
				drxAdapterFound = true;
				break; // Adapter found!!!
			}
			else {
				drxAdapterIdx++;
				continue; // Adapter does not support DirectX 12
			}
			// OPTIMIZE THIS CODE!!!!!!!!
		}
	}

	if (!drxAdapterFound) return DRX12_Init_Fail_NoAdapter;

	hr = D3D12CreateDevice(drxAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
	if (FAILED(hr)) return DRX12_Init_Fail_CreateDevice;

	// Command queue time!
	// D3D12_COMMAND_QUEUE_DESC drxCmdQueue;

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = { }; // Using all default values
	hr = m_device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_cmdQueue));
	if (FAILED(hr)) return DRX12_Init_Fail_CreateCmdQueue;


	// Swap Chain Time!

	DXGI_SWAP_CHAIN_DESC drxSwapChainDesc;

	DXGI_MODE_DESC backBufferDesc = { };
	backBufferDesc.Width = 0; // This scales it to default
	backBufferDesc.Height = 0; // This scales it to default
	backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	DXGI_SAMPLE_DESC sampleDesc = { };
	sampleDesc.Count = 1; // No multi-sampling

	DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
	// swapChainDesc.BufferCount = 3; // 3 back buffers
	swapChainDesc.BufferCount = m_frameBuffCount;
	swapChainDesc.BufferDesc = backBufferDesc;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // render to this swapchain
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // dxgi will discard buffer after call
	swapChainDesc.OutputWindow = hwnd; // passed as arg
	swapChainDesc.SampleDesc = sampleDesc;
	swapChainDesc.Windowed = true;

	IDXGISwapChain* tmpSwapChain;
	dxgiFactory->CreateSwapChain(m_cmdQueue, &swapChainDesc, &tmpSwapChain);
	m_swapChain = static_cast<IDXGISwapChain3*>(tmpSwapChain); // Cast the temp structure
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	
	// Descriptor Heap Time!

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = { };
	rtvHeapDesc.NumDescriptors = m_frameBuffCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; // Not shader visible

	hr = m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvDescriptHeap));
	if (FAILED(hr)) return DRX12_Init_Fail_CreateDescHeap;

	m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvDescriptHeap->GetCPUDescriptorHandleForHeapStart());

	for (short b = 0; b < m_frameBuffCount; b++) {
		hr = m_swapChain->GetBuffer(b, IID_PPV_ARGS(&m_renderTargets[b]));
		if (FAILED(hr)) return DRX12_Init_Fail_SwpChainGetBuffer;

		m_device->CreateRenderTargetView(m_renderTargets[b], nullptr, rtvHandle);
		rtvHandle.Offset(1, m_rtvDescriptorSize);
	}

	// Command Allocator Time!
	// Each rtv needs its own command allocator

	for (short c = 0; c < m_frameBuffCount; c++) {
		hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmdAllocator[c]));
		if (FAILED(hr)) return DRX12_Init_Fail_CreateCmdAllocator;
	}

	// Command List Time!

	hr = m_device->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmdAllocator[0], NULL, IID_PPV_ARGS(&m_cmdGList));
	if (FAILED(hr)) return DRX12_Init_Fail_CreateCmdList;

	m_cmdGList->Close();

	// Fence Time!

	ID3D12Fence* fence[3] = { m_fence[0], m_fence[1], m_fence[2] }; // For Testing

	for (short f = 0; f < m_frameBuffCount; f++) {
		// hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence[f])); // For Testing
		hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence[f]));
		if (FAILED(hr)) return DRX12_Init_Fail_CreateFence;

		m_fenceValue[f] = 0; // Starting value of fence
	}

	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fenceEvent == nullptr) return DRX12_Init_Fail_CreateFenceEvent;

	return DRX12_Init_Good;
}