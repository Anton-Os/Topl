#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// Windows runtime library COM-Specific items
#include <wrl.h>
using namespace Microsoft::WRL;

//#include <d3d12.h>
#include "d3dx12.h" // Helper that also includes d3d12.h
#include <dxgi1_4.h>
// #include <d3dx12.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#pragma comment(lib, "D3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")

namespace GlobDrx {
	const int frameBuffCount = 3; // For triple buffering
	ID3D12Device* device;
	IDXGISwapChain3* swapChain; // For switching between render targets
	ID3D12CommandQueue* cmdQueue; // contatiner of command list
	ID3D12DescriptorHeap* rtvDescriptHeap; // descriptor heap to hold resources
	ID3D12Resource* renderTargets[frameBuffCount]; // Render targets
	ID3D12CommandAllocator* cmdAllocator[frameBuffCount]; // Command allocator for each frame
	ID3D12GraphicsCommandList* cmdGList; // Command list to record and render onto frame
	
	ID3D12Fence* fence[frameBuffCount]; // For each allocator
	HANDLE fenceEvent; // Event when handle is unlocked by GPU
	UINT64 fenceValue[frameBuffCount]; // Incremented each frame
	
	int frameIndex; // current rtv we are on
	int rtvDescriptorSize; // Size of rtv descriptor on the device (front/back buffers will be same size)
	bool isRunning = true; // USE THIS AS CONDITIONAL FOR LOOP
}

ID3D12Fence* fenceTest[GlobDrx::frameBuffCount];

void waitPrevFrame() {
	HRESULT hr;
	GlobDrx::frameIndex = GlobDrx::swapChain->GetCurrentBackBufferIndex();

	// D3D12Fence** fenceRef[3] = &GlobDrx::fence; // For testing

	if (GlobDrx::fence[GlobDrx::frameIndex]->GetCompletedValue() < GlobDrx::fenceValue[GlobDrx::frameIndex]) {
		hr = GlobDrx::fence[GlobDrx::frameIndex]->SetEventOnCompletion(
			GlobDrx::fenceValue[GlobDrx::frameIndex],
			GlobDrx::fenceEvent);
		if (FAILED(hr)) GlobDrx::isRunning = false;

		WaitForSingleObject(GlobDrx::fenceEvent, INFINITE);
	}

	GlobDrx::fenceValue[GlobDrx::frameIndex]++;
}

int initDrx(HWND hwnd) {
	HRESULT hr;

	IDXGIFactory4* dxgiFactory;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(hr)) return -1;

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

	if (!drxAdapterFound) return -2;

	hr = D3D12CreateDevice(drxAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&GlobDrx::device));
	if (FAILED(hr)) return -3;

	// Command queue time!
	// D3D12_COMMAND_QUEUE_DESC drxCmdQueue;

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = { }; // Using all default values
	hr = GlobDrx::device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&GlobDrx::cmdQueue));
	if (FAILED(hr)) return -4;


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
	swapChainDesc.BufferCount = GlobDrx::frameBuffCount;
	swapChainDesc.BufferDesc = backBufferDesc;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // render to this swapchain
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // dxgi will discard buffer after call
	swapChainDesc.OutputWindow = hwnd; // passed as arg
	swapChainDesc.SampleDesc = sampleDesc;
	swapChainDesc.Windowed = true;

	IDXGISwapChain* tmpSwapChain;
	dxgiFactory->CreateSwapChain(GlobDrx::cmdQueue, &swapChainDesc, &tmpSwapChain);
	GlobDrx::swapChain = static_cast<IDXGISwapChain3*>(tmpSwapChain); // Cast the temp structure
	GlobDrx::frameIndex = GlobDrx::swapChain->GetCurrentBackBufferIndex();

	
	// Descriptor Heap Time!

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = { };
	rtvHeapDesc.NumDescriptors = GlobDrx::frameBuffCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; // Not shader visible

	hr = GlobDrx::device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&GlobDrx::rtvDescriptHeap));
	if (FAILED(hr)) return -5;

	GlobDrx::rtvDescriptorSize = GlobDrx::device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(GlobDrx::rtvDescriptHeap->GetCPUDescriptorHandleForHeapStart());

	for (short b = 0; b < GlobDrx::frameBuffCount; b++) {
		hr = GlobDrx::swapChain->GetBuffer(b, IID_PPV_ARGS(&GlobDrx::renderTargets[b]));
		if (FAILED(hr)) return -6;

		GlobDrx::device->CreateRenderTargetView(GlobDrx::renderTargets[b], nullptr, rtvHandle);
		rtvHandle.Offset(1, GlobDrx::rtvDescriptorSize);
	}

	// Command Allocator Time!
	// Each rtv needs its own command allocator

	for (short c = 0; c < GlobDrx::frameBuffCount; c++) {
		hr = GlobDrx::device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&GlobDrx::cmdAllocator[c]));
		if (FAILED(hr)) return -7;
	}

	// Command List Time!

	hr = GlobDrx::device->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, GlobDrx::cmdAllocator[0], NULL, IID_PPV_ARGS(&GlobDrx::cmdGList));
	if (FAILED(hr)) return -8;

	GlobDrx::cmdGList->Close();

	// Fence Time!

	ID3D12Fence* fence[3] = { GlobDrx::fence[0], GlobDrx::fence[1], GlobDrx::fence[2] }; // For Testing

	for (short f = 0; f < GlobDrx::frameBuffCount; f++) {
		// hr = GlobDrx::device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence[f])); // For Testing
		hr = GlobDrx::device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&GlobDrx::fence[f]));
		if (FAILED(hr)) return -9;

		GlobDrx::fenceValue[f] = 0; // Starting value of fence
	}

	GlobDrx::fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (GlobDrx::fenceEvent == nullptr) return -10;

	return 1;
}

