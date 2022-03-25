#include "stdafx.h"
#include "ResourceManage.h"
#include "Engine.h"
#include "StoWS.h"


ResourceManage::~ResourceManage()
{

}

bool ResourceManage::Init()
{
	ClearAsset();
	return true;
}


void ResourceManage::LoadMap(std::string MapName, std::vector<std::shared_ptr<Primitive>>& Actors)
{
	std::string FilePath = "Data/" + MapName + ".dat";
	std::ifstream fin(FilePath, std::ios::binary);

	if (!fin.is_open())
	{
		return;
	}

	INT32 Num;
	fin.read((char*)&Num, sizeof(int));

	for (int i = 0; i < Num; i++)
	{
		std::shared_ptr<Primitive> actorinfo(new Primitive());
		actorinfo->SetCommon(new CommonBuffer());
		INT32 TempNum;
		fin.read((char*)&actorinfo->GetTransform(), sizeof(int) * 10);
		fin.read((char*)&TempNum, sizeof(int));
		actorinfo->MeshName.resize(TempNum);
		fin.read((char*)actorinfo->MeshName.data(), TempNum);
		actorinfo->SetMesh(LoadMeshAsset(actorinfo->MeshName));

		Actors.push_back(actorinfo);
	}
	fin.close();

	LoadTexture("jacket_diff");
}

Texture* ResourceManage::GetTexture(std::string name)
{
	return TextureAsset1.find(name)->second.get();
}

Mesh* ResourceManage::LoadMeshAsset(std::string filename)
{
	filename = filename.erase(filename.length() - 1, 1);
	std::string FilePath = "Data/" + filename + ".dat";
	std::ifstream fin(FilePath, std::ios::binary);
	if (!fin.is_open())
	{
		return nullptr;
	}
	if (MeshAsset1.find(filename) == MeshAsset1.end())
	{
		std::shared_ptr<Mesh> meshinfo(new Mesh());

		INT32 Num;
		fin.read((char*)&Num, sizeof(int));
		meshinfo->MeshName.resize(Num);
		fin.read((char*)meshinfo->MeshName.data(), Num);
		fin.read((char*)&meshinfo->NumLod, sizeof(int));
		fin.read((char*)&meshinfo->NumVertices, sizeof(int));
		fin.read((char*)&meshinfo->NumTriangles, sizeof(int));
		fin.read((char*)&meshinfo->NumIndices, sizeof(int));
		fin.read((char*)&Num, sizeof(int));
		meshinfo->IndexVector.resize(Num);
		fin.read((char*)meshinfo->IndexVector.data(), sizeof(int) * Num);
		fin.read((char*)&Num, sizeof(int));
		meshinfo->VertexInfo.resize(Num);
		fin.read((char*)meshinfo->VertexInfo.data(), sizeof(glm::vec3) * Num);
		fin.read((char*)&Num, sizeof(int));
		meshinfo->Normal.resize(Num);
		fin.read((char*)meshinfo->Normal.data(), sizeof(glm::vec4) * Num);
		fin.read((char*)&Num, sizeof(int));
		meshinfo->TexCoord.resize(Num);
		fin.read((char*)meshinfo->TexCoord.data(), sizeof(glm::vec2) * Num);
		fin.close();

		meshinfo->SetTexture(new Texture("jacket_diff", L"Textures/jacket_diff.dds"));

		MeshAsset1.insert({ filename ,meshinfo });
		return meshinfo.get();
	}
	else
	{
		return MeshAsset1.find(filename)->second.get();
	}
}

void ResourceManage::ClearAsset()
{
	MeshAsset1.clear();
	TextureAsset1.clear();
}

void ResourceManage::LoadTexture(std::string Name)
{
	auto Tex = std::make_shared<Texture>();
	Tex->GetTextureName() = Name;
	Tex->GetFileName() = L"Textures/" + SToWS(Name) + L".dds";
	TextureAsset1.insert({ Name, Tex });

}
