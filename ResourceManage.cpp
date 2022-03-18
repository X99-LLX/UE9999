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

void ResourceManage::LoadMap(std::string MapName, std::vector<std::shared_ptr<RenderItem>>& Actors)
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
		std::shared_ptr<RenderItem> actorinfo(new RenderItem());
		INT32 TempNum;
		fin.read((char*)&actorinfo->Trans, sizeof(int) * 10);
		fin.read((char*)&TempNum, sizeof(int));
		actorinfo->AssetName.resize(TempNum);
		fin.read((char*)actorinfo->AssetName.data(), TempNum);
		actorinfo->Asset = LoadMeshAsset(actorinfo->AssetName);
		Actors.push_back(actorinfo);
	}
	fin.close();

	LoadTexture("bricks3");
}

GPUTexture* ResourceManage::GetTexture(std::string name)
{
	return TextureAsset.find(name)->second.get();
}

std::shared_ptr<GPUMesh> ResourceManage::LoadMeshAsset(std::string filename)
{
	filename = filename.erase(filename.length() - 1, 1);
	std::string FilePath = "Data/" + filename + ".dat";
	std::ifstream fin(FilePath, std::ios::binary);
	if (!fin.is_open())
	{
		return nullptr;
	}
	if (MeshAsset.find(filename) == MeshAsset.end())
	{
		std::shared_ptr<GPUMesh> meshinfo(new GPUMesh());

		INT32 Num;
		fin.read((char*)&Num, sizeof(int));
		meshinfo->MeshName.resize(Num);
		fin.read((char*)meshinfo->MeshName.data(), Num);
		fin.read((char*)&meshinfo->NumLod, sizeof(int));
		fin.read((char*)&meshinfo->NumVertices, sizeof(int));
		fin.read((char*)&meshinfo->NumTriangles, sizeof(int));
		fin.read((char*)&meshinfo->NumIndices, sizeof(int));
		fin.read((char*)&Num, sizeof(int));
		meshinfo->indices.resize(Num);
		fin.read((char*)meshinfo->indices.data(), sizeof(int) * Num);
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
		meshinfo->Tex = std::make_shared<GPUTexture>("bricks3",L"Textures/bricks3.dds");


		MeshAsset.insert({ filename ,meshinfo });
		return meshinfo;
	}
	else
	{
		return MeshAsset.find(filename)->second;
	}
}

void ResourceManage::ClearAsset()
{
	MeshAsset.clear();
	TextureAsset.clear();
}



void ResourceManage::LoadTexture(std::string Name)
{
	auto woodCrateTex = std::make_shared<GPUTexture>();
	woodCrateTex->Name = Name;
	woodCrateTex->Filename = L"Textures/" + SToWS(Name) + L".dds";
	TextureAsset.insert({ Name, woodCrateTex });

}
