#pragma once
#include "Texture.h"
class DX12Texture : public Texture
{
public:
	DX12Texture() {}
	DX12Texture(std::string TextureName, std::wstring FileName) : Texture(TextureName, FileName)
	{
		
	}
	virtual ~DX12Texture() {}
	
	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;

	int DescOffset = 0;
};

