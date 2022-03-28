#include "stdafx.h"
#include "ResourceManage.h"
#include "Engine.h"
#include "StoWS.h"
#include "DX12Primitive.h"
#include "DX12Mesh.h"
#include "DX12Texture.h"

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
#ifdef _RHI_DX12
		auto actorinfo = std::make_shared<DX12Primitive>();
#endif
		INT32 TempNum;
		fin.read((char*)&actorinfo->GetTransform(), sizeof(int) * 10);
		fin.read((char*)&TempNum, sizeof(int));
		actorinfo->MeshName.resize(TempNum);
		fin.read((char*)actorinfo->MeshName.data(), TempNum);
		actorinfo->MeshBuffer = LoadMeshAsset(actorinfo->MeshName);
		Actors.push_back(actorinfo);
	}
	fin.close();

	LoadTexture("jacket_diff");
}

Texture* ResourceManage::GetTexture(std::string name)
{
	return TextureAsset1.find(name)->second.get();
}

std::shared_ptr<Mesh> ResourceManage::LoadMeshAsset(std::string filename)
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
#ifdef _RHI_DX12
		auto meshinfo = std::make_shared<DX12Mesh>();
#endif
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

		meshinfo->MeshTex = std::make_shared<DX12Texture>("jacket_diff", L"Textures/jacket_diff.dds");

		MeshAsset1.insert({ filename ,meshinfo });
		return meshinfo;
	}
	else
	{
		return MeshAsset1.find(filename)->second;
	}
}

void ResourceManage::ClearAsset()
{
	MeshAsset1.clear();
	TextureAsset1.clear();
}

void ResourceManage::LoadTexture(std::string Name)
{
#ifdef _RHI_DX12
	auto Tex = std::make_shared<DX12Texture>();
	Tex->TextureName = Name;
	Tex->FileName = L"Textures/" + SToWS(Name) + L".dds";
	TextureAsset1.insert({ Name, Tex });
#endif
}
