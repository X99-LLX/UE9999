#pragma once
#include "Primitive.h"
#include "UpLoadBuffer.h"
class DX12Primitive : public Primitive
{
public:
	DX12Primitive() {}
	DX12Primitive(Primitive* p) : Primitive(p) {}
	virtual ~DX12Primitive() {}

	UploadBuffer<ConstantBuffer>* GetCB() { return CB.get(); }
	void SetCB(std::shared_ptr<UploadBuffer<ConstantBuffer>> c) { CB = c; }
protected:
	std::shared_ptr<UploadBuffer<ConstantBuffer>> CB = nullptr;
};

