#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// Windows runtime library COM-Specific items
#include <wrl.h>
using namespace Microsoft::WRL;

#include "d3dx12.h" // Helper that also includes d3d12.h
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

typedef enum {
    DRX12_Init_Good = 1,
    DRX12_Init_Fail = -1,
    DRX12_Init_Fail_CreateFactory = -2,
    DRX12_Init_Fail_NoAdapter = -3,
    DRX12_Init_Fail_CreateDevice = -4,
    DRX12_Init_Fail_CreateCmdQueue = -5,
    DRX12_Init_Fail_CreateDescHeap = -6,
    DRX12_Init_Fail_SwpChainGetBuffer = -7,
    DRX12_Init_Fail_CreateCmdAllocator = -8,
    DRX12_Init_Fail_CreateCmdList = -9,
    DRX12_Init_Fail_CreateFence = -10,
    DRX12_Init_Fail_CreateFenceEvent = -11
} DRX12_INIT_STATUS;

class Topl_Drx12 {
private:
    const int m_frameBuffCount = 3; // For triple buffering
	ID3D12Device* m_device;
	IDXGISwapChain3* m_swapChian; // For switching between render targets
	ID3D12CommandQueue* m_cmdQueue; // contatiner of command list
	ID3D12DescriptorHeap* m_rtvDescriptHeap; // descriptor heap to hold resources
	ID3D12Resource* m_renderTargets[frameBuffCount]; // Render targets
	ID3D12CommandAllocator* m_cmdAllocator[frameBuffCount]; // Command allocator for each frame
	ID3D12GraphicsCommandList* m_cmdGList; // Command list to record and render onto frame
	
	ID3D12Fence* m_fence[frameBuffCount]; // For each allocator
	HANDLE m_fenceEvent; // Event when handle is unlocked by GPU
	UINT64 m_fenceValue[frameBuffCount]; // Incremented each frame
	
	int m_frameIndex; // current rtv we are on
	int m_rtvDescriptorSize; // Size of rtv descriptor on the device (front/back buffers will be same size)

    DRX12_INIT_STATUS initStatus;
public:
    Topl_Drx12(void){ initStatus = init(HWND hwnd); }

    DRX12_INIT_STATUS init(void);
    void print(void);
};