#pragma once
#include "Texture.h"

class Mesh
{
public:
	Mesh() {}
	Mesh(Mesh* m)
	{
		MeshTex = m->MeshTex;
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
	virtual ~Mesh() {}

	UINT32 GetVertexByteStride() const { return VertexByteSize; }
	UINT32 GetVertexCount() const { return VertexCount; }
	UINT32 GetIndexCount() const { return IndexCount; }
	UINT32 GetVertexBufferByteSize() const { return VertexBufferByteSize; }
	UINT32 GetIndexBufferByteSize() const { return IndexBufferByteSize; }
	UINT32 GetBufferSize() const { return BufferSize; }

	std::shared_ptr<Texture> MeshTex;

	int NumLod = 0;
	int NumVertices = 0;
	int NumTriangles = 0;
	int NumIndices = 0;
	std::string MeshName = "";

	std::vector<glm::vec3> VertexInfo;
	std::vector<glm::vec4> color;
	std::vector<glm::vec4> Normal;
	std::vector<glm::vec2> TexCoord;
	std::vector<int> IndexVector;


	UINT32 VertexByteSize = 0;
	UINT32 VertexBufferByteSize = 0;
	UINT32 IndexBufferByteSize = 0;
	UINT32 BufferSize;

	UINT32 VertexCount = 0;
	UINT32 IndexCount = 0;

};

