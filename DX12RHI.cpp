#include "stdafx.h"
#include "DX12RHI.h"
#include "Engine.h"
#include <pix.h>

#include "DX12Pipeline.h"
#include "DX12Texture.h"
#include "DX12Mesh.h"
#include "DX12Primitive.h"
#include "DX12Shader.h"
#include "DX12Material.h"
#include "DX12RenderTarget.h"

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
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth = 1.0f;         //距离设置为最远
	optClear.DepthStencil.Stencil = 0;

	ThrowIfFailed(md3dDevice->CreateCommittedResource(          
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&optClear,
		IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));


	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;     
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = mDepthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;

	TEST_dsvoffset = mHeapMng.GetDescNum(HeapType::DSV);
	md3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, mHeapMng.GetCPUCurrentHandleStart(HeapType::DSV));

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
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
}

void DX12RHI::CloseCmdList()
{
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	FlushCommandQueue();
}

void DX12RHI::ResetViewportsAndScissorRects(RenderTarget* rt)
{
	if (rt == nullptr)
	{
		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);
		return;
	}

	auto dxrt = dynamic_cast<DX12RenderTarget*>(rt);
	mCommandList->RSSetViewports(1, &dxrt->GetViewPort());
	mCommandList->RSSetScissorRects(1, &dxrt->GetRect());
}

void DX12RHI::SetRTVAndDSV(RenderTarget* rt)
{
	float BackColor[4] = { 0.000000000f, 0.000000000f, 0.000000000f, 0.000000000f };
	if (rt == nullptr)
	{
		mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());
		return;
	}

	auto dxrt = dynamic_cast<DX12RenderTarget*>(rt);
	if (rt->GetRTType() == RtType::ShadowRt)
	{
		auto dsv = mHeapMng.GetHeapCPUHandlePos(HeapType::DSV, dxrt->GetOffset("DSV"));
		mCommandList->OMSetRenderTargets(0, nullptr, false, &dsv);
		return;
	}
	
	auto rtv = mHeapMng.GetHeapCPUHandlePos(HeapType::RTV, dxrt->GetOffset("RTV"));
	auto dsv = mHeapMng.GetHeapCPUHandlePos(HeapType::DSV, dxrt->GetOffset("DSV"));
	mCommandList->OMSetRenderTargets(1, &rtv, true, &dsv);

}

void DX12RHI::SetRootSignature(Shader* s)
{
	auto ds = dynamic_cast<DX12Shader*>(s);
	auto rs = ds->GetRootSignature();
	mCommandList->SetGraphicsRootSignature(rs.Get());
}

void DX12RHI::ChangeResState()
{
	/*mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mShadowMap->mShadowMap.Get(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));*/
}

void DX12RHI::ChangeResState(BufferState CurrnetState, BufferState ChangeState, RenderTarget* rt, RTBufferType type)
{

	if (rt == nullptr)
	{
		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATES(CurrnetState), D3D12_RESOURCE_STATES(ChangeState)));
		return;
	}
	auto dxrt = dynamic_cast<DX12RenderTarget*>(rt);
	auto buffer = dxrt->GetBuffer(type);

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(),
		D3D12_RESOURCE_STATES(CurrnetState), D3D12_RESOURCE_STATES(ChangeState)));
}

void DX12RHI::ClearRenderTarget(RenderTarget* rt)
{
	const FLOAT Color[4] = { 0.0f,0.0f,0.0f,0.0f };
	if (rt == nullptr)
	{
		mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Color, 0, nullptr);
		return;
	}
	auto dxrt = dynamic_cast<DX12RenderTarget*>(rt);
	auto rtv = mHeapMng.GetHeapCPUHandlePos(HeapType::RTV, dxrt->GetOffset("RTV"));
	mCommandList->ClearRenderTargetView(rtv, Color, 0, nullptr);
}

void DX12RHI::ClearDepthStencil(RenderTarget* rt)
{
	if (rt == nullptr)
	{
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		return;
	}
	auto dxrt = dynamic_cast<DX12RenderTarget*>(rt);
	auto dsv = mHeapMng.GetHeapCPUHandlePos(HeapType::DSV, dxrt->GetOffset("DSV"));
	mCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
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
	ThrowIfFailed(mDirectCmdListAlloc->Reset());
	OpenCmdList();
	SetDescHeap(HeapType::CBV_SRV_UAV);
}

void DX12RHI::EndFrame()
{
	CloseCmdList();
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrentBackBuffer = (mCurrentBackBuffer + 1) % SwapChainBufferCount;
	FlushCommandQueue();
}

void DX12RHI::BindDataTable(UINT32 Slot, UINT32 HandleOffset, HeapType ht)
{
	mCommandList->SetGraphicsRootDescriptorTable(Slot, mHeapMng.GetHeapGPUHandlePos(ht, HandleOffset));
}

