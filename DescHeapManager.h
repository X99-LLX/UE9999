#pragma once
#define Num_Heap 4
#define Num_DESC 2048

#include "Singleton.h"




class DescHeapManager :public Singleton<DescHeapManager>
{
public:
	void InitHeapMng(ID3D12Device* mDevice);
	ID3D12DescriptorHeap* GetHeap(HeapType ht);

	D3D12_CPU_DESCRIPTOR_HANDLE GetHeapCPUHandlePos(HeapType ht, int offset);
	D3D12_GPU_DESCRIPTOR_HANDLE GetHeapGPUHandlePos(HeapType ht, int offset);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCurrentHandleStart(HeapType ht);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCurrentHandleStart(HeapType ht);
	void AddDesc(HeapType ht);
	void SubDesc(HeapType ht);
	int GetDescNum(HeapType ht);

private:
	int GetDescSize(HeapType ht);

private:

	std::array<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>,Num_Heap> mHeaps;
	std::array<UINT, Num_Heap> mDescNum;
	std::array<UINT, Num_Heap> mDescSize;
	std::array<UINT, Num_Heap> mCurrentOffset;
};

