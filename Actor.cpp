#include "stdafx.h"
#include "Actor.h"

Actor::Actor()
{

}

void Actor::ChangePosition(glm::vec3 Pos)
{
	Trans.Translation = Pos;
}

void Actor::ChangeRotation(glm::vec4 Rot)
{
	Trans.Rotation = Rot;
}

void Actor::ChangeScale3D(glm::vec3 Sca)
{
	Trans.Scale3D = Sca;
}

void Actor::CreateCbvHeap(ID3D12Device* device)
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(device->CreateDescriptorHeap(&cbvHeapDesc,
		IID_PPV_ARGS(&CbvHeap)));
}

void Actor::CreateConstantBuffer(ID3D12Device* device)
{
	CB = std::make_unique<UploadBuffer<ConstantBuffer>>(device, 1, true);
	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = CB->Resource()->GetGPUVirtualAddress();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ConstantBuffer));

	device->CreateConstantBufferView(
		&cbvDesc,
		CbvHeap->GetCPUDescriptorHandleForHeapStart());
}
