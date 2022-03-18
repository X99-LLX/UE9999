#pragma once
#include "GPUTexture.h"

class GPUMesh
{
public:
	~GPUMesh() {};
	std::string MeshName;
	int NumLod;
	int NumVertices;
	int NumTriangles;
	int NumIndices;

	std::vector<int> indices;
	std::vector<glm::vec3> VertexInfo;
	std::vector<glm::vec4> color;
	std::vector<glm::vec4> Normal;
	std::vector<glm::vec2> TexCoord;

	std::shared_ptr<GPUTexture> Tex = nullptr;
};