void DX12RHI::BindDataConstantBuffer(UINT32 Slot, UINT32 Address)
{
	mCommandList->SetGraphicsRootConstantBufferView(Slot, Address);
}

void DX12RHI::Bind32BitConstants(UINT32 Slot, UINT32 num, const void* data, UINT32 offset)
{
	mCommandList->SetGraphicsRoot32BitConstants(Slot, num, data, offset);
}

void DX12RHI::DrawMesh(UINT32 IndexCount)
{
	mCommandList->DrawIndexedInstanced(IndexCount, 1, 0, 0, 0);
}

void DX12RHI::EventBegin(std::string EventName)
{
	PIXBeginEvent(mCommandList.Get(), 0, EventName.c_str());
}

void DX12RHI::EventEnd()
{
	PIXEndEvent(mCommandList.Get());
}

std::shared_ptr<Texture> DX12RHI::CreateTexture(Texture* t)
{
	auto Tex = std::make_shared<DX12Texture>(t);
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

std::shared_ptr<Mesh> DX12RHI::CreateMesh(std::shared_ptr<Mesh> m)
{
	auto mesh = std::make_shared<DX12Mesh>(m.get());
	std::vector<int> indices;
	std::vector<Vertex> vertices;
	MeshVertexInfo Vectexs = mesh->GetVertexinfo();

	for (int i = 0; i < Vectexs.mVertex.size(); i++)
	{
		vertices.push_back({ Vectexs.mVertex[i],Vectexs.mTangent[i],Vectexs.mNormal[i] ,Vectexs.mTexCoord[i] });
	}

	indices.insert(indices.end(), Vectexs.mIndex.begin(), Vectexs.mIndex.end());

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(int);

	mesh->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), vertices.data(), vbByteSize, mesh->BufferUploader);
	mesh->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), indices.data(), ibByteSize, mesh->indexUploader);

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

std::shared_ptr<Pipeline> DX12RHI::CreatePipeline(Pipeline* p)
{
	auto Pso = std::make_shared<DX12Pipeline>(p);
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	auto il = SceneRender::Get()->GetShader(Pso->GetShaderName());
	auto dxil = dynamic_cast<DX12Shader*>(il.get());

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
	psoDesc.RTVFormats[0] = DXGI_FORMAT(Pso->mFormat);
	psoDesc.SampleDesc.Count = false ? 4 : 1;
	psoDesc.SampleDesc.Quality = false ? (0 - 1) : 0;
	psoDesc.DSVFormat = mDepthStencilFormat;

	auto test = Pso->mType;


	if (Pso->mType == PsoType::ShadowPSO)
	{
		psoDesc.RasterizerState.DepthBias = 10000.0f;
		psoDesc.RasterizerState.DepthBiasClamp = 0.0f;
		psoDesc.RasterizerState.SlopeScaledDepthBias = 0.5f;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
		psoDesc.NumRenderTargets = 0;
	}

	Microsoft::WRL::ComPtr<ID3D12PipelineState> TempPso;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&TempPso)));
	Pso->SetPipeline(TempPso);
	return Pso;
}

