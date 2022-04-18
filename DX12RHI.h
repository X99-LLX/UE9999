#pragma once
#include "Win32Window.h"
#include "d3dUtil.h"
#include "GameTimer.h"
#include "Scene.h"
#include "RHI.h"
#include "ShadowMap.h"
#include "UpLoadBuffer.h"
#include "DescHeapManager.h"
#include "RenderTarget.h"

class DX12RHI : public RHI
{
public:
	DX12RHI();
	~DX12RHI();

	bool Init();



	void OnResize();
	ID3D12Resource* CurrentBackBuffer();
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView();
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView();
	ID3D12Device* GetDevice();
	ID3D12GraphicsCommandList* GetCmdList();
	void OpenCmdList();
	void CloseCmdList();

	void ResetViewportsAndScissorRects(RenderTarget* rt);
	void SetRTVAndDSV(RenderTarget* rt);


	void SetRootSignature(Shader* s);

	void ChangeResState();
	void ChangeResState(BufferState CurrnetState, BufferState ChangeState, RenderTarget* rt, RTBufferType type);

	void ClearRenderTarget(RenderTarget* rt);
	void ClearDepthStencil(RenderTarget* rt);
	//some test
	void InputAssetInfo(Mesh* mesh);
	void SetDescHeap(HeapType ht);
	void BeginFrame();
	void EndFrame();
	void BindDataTable(UINT32 Slot, UINT32 HandleOffset, HeapType ht);
	void BindDataConstantBuffer(UINT32 Slot, UINT32 Address);
	void Bind32BitConstants(UINT32 Slot, UINT32 num, const void* data, UINT32 offset);
	void DrawMesh(UINT32 IndexCount);
	void EventBegin(std::string EventName);
	void EventEnd();

	std::shared_ptr<Texture> CreateTexture(Texture* t);
	std::shared_ptr<Mesh> CreateMesh(std::shared_ptr<Mesh> m);
	std::shared_ptr<Pipeline> CreatePipeline(Pipeline* p);
	std::shared_ptr<Shader> CreateShader(Shader* s);
	std::shared_ptr<Primitive> CreatePrimitive(Primitive* p);
	std::shared_ptr<Material> CreateMaterial(Material* m);
	std::shared_ptr<Light> CreateLight();
	std::shared_ptr<RenderTarget> CreateRenderTarget(std::string name, UINT32 width, UINT32 height, int num, RtType type);
	void UpdateRenderTarget(std::shared_ptr<RenderTarget> rt, ColorFormat format);

	void SetPSO(Pipeline* pl);

	void ReSetCmdAlloc();
protected:
	
	void BuildInputLayout();
	void CreateSwapChain();
	void CreateCommandObjects();
	void FlushCommandQueue();


	static const int SwapChainBufferCount = 2;
	int mCurrentBackBuffer = 0;

	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	UINT32 mOffsetMap[SwapChainBufferCount];

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
	//std::unique_ptr<ShadowMap> mShadowMap = std::make_unique<ShadowMap>();
	DescHeapManager mHeapMng;
	int TEST_dsvoffset = 0;
};

