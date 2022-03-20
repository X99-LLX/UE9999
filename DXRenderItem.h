#pragma once
#include "DXMesh.h"
#include "UpLoadBuffer.h"
#include "RenderItem.h"

class DXRenderItem : public RenderItem 
{
public:
	DXRenderItem();
	DXRenderItem(RenderItem* ri) 
	{
		Trans = ri->Trans;
		AssetName = ri->AssetName;
	}
	virtual ~DXRenderItem();


	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CbvSrvUavHeap = nullptr;

	std::shared_ptr<DXMesh> Asset = nullptr;

	std::unique_ptr<UploadBuffer<ConstantBuffer>> CB = nullptr;


};

