#pragma once
#include "CommonBuffer.h"
#include "UpLoadBuffer.h"

class DX12CommonBuffer :
    public CommonBuffer
{
public:
	DX12CommonBuffer() {}
	virtual ~DX12CommonBuffer() {}

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetHeap() { return CbvSrvUavHeap; }
	std::shared_ptr<UploadBuffer<ConstantBuffer>> GetCB() { return CB; }


	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CbvSrvUavHeap = nullptr;
	std::shared_ptr<UploadBuffer<ConstantBuffer>> CB = nullptr;
};

