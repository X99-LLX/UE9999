#include "stdafx.h"
#include "DX12RenderTarget.h"

void DX12RenderTarget::SetRTBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> ryb)
{
	mRTBuffers = ryb;
}

void DX12RenderTarget::SetDSBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> dsb)
{
	mDSBuffer = dsb;
}

void DX12RenderTarget::AddOffset(std::string name, int offset)
{
	OffsetMap.insert({ name,offset });
}

UINT32 DX12RenderTarget::GetOffset(std::string name)
{
	if (OffsetMap.find(name) != OffsetMap.end())
	{
		return OffsetMap.at(name);
	}
	return NULL;
}

Microsoft::WRL::ComPtr<ID3D12Resource>& DX12RenderTarget::GetBuffer(RTBufferType type)
{
	if (type == RTBufferType::ColorBuffer)
	{
		return mRTBuffers;
	}
	else
	{
		return mDSBuffer;
	}
}
