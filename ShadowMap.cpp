#include "stdafx.h"
#include "ShadowMap.h"


void ShadowMap::Init(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE Hsrv, D3D12_CPU_DESCRIPTOR_HANDLE Hdsv)
{
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = VIEW_SIZE;
	depthStencilDesc.Height = VIEW_SIZE;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;

	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;                 //����������Ż���ֵ
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth = 1.0f;         //��������Ϊ��Զ
	optClear.DepthStencil.Stencil = 0;

	ThrowIfFailed(device->CreateCommittedResource(          //ͬʱ����һ����Դ��һ����ʽ�ѣ�ʹ�ö��㹻���԰���������Դ��������Դ��ӳ�䵽�ѡ�
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&optClear,
		IID_PPV_ARGS(mShadowMap.GetAddressOf())));

	
	CreateSRVAndDSV(device,Hsrv,Hdsv);
}


void ShadowMap::CreateSRVAndDSV(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE Hsrv, D3D12_CPU_DESCRIPTOR_HANDLE Hdsv)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srv = {};
	srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MostDetailedMip = 0;
	srv.Texture2D.MipLevels = 1;
	srv.Texture2D.ResourceMinLODClamp = 0.0f;
	srv.Texture2D.PlaneSlice = 0;
	device->CreateShaderResourceView(mShadowMap.Get(), &srv, Hsrv);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsv;
	dsv.Flags = D3D12_DSV_FLAG_NONE;
	dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsv.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(mShadowMap.Get(), &dsv, Hdsv);
}

void ShadowMap::SetOffset(int s, int d)
{
	srvOffset = s;
	dsvOffset = d;
}
