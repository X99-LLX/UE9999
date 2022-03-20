#pragma once
#include "Mesh.h"
class DX12Mesh : public Mesh
{
public:
	DX12Mesh() {}
	~DX12Mesh() {};
	DX12Mesh(Mesh* m)
	{
		NumLod = m->NumLod;
		NumVertices = m->NumVertices;
		NumTriangles = m->NumTriangles;
		NumIndices = m->NumIndices;
		MeshName = m->MeshName;

		VertexInfo = m->VertexInfo;
		color = m->color;
		Normal = m->Normal;
		TexCoord = m->TexCoord;

		IndexVector = m->IndexVector;


		VertexByteSize = m->VertexByteSize;
		VertexBufferByteSize = m->VertexBufferByteSize;
		IndexBufferByteSize = m->IndexBufferByteSize;
		BufferSize = m->BufferSize;

		VertexCount = m->VertexCount;
		IndexCount = m->IndexCount;
	}

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView;

	Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
};

