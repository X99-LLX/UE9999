//#pragma once
//#include "GPUMesh.h"
//#include "DXTexture.h"
//
//class DXMesh : public GPUMesh
//{
//public:
//	DXMesh() {};
//	/*DXMesh(GPUMesh* mesh)
//	{
//		MeshName = mesh->MeshName;
//		NumLod = mesh->NumLod;
//		NumVertices = mesh->NumVertices;
//		NumTriangles = mesh->NumTriangles;
//		NumIndices = mesh->NumIndices;
//		indices = mesh->indices;
//		VertexInfo = mesh->VertexInfo;
//		color = mesh->color;
//		Normal = mesh->Normal;
//		TexCoord = mesh->TexCoord;
//	}*/
//
//	virtual ~DXMesh();
//
//	std::shared_ptr<DXTexture> Tex = nullptr;
//
//	Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
//	Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;
//
//	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
//	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;
//
//	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
//	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;
//
//	UINT VertexByteSize = 0;
//	UINT VertexBufferByteSize = 0;
//	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
//	UINT IndexBufferByteSize = 0;
//	UINT IndexCount = 0;
//
//	D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
//	{
//		D3D12_VERTEX_BUFFER_VIEW vbv;
//		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
//		vbv.StrideInBytes = VertexByteSize;
//		vbv.SizeInBytes = VertexBufferByteSize;
//		return vbv;
//	}
//
//	D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
//	{
//		D3D12_INDEX_BUFFER_VIEW ibv;
//		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
//		ibv.Format = IndexFormat;
//		ibv.SizeInBytes = IndexBufferByteSize;
//
//		return ibv;
//	}
//};
//
