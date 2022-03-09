#include "stdafx.h"
#include "Scene.h"

bool Scene::Init()
{
	mCamera.SetCameraPos(glm::vec3(0.0f, 0.0f, 10000.0f));
	return true;
}

//void Scene::ReadScenceDat(const std::string filename)
//{
//	std::string FilePath = "Data/" + filename + ".dat";
//	std::ifstream fin(FilePath, std::ios::binary);
//
//	bool a = fin.is_open();
//
//	INT32 Num;
//	fin.read((char*)&Num, sizeof(int));
//
//	for (int i = 0; i < Num; i++)
//	{
//		std::shared_ptr<Actor> actorinfo(new Actor());
//		INT32 TempNum;
//		fin.read((char*)&actorinfo->Trans, sizeof(int) * 10);
//		fin.read((char*)&TempNum, sizeof(int));
//		actorinfo->AssetName.resize(TempNum);
//		fin.read((char*)actorinfo->AssetName.data(), TempNum);
//		std::shared_ptr<StaticMesh> TempMesh(new StaticMesh());
//		if (ReadStaticMeshDat(actorinfo->AssetName, *TempMesh))
//		{
//			AssetIndex.insert({ actorinfo->AssetName ,TempMesh });
//			
//		}
//		else
//		{
//			TempMesh = AssetIndex.find(actorinfo->AssetName)->second;
//		}
//		actorinfo->Asset = TempMesh;
//		Actors.push_back(actorinfo);
//	}
//	fin.close();
//}
//
//bool Scene::ReadStaticMeshDat(std::string filename,StaticMesh& meshinfo)
//{
//	filename = filename.erase(filename.length() - 1, 1);
//	std::string FilePath = "Data/" + filename + ".dat";
//	std::ifstream fin(FilePath, std::ios::binary);
//	if (!fin.is_open())
//	{
//		return false;
//	}
//
//	if (AssetIndex.find(filename) == AssetIndex.end())
//	{
//		INT32 Num;
//		fin.read((char*)&Num, sizeof(int));
//		meshinfo.MeshName.resize(Num);
//		fin.read((char*)meshinfo.MeshName.data(), Num);
//
//		fin.read((char*)&meshinfo.NumLod, sizeof(int));
//
//
//		fin.read((char*)&meshinfo.NumVertices, sizeof(int));
//		fin.read((char*)&meshinfo.NumTriangles, sizeof(int));
//		fin.read((char*)&meshinfo.NumIndices, sizeof(int));
//
//		fin.read((char*)&Num, sizeof(int));
//		meshinfo.Index.resize(Num);
//		fin.read((char*)meshinfo.Index.data(), sizeof(int) * Num);
//
//		fin.read((char*)&Num, sizeof(int));
//		meshinfo.VertexInfo.resize(Num);
//		fin.read((char*)meshinfo.VertexInfo.data(), sizeof(glm::vec3) * Num);
//
//		fin.read((char*)&Num, sizeof(int));
//		meshinfo.Normal.resize(Num);
//		fin.read((char*)meshinfo.Normal.data(), sizeof(glm::vec4) * Num);
//
//		fin.close();
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}
