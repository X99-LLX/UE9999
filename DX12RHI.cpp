#include "stdafx.h"
#include "DX12RHI.h"
#include "Engine.h"

#include "DX12Texture.h"
#include "DX12CommonBuffer.h"
#include "DX12Mesh.h"

DX12RHI::DX12RHI()
{

}

DX12RHI::~DX12RHI()
{

}

bool DX12RHI::Init()
{
#if defined(DEBUG) || defined(_DEBUG) 
	// Enable the D3D12 debug layer.
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)));


	HRESULT hardwareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&md3dDevice));

	if (FAILED(hardwareResult))
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(mdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&md3dDevice)));
	}

	ThrowIfFailed(md3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&mFence)));

	mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//检查是否支持 4X MSAA
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = mBackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(md3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));

	m4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(m4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

	CreateCommandObjects();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeap();
	OnResize();
	
	BuildShadersAndInputLayout();
	BuildRootSignature();
	BuildPSO();
	
	FlushCommandQueue();

	return true;
}

void DX12RHI::Update(Scene* mScene, Primitive* actor)
{
	mScene->mCamera.UpdateViewMatrix();
	
	actor->WorldTrans = glm::translate(glm::mat4(1.0f), actor->GetTransform().Translation);
	actor->Scale3DTrans = glm::scale(glm::mat4(1.0f), actor->GetTransform().Scale3D);

	actor->RotateTrans = glm::mat4_cast(glm::qua<float>(actor->GetTransform().Rotation.w, actor->GetTransform().Rotation.x,
		actor->GetTransform().Rotation.y, actor->GetTransform().Rotation.z));

	glm::mat4 worldViewProj = mScene->mCamera.GetProj() * mScene->mCamera.GetView()
		* actor->WorldTrans;
	actor->MVP = worldViewProj;
	
}

void DX12RHI::DrawCall(Primitive* actor)
{
	//Swapchain();
	DrawInstance(actor);

}
 

void DX12RHI::OnResize()
{
	assert(md3dDevice);
	assert(mSwapChain);
	assert(mDirectCmdListAlloc);

	FlushCommandQueue();        

	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));  

	
	for (int i = 0; i < SwapChainBufferCount; i++)
	{
		mSwapChainBuffer[i].Reset();             
	}

	mDepthStencilBuffer.Reset();

	//调整交换链大小
	Window* TempWND = Engine::GetEngine()->GetWindow();

	ThrowIfFailed(mSwapChain->ResizeBuffers(     
		SwapChainBufferCount,
		TempWND->Width, TempWND->Height,
		mBackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	mCurrentBackBuffer = 0;      

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());    

	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));
		md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, mRtvDescriptorSize);             
	}

	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = TempWND->Width;
	depthStencilDesc.Height = TempWND->Height;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;

	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	depthStencilDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;                 //清除操作最优化的值
	optClear.Format = mDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;         //距离设置为最远
	optClear.DepthStencil.Stencil = 0;

	ThrowIfFailed(md3dDevice->CreateCommittedResource(          //同时创建一个资源和一个隐式堆，使得堆足够大以包含整个资源，并且资源被映射到堆。
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));


	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;     //描述可以从深度模板视图访问的资源
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = mDepthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;

	md3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, DepthStencilView());   //创建用于资源访问的深度模板视图

	//改变资源状态类型？
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));


	//执行调整大小命令
	ThrowIfFailed(mCommandList->Close());

	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);   //提交一组命令到队列

	FlushCommandQueue();

	//更新viewport大小 覆盖客户端 
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(TempWND->Width);
	mScreenViewport.Height = static_cast<float>(TempWND->Height);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	mScissorRect = { 0,0,TempWND->Width,TempWND->Height };

	Scene* mScene = Engine::GetEngine()->GetScene();
	mScene->mCamera.UpdateProjMatrix(0.25f * glm::pi<float>(), TempWND->GetAspectRatio(), 1.0f, 100000.0f);
}

