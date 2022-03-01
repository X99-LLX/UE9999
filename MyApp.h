#pragma once
#include "d3dApp.h"
#include "UpLoadBuffer.h"
#include "Camera.h"
#include "MathHelper.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

struct ActorTransform
{
	XMFLOAT4 Rotation;
	XMFLOAT3 Translation;
	XMFLOAT3 Scale3D;
};

struct ActorInfo
{
	ActorTransform transform;
	std::string AssetName;

};

struct AllActor
{
	std::vector<ActorInfo> Actors;
};



struct Vectex
{
	float x;
	float y;
	float z;
	void TransWorld(XMFLOAT3 trans)
	{
		x = x + trans.x;
		y = y + trans.y;
		z = z + trans.z;
	}
};

struct FVertexInfo
{
	int NumVertices;
	int NumTriangles;
	int NumIndices;
	std::vector<int> Index;
	std::vector<Vectex> VertexInfo;
};


struct FStaticMeshInfo
{
	std::string MeshName;
	int NumLod;
	FVertexInfo InfoVertex;
};


struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color = XMFLOAT4(Colors::Pink);
	void Setpos(const Vectex& v)
	{
		Pos = XMFLOAT3(v.x, v.y, v.z);
	}
};

struct ObjectConstants
{
	XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
};


class MyApp : public d3dApp
{
public:
	MyApp(HINSTANCE hInstance);
	MyApp(const MyApp& rhs) = delete;
	MyApp& operator=(const MyApp& rhs) = delete;
	~MyApp();

	virtual bool Initialize()override;

private:
	virtual void OnResize()override;
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	void BuildDescriptorHeaps();
	void BuildConstantBuffers();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildGeometry();
	void BuildPSO();

	void OnKeyboardInput(const GameTimer& gt);

	bool ReadMeshdat(const std::string filename,FStaticMeshInfo& meshinfo);
	void ReadAllMeshdat(const std::string filename, AllActor& meshinfo);

private:

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;

	std::vector <MeshGeometry> mMeshGeo ;

	ComPtr<ID3DBlob> mvsByteCode = nullptr;
	ComPtr<ID3DBlob> mpsByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	ComPtr<ID3D12PipelineState> mPSO = nullptr;

	XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();


	Camera mCamera;

	POINT mLastMousePos;
};

