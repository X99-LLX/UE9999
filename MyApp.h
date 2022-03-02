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

struct MyNormal
{
	float x;
	float y;
	float z;
	float w;
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

	void Scale3D(XMFLOAT3 scale)
	{
		x *= scale.x;
		y *= scale.y;
		z *= scale.z;
	}

	void Rotate(XMFLOAT4 rotate)
	{
		
	}
};

struct FVertexInfo
{
	int NumVertices;
	int NumTriangles;
	int NumIndices;
	std::vector<int> Index;
	std::vector<Vectex> VertexInfo;
	std::vector<MyNormal> Normal;
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
	void Setpos(const Vectex& v, XMFLOAT4 c)
	{
		Pos = XMFLOAT3(v.x, v.y, v.z);
		Color = c;
	}
	void Setpos(const Vectex& v, MyNormal c)
	{
		Pos = XMFLOAT3(v.x, v.y, v.z);
		Color.x = c.x * 0.5f + 0.5f;
		Color.y = c.y * 0.5f + 0.5f;
		Color.z = c.z * 0.5f + 0.5f;
		Color.w = c.w * 0.5f + 0.5f;
	}
};

struct ObjectTransform
{
	XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
	XMFLOAT4X4 Scale3D = MathHelper::Identity4x4();
	XMFLOAT4X4 Rotate = MathHelper::Identity4x4();
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

	void BuildDescriptorHeaps(int index);
	void BuildConstantBuffers(int index);


	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildGeometry();
	void BuildPSO();

	void OnKeyboardInput(const GameTimer& gt);

	bool ReadMeshdat(const std::string filename,FStaticMeshInfo& meshinfo);
	void ReadAllMeshdat(const std::string filename, AllActor& meshinfo);

private:

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

	std::vector<ComPtr<ID3D12DescriptorHeap>> mCbvHeap ;
	std::vector<std::unique_ptr<UploadBuffer<ObjectTransform>>> mObjectCB ;

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

