#pragma once
#include "Mesh.h"

class Primitive
{
public:
	Primitive() {};
	virtual ~Primitive() {}
	Transform& GetTransform() { return Trans; }
	Mesh* GetMesh() { return MeshBuffer.get(); }

	std::string MeshName;

	std::shared_ptr<Mesh> MeshBuffer;

	glm::mat4 WorldTrans = glm::mat4(1.0f);
	glm::mat4 Scale3DTrans = glm::mat4(1.0f);
	glm::mat4 RotateTrans = glm::mat4(1.0f);
	glm::mat4 MVP = glm::mat4(1.0f);

	Transform Trans;
	
};

