#include "stdafx.h"
#include "ShadowMap.h"


void ShadowMap::Init(ID3D12Device* device)
{
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = 2000;
	depthStencilDesc.Height = 2000;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;

	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;                 //清除操作最优化的值
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth = 1.0f;         //距离设置为最远
	optClear.DepthStencil.Stencil = 0;

	ThrowIfFailed(device->CreateCommittedResource(          //同时创建一个资源和一个隐式堆，使得堆足够大以包含整个资源，并且资源被映射到堆。
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&optClear,
		IID_PPV_ARGS(mShadowMap.GetAddressOf())));

	CreateHeap(device);
	CreateSRVAndDSV(device);
}

void ShadowMap::CreateHeap(ID3D12Device* device)
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(device->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(DsvHeap.GetAddressOf())));

	D3D12_DESCRIPTOR_HEAP_DESC SrvHeapDesc;
	SrvHeapDesc.NumDescriptors = 1;
	SrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	SrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	SrvHeapDesc.NodeMask = 0;
	ThrowIfFailed(device->CreateDescriptorHeap(
		&SrvHeapDesc, IID_PPV_ARGS(SrvHeap.GetAddressOf())));
}

void ShadowMap::CreateSRVAndDSV(ID3D12Device* device)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srv = {};
	srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MostDetailedMip = 0;
	srv.Texture2D.MipLevels = 1;
	srv.Texture2D.ResourceMinLODClamp = 0.0f;
	srv.Texture2D.PlaneSlice = 0;
	device->CreateShaderResourceView(mShadowMap.Get(), &srv, SrvHeap->GetCPUDescriptorHandleForHeapStart());
	mSrv = srv;

	D3D12_DEPTH_STENCIL_VIEW_DESC dsv;
	dsv.Flags = D3D12_DSV_FLAG_NONE;
	dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsv.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(mShadowMap.Get(), &dsv, DsvHeap->GetCPUDescriptorHandleForHeapStart());
	mDsv = dsv;
}
