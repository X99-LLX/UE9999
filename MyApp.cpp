#include "stdafx.h"
#include "MyApp.h"
#include <set>
#include <unordered_map>

MyApp::MyApp(HINSTANCE hInstance):d3dApp(hInstance)
{

}

MyApp::~MyApp()
{

}

bool MyApp::Initialize()
{
	if (!d3dApp::Initialize())
		return false;

	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	mCamera.SetCameraPos(0.0f, 0.0f, 0.0f);

	/*BuildDescriptorHeaps();
	BuildConstantBuffers();*/

	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildGeometry();
	BuildPSO();

	// Execute the initialization commands.
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	FlushCommandQueue();

	return true;
}

void MyApp::OnResize()
{
	d3dApp::OnResize();
	mCamera.SetLens(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 100000.0f);
}

void MyApp::Update(const GameTimer& gt)
{
	OnKeyboardInput(gt);

	
	
}

void MyApp::Draw(const GameTimer& gt)
{
	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(mDirectCmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), mPSO.Get()));

	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	// Indicate a state transition on the resource usage.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Clear the back buffer and depth buffer.
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightPink, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	

	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	int index = 0;
	for (auto mesh:mMeshGeo)
	{
		ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvHeap[index].Get() };
		mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		mCommandList->IASetVertexBuffers(0, 1, &mesh.VertexBufferView());
		mCommandList->IASetIndexBuffer(&mesh.IndexBufferView());
		mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		XMMATRIX worldViewProj = mesh.mWorld * mCamera.GetView() * mCamera.GetProj();
		ObjectTransform objConstants;
		XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
		XMStoreFloat4x4(&objConstants.Scale3D, mesh.mScale3D);
		XMStoreFloat4x4(&objConstants.Rotate, mesh.mRotate);
		mObjectCB[index]->CopyData(0, objConstants);
		
		mCommandList->SetGraphicsRootDescriptorTable(0, mCbvHeap[index]->GetGPUDescriptorHandleForHeapStart());
		mCommandList->DrawIndexedInstanced(
			mesh.IndexCount,
			1, 0, 0, 0);
		index++;
	}
	

	// Indicate a state transition on the resource usage.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// Done recording commands.
	ThrowIfFailed(mCommandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// swap the back and front buffers
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrentBackBuffer = (mCurrentBackBuffer + 1) % SwapChainBufferCount;

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	FlushCommandQueue();

}

void MyApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void MyApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void MyApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		mCamera.Pitch(dy);
		mCamera.RotateY(dx);
	
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void MyApp::BuildDescriptorHeaps(int index)
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&cbvHeapDesc,
		IID_PPV_ARGS(&mCbvHeap[index])));
}

void MyApp::BuildConstantBuffers(int index)
{
	mObjectCB[index] = std::make_unique<UploadBuffer<ObjectTransform>>(md3dDevice.Get(), 1, true);
	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mObjectCB[index]->Resource()->GetGPUVirtualAddress();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectTransform));

	md3dDevice->CreateConstantBufferView(
		&cbvDesc,
		mCbvHeap[index]->GetCPUDescriptorHandleForHeapStart());
}



void MyApp::BuildRootSignature()
{
	// Shader programs typically require resources as input (constant buffers,
	// textures, samplers).  The root signature defines the resources the shader
	// programs expect.  If we think of the shader programs as a function, and
	// the input resources as function parameters, then the root signature can be
	// thought of as defining the function signature.  

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[1];

	// Create a single descriptor table of CBVs.
	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(md3dDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&mRootSignature)));
}

void MyApp::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	mvsByteCode = d3dUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
	mpsByteCode = d3dUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "PS", "ps_5_0");

	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

