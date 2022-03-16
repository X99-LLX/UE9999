#include "stdafx.h"
#include "Actor.h"
#include "Engine.h"
#include "ResourceManage.h"

Actor::Actor()
{

}

Actor::~Actor()
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

void Actor::CreateDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	cbvHeapDesc.NumDescriptors = 1000;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ID3D12Device* device = Engine::GetEngine()->GetRender()->GetDevice();
	ThrowIfFailed(device->CreateDescriptorHeap(&cbvHeapDesc,
		IID_PPV_ARGS(&CbvSrvUavHeap)));
}

void Actor::CreateSRV()
{
	ID3D12Device* device = Engine::GetEngine()->GetRender()->GetDevice();
	CB = std::make_unique<UploadBuffer<ConstantBuffer>>(device, 1, true);
	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = CB->Resource()->GetGPUVirtualAddress();


	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(CbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart());

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	ResourceManage* Res = Engine::GetEngine()->GetAssetMgr();
	srvDesc.Format = Res->GetTexture("bricks3")->Resource->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = Res->GetTexture("bricks3")->Resource->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;


	device->CreateShaderResourceView(Res->GetTexture("bricks3")->Resource.Get(), &srvDesc, hDescriptor);
	 
	hDescriptor.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	device->CreateShaderResourceView(Res->GetTexture("bricks3")->Resource.Get(), &srvDesc, hDescriptor);
}
