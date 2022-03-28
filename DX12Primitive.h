#pragma once
#include "Primitive.h"
#include "UpLoadBuffer.h"
class DX12Primitive : public Primitive
{
public:
	DX12Primitive() {}
	virtual ~DX12Primitive() {}

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetHeap() { return CbvSrvUavHeap; }
	UploadBuffer<ConstantBuffer>* GetCB() { return CB.get(); }

public:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CbvSrvUavHeap = nullptr;
	std::shared_ptr<UploadBuffer<ConstantBuffer>> CB = nullptr;
};

