#include "Topl_Renderer_Drx12.hpp"

namespace _Drx12_Init {

	// Creates a factory, searches for adapter and uses it to create a device
	static DRX12_INIT_STATUS create2_Factory_Device(ID3D12Device* device, IDXGIFactory4* dxgiFactory,){
		
		if (FAILED(   CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory))   )) 
			return DRX12_Init_Fail_CreateFactory;

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
				// DOES NOT CREATE DEVICE, JUST TESTS FOR D3D12 SUPPORT
				if (SUCCEEDED(   D3D12CreateDevice(drxAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)   )) {
					drxAdapterFound = true;
					break; // Adapter found!!!
				}
				else {
					drxAdapterIdx++;
					continue; // Adapter does not support DirectX 12
				}
			}
		}

		if (!drxAdapterFound) return DRX12_Init_Fail_NoAdapter;

		if (FAILED(   D3D12CreateDevice(drxAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device))   )) 
			return DRX12_Init_Fail_CreateDevice;

		return DRX12_Init_Good;
	}


	// Creates a command view based on a device
	static DRX12_INIT_STATUS create_cmdQueue(ID3D12CommandQueue* queue, ID3D12Device* device){
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = { }; // Using all default values
		if (FAILED(  device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&queue))   )) 
			return DRX12_Init_Fail_CreateCmdQueue;

		return DRX12_Init_Good;
	}


	// creates a swapChain using many input params
	static DRX12_INIT_STATUS create_swapChain(IDXGISwapChain3* swapChain, 
											  ID3D12CommandQueue* cmdQueue,
											  HWND hwnd,
											  int* frameIndex, 
											  IDXGIFactory4* factory,
											  const int frameBuffCount){
		DXGI_MODE_DESC backBufferDesc = { };
		backBufferDesc.Width = 0; // This scales it to default
		backBufferDesc.Height = 0; // This scales it to default
		backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		DXGI_SAMPLE_DESC sampleDesc = { };
		sampleDesc.Count = 1; // No multi-sampling

		DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
		// swapChainDesc.BufferCount = 3; // 3 back buffers
		swapChainDesc.BufferCount = frameBuffCount;
		swapChainDesc.BufferDesc = backBufferDesc;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // render to this swapchain
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // dxgi will discard buffer after call
		swapChainDesc.OutputWindow = hwnd; // passed as arg
		swapChainDesc.SampleDesc = sampleDesc;
		swapChainDesc.Windowed = true;

		IDXGISwapChain* tmpSwapChain;
		factory->CreateSwapChain(cmdQueue, &swapChainDesc, &tmpSwapChain);
		swapChain = static_cast<IDXGISwapChain3*>(tmpSwapChain); // Cast the temp structure
		*(frameIndex) = swapChain->GetCurrentBackBufferIndex(); // modifying value pointed by...

		return DRX12_Init_Good;
	}


	// Creates everything relevant to the render target view descriptor heap, double-check this code
	static DRX12_INIT_STATUS create3_rtvDescHeap_rTarg_descHeapSz(ID3D12DescriptorHeap* descHeap, ID3D12Resource* renderTarg[], int* descHeapSize,
																  ID3D12Device* device, IDXGISwapChain3* swapChain, int frameBuffCount){
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = { };
		rtvHeapDesc.NumDescriptors = frameBuffCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; // Not shader visible

		if (FAILED(   device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&descHeap))   )) 
			return DRX12_Init_Fail_CreateDescHeap;

		*(descHeapSize) = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descHeap->GetCPUDescriptorHandleForHeapStart());

		for (short b = 0; b < frameBuffCount; b++) {
			if (FAILED(   swapChain->GetBuffer(b, IID_PPV_ARGS(&renderTarg[b]))   )) 
				return DRX12_Init_Fail_SwpChainGetBuffer;

			device->CreateRenderTargetView(renderTarg[b], nullptr, rtvHandle);
			rtvHandle.Offset(1, *(descHeapSize));
		}

		return DRX12_Init_Good;
	}
}

DRX12_INIT_STATUS Topl_Drx12::init(NATIVE_WINDOW hwnd){
	HRESULT hr;
	DRX12_INIT_STATUS status;

	status = _Drx12_Init::create2_Factory_Device(m_device, m_factory);
	if(status != DRX12_Init_Good) return status;

	status = _Drx12_Init::create_cmdQueue(m_cmdQueue, m_device);
	if(status != DRX12_Init_Good) return status;

	status = _Drx12_Init::create_swapChain(m_swapChain, m_cmdQueue, hwnd, &m_frameIndex, m_factory, m_frameBuffCount);
	if(status != DRX12_Init_Good) return status;

	status = _Drx12_Init::create3_rtvDescHeap_rTarg_descHeapSz(m_rtvDescriptHeap, m_renderTargets, m_rtvDescriptorSize, 
															   m_device, m_swapChain, m_frameBuffCount);
	if(status != DRX12_Init_Good) return status;

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