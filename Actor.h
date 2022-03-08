#pragma once
#include "StaticMesh.h"
#include "UpLoadBuffer.h"


struct ConstantBuffer
{
	glm::mat4x4 MVP = glm::mat4x4(1.0f);
	glm::mat4x4 Scale3D = glm::mat4x4(1.0f);
	glm::mat4x4 Rotate = glm::mat4x4(1.0f);
	float Offset = 1;
};

struct Transform
{
	glm::vec4 Rotation = glm::vec4(1.0f);
	glm::vec3 Translation = glm::vec3(1.0f);
	glm::vec3 Scale3D = glm::vec3(1.0f);
};

class Actor
{
public:
	Actor();
	~Actor();

	Transform Trans;

	glm::mat4 WorldTrans = glm::mat4(1.0f);
	glm::mat4 Scale3DTrans = glm::mat4(1.0f);
	glm::mat4 RotateTrans = glm::mat4(1.0f);

	glm::mat4 MVP = glm::mat4(1.0f);

	std::string AssetName = "";

	std::shared_ptr<StaticMesh> Asset = nullptr;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CbvHeap = nullptr;
	std::unique_ptr<UploadBuffer<ConstantBuffer>> CB = nullptr;

	void ChangePosition(glm::vec3 Pos);
	void ChangeRotation(glm::vec4 Rot);
	void ChangeScale3D(glm::vec3 Sca);

	void CreateCbvHeap(ID3D12Device* device);
	void CreateConstantBuffer(ID3D12Device* device);
};

