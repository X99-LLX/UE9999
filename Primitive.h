#pragma once
#include "Mesh.h"

class Primitive
{
public:
	Primitive() {};
	Primitive(Primitive* p)
	{
		MeshName = p->GetMeshName();
		Trans = p->GetTransform();
	}
	virtual ~Primitive() {}

	void SetWorldTrans(glm::mat4 wt) { WorldTrans = wt; }
	void SetMeshName(std::string n) { MeshName = n; }

	glm::mat4 GetWorldTrans() { return WorldTrans; }
	Transform& GetTransform() { return Trans; }
	std::string GetMeshName() { return MeshName; }
	
protected:
	glm::mat4 WorldTrans = glm::mat4(1.0f);
	std::string MeshName;
	Transform Trans;
};

