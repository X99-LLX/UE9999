#pragma once
#include "GPUMesh.h"

//testwl19990925
#include "DXMesh.h"

class RenderItem
{
public:
	RenderItem() {};
	~RenderItem() {};

	Transform Trans;

	glm::mat4 WorldTrans = glm::mat4(1.0f);
	glm::mat4 Scale3DTrans = glm::mat4(1.0f);
	glm::mat4 RotateTrans = glm::mat4(1.0f);
	glm::mat4 MVP = glm::mat4(1.0f);

	std::string AssetName = "";


	//testwl19990925
	std::shared_ptr<GPUMesh> Asset = nullptr;

};

