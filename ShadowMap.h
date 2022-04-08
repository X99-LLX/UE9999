#pragma once
#define VIEW_SIZE 2048

class ShadowMap
{
public:
	ShadowMap() {}
	~ShadowMap() {}
	void Init(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE Hsrv, D3D12_CPU_DESCRIPTOR_HANDLE Hdsv);
	void CreateSRVAndDSV(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE Hsrv, D3D12_CPU_DESCRIPTOR_HANDLE Hdsv);
	void SetOffset(int s, int d);

	Microsoft::WRL::ComPtr<ID3D12Resource> mShadowMap = nullptr;

	D3D12_VIEWPORT mViewport = { 0.0f, 0.0f, (float)VIEW_SIZE, (float)VIEW_SIZE, 0.0f, 1.0f };
	D3D12_RECT mScissorRect = { 0, 0, VIEW_SIZE, VIEW_SIZE };

	int srvOffset = 0;
	int dsvOffset = 0;
};

