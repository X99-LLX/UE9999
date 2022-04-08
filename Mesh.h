#pragma once
#include "Texture.h"

class Mesh
{
public:
	Mesh() {}
	Mesh(Mesh* m)
	{
		auto a = m->GetMaterialName();
		MeshName = m->GetName();
		MaterialName = m->GetMaterialName();
		mVertexInfo = m->GetVertexinfo();
	}
	virtual ~Mesh() {}
	void SetName(std::string n) { MeshName = n; }
	void SetMaterialName(std::string n) { MaterialName = n; }
	void SetMeshInfo(MeshVertexInfo mi) { mVertexInfo = mi; }

	std::string GetName() { return MeshName; }
	std::string GetMaterialName() { return MaterialName; }
	MeshVertexInfo& GetVertexinfo() { return mVertexInfo; }

	UINT32 IndexCount = 0;
protected:
	std::string MeshName;
	std::string MaterialName;
	MeshVertexInfo mVertexInfo;
};

