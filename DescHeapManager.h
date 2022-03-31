#pragma once
#define Num_Heap 4
#define Num_DESC 2048
/*D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV*/
#include "Singleton.h"
enum class HeapType
{
	CBV_SRV_UAV = 0,
	SAMPLER = 1,
	RTV = 2,
	DSV = 3,
};



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

