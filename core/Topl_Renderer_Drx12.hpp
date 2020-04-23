#include "native_os_def.h"

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
    static const int m_frameBuffCount = 3; // For triple buffering
    IDXGIFactory4* m_factory;
	ID3D12Device* m_device;
	IDXGISwapChain3* m_swapChain; // For switching between render targets
	ID3D12CommandQueue* m_cmdQueue; // contatiner of command list
	ID3D12DescriptorHeap* m_rtvDescriptHeap; // descriptor heap to hold resources
	ID3D12Resource* m_renderTargets[m_frameBuffCount]; // Render targets
	ID3D12CommandAllocator* m_cmdAllocator[m_frameBuffCount]; // Command allocator for each frame
	ID3D12GraphicsCommandList* m_cmdGList; // Command list to record and render onto frame
	
	ID3D12Fence* m_fence[m_frameBuffCount]; // For each allocator
	HANDLE m_fenceEvent; // Event when handle is unlocked by GPU
	UINT64 m_fenceValue[m_frameBuffCount]; // Incremented each frame
	
	int m_frameIndex; // current rtv we are on
	int m_rtvDescriptorSize; // Size of rtv descriptor on the device (front/back buffers will be same size)

    // Some newer additions
    ID3D12PipelineState* pso; // Pipeline state object
	ID3D12RootSignature* rootSig;
	D3D12_VIEWPORT viewPort;
	D3D12_RECT scissorRect;
	ID3D12Resource* vertexBuff;
	D3D12_VERTEX_BUFFER_VIEW vertexBuffView;

    DRX12_INIT_STATUS initStatus;
    NATIVE_WINDOW m_hwnd;
public:
    Topl_Drx12(NATIVE_WINDOW hwnd){ initStatus = init(hwnd); }

    DRX12_INIT_STATUS init(NATIVE_WINDOW hwnd);
    void print(void);
};