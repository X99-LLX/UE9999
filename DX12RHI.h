#pragma once
#include "Win32Window.h"
#include "d3dUtil.h"
#include "GameTimer.h"
#include "Scene.h"
#include "RHI.h"
#include "ShadowMap.h"
#include "UpLoadBuffer.h"
#include "DescHeapManager.h"

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

	void ResetViewportsAndScissorRects(RtType rt);
	void SetRTVAndDSV(RtType rt);


	void SetRootSignature(Shader* s);

	void ChangeResState();
	void CloseRtv();

	void BeginDrawShadow();

	//some test
	void InputAssetInfo(Mesh* mesh);
	void SetDescHeap(HeapType ht);
	void BeginFrame();
	void EndFrame();
	void BindDataTable(UINT32 Slot, UINT32 HandleOffset, HeapType ht);
	//void BindDataTable(UINT32 Slot, UINT32 HandleOffset, HeapType ht);
	void BindDataConstantBuffer(UINT32 Slot, UINT32 Address);
	void Bind32BitConstants(UINT32 Slot, UINT32 num, const void* data, UINT32 offset);
	void DrawMesh(UINT32 IndexCount);

	std::shared_ptr<Texture> CreateTexture(Texture* t);
	std::shared_ptr<Mesh> CreateMesh(Mesh* m);
	std::shared_ptr<Pipeline> CreatePipeline(Pipeline* p);
	std::shared_ptr<Shader> CreateShader(Shader* s);
	std::shared_ptr<Primitive> CreatePrimitive(Primitive* p);
	std::shared_ptr<Material> CreateMaterial(Material* m);
	std::shared_ptr<Light> CreateLight();
	void SetPSO(Pipeline* pl);

	void ReSetCmdAlloc();
protected:
	
	void BuildInputLayout();
	void BuildPSO();
	void CreateSwapChain();
	void CreateCommandObjects();
	void FlushCommandQueue();


	static const int SwapChainBufferCount = 2;
	int mCurrentBackBuffer = 0;

	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	
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

	Microsoft::WRL::ComPtr<ID3D12PipelineState> mCommonPSO = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mShadowPSO = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> mCommonVS = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mCommonPS = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mShadowVS = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mShadowPS = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> mCommonRS = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mShadowRS = nullptr;


	std::unique_ptr<ShadowMap> mShadowMap = std::make_unique<ShadowMap>();


	glm::mat4 mLightView = glm::mat4(1.0f);
	glm::mat4 mLightProj = glm::mat4(1.0f);

	DescHeapManager mHeapMng;

	int TEST_dsvoffset = 0;
};

