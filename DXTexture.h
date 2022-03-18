#pragma once
#include "GPUTexture.h"

class DXTexture : public GPUTexture
{
public:
	DXTexture() {}
	DXTexture(GPUTexture* tex)
	{
		Name = tex->Name;
		Filename = tex->Filename;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
};