ID3D12Resource* DX12RHI::CurrentBackBuffer() const
{
	return mSwapChainBuffer[mCurrentBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12RHI::CurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
		mCurrentBackBuffer,
		mRtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12RHI::DepthStencilView() const
{
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

ID3D12Device* DX12RHI::GetDevice()
{
	return md3dDevice.Get();
}

ID3D12GraphicsCommandList* DX12RHI::GetCmdList()
{
	return mCommandList.Get();
}

void DX12RHI::OpenCmdList()
{
	ThrowIfFailed(mDirectCmdListAlloc->Reset());
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), mPSO.Get()));
}

void DX12RHI::CloseCmdList()
{
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	/*FlushCommandQueue();*/
}

void DX12RHI::ResetViewportsAndScissorRects()
{
	OpenCmdList();
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);
	
}

void DX12RHI::ClearRTVAndDSV()
{
	
	float BackColor[4] = { 1.000000000f, 0.713725507f, 0.756862819f, 1.000000000f };
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), BackColor, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	
}

void DX12RHI::SetRTVAndDSV()
{
	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());
}

void DX12RHI::SetRootSignature()
{
	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());
}

void DX12RHI::Swapchain()
{
	CloseCmdList();
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrentBackBuffer = (mCurrentBackBuffer + 1) % SwapChainBufferCount;
	
	FlushCommandQueue();
}

void DX12RHI::DrawInstance(Primitive* actor)
{
	auto Temp = dynamic_cast<DX12CommonBuffer*>(actor->GetCommon());

	ID3D12DescriptorHeap* descriptorHeaps[] = { Temp->GetHeap().Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	auto* TempMesh = dynamic_cast<DX12Mesh*>(actor->GetMesh());



	mCommandList->IASetVertexBuffers(0, 1, &TempMesh->VertexBufferView);
	mCommandList->IASetIndexBuffer(&TempMesh->IndexBufferView);

	mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ConstantBuffer objConstants;
	objConstants.MVP = glm::transpose(actor->MVP);
	objConstants.Scale3D = actor->Scale3DTrans;
	objConstants.Rotate = actor->RotateTrans;
	objConstants.Offset = Engine::GetEngine()->GetTimer()->TotalTime();

	Temp->GetCB()->CopyData(0, objConstants);

	mCommandList->SetGraphicsRoot32BitConstants(0, 3, &actor->GetTransform().Translation, 0);
	mCommandList->SetGraphicsRootConstantBufferView(1, Temp->GetCB()->Resource()->GetGPUVirtualAddress());
	mCommandList->SetGraphicsRootDescriptorTable(2, Temp->GetHeap()->GetGPUDescriptorHandleForHeapStart());

	mCommandList->DrawIndexedInstanced(
		TempMesh->IndexCount,
		1, 0, 0, 0);
}

void DX12RHI::OpenRtv()
{
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
}

void DX12RHI::CloseRtv()
{
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	
}

void DX12RHI::SetCommonBuffer(Primitive* actor)
{
	DX12CommonBuffer* C = new DX12CommonBuffer();
	actor->SetCommon(C);
	auto Temp = dynamic_cast<DX12CommonBuffer*>(actor->GetCommon());
	
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	cbvHeapDesc.NumDescriptors = 1000;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;

	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&cbvHeapDesc,
		IID_PPV_ARGS(&Temp->CbvSrvUavHeap)));

	Temp->CB = std::make_shared<UploadBuffer<ConstantBuffer>>(md3dDevice.Get(), 1, true);

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = Temp->GetCB()->Resource()->GetGPUVirtualAddress();
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(Temp->CbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart());
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	auto Tex = dynamic_cast<DX12Texture*>(actor->GetMesh()->MeshTex);
	srvDesc.Format = Tex->Resource->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = Tex->Resource->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	md3dDevice->CreateShaderResourceView(Tex->Resource.Get(), &srvDesc, hDescriptor);
	hDescriptor.ptr += md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	md3dDevice->CreateShaderResourceView(Tex->Resource.Get(), &srvDesc, hDescriptor);
}

void DX12RHI::BuildTexture(Primitive* actor)
{
	OpenCmdList();
	

	DX12Mesh* m = new DX12Mesh(actor->MeshBuffer);
	m->MeshTex = new DX12Texture(actor->GetMesh()->MeshTex->GetTextureName(), actor->GetMesh()->MeshTex->GetFileName());
	actor->MeshBuffer = m;

	auto Tex = dynamic_cast<DX12Texture*>(actor->GetMesh()->MeshTex);
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->GetFileName().c_str(),
		Tex->Resource, Tex->UploadHeap));

	CloseCmdList();
	
}

