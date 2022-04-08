#pragma once
#include "Texture.h"
class DX12Texture : public Texture
{
public:
	DX12Texture() {}
	DX12Texture(Texture* t) : Texture(t) {}
	virtual ~DX12Texture() {}

	void SetViewOffset(UINT32 Offset) { DescOffset = Offset; }
	int GetViewOffset() { return DescOffset; }
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetResource() { return Resource; }
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetUploader() { return Uploader; }
protected:
	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> Uploader = nullptr;
	UINT32 DescOffset = 0;
};

