#include "stdafx.h"
#include "DescHeapManager.h"

void DescHeapManager::InitHeapMng(ID3D12Device* mDevice)
{
	D3D12_DESCRIPTOR_HEAP_DESC HeapDesc;
	HeapDesc.NumDescriptors = Num_DESC;
	HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	HeapDesc.NodeMask = 0;
	ThrowIfFailed(mDevice->CreateDescriptorHeap(
		&HeapDesc, IID_PPV_ARGS(mHeaps[0].GetAddressOf())));

	HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(mDevice->CreateDescriptorHeap(
		&HeapDesc, IID_PPV_ARGS(mHeaps[1].GetAddressOf())));

	HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(mDevice->CreateDescriptorHeap(
		&HeapDesc, IID_PPV_ARGS(mHeaps[2].GetAddressOf())));

	HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(mDevice->CreateDescriptorHeap(
		&HeapDesc, IID_PPV_ARGS(mHeaps[3].GetAddressOf())));


	for (int Index = 0; Index < Num_Heap; Index++)
	{
		mDescNum[Index] = 0;
		mDescSize[Index] = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE(Index));
	}
}

ID3D12DescriptorHeap* DescHeapManager::GetHeap(HeapType ht)
{
	return mHeaps[(int)ht].Get();
}


D3D12_CPU_DESCRIPTOR_HANDLE DescHeapManager::GetHeapCPUHandlePos(HeapType ht, int offset)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = mHeaps[(int)ht]->GetCPUDescriptorHandleForHeapStart();
	handle.ptr = handle.ptr + mDescSize[(int)ht] * offset;
	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescHeapManager::GetHeapGPUHandlePos(HeapType ht, int offset)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = mHeaps[(int)ht]->GetGPUDescriptorHandleForHeapStart();
	handle.ptr = handle.ptr + mDescSize[(int)ht] * offset;
	return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescHeapManager::GetCPUCurrentHandleStart(HeapType ht)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = mHeaps[(int)ht]->GetCPUDescriptorHandleForHeapStart();
	handle.ptr = handle.ptr + mDescSize[(int)ht] * mDescNum[(int)ht];
	AddDesc(ht);
	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescHeapManager::GetGPUCurrentHandleStart(HeapType ht)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = mHeaps[(int)ht]->GetGPUDescriptorHandleForHeapStart();
	handle.ptr = handle.ptr + mDescSize[(int)ht] * mDescNum[(int)ht];
	return handle;
}

void DescHeapManager::AddDesc(HeapType ht)
{
	mDescNum[(int)ht] += 1;
}

void DescHeapManager::SubDesc(HeapType ht)
{
	mDescNum[(int)ht] -= 1;
}

int DescHeapManager::GetDescNum(HeapType ht)
{
	return mDescNum[(int)ht];
}

int DescHeapManager::GetDescSize(HeapType ht)
{
	return mDescSize[int(ht)];
}
