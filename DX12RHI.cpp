#include "stdafx.h"
#include "DX12RHI.h"
#include "Engine.h"

#include "DX12Pipeline.h"
#include "DX12Texture.h"
#include "DX12Mesh.h"
#include "DX12Primitive.h"
#include "DX12Shader.h"

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
	CreateCommandObjects();
	CreateSwapChain();
	mHeapMng.InitHeapMng(md3dDevice.Get());
	OnResize();
	BuildInputLayout();
	FlushCommandQueue();
	mShadowMap->SetOffset(mHeapMng.GetDescNum(HeapType::CBV_SRV_UAV), mHeapMng.GetDescNum(HeapType::DSV));
	mShadowMap->Init(md3dDevice.Get(),mHeapMng.GetCPUCurrentHandleStart(HeapType::CBV_SRV_UAV),mHeapMng.GetCPUCurrentHandleStart(HeapType::DSV));
	return true;
}
 
void DX12RHI::OnResize()
{
	assert(md3dDevice);
	assert(mSwapChain);
	assert(mDirectCmdListAlloc);

	FlushCommandQueue();  

	OpenCmdList();
	
	for (int i = 0; i < SwapChainBufferCount; i++)
	{
		mSwapChainBuffer[i].Reset();             
	}

	mDepthStencilBuffer.Reset();

	Window* TempWND = Engine::GetEngine()->GetWindow();

	ThrowIfFailed(mSwapChain->ResizeBuffers(     
		SwapChainBufferCount,
		TempWND->Width, TempWND->Height,
		mBackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	mCurrentBackBuffer = 0;      

	   
	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));
		md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, mHeapMng.GetCPUCurrentHandleStart(HeapType::RTV));   
	}

	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = TempWND->Width;
	depthStencilDesc.Height = TempWND->Height;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;

	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	depthStencilDesc.SampleDesc.Count = false ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = false ? (0 - 1) : 0;
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
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&optClear,
		IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));


	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;     //描述可以从深度模板视图访问的资源
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = mDepthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;

	TEST_dsvoffset = mHeapMng.GetDescNum(HeapType::DSV);
	md3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, mHeapMng.GetCPUCurrentHandleStart(HeapType::DSV));   //创建用于资源访问的深度模板视图

	CloseCmdList();
	FlushCommandQueue();


	mScreenViewport = { 0.0f, 0.0f, static_cast<float>(TempWND->Width), static_cast<float>(TempWND->Height), 0.0f, 1.0f };
	mScissorRect = { 0,0,TempWND->Width,TempWND->Height };

	Scene* mScene = Engine::GetEngine()->GetScene();
	mScene->mCamera.UpdateProjMatrix(0.25f * glm::pi<float>(), TempWND->GetAspectRatio(), 1.0f, 100000.0f);
}

ID3D12Resource* DX12RHI::CurrentBackBuffer()
{
	return mSwapChainBuffer[mCurrentBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12RHI::CurrentBackBufferView()
{
	return mHeapMng.GetHeapCPUHandlePos(HeapType::RTV, mCurrentBackBuffer);
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12RHI::DepthStencilView()
{
	return mHeapMng.GetHeapCPUHandlePos(HeapType::DSV,TEST_dsvoffset);
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
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), mCommonPSO.Get()));
}

void DX12RHI::CloseCmdList()
{
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
}

void DX12RHI::ResetViewportsAndScissorRects(RtType rt)
{
	if (rt == RtType::BaseRt)
	{
		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);
	}
	else
	{
		mCommandList->RSSetViewports(1, &mShadowMap->mViewport);
		mCommandList->RSSetScissorRects(1, &mShadowMap->mScissorRect);
	}
}

void DX12RHI::SetRTVAndDSV(RtType rt)
{
	if (rt == RtType::BaseRt)
	{
		float BackColor[4] = { 1.000000000f, 0.713725507f, 0.756862819f, 1.000000000f };
		mCommandList->ClearRenderTargetView(CurrentBackBufferView(), BackColor, 0, nullptr);
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());
	}
	else
	{
		mCommandList->ClearDepthStencilView(mHeapMng.GetHeapCPUHandlePos(HeapType::DSV, mShadowMap->dsvOffset),
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		mCommandList->OMSetRenderTargets(0, nullptr, false, &mHeapMng.GetHeapCPUHandlePos(HeapType::DSV, mShadowMap->dsvOffset));
	}
}

void DX12RHI::SetRootSignature(Shader* s)
{
	auto ds = dynamic_cast<DX12Shader*>(s);
	auto rs = ds->GetRootSignature();
	mCommandList->SetGraphicsRootSignature(rs.Get());
}