void updateDrxPipeline() {
	HRESULT hr;
	// Here the descriptor heap and cmd list are heavily used

	waitPrevFrame(); // We are creating this some other place

	hr = GlobDrx::cmdAllocator[GlobDrx::frameIndex]->Reset();
	hr = GlobDrx::cmdGList->Reset(GlobDrx::cmdAllocator[GlobDrx::frameIndex], NULL);

	if (FAILED(hr)) return;

	GlobDrx::cmdGList->ResourceBarrier( // Transition from present state to render target
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			GlobDrx::renderTargets[GlobDrx::frameIndex],
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET)
	);
	// Command recording start?

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
		GlobDrx::rtvDescriptHeap->GetCPUDescriptorHandleForHeapStart(),
		GlobDrx::frameIndex,
		GlobDrx::rtvDescriptorSize
	);

	GlobDrx::cmdGList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	const float clearColor[] = { 0.4f, 0.4f, 0.9f, 1.0f };
	GlobDrx::cmdGList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr); // This actually clears is!

	// Command recording end?
	GlobDrx::cmdGList->ResourceBarrier( // Transition from render target to present state
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			GlobDrx::renderTargets[GlobDrx::frameIndex],
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT)
	);

	hr = GlobDrx::cmdGList->Close();
	if (FAILED(hr)) return;
}

void renderDrx() {
	HRESULT hr;
	updateDrxPipeline(); // Maybe call outside
	// Here the command queue and swap chain are utilized

	ID3D12CommandList* ppCmdLists[] = { GlobDrx::cmdGList }; // We need the correct type but we use same cmdList
	GlobDrx::cmdQueue->ExecuteCommandLists(_countof(ppCmdLists), ppCmdLists);

	hr = GlobDrx::cmdQueue->Signal(GlobDrx::fence[GlobDrx::frameIndex], GlobDrx::fenceValue[GlobDrx::frameIndex]);
	if (FAILED(hr)) GlobDrx::isRunning = false;

	hr = GlobDrx::swapChain->Present(0, 0);
	if (FAILED(hr)) GlobDrx::isRunning = false;
}

void cleanupDrx() {
	// Need to wait for gpu to finish all frames
	for (int f = 0; f < GlobDrx::frameBuffCount; f++) {
		GlobDrx::frameIndex = f;
		waitPrevFrame();
	}

	BOOL isFullscreen = false;
	if (GlobDrx::swapChain->GetFullscreenState(&isFullscreen, NULL))
		GlobDrx::swapChain->SetFullscreenState(false, NULL);

	if (GlobDrx::device) { GlobDrx::device->Release(); GlobDrx::device = NULL; }
	if (GlobDrx::swapChain) { GlobDrx::swapChain->Release(); GlobDrx::swapChain = NULL; }
	if (GlobDrx::cmdQueue) { GlobDrx::cmdQueue->Release(); GlobDrx::cmdQueue = NULL; }
	if (GlobDrx::rtvDescriptHeap) { GlobDrx::rtvDescriptHeap->Release(); GlobDrx::rtvDescriptHeap = NULL; }
	if (GlobDrx::cmdGList) { GlobDrx::cmdGList->Release(); GlobDrx::cmdGList = NULL; }

	for (int f = 0; f < GlobDrx::frameBuffCount; f++) {
		if (GlobDrx::renderTargets[f]) { GlobDrx::renderTargets[f]->Release(); GlobDrx::renderTargets[f] = NULL; }
		if (GlobDrx::cmdAllocator[f]) { GlobDrx::cmdAllocator[f]->Release(); GlobDrx::cmdAllocator[f] = NULL; }
		if (GlobDrx::fence[f]) { GlobDrx::fence[f]->Release(); GlobDrx::fence[f] = NULL; }
	}
}

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
	}
	case (WM_PAINT): {
	}
	case (WM_ERASEBKGND):
		return TRUE;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

int main(int argc, char** argv) {

	WNDCLASS wndClass = { 0 };
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.lpfnWndProc = wndProc;
	wndClass.lpszClassName = "Default Class";
	RegisterClass(&wndClass);

	HWND wndWindow = CreateWindow(
		"Default Class",
		"Some DRX Rendering",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
		NULL, NULL, GetModuleHandle(NULL), NULL
	);

	ShowWindow(wndWindow, 1);
	UpdateWindow(wndWindow);

	int initStatus = initDrx(wndWindow);

	for (int f = 0; f < 99999; f++) {
		renderDrx();
	}

	waitPrevFrame();
	CloseHandle(GlobDrx::fenceEvent);
	cleanupDrx();

	return 0;
}