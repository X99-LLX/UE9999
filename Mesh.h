#pragma once
#include "Texture.h"

class Mesh
{
public:
	Mesh() {}
	virtual ~Mesh() { delete MeshTex; }

	UINT32 GetVertexByteStride() const { return VertexByteSize; }
	UINT32 GetVertexCount() const { return VertexCount; }
	UINT32 GetIndexCount() const { return IndexCount; }
	UINT32 GetVertexBufferByteSize() const { return VertexBufferByteSize; }
	UINT32 GetIndexBufferByteSize() const { return IndexBufferByteSize; }
	UINT32 GetBufferSize() const { return BufferSize; }
	void SetTexture(Texture* t) { MeshTex = t; }

	Texture* MeshTex;

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

