#pragma once
#include "Mesh.h"
class DX12Mesh : public Mesh
{
public:
	DX12Mesh() {}
	DX12Mesh(Mesh* m) : Mesh(m) {
	
	}
	~DX12Mesh() {};

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> BufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexUploader = nullptr;

	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R32_UINT;
	D3D_PRIMITIVE_TOPOLOGY mTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	
};

