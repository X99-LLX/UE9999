#pragma once

class HeapManager
{
public:
	void Init();


private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CbvSrvUavHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RtvHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DsvHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SampleHeap = nullptr;
};