std::shared_ptr<Shader> DX12RHI::CreateShader(Shader* s)
{
	auto shader = std::make_shared<DX12Shader>(s);

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

std::shared_ptr<Primitive> DX12RHI::CreatePrimitive(Primitive* p)
{
	auto primitive = std::make_shared <DX12Primitive>(p);
	auto cb = std::make_shared<UploadBuffer<ConstantBuffer>>(md3dDevice.Get(), 1, true);
	primitive->SetCB(cb);
	return primitive;
}

std::shared_ptr<Material> DX12RHI::CreateMaterial(Material* m)
{
	auto material = std::make_shared <DX12Material>(m);
	auto cb = std::make_shared<UploadBuffer<MatData>>(md3dDevice.Get(), 1, true);
	material->SetCB(cb);
	return material;
}

std::shared_ptr<Light> DX12RHI::CreateLight()
{
	auto light = std::make_shared <Light>();
	light->CB = std::make_shared<UploadBuffer<LightData>>(md3dDevice.Get(), 1, true);
	return light;
}

std::shared_ptr<RenderTarget> DX12RHI::CreateRenderTarget(std::string name, UINT32 width, UINT32 height, int num, RtType type)
{
	return std::make_shared<DX12RenderTarget>(name, width, height, num, type);
}

void DX12RHI::UpdateRenderTarget(std::shared_ptr<RenderTarget> rt, ColorFormat format)
{
	auto dxrt = dynamic_cast<DX12RenderTarget*>(rt.get());
	const FLOAT Color[4] = { 0.0f,0.0f,0.0f,0.0f };
	CD3DX12_CLEAR_VALUE optClear(DXGI_FORMAT(format), Color);
	if (rt->GetRTType() != RtType::ShadowRt)
	{
		D3D12_RESOURCE_DESC RTdesc;
		RTdesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		RTdesc.Alignment = 0;
		RTdesc.Width = dxrt->GetWidth();
		RTdesc.Height = dxrt->GetHeight();
		RTdesc.DepthOrArraySize = 1;
		RTdesc.MipLevels = 1;
		RTdesc.Format = DXGI_FORMAT(format);
		RTdesc.SampleDesc.Count = false ? 4 : 1;
		RTdesc.SampleDesc.Quality = false ? (0 - 1) : 0;
		RTdesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		RTdesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		
		optClear.Format = DXGI_FORMAT(format);
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;
		optClear.Color[0] = Color[0];
		optClear.Color[1] = Color[1];
		optClear.Color[2] = Color[2];
		optClear.Color[3] = Color[3];

		ThrowIfFailed(md3dDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&RTdesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			IID_PPV_ARGS(dxrt->GetRTBuffer().GetAddressOf())));

		//create rtv view
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Format = DXGI_FORMAT(format);
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Texture2D.PlaneSlice = 0;

		auto rtvoffset = mHeapMng.GetDescNum(HeapType::RTV);
		dxrt->AddOffset("RTV", rtvoffset);
		md3dDevice->CreateRenderTargetView(dxrt->GetRTBuffer().Get(), &rtvDesc, mHeapMng.GetCPUCurrentHandleStart(HeapType::RTV));

		//create srv view
		D3D12_SHADER_RESOURCE_VIEW_DESC rtsrvDesc = {};
		rtsrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		rtsrvDesc.Format = DXGI_FORMAT(format);
		rtsrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		rtsrvDesc.Texture2D.MostDetailedMip = 0;
		rtsrvDesc.Texture2D.MipLevels = 1;
		rtsrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		rtsrvDesc.Texture2D.PlaneSlice = 0;
		auto srvoffset = mHeapMng.GetDescNum(HeapType::CBV_SRV_UAV);
		dxrt->AddOffset("RTSRV", srvoffset);
		md3dDevice->CreateShaderResourceView(dxrt->GetRTBuffer().Get(), &rtsrvDesc, mHeapMng.GetCPUCurrentHandleStart(HeapType::CBV_SRV_UAV));
	}
	

	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = dxrt->GetWidth();
	depthStencilDesc.Height = dxrt->GetHeight();
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilDesc.SampleDesc.Count = false ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = false ? (0 - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&optClear,
		IID_PPV_ARGS(dxrt->GetDSBuffer().GetAddressOf())));


	// create dsv view
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;

	auto dsvoffset = mHeapMng.GetDescNum(HeapType::DSV);
	dxrt->AddOffset("DSV", dsvoffset);
	md3dDevice->CreateDepthStencilView(dxrt->GetDSBuffer().Get(), &dsvDesc, mHeapMng.GetCPUCurrentHandleStart(HeapType::DSV));  
	

	D3D12_SHADER_RESOURCE_VIEW_DESC dssrvDesc = {};
	dssrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	dssrvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	dssrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	dssrvDesc.Texture2D.MostDetailedMip = 0;
	dssrvDesc.Texture2D.MipLevels = 1;
	dssrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	dssrvDesc.Texture2D.PlaneSlice = 0;

	auto dssrvoffset = mHeapMng.GetDescNum(HeapType::CBV_SRV_UAV);
	dxrt->AddOffset("DSSRV", dssrvoffset);
	md3dDevice->CreateShaderResourceView(dxrt->GetDSBuffer().Get(), &dssrvDesc, mHeapMng.GetCPUCurrentHandleStart(HeapType::CBV_SRV_UAV));
	
	auto w = dxrt->GetWidth();
	auto h = dxrt->GetHeight();

	D3D12_VIEWPORT vp = { 0.0f, 0.0f, (float)w, (float)h, 0.0f, 1.0f };
	D3D12_RECT sr = { 0, 0, w, h };
	dxrt->SetViewportAndRect(vp,sr);

 	dxrt->mSize = glm::vec2{ (float)w,(float)h };
}

void DX12RHI::SetPSO(Pipeline* pl)
{
	auto DXpl = dynamic_cast<DX12Pipeline*>(pl);
	auto pso = DXpl->GetPipelineState();
	mCommandList->SetPipelineState(pso.Get());
}

void DX12RHI::ReSetCmdAlloc()
{
	ThrowIfFailed(mDirectCmdListAlloc->Reset());
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