void DX12RHI::BuildRootSignature()
{
	ThrowIfFailed(md3dDevice->CreateRootSignature(
		0,
		mvsByteCode->GetBufferPointer(),
		mvsByteCode->GetBufferSize(),
		IID_PPV_ARGS(&mRootSignature)));
}

void DX12RHI::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	mvsByteCode = d3dUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
	mpsByteCode = d3dUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "PS", "ps_5_0");

	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

void DX12RHI::BuildPSO()
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

void DX12RHI::CreateRtvAndDsvDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
}

void DX12RHI::CreateSwapChain()
{
	//释放原来的交换链
	mSwapChain.Reset();

	Window* TempWND = Engine::GetEngine()->GetWindow();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = TempWND->Width;
	sd.BufferDesc.Height = TempWND->Height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = mBackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	sd.SampleDesc.Count = m4xMsaaState ? 4 : 1;                                 //填写采样模式
	sd.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;


	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.OutputWindow = TempWND->mMainWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//交换链使用队列来刷新
	ThrowIfFailed(mdxgiFactory->CreateSwapChain(
		mCommandQueue.Get(),
		&sd,
		mSwapChain.GetAddressOf()));
	
}

void DX12RHI::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(md3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

	ThrowIfFailed(md3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(mDirectCmdListAlloc.GetAddressOf())));

	ThrowIfFailed(md3dDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		mDirectCmdListAlloc.Get(), // 关联命令分配器
		nullptr,                   // 初始化管道对象状态
		IID_PPV_ARGS(mCommandList.GetAddressOf())));

	mCommandList->Close();
}

void DX12RHI::FlushCommandQueue()
{
	mCurrentFence++;
	ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mCurrentFence));

	if (mFence->GetCompletedValue() < mCurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}


void DX12RHI::BuildGeo(Primitive* actor)
{
	BuildTexture(actor);
	
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
	std::vector<int> indices;
	std::vector<Vertex> vertices;

	SetCommonBuffer(actor);

	auto* TempMesh = dynamic_cast<DX12Mesh*>(actor->GetMesh());
	

	for (int i = 0; i < TempMesh->NumVertices; i++)
	{
		vertices.push_back({ TempMesh->VertexInfo[i],glm::vec4(1.0f),TempMesh->Normal[i] ,TempMesh->TexCoord[i]});
	}

	indices.insert(indices.end(), TempMesh->IndexVector.begin(), TempMesh->IndexVector.end());

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(int);

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &TempMesh->VertexBufferCPU));
	CopyMemory(TempMesh->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);
	ThrowIfFailed(D3DCreateBlob(ibByteSize, &TempMesh->IndexBufferCPU));
	CopyMemory(TempMesh->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	TempMesh->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), vertices.data(), vbByteSize, TempMesh->VertexBufferUploader);
	TempMesh->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), indices.data(), ibByteSize, TempMesh->IndexBufferUploader);

	TempMesh->VertexByteSize = sizeof(Vertex);
	TempMesh->VertexBufferByteSize = vbByteSize;
	TempMesh->IndexFormat = DXGI_FORMAT_R32_UINT;
	TempMesh->IndexBufferByteSize = ibByteSize;
	TempMesh->IndexCount = (UINT)indices.size();

	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = TempMesh->VertexBufferGPU->GetGPUVirtualAddress();
	vbv.StrideInBytes = TempMesh->VertexByteSize;
	vbv.SizeInBytes = TempMesh->VertexBufferByteSize;
	TempMesh->VertexBufferView = vbv;

	D3D12_INDEX_BUFFER_VIEW ibv;
	ibv.BufferLocation = TempMesh->IndexBufferGPU->GetGPUVirtualAddress();
	ibv.Format = TempMesh->IndexFormat;
	ibv.SizeInBytes = TempMesh->IndexBufferByteSize;
	TempMesh->IndexBufferView = ibv;

	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	FlushCommandQueue();
	
}

void DX12RHI::CreateRenderItem(Primitive* actor)
{

	BuildGeo(actor);
}
