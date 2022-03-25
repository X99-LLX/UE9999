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
	std::shared_ptr<UploadBuffer<ConstantBuffer>> GetCB1() { return CB1; }

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CbvSrvUavHeap = nullptr;
	std::shared_ptr<UploadBuffer<ConstantBuffer>> CB = nullptr;
	std::shared_ptr<UploadBuffer<ConstantBuffer>> CB1 = nullptr;
};

