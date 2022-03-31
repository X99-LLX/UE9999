#pragma once
#include "Primitive.h"
#include "UpLoadBuffer.h"
class DX12Primitive : public Primitive
{
public:
	DX12Primitive() {}
	virtual ~DX12Primitive() {}

	UploadBuffer<ConstantBuffer>* GetCB() { return CB.get(); }

public:
	std::shared_ptr<UploadBuffer<ConstantBuffer>> CB = nullptr;
};

