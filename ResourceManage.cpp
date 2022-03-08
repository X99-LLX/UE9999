#include "stdafx.h"
#include "ResourceManage.h"

bool ResourceManage::Init()
{
	ClearAsset();
	return true;
}

void ResourceManage::LoadMap(std::string MapName, std::vector<std::shared_ptr<Actor>>& Actors)
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
		std::shared_ptr<Actor> actorinfo(new Actor());
		INT32 TempNum;
		fin.read((char*)&actorinfo->Trans, sizeof(int) * 10);
		fin.read((char*)&TempNum, sizeof(int));
		actorinfo->AssetName.resize(TempNum);
		fin.read((char*)actorinfo->AssetName.data(), TempNum);
		actorinfo->Asset = LoadMeshAsset(actorinfo->AssetName);
		Actors.push_back(actorinfo);
	}
	fin.close();
}

std::shared_ptr<StaticMesh> ResourceManage::LoadMeshAsset(std::string filename)
{
	filename = filename.erase(filename.length() - 1, 1);
	std::string FilePath = "Data/" + filename + ".dat";
	std::ifstream fin(FilePath, std::ios::binary);
	if (!fin.is_open())
	{
		return nullptr;
	}
	if (AssetIndex.find(filename) == AssetIndex.end())
	{
		std::shared_ptr<StaticMesh> meshinfo(new StaticMesh());
		INT32 Num;
		fin.read((char*)&Num, sizeof(int));
		meshinfo->MeshName.resize(Num);
		fin.read((char*)meshinfo->MeshName.data(), Num);
		fin.read((char*)&meshinfo->NumLod, sizeof(int));
		fin.read((char*)&meshinfo->NumVertices, sizeof(int));
		fin.read((char*)&meshinfo->NumTriangles, sizeof(int));
		fin.read((char*)&meshinfo->NumIndices, sizeof(int));
		fin.read((char*)&Num, sizeof(int));
		meshinfo->Index.resize(Num);
		fin.read((char*)meshinfo->Index.data(), sizeof(int) * Num);
		fin.read((char*)&Num, sizeof(int));
		meshinfo->VertexInfo.resize(Num);
		fin.read((char*)meshinfo->VertexInfo.data(), sizeof(glm::vec3) * Num);
		fin.read((char*)&Num, sizeof(int));
		meshinfo->Normal.resize(Num);
		fin.read((char*)meshinfo->Normal.data(), sizeof(glm::vec4) * Num);
		fin.close();
		AssetIndex.insert({ filename,meshinfo });
		return meshinfo;
	}
	else
	{
		return AssetIndex.find(filename)->second;
	}
}

void ResourceManage::ClearAsset()
{
	AssetIndex.clear();
}
