#include "stdafx.h"
#include "HeapManager.h"
#include "Engine.h"

void HeapManager::Init()
{
	ID3D12Device* device = Engine::GetEngine()->GetRender()->GetDevice();

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 1000;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(device->CreateDescriptorHeap(&cbvHeapDesc,
		IID_PPV_ARGS(&CbvSrvUavHeap)));

	D3D12_DESCRIPTOR_HEAP_DESC RtvHeapDesc;
	RtvHeapDesc.NumDescriptors = 1000;
	RtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	RtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	RtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(device->CreateDescriptorHeap(&RtvHeapDesc,
		IID_PPV_ARGS(&RtvHeap)));

	D3D12_DESCRIPTOR_HEAP_DESC DsvHeapDesc;
	DsvHeapDesc.NumDescriptors = 1000;
	DsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	DsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(device->CreateDescriptorHeap(&DsvHeapDesc,
		IID_PPV_ARGS(&DsvHeap)));

	D3D12_DESCRIPTOR_HEAP_DESC SamHeapDesc;
	SamHeapDesc.NumDescriptors = 1000;
	SamHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	SamHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	SamHeapDesc.NodeMask = 0;
	ThrowIfFailed(device->CreateDescriptorHeap(&SamHeapDesc,
		IID_PPV_ARGS(&SampleHeap)));
}
