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

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CbvSrvUavHeap = nullptr;
	std::unique_ptr<UploadBuffer<ConstantBuffer>> CB = nullptr;
};