void MyApp::BuildGeometry()
{

	AllActor allactor;
	ReadAllMeshdat("mapactor",allactor);

	std::set<std::string> AllAssetPath;
	for (int i = 0; i < allactor.Actors.size(); i++)
	{
		AllAssetPath.insert(allactor.Actors[i].AssetName);
	}


	std::unordered_map<std::string, FStaticMeshInfo> AssetIndex;

	for (auto actorname :AllAssetPath)
	{
		FStaticMeshInfo StaticMeshInfo;
		if (ReadMeshdat(actorname, StaticMeshInfo)) 
		{
			AssetIndex.insert({ actorname, StaticMeshInfo });
		}
	}


	std::vector<int> indices;
	std::vector<Vertex> vertices;

	int Index = 0;
	for (auto actor:allactor.Actors)
	{
		mCbvHeap.resize(allactor.Actors.size());
		mObjectCB.resize(allactor.Actors.size());
		BuildDescriptorHeaps(Index);
		BuildConstantBuffers(Index);
		Index++;
		FStaticMeshInfo TempMeshInfo = AssetIndex[actor.AssetName];
		
		vertices.resize(TempMeshInfo.InfoVertex.VertexInfo.size());
		for (int i = 0; i < TempMeshInfo.InfoVertex.VertexInfo.size(); i++)
		{
			//TempMeshInfo.InfoVertex.VertexInfo[i].Scale3D(actor.transform.Scale3D);
			float j = (rand() % 10 ) / 10.0f;
			XMFLOAT4 color = { 2*j,j - 0.05f,j ,1 };
			vertices[i].Setpos(TempMeshInfo.InfoVertex.VertexInfo[i], TempMeshInfo.InfoVertex.Normal[i]);
		}

		indices.resize(TempMeshInfo.InfoVertex.Index.size());
		indices = TempMeshInfo.InfoVertex.Index;

		const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
		const UINT ibByteSize = (UINT)indices.size() * sizeof(int);

		MeshGeometry mBoxGeo ;
		mBoxGeo.Name = actor.AssetName;

		mBoxGeo.mWorld = XMMatrixTranslation(actor.transform.Translation.x, actor.transform.Translation.y, actor.transform.Translation.z);

		mBoxGeo.mScale3D = XMMatrixScaling(actor.transform.Scale3D.x, actor.transform.Scale3D.y, actor.transform.Scale3D.z);

		mBoxGeo.mRotate = XMMatrixRotationRollPitchYaw(actor.transform.Rotation.x, actor.transform.Rotation.y, actor.transform.Rotation.z);

		ThrowIfFailed(D3DCreateBlob(vbByteSize, &mBoxGeo.VertexBufferCPU));
		CopyMemory(mBoxGeo.VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

		ThrowIfFailed(D3DCreateBlob(ibByteSize, &mBoxGeo.IndexBufferCPU));
		CopyMemory(mBoxGeo.IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

		mBoxGeo.VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
			mCommandList.Get(), vertices.data(), vbByteSize, mBoxGeo.VertexBufferUploader);

		mBoxGeo.IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
			mCommandList.Get(), indices.data(), ibByteSize, mBoxGeo.IndexBufferUploader);


		mBoxGeo.VertexByteStride = sizeof(Vertex);
		mBoxGeo.VertexBufferByteSize = vbByteSize;  
		mBoxGeo.IndexFormat = DXGI_FORMAT_R32_UINT;
		mBoxGeo.IndexBufferByteSize = ibByteSize;
		mBoxGeo.IndexCount = (UINT)indices.size();
		mMeshGeo.push_back(mBoxGeo);
	}
}

void MyApp::BuildPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	psoDesc.pRootSignature = mRootSignature.Get();
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()),
		mvsByteCode->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()),
		mpsByteCode->GetBufferSize()
	};
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.FrontCounterClockwise = true;
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = mBackBufferFormat;
	psoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	psoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	psoDesc.DSVFormat = mDepthStencilFormat;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));
}

void MyApp::OnKeyboardInput(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (GetAsyncKeyState('W') & 0x8000)
		mCamera.Walk(10.0f * dt);

	if (GetAsyncKeyState('S') & 0x8000)
		mCamera.Walk(-10.0f * dt);

	if (GetAsyncKeyState('A') & 0x8000)
		mCamera.Strafe(-10.0f * dt);

	if (GetAsyncKeyState('D') & 0x8000)
		mCamera.Strafe(10.0f * dt);

	if (GetAsyncKeyState('Q') & 0x8000)
		mCamera.RotateZ(2.0f * dt);

	if (GetAsyncKeyState('E') & 0x8000)
		mCamera.RotateZ(-2.0f * dt);

	mCamera.UpdateViewMatrix();
}

bool MyApp::ReadMeshdat(std::string filename,FStaticMeshInfo& meshinfo)
{
	filename = filename.erase(filename.length() - 1, 1);
	std::string FilePath = "Data/" + filename + ".dat";
	std::ifstream fin(FilePath, std::ios::binary);
	if (!fin.is_open())
	{
		return false;
	}
	
	INT32 Num;


	fin.read((char*)&Num, sizeof(int));
	meshinfo.MeshName.resize(Num);
	fin.read((char*)meshinfo.MeshName.data(), Num);

	fin.read((char*)&meshinfo.NumLod, sizeof(int));


	fin.read((char*)&meshinfo.InfoVertex.NumVertices, sizeof(int));
	fin.read((char*)&meshinfo.InfoVertex.NumTriangles, sizeof(int));
	fin.read((char*)&meshinfo.InfoVertex.NumIndices, sizeof(int));

	fin.read((char*)&Num, sizeof(int));
	meshinfo.InfoVertex.Index.resize(Num);
	fin.read((char*)meshinfo.InfoVertex.Index.data(), sizeof(int) * Num);

	fin.read((char*)&Num, sizeof(int));
	meshinfo.InfoVertex.VertexInfo.resize(Num);
	fin.read((char*)meshinfo.InfoVertex.VertexInfo.data(), sizeof(Vectex) * Num);

	fin.read((char*)&Num, sizeof(int));
	meshinfo.InfoVertex.Normal.resize(Num);
	fin.read((char*)meshinfo.InfoVertex.Normal.data(), sizeof(MyNormal) * Num);

	fin.close();
}

void MyApp::ReadAllMeshdat(const std::string filename, AllActor& meshinfo)
{
	
	std::string FilePath = "Data/" + filename + ".dat";
	std::ifstream fin(FilePath, std::ios::binary);


	INT32 Num;
	fin.read((char*)&Num,sizeof(int));

	for (int i = 0; i < Num; i++)
	{
		ActorInfo actorinfo;
		INT32 TempNum;
		fin.read((char*)&actorinfo.transform, sizeof(int) * 10);
		fin.read((char*)&TempNum, sizeof(int));
		actorinfo.AssetName.resize(TempNum);
		fin.read((char*)actorinfo.AssetName.data(), TempNum);
		meshinfo.Actors.push_back(actorinfo);
	}
 
	fin.close();
}

