#pragma once
#include "Material.h"
#include "UpLoadBuffer.h"

class DX12Material :
    public Material
{
public:
    DX12Material() {}
    DX12Material(Material* m) : Material(m) {}
    ~DX12Material() {}

	UploadBuffer<MatData>* GetCB() { return CB.get(); }
	void SetCB(std::shared_ptr<UploadBuffer<MatData>> c) { CB = c; }
protected:
    std::shared_ptr<UploadBuffer<MatData>> CB = nullptr;
};

