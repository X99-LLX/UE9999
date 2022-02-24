#pragma once
#include "d3dApp.h"
#include "UpLoadBuffer.h"
#include "MathHelper.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;



struct Vectex
{
	float x;
	float y;
	float z;
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
	void BuildBoxGeometry();
	void BuildPSO();

	void Readdat(const std::string filename);

private:

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;

	std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;

	ComPtr<ID3DBlob> mvsByteCode = nullptr;
	ComPtr<ID3DBlob> mpsByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	ComPtr<ID3D12PipelineState> mPSO = nullptr;

	XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();


	FStaticMeshInfo StaticMeshInfo;

	float mTheta = 1.5f * XM_PI;
	float mPhi = XM_PIDIV4;
	float mRadius = 500.0f;

	//test
	float px = 0;
	float py = 0;
	float pz = 0;

	POINT mLastMousePos;
};

