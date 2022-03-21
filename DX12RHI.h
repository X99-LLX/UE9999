#pragma once
#include "Win32Window.h"
#include "d3dUtil.h"
#include "GameTimer.h"
#include "Scene.h"
#include "RHI.h"


class DX12RHI : public RHI
{
public:
	DX12RHI();
	~DX12RHI();

	bool Init();
	void Update(Scene* mScene, Primitive* actor);
	void DrawCall(Primitive* actor);
	void BuildGeo(Primitive* actor);
	void CreateRenderItem(Primitive* actor);

	void OnResize();
	ID3D12Resource* CurrentBackBuffer()const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;
	ID3D12Device* GetDevice();
	ID3D12GraphicsCommandList* GetCmdList();
	void OpenCmdList();
	void CloseCmdList();

	void ResetViewportsAndScissorRects();
	void ClearRTVAndDSV();
	void SetRTVAndDSV();
	void SetRootSignature();
	void Swapchain();
	void DrawInstance(Primitive* actor);

	void OpenRtv();
	void CloseRtv();

protected:
	
	void SetCommonBuffer(Primitive* actor);

	void BuildTexture(Primitive* actor);
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildPSO();
	void CreateRtvAndDsvDescriptorHeap();
	void CreateSwapChain();
	void CreateCommandObjects();
	void FlushCommandQueue();


	static const int SwapChainBufferCount = 2;
	int mCurrentBackBuffer = 0;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mvsByteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mpsByteCode = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

	Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;
	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
	UINT64 mCurrentFence = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
	
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
	
	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;

	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;

	bool m4xMsaaState = false;
	UINT m4xMsaaQuality = 0;


};

