#pragma once
#include "Mesh.h"
#include "CommonBuffer.h"

class Primitive
{
public:
	Primitive() {};
	virtual ~Primitive() { delete HeapAndCB; delete MeshBuffer; };
	Transform& GetTransform() { return Trans; }
	Mesh* GetMesh() { return MeshBuffer; }
	CommonBuffer* GetCommon() { return HeapAndCB; }
	void SetMesh(Mesh* m) { MeshBuffer = m; }
	void SetCommon(CommonBuffer* c) { HeapAndCB = c; }
	std::string MeshName;

	Mesh* MeshBuffer;

	glm::mat4 WorldTrans = glm::mat4(1.0f);
	glm::mat4 Scale3DTrans = glm::mat4(1.0f);
	glm::mat4 RotateTrans = glm::mat4(1.0f);
	glm::mat4 MVP = glm::mat4(1.0f);
private:
	CommonBuffer* HeapAndCB;
	Transform Trans;
	
};