void DX12RHI::ChangeResState()
{
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mShadowMap->mShadowMap.Get(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
}

void DX12RHI::CloseRtv()
{
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mShadowMap->mShadowMap.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE));
}

void DX12RHI::BeginDrawShadow()
{
	mCommandList->SetGraphicsRootSignature(mShadowRS.Get());

	mCommandList->RSSetViewports(1, &mShadowMap->mViewport);
	mCommandList->RSSetScissorRects(1, &mShadowMap->mScissorRect);

	mCommandList->ClearDepthStencilView(mHeapMng.GetHeapCPUHandlePos(HeapType::DSV,mShadowMap->dsvOffset),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	mCommandList->OMSetRenderTargets(0, nullptr, false, &mHeapMng.GetHeapCPUHandlePos(HeapType::DSV,mShadowMap->dsvOffset));
	
	mCommandList->SetPipelineState(mShadowPSO.Get());
}

void DX12RHI::InputAssetInfo(Mesh* mesh)
{
	auto* TempMesh = dynamic_cast<DX12Mesh*>(mesh);
	mCommandList->IASetVertexBuffers(0, 1, &TempMesh->VertexBufferView);
	mCommandList->IASetIndexBuffer(&TempMesh->IndexBufferView);
	mCommandList->IASetPrimitiveTopology(TempMesh->mTopology);
}

void DX12RHI::SetDescHeap(HeapType ht)
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { mHeapMng.GetHeap(ht) };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
}

void DX12RHI::BeginFrame()
{
	//ThrowIfFailed(mDirectCmdListAlloc->Reset());
	OpenCmdList();
	SetDescHeap(HeapType::CBV_SRV_UAV);
}

void DX12RHI::EndFrame()
{
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mShadowMap->mShadowMap.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE));
	CloseCmdList();
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrentBackBuffer = (mCurrentBackBuffer + 1) % SwapChainBufferCount;
	FlushCommandQueue();
}

void DX12RHI::BindDataTable(UINT32 Slot, UINT32 HandleOffset, HeapType ht)
{
	//must fix ,write first
	if (ht == HeapType::SAMPLER)
	{
		mCommandList->SetGraphicsRootDescriptorTable(4, mHeapMng.GetHeapGPUHandlePos(HeapType::CBV_SRV_UAV, mShadowMap->srvOffset));
	}
	else
	{
		mCommandList->SetGraphicsRootDescriptorTable(Slot, mHeapMng.GetHeapGPUHandlePos(ht, HandleOffset));
	}
}

void DX12RHI::BindDataConstantBuffer(UINT32 Slot, UINT32 Address)
{
	mCommandList->SetGraphicsRootConstantBufferView(1, Address);
}

void DX12RHI::Bind32BitConstants(UINT32 Slot, UINT32 num, const void* data, UINT32 offset)
{
	mCommandList->SetGraphicsRoot32BitConstants(Slot, num, data, offset);
}

void DX12RHI::DrawMesh(UINT32 IndexCount)
{
	mCommandList->DrawIndexedInstanced(IndexCount, 1, 0, 0, 0);
}

Texture* DX12RHI::CreateTexture(Texture* t)
{
	auto Tex = new DX12Texture(t);
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->GetPath().c_str(),
		Tex->GetResource(), Tex->GetUploader()));
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = Tex->GetResource()->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = Tex->GetResource()->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	Tex->SetViewOffset(mHeapMng.GetDescNum(HeapType::CBV_SRV_UAV));
	md3dDevice->CreateShaderResourceView(Tex->GetResource().Get(), &srvDesc, mHeapMng.GetCPUCurrentHandleStart(HeapType::CBV_SRV_UAV));
	
	return Tex;

}

Mesh* DX12RHI::CreateMesh(Mesh* m)
{
	auto mesh = new DX12Mesh(m);
	std::vector<int> indices;
	std::vector<Vertex> vertices;
	MeshVertexInfo Vectexs = mesh->GetVertexinfo();

	for (int i = 0; i < Vectexs.mVertex.size(); i++)
	{
		vertices.push_back({ Vectexs.mVertex[i],glm::vec4(1.0f),Vectexs.mNormal[i] ,Vectexs.mTexCoord[i] });
	}

	indices.insert(indices.end(), Vectexs.mIndex.begin(), Vectexs.mIndex.end());

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(int);

	mesh->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), vertices.data(), vbByteSize, mesh->BufferUploader);
	mesh->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), indices.data(), ibByteSize, mesh->BufferUploader);

	mesh->IndexCount = (UINT32)indices.size();

	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = mesh->VertexBufferGPU->GetGPUVirtualAddress();
	vbv.StrideInBytes = sizeof(Vertex);
	vbv.SizeInBytes = vbByteSize;
	mesh->VertexBufferView = vbv;

	D3D12_INDEX_BUFFER_VIEW ibv;
	ibv.BufferLocation = mesh->IndexBufferGPU->GetGPUVirtualAddress();
	ibv.Format = mesh->IndexFormat;
	ibv.SizeInBytes = ibByteSize;
	mesh->IndexBufferView = ibv;
	return mesh;
}

