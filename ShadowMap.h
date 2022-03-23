#pragma once
class ShadowMap
{
public:
	ShadowMap() {}
	~ShadowMap() {}
	void Init(ID3D12Device* device);
	void CreateHeap(ID3D12Device* device);
	void CreateSRVAndDSV(ID3D12Device* device);





	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DsvHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SrvHeap = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> mShadowMap = nullptr;

	D3D12_DEPTH_STENCIL_VIEW_DESC mDsv;
	D3D12_SHADER_RESOURCE_VIEW_DESC mSrv;

	D3D12_VIEWPORT mViewport = { 0.0f, 0.0f, (float)960, (float)540, 0.0f, 1.0f };
	D3D12_RECT mScissorRect = { 0, 0, 960, 540 };
};