Pipeline* DX12RHI::CreatePipeline(Pipeline* p)
{
	auto Pso = new DX12Pipeline(p);
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	auto il = SceneRender::Get()->GetShader(Pso->GetShaderName());
	auto dxil = dynamic_cast<DX12Shader*>(il);

	psoDesc.InputLayout = { dxil->InputLayOut.data(), (UINT)dxil->InputLayOut.size() };
	psoDesc.pRootSignature = dxil->GetRootSignature().Get();
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(dxil->GetVS()->GetBufferPointer()),
		dxil->GetVS()->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(dxil->GetPS()->GetBufferPointer()),
		dxil->GetPS()->GetBufferSize()
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
	psoDesc.SampleDesc.Count = false ? 4 : 1;
	psoDesc.SampleDesc.Quality = false ? (0 - 1) : 0;
	psoDesc.DSVFormat = mDepthStencilFormat;

	if (Pso->mType == PsoType::ShadowPSO)
	{
		psoDesc.RasterizerState.DepthBias = 5000000000;
		psoDesc.RasterizerState.DepthBiasClamp = 0.0f;
		psoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
		psoDesc.pRootSignature = mShadowRS.Get();
		psoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
		psoDesc.NumRenderTargets = 0;
	}
	Microsoft::WRL::ComPtr<ID3D12PipelineState> TempPso;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&TempPso)));
	Pso->SetPipeline(TempPso);
	return Pso;
}

Shader* DX12RHI::CreateShader(Shader* s)
{
	auto shader = new DX12Shader(s);
	auto a = s->GetShaderFilePath();
	auto vs = d3dUtil::CompileShader(s->GetShaderFilePath(), nullptr, "VS", "vs_5_0");
	auto ps = d3dUtil::CompileShader(s->GetShaderFilePath(), nullptr, "PS", "ps_5_0");
	shader->SetVS(vs);
	shader->SetPS(ps);

	Microsoft::WRL::ComPtr<ID3D12RootSignature> TempRS;

	ThrowIfFailed(md3dDevice->CreateRootSignature(
		0,
		shader->GetVS()->GetBufferPointer(),
		shader->GetVS()->GetBufferSize(),
		IID_PPV_ARGS(&TempRS)));

	shader->SetRootSignature(TempRS);
	shader->InputLayOut = mInputLayout;
	return shader;
}

Primitive* DX12RHI::CreatePrimitive(Primitive* p)
{
	auto primitive = new DX12Primitive(p);
	auto cb = std::make_shared<UploadBuffer<ConstantBuffer>>(md3dDevice.Get(), 1, true);
	primitive->SetCB(cb);
	return primitive;
}

void DX12RHI::SetPSO(Pipeline* pl)
{
	auto DXpl = dynamic_cast<DX12Pipeline*>(pl);
	auto pso = DXpl->GetPipelineState();
	mCommandList->SetPipelineState(pso.Get());
}

void DX12RHI::BuildInputLayout()
{
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
	psoDesc.pRootSignature = mCommonRS.Get();
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mCommonVS->GetBufferPointer()),
		mCommonVS->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mCommonPS->GetBufferPointer()),
		mCommonPS->GetBufferSize()
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
	psoDesc.SampleDesc.Count = false ? 4 : 1;
	psoDesc.SampleDesc.Quality = false ? (0 - 1) : 0;
	psoDesc.DSVFormat = mDepthStencilFormat;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mCommonPSO)));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC smapPsoDesc = psoDesc;
	smapPsoDesc.RasterizerState.DepthBias = 50000;
	smapPsoDesc.RasterizerState.DepthBiasClamp = 0.0f;
	smapPsoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
	smapPsoDesc.pRootSignature = mShadowRS.Get();
	smapPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShadowVS->GetBufferPointer()),
		mShadowVS->GetBufferSize()
	};
	smapPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShadowPS->GetBufferPointer()),
		mShadowPS->GetBufferSize()
	};
	smapPsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
	smapPsoDesc.NumRenderTargets = 0;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&smapPsoDesc, IID_PPV_ARGS(&mShadowPSO)));
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

	sd.SampleDesc.Count = false ? 4 : 1;                                 //填写采样模式
	sd.SampleDesc.Quality = false ? (0 - 1) : 0;


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



