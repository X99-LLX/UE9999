#include "stdafx.h"
#include "Render.h"
#include "Engine.h"

#ifdef _RHI_DX12
#include "DX12RHI.h"
#include "DX12Primitive.h"
#include "DX12Texture.h"
#include "DX12Material.h"
#include "DX12RenderTarget.h"
#endif 


Render::Render()
{
	mRHI = CreateRHI();
}

Render::~Render()
{
	delete mRHI;
	mRHI = nullptr;
}

RHI* Render::GetRHI()
{
	return mRHI;
}

void Render::BuildResource()
{
	mRHI->OpenCmdList();

	TestTriangle();

	auto resmng = ResourceManage::Get();
	for (auto shader : resmng->ShaderAsset)
	{
		auto a = shader.second.use_count();
		auto s = mRHI->CreateShader(shader.second.get());
		mSceneRender.AddShader(s->GetShaderName(),s);
	}
	for (auto pso : resmng->PipelineAsset)
	{
		auto p = mRHI->CreatePipeline(pso.second.get());
		mSceneRender.AddPipeline(p->GetPsoName(), p);
	}
	for (auto material : resmng->MaterialAsset)
	{
		auto m = mRHI->CreateMaterial(material.second.get());
		mSceneRender.AddMaterial(m->GetName(), m);
	}
	for (auto texture : resmng->TextureAsset)
	{
		auto t = mRHI->CreateTexture(texture.second.get());
		mSceneRender.AddTexture(t->GetName(), t);
	}
	for (auto mesh : resmng->MeshAsset)
	{
		auto m = mRHI->CreateMesh(mesh.second);
		mSceneRender.AddMesh(m->GetName(), m);
	}
	auto scene = Engine::GetEngine()->GetScene();
	for (auto prim : scene->mPrimitives)
	{
		auto p = mRHI->CreatePrimitive(prim.get());
		mSceneRender.AddPrimitive(p);
	}
	scene->mLight = mRHI->CreateLight();
	scene->mLight->CB->CopyData(0, scene->mLight->mLightData);

	auto shadowRT = mRHI->CreateRenderTarget("Shadow", 4096, 4096, 0, RtType::ShadowRt);
	mRHI->UpdateRenderTarget(shadowRT,ColorFormat::DXGI_FORMAT_A8_UNORM);
	mSceneRender.AddRenderTarget(shadowRT->GetRenderTargetName(), shadowRT);

	auto ColorRT = mRHI->CreateRenderTarget("Color", 1920, 1080, 0, RtType::BaseRt);
	mRHI->UpdateRenderTarget(ColorRT, ColorFormat::DXGI_FORMAT_R16G16B16A16_FLOAT);
	mSceneRender.AddRenderTarget(ColorRT->GetRenderTargetName(), ColorRT);

	auto SetUpRT = mRHI->CreateRenderTarget("BloomSetUp", 480, 270, 0, RtType::BaseRt);
	mRHI->UpdateRenderTarget(SetUpRT, ColorFormat::DXGI_FORMAT_R16G16B16A16_FLOAT);
	mSceneRender.AddRenderTarget(SetUpRT->GetRenderTargetName(), SetUpRT);

	auto DownRT = mRHI->CreateRenderTarget("BloomDown", 240, 135, 0, RtType::BaseRt);
	mRHI->UpdateRenderTarget(DownRT, ColorFormat::DXGI_FORMAT_R16G16B16A16_FLOAT);
	mSceneRender.AddRenderTarget(DownRT->GetRenderTargetName(), DownRT);

	auto DownRT1 = mRHI->CreateRenderTarget("BloomDown1", 120, 67, 0, RtType::BaseRt);
	mRHI->UpdateRenderTarget(DownRT1, ColorFormat::DXGI_FORMAT_R16G16B16A16_FLOAT);
	mSceneRender.AddRenderTarget(DownRT1->GetRenderTargetName(), DownRT1);

	auto DownRT2 = mRHI->CreateRenderTarget("BloomDown2", 60, 33, 0, RtType::BaseRt);
	mRHI->UpdateRenderTarget(DownRT2, ColorFormat::DXGI_FORMAT_R16G16B16A16_FLOAT);
	mSceneRender.AddRenderTarget(DownRT2->GetRenderTargetName(), DownRT2);

	auto UpRT = mRHI->CreateRenderTarget("BloomUp", 120, 67, 0, RtType::BaseRt);
	mRHI->UpdateRenderTarget(UpRT, ColorFormat::DXGI_FORMAT_R16G16B16A16_FLOAT);
	mSceneRender.AddRenderTarget(UpRT->GetRenderTargetName(), UpRT);

	auto UpRT1 = mRHI->CreateRenderTarget("BloomUp1", 240, 135, 0, RtType::BaseRt);
	mRHI->UpdateRenderTarget(UpRT1, ColorFormat::DXGI_FORMAT_R16G16B16A16_FLOAT);
	mSceneRender.AddRenderTarget(UpRT1->GetRenderTargetName(), UpRT1);

	auto Mergeps = mRHI->CreateRenderTarget("BloomMergeps", 480, 270, 0, RtType::BaseRt);
	mRHI->UpdateRenderTarget(Mergeps, ColorFormat::DXGI_FORMAT_R16G16B16A16_FLOAT);
	mSceneRender.AddRenderTarget(Mergeps->GetRenderTargetName(), Mergeps);

	auto Mix = mRHI->CreateRenderTarget("BloomMix", 1920, 1080, 0, RtType::BaseRt);
	mRHI->UpdateRenderTarget(Mix, ColorFormat::DXGI_FORMAT_R16G16B16A16_FLOAT);
	mSceneRender.AddRenderTarget(Mix->GetRenderTargetName(), Mix);

	auto Cyberpunk = mRHI->CreateRenderTarget("Cyberpunk", 1920, 1080, 0, RtType::BaseRt);
	mRHI->UpdateRenderTarget(Cyberpunk, ColorFormat::DXGI_FORMAT_R16G16B16A16_FLOAT);
	mSceneRender.AddRenderTarget(Cyberpunk->GetRenderTargetName(), Cyberpunk);

	auto Water = mRHI->CreateRenderTarget("Water", 1920, 1080, 0, RtType::BaseRt);
	mRHI->UpdateRenderTarget(Water, ColorFormat::DXGI_FORMAT_R16G16B16A16_FLOAT);
	mSceneRender.AddRenderTarget(Water->GetRenderTargetName(), Water);

	auto Kernel = mRHI->CreateRenderTarget("Kernel", 1920, 1080, 0, RtType::BaseRt);
	mRHI->UpdateRenderTarget(Kernel, ColorFormat::DXGI_FORMAT_R16G16B16A16_FLOAT);
	mSceneRender.AddRenderTarget(Kernel->GetRenderTargetName(), Kernel);

	auto a = DescHeapManager::Get();
	mRHI->CloseCmdList();
}

void Render::UpdateRenderData()
{
	for (auto pri : mSceneRender.GetPrimitive())
	{
		UpdatePrimitiveMVP(pri.get());
	}
}

void Render::UpdatePrimitiveMVP(Primitive* p)
{
	auto mCamera = Engine::GetEngine()->GetScene()->mCamera;
	auto mLight = Engine::GetEngine()->GetScene()->mLight.get();
	mCamera.UpdateViewMatrix();
	auto& Trans = p->GetTransform();
	auto world = glm::translate(glm::mat4(1.0f), Trans.Translation);
	auto rotate = glm::mat4_cast(glm::qua<float>(Trans.Rotation.w, Trans.Rotation.x,
		Trans.Rotation.y, Trans.Rotation.z));
	auto scale = glm::scale(glm::mat4(1.0f), Trans.Scale3D);
	auto cmvp = mCamera.GetProj() * mCamera.GetView() * world * rotate * scale;
	auto ltvp = lightT * mLight->GetProj() * mLight->GetView();
	auto lmvp = mLight->GetProj() * mLight->GetView() * world * rotate * scale;
	world = world * rotate * scale;

	ConstantBuffer ocb;
	ocb.LightTVP = glm::transpose(ltvp);
	ocb.LightMVP = glm::transpose(lmvp);
	ocb.World = glm::transpose(world);
	ocb.CameraMVP = glm::transpose(cmvp);
	
#ifdef _RHI_DX12
	auto dp = dynamic_cast<DX12Primitive*>(p);
	dp->SetWorldTrans(cmvp);
	dp->GetCB()->CopyData(0, ocb);
#endif
}

void Render::TestDraw()
{
	mRHI->BeginFrame();
	UpdateRenderData();
	ShadowPass();
	BasePass();

	mRHI->EventBegin("PostProcess");
	BloomPass();
	CyberpunkPass();
	WaterPass();
	KernelPass();
	mRHI->EventEnd();

	ShowColorBufferPass();
	mRHI->EndFrame();
}

void Render::BasePass()
{
	mRHI->EventBegin("-----BasePass-----");
	auto brt = mSceneRender.GetRenderTarget("Color");
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_COMMON, BufferState::D3D12_RESOURCE_STATE_RENDER_TARGET, brt.get(), RTBufferType::ColorBuffer);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, brt.get(), RTBufferType::DepthBuffer);
	mRHI->ResetViewportsAndScissorRects(brt.get());
	
	//mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, srt.get(), RTBufferType::DepthBuffer);

	mRHI->ClearRenderTarget(brt.get());
	mRHI->ClearDepthStencil(brt.get());
	mRHI->SetRTVAndDSV(brt.get());
	auto actors = mSceneRender.GetPrimitive();
	for (auto actor : actors)
	{
		auto Mesh = mSceneRender.GetMesh(actor->GetMeshName());
		auto Material = mSceneRender.GetMaterial(Mesh->GetMaterialName());
		auto Pipeline = mSceneRender.GetPipeline(Material->GetPipelineName());
		auto Shader = mSceneRender.GetShader(Pipeline->GetShaderName());
		auto Texture = mSceneRender.GetTexture(Material->GetTextures().at(0));
		auto NorTex = mSceneRender.GetTexture(Material->GetTextures().at(1));
		mRHI->SetRootSignature(Shader.get());
		mRHI->SetPSO(Pipeline.get());
		mRHI->InputAssetInfo(Mesh.get());
#ifdef _RHI_DX12   //bind res
		auto camera = Engine::GetEngine()->GetScene()->mCamera.GetCameraPos();
		mRHI->Bind32BitConstants(0, 3, &camera, 0);
		auto dp = dynamic_cast<DX12Primitive*>(actor.get());
		auto address = dp->GetCB()->Resource()->GetGPUVirtualAddress();
		mRHI->BindDataConstantBuffer(1, address);

		auto dm = dynamic_cast<DX12Material*>(Material.get());
		auto& test = dm->GetMatData();
		dm->GetCB()->CopyData(0, test);
		auto address1 = dm->GetCB()->Resource()->GetGPUVirtualAddress();
		mRHI->BindDataConstantBuffer(2, address1);

		auto light = Engine::GetEngine()->GetScene()->mLight;
		auto address2 = light->CB->Resource()->GetGPUVirtualAddress();
		mRHI->BindDataConstantBuffer(3, address2);

		auto dt = dynamic_cast<DX12Texture*>(Texture.get());

		mRHI->BindDataTable(4, dt->GetViewOffset(), HeapType::CBV_SRV_UAV);

		auto srt = mSceneRender.GetRenderTarget("Shadow");
		auto dxsrt = dynamic_cast<DX12RenderTarget*>(srt.get());

		mRHI->BindDataTable(5, dxsrt->GetOffset("DSSRV"), HeapType::CBV_SRV_UAV);

		auto dnt = dynamic_cast<DX12Texture*>(NorTex.get());
		mRHI->BindDataTable(6, dnt->GetViewOffset(), HeapType::CBV_SRV_UAV);
#endif 
		auto indexCount = Mesh->IndexCount;
		mRHI->DrawMesh(indexCount);
	}
	//mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, srt.get(), RTBufferType::DepthBuffer);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_RENDER_TARGET, BufferState::D3D12_RESOURCE_STATE_COMMON, brt.get(), RTBufferType::ColorBuffer);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, brt.get(), RTBufferType::DepthBuffer);


	mRHI->EventEnd();
}

void Render::BloomPass()
{
	mRHI->EventBegin("-----BloomPass-----");
	SetUpPass();
	BloomDown("BloomDown", "BloomSetUp");
	BloomDown("BloomDown1", "BloomDown");
	BloomDown("BloomDown2", "BloomDown1");
	BloomUp("BloomDown2", "BloomDown1", "BloomUp");
	BloomUp("BloomUp", "BloomDown", "BloomUp1");
	BloomMergeps("BloomSetUp", "BloomMergeps");
	BloomMix("Color");
	mRHI->EventEnd();
}

void Render::CyberpunkPass()
{
	mRHI->EventBegin("-----CyberpunkPass-----");
	auto bdrt = mSceneRender.GetRenderTarget("Cyberpunk");
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_COMMON, BufferState::D3D12_RESOURCE_STATE_RENDER_TARGET, bdrt.get(), RTBufferType::ColorBuffer);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, bdrt.get(), RTBufferType::DepthBuffer);

	mRHI->ResetViewportsAndScissorRects(bdrt.get());
	mRHI->ClearRenderTarget(bdrt.get());
	mRHI->ClearDepthStencil(bdrt.get());
	mRHI->SetRTVAndDSV(bdrt.get());
#ifdef _RHI_DX12 
 	auto Shader = mSceneRender.GetShader("CyberpunkShader");
 	auto Pipeline = mSceneRender.GetPipeline("CyberpunkPSO");
	auto Mesh = mTriangle;
 	mRHI->SetRootSignature(Shader.get());
 	mRHI->SetPSO(Pipeline.get());
	mRHI->InputAssetInfo(Mesh.get());
 
 	auto srrt = mSceneRender.GetRenderTarget(mCurrentColorBuffer);
 
 	auto dxsrrt = dynamic_cast<DX12RenderTarget*>(srrt.get());
 	mRHI->BindDataTable(0, dxsrrt->GetOffset("RTSRV"), HeapType::CBV_SRV_UAV);
 	auto size = dxsrrt->mSize;
 	mRHI->Bind32BitConstants(2, 2, &size, 0);
 	glm::vec2 Offset;
 	Offset.x = Engine::GetEngine()->GetTimer()->TotalTime();
 	Offset.y = Engine::GetEngine()->GetTimer()->DeltaTime();
 	mRHI->Bind32BitConstants(2, 2, &Offset, 2);
#endif
	auto indexCount = Mesh->IndexCount;
	mRHI->DrawMesh(indexCount);

	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_RENDER_TARGET, BufferState::D3D12_RESOURCE_STATE_COMMON, bdrt.get(), RTBufferType::ColorBuffer);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, bdrt.get(), RTBufferType::DepthBuffer);


	mRHI->EventEnd();
	mCurrentColorBuffer = "Cyberpunk";

}

void Render::WaterPass()
{
	mRHI->EventBegin("-----WaterPass-----");
	auto bdrt = mSceneRender.GetRenderTarget("Water");
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_COMMON, BufferState::D3D12_RESOURCE_STATE_RENDER_TARGET, bdrt.get(), RTBufferType::ColorBuffer);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, bdrt.get(), RTBufferType::DepthBuffer);

	mRHI->ResetViewportsAndScissorRects(bdrt.get());
	mRHI->ClearRenderTarget(bdrt.get());
	mRHI->ClearDepthStencil(bdrt.get());
	mRHI->SetRTVAndDSV(bdrt.get());
#ifdef _RHI_DX12 
	auto Shader = mSceneRender.GetShader("Water");
	auto Pipeline = mSceneRender.GetPipeline("WaterPSO");
	auto Mesh = mTriangle;
	mRHI->SetRootSignature(Shader.get());
	mRHI->SetPSO(Pipeline.get());
	mRHI->InputAssetInfo(Mesh.get());

	auto srrt = mSceneRender.GetRenderTarget(mCurrentColorBuffer);

	auto dxsrrt = dynamic_cast<DX12RenderTarget*>(srrt.get());
	mRHI->BindDataTable(0, dxsrrt->GetOffset("RTSRV"), HeapType::CBV_SRV_UAV);
	auto size = dxsrrt->mSize;
	mRHI->Bind32BitConstants(2, 2, &size, 0);
	glm::vec2 Offset;
	Offset.x = Engine::GetEngine()->GetTimer()->TotalTime();
	Offset.y = Engine::GetEngine()->GetTimer()->DeltaTime();
	mRHI->Bind32BitConstants(2, 2, &Offset, 2);
#endif
	auto indexCount = Mesh->IndexCount;
	mRHI->DrawMesh(indexCount);

	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_RENDER_TARGET, BufferState::D3D12_RESOURCE_STATE_COMMON, bdrt.get(), RTBufferType::ColorBuffer);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, bdrt.get(), RTBufferType::DepthBuffer);


	mRHI->EventEnd();
	mCurrentColorBuffer = "Water";
}

void Render::KernelPass()
{
	mRHI->EventBegin("-----KernelPass-----");
	auto bdrt = mSceneRender.GetRenderTarget("Kernel");
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_COMMON, BufferState::D3D12_RESOURCE_STATE_RENDER_TARGET, bdrt.get(), RTBufferType::ColorBuffer);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, bdrt.get(), RTBufferType::DepthBuffer);

	mRHI->ResetViewportsAndScissorRects(bdrt.get());
	mRHI->ClearRenderTarget(bdrt.get());
	mRHI->ClearDepthStencil(bdrt.get());
	mRHI->SetRTVAndDSV(bdrt.get());
#ifdef _RHI_DX12 
	auto Shader = mSceneRender.GetShader("Kernel");
	auto Pipeline = mSceneRender.GetPipeline("KernelPSO");
	auto Mesh = mTriangle;
	mRHI->SetRootSignature(Shader.get());
	mRHI->SetPSO(Pipeline.get());
	mRHI->InputAssetInfo(Mesh.get());

	auto srrt = mSceneRender.GetRenderTarget(mCurrentColorBuffer);

	auto dxsrrt = dynamic_cast<DX12RenderTarget*>(srrt.get());
	mRHI->BindDataTable(0, dxsrrt->GetOffset("RTSRV"), HeapType::CBV_SRV_UAV);
	auto size = dxsrrt->mSize;
	mRHI->Bind32BitConstants(2, 2, &size, 0);
	glm::vec2 Offset;
	Offset.x = Engine::GetEngine()->GetTimer()->TotalTime();
	Offset.y = Engine::GetEngine()->GetTimer()->DeltaTime();
	mRHI->Bind32BitConstants(2, 2, &Offset, 2);
#endif
	auto indexCount = Mesh->IndexCount;
	mRHI->DrawMesh(indexCount);

	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_RENDER_TARGET, BufferState::D3D12_RESOURCE_STATE_COMMON, bdrt.get(), RTBufferType::ColorBuffer);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, bdrt.get(), RTBufferType::DepthBuffer);

	mRHI->EventEnd();
	mCurrentColorBuffer = "Kernel";
}

void Render::SetUpPass()
{
	mRHI->EventBegin("-----BloomSetUp-----");
	auto bsurt = mSceneRender.GetRenderTarget("BloomSetUp");
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_COMMON, BufferState::D3D12_RESOURCE_STATE_RENDER_TARGET, bsurt.get(), RTBufferType::ColorBuffer);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, bsurt.get(), RTBufferType::DepthBuffer);

	mRHI->ResetViewportsAndScissorRects(bsurt.get());
	mRHI->ClearRenderTarget(bsurt.get());
	mRHI->ClearDepthStencil(bsurt.get());
	mRHI->SetRTVAndDSV(bsurt.get());

#ifdef _RHI_DX12   //bind res
	auto Shader = mSceneRender.GetShader("BloomSetUpShader");
	auto Pipeline = mSceneRender.GetPipeline("SetUpPSO");
	auto Mesh = mTriangle;

	mRHI->SetRootSignature(Shader.get());
	mRHI->SetPSO(Pipeline.get());
	mRHI->InputAssetInfo(Mesh.get());

	auto colorrt = mSceneRender.GetRenderTarget(mCurrentColorBuffer);
	auto dxcolorrt = dynamic_cast<DX12RenderTarget*>(colorrt.get());
	mRHI->BindDataTable(0, dxcolorrt->GetOffset("RTSRV"), HeapType::CBV_SRV_UAV);
	auto size = dxcolorrt->mSize;
	mRHI->Bind32BitConstants(1, 2, &size, 0);

#endif 
	auto indexCount = Mesh->IndexCount;
	mRHI->DrawMesh(indexCount);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_RENDER_TARGET, BufferState::D3D12_RESOURCE_STATE_COMMON, bsurt.get(), RTBufferType::ColorBuffer);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, bsurt.get(), RTBufferType::DepthBuffer);

	mRHI->EventEnd();
	mCurrentColorBuffer = "BloomSetUp";
}

void Render::BloomDown(std::string rtname, std::string srname)
{
	mRHI->EventBegin("-----BloomDown-----");
	auto bdrt = mSceneRender.GetRenderTarget(rtname);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_COMMON, BufferState::D3D12_RESOURCE_STATE_RENDER_TARGET, bdrt.get(), RTBufferType::ColorBuffer);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, bdrt.get(), RTBufferType::DepthBuffer);

	mRHI->ResetViewportsAndScissorRects(bdrt.get());
	mRHI->ClearRenderTarget(bdrt.get());
	mRHI->ClearDepthStencil(bdrt.get());
	mRHI->SetRTVAndDSV(bdrt.get());

#ifdef _RHI_DX12   //bind res
	auto Shader = mSceneRender.GetShader("BloomDown");
	auto Pipeline = mSceneRender.GetPipeline("DownPSO");
	auto Mesh = mTriangle;

	mRHI->SetRootSignature(Shader.get());
	mRHI->SetPSO(Pipeline.get());
	mRHI->InputAssetInfo(Mesh.get());

	auto colorrt = mSceneRender.GetRenderTarget(mCurrentColorBuffer);
	auto dxcolorrt = dynamic_cast<DX12RenderTarget*>(colorrt.get());
	mRHI->BindDataTable(0, dxcolorrt->GetOffset("RTSRV"), HeapType::CBV_SRV_UAV);
	auto size = dxcolorrt->mSize;
	mRHI->Bind32BitConstants(1, 2, &size, 0);

#endif 

	auto indexCount = Mesh->IndexCount;
	mRHI->DrawMesh(indexCount);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_RENDER_TARGET, BufferState::D3D12_RESOURCE_STATE_COMMON, bdrt.get(), RTBufferType::ColorBuffer);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, bdrt.get(), RTBufferType::DepthBuffer);

	mRHI->EventEnd();
	mCurrentColorBuffer = rtname;
}

void Render::BloomUp(std::string minRt, std::string OtherRt, std::string UpRT)
{
	mRHI->EventBegin("-----BloomUp-----");
	auto rt = mSceneRender.GetRenderTarget(UpRT);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_COMMON, BufferState::D3D12_RESOURCE_STATE_RENDER_TARGET, rt.get(), RTBufferType::ColorBuffer);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, rt.get(), RTBufferType::DepthBuffer);

	mRHI->ResetViewportsAndScissorRects(rt.get());
	mRHI->ClearRenderTarget(rt.get());
	mRHI->ClearDepthStencil(rt.get());
	mRHI->SetRTVAndDSV(rt.get());
#ifdef _RHI_DX12 
	auto Shader = mSceneRender.GetShader("BloomUp");
	auto Pipeline = mSceneRender.GetPipeline("UpPSO");
	auto Mesh = mTriangle;

	mRHI->SetRootSignature(Shader.get());
	mRHI->SetPSO(Pipeline.get());
	mRHI->InputAssetInfo(Mesh.get());

	auto otherrt = mSceneRender.GetRenderTarget(OtherRt);
	auto dxotherrt = dynamic_cast<DX12RenderTarget*>(otherrt.get());
	mRHI->BindDataTable(0, dxotherrt->GetOffset("RTSRV"), HeapType::CBV_SRV_UAV);
	glm::vec2 size = dxotherrt->mSize;
	mRHI->Bind32BitConstants(2, 2, &size, 0);

	auto minrt = mSceneRender.GetRenderTarget(minRt);
	auto dxminrt = dynamic_cast<DX12RenderTarget*>(minrt.get());
	mRHI->BindDataTable(1, dxminrt->GetOffset("RTSRV"), HeapType::CBV_SRV_UAV);
	size = dxminrt->mSize;
	mRHI->Bind32BitConstants(2, 2, &size, 2);
#endif
	auto indexCount = Mesh->IndexCount;
	mRHI->DrawMesh(indexCount);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_RENDER_TARGET, BufferState::D3D12_RESOURCE_STATE_COMMON, rt.get(), RTBufferType::ColorBuffer);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, rt.get(), RTBufferType::DepthBuffer);

	mRHI->EventEnd();
	mCurrentColorBuffer = UpRT;
}

void Render::BloomMergeps(std::string OtherRt, std::string UpRT)
{
	mRHI->EventBegin("-----BloomMergeps-----");
	auto rt = mSceneRender.GetRenderTarget(UpRT);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_COMMON, BufferState::D3D12_RESOURCE_STATE_RENDER_TARGET, rt.get(), RTBufferType::ColorBuffer);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, rt.get(), RTBufferType::DepthBuffer);

	mRHI->ResetViewportsAndScissorRects(rt.get());
	mRHI->ClearRenderTarget(rt.get());
	mRHI->ClearDepthStencil(rt.get());
	mRHI->SetRTVAndDSV(rt.get());
#ifdef _RHI_DX12 
	auto Shader = mSceneRender.GetShader("BloomMergeps");
	auto Pipeline = mSceneRender.GetPipeline("MergepsPSO");
	auto Mesh = mTriangle;

	mRHI->SetRootSignature(Shader.get());
	mRHI->SetPSO(Pipeline.get());
	mRHI->InputAssetInfo(Mesh.get());

	auto otherrt = mSceneRender.GetRenderTarget(mCurrentColorBuffer);
	auto dxotherrt = dynamic_cast<DX12RenderTarget*>(otherrt.get());
	mRHI->BindDataTable(0, dxotherrt->GetOffset("RTSRV"), HeapType::CBV_SRV_UAV);
	auto size = dxotherrt->mSize;
	size.x = 4;
	size.y = 80;
	mRHI->Bind32BitConstants(2, 2, &size, 2);

	auto minrt = mSceneRender.GetRenderTarget(OtherRt);
	auto dxminrt = dynamic_cast<DX12RenderTarget*>(minrt.get());
	mRHI->BindDataTable(1, dxminrt->GetOffset("RTSRV"), HeapType::CBV_SRV_UAV);
	size = dxminrt->mSize;
	mRHI->Bind32BitConstants(2, 2, &size, 0);
#endif
	auto indexCount = Mesh->IndexCount;
	mRHI->DrawMesh(indexCount);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_RENDER_TARGET, BufferState::D3D12_RESOURCE_STATE_COMMON, rt.get(), RTBufferType::ColorBuffer);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, rt.get(), RTBufferType::DepthBuffer);

	mRHI->EventEnd();
	mCurrentColorBuffer = UpRT;
}

void Render::BloomMix(std::string OtherRt)
{
	mRHI->EventBegin("-----BloomMix-----");
	auto rt = mSceneRender.GetRenderTarget("BloomMix");
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_COMMON, BufferState::D3D12_RESOURCE_STATE_RENDER_TARGET, rt.get(), RTBufferType::ColorBuffer);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, rt.get(), RTBufferType::DepthBuffer);

	mRHI->ResetViewportsAndScissorRects(rt.get());
	mRHI->ClearRenderTarget(rt.get());
	mRHI->ClearDepthStencil(rt.get());
	mRHI->SetRTVAndDSV(rt.get());
#ifdef _RHI_DX12 
	auto Shader = mSceneRender.GetShader("BloomMix");
	auto Pipeline = mSceneRender.GetPipeline("MixPSO");
	auto Mesh = mTriangle;

	mRHI->SetRootSignature(Shader.get());
	mRHI->SetPSO(Pipeline.get());
	mRHI->InputAssetInfo(Mesh.get());

	auto otherrt = mSceneRender.GetRenderTarget(mCurrentColorBuffer);
	auto dxotherrt = dynamic_cast<DX12RenderTarget*>(otherrt.get());
	mRHI->BindDataTable(0, dxotherrt->GetOffset("RTSRV"), HeapType::CBV_SRV_UAV);
	

	auto minrt = mSceneRender.GetRenderTarget(OtherRt);
	auto dxminrt = dynamic_cast<DX12RenderTarget*>(minrt.get());
	mRHI->BindDataTable(1, dxminrt->GetOffset("RTSRV"), HeapType::CBV_SRV_UAV);
	auto size = dxminrt->mSize;
	mRHI->Bind32BitConstants(2, 2, &size, 0);
#endif
	auto indexCount = Mesh->IndexCount;
	mRHI->DrawMesh(indexCount);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_RENDER_TARGET, BufferState::D3D12_RESOURCE_STATE_COMMON, rt.get(), RTBufferType::ColorBuffer);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, rt.get(), RTBufferType::DepthBuffer);

	mRHI->EventEnd();
	mCurrentColorBuffer = "BloomMix";
}

void Render::ShowColorBufferPass()
{
	mRHI->EventBegin("-----ShowColorBuffer-----");
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_PRESENT, BufferState::D3D12_RESOURCE_STATE_RENDER_TARGET, nullptr, RTBufferType::ColorBuffer);

	mRHI->ResetViewportsAndScissorRects(nullptr);
	mRHI->ClearRenderTarget(nullptr);
	mRHI->ClearDepthStencil(nullptr);
	mRHI->SetRTVAndDSV(nullptr);
#ifdef _RHI_DX12 
	auto Shader = mSceneRender.GetShader("BloomToneMap");
	auto Pipeline = mSceneRender.GetPipeline("ToneMapPSO");
	auto Mesh = mTriangle;
	mRHI->SetRootSignature(Shader.get());
	mRHI->SetPSO(Pipeline.get());
	mRHI->InputAssetInfo(Mesh.get());

	auto otherrt = mSceneRender.GetRenderTarget(mCurrentColorBuffer);
	auto dxotherrt = dynamic_cast<DX12RenderTarget*>(otherrt.get());
	mRHI->BindDataTable(0, dxotherrt->GetOffset("RTSRV"), HeapType::CBV_SRV_UAV);
	auto size = dxotherrt->mSize;
	mRHI->Bind32BitConstants(2, 2, &size, 0);

#endif
	auto indexCount = Mesh->IndexCount;
	mRHI->DrawMesh(indexCount);
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_RENDER_TARGET, BufferState::D3D12_RESOURCE_STATE_PRESENT, nullptr, RTBufferType::ColorBuffer);
	mRHI->EventEnd();
	mCurrentColorBuffer = "Color";
}

void Render::TestTriangle()
{
	MeshVertexInfo info;
	info.mIndex = { 0,1,2 };
	info.mVertex = { glm::vec3{-1,1,0},glm::vec3{-1,-3,0},glm::vec3{3,1,0} };
	info.mTangent = { glm::vec4(0.0f), glm::vec4(0.0f), glm::vec4(0.0f) };
	info.mNormal = { glm::vec4(0.0f), glm::vec4(0.0f), glm::vec4(0.0f) };
	info.mTexCoord = { glm::vec2(0.0f), glm::vec2(0.0f), glm::vec2(0.0f) };

	auto test = std::make_shared<Mesh>();
	test->SetMeshInfo(info);

	mTriangle = mRHI->CreateMesh(test);
	mTriangle->IndexCount = 3;
}

void Render::ShadowPass()
{
	mRHI->EventBegin("-----ShadowPass-----");
	auto srt = mSceneRender.GetRenderTarget("Shadow");
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, srt.get(), RTBufferType::DepthBuffer);

	mRHI->ResetViewportsAndScissorRects(srt.get());
	mRHI->ClearDepthStencil(srt.get());
	mRHI->SetRTVAndDSV(srt.get());
	auto actors = mSceneRender.GetPrimitive();
	for (auto actor : actors)
	{
		auto Mesh = mSceneRender.GetMesh(actor->GetMeshName());
		if (Mesh == nullptr)
		{
			continue;
		}
		auto Pipeline = mSceneRender.GetPipeline("ShadowPSO");
		auto Shader = mSceneRender.GetShader("ShadowShader");
		if (Shader == nullptr)
		{
			continue;
		}
		mRHI->SetRootSignature(Shader.get());
		mRHI->SetPSO(Pipeline.get());
		mRHI->InputAssetInfo(Mesh.get());
#ifdef _RHI_DX12
		auto dp = dynamic_cast<DX12Primitive*>(actor.get());
		auto address = dp->GetCB()->Resource()->GetGPUVirtualAddress();
		mRHI->BindDataConstantBuffer(1, address);
#endif
		auto indexCount = Mesh->IndexCount;
		mRHI->DrawMesh(indexCount);
	}
	mRHI->ChangeResState(BufferState::D3D12_RESOURCE_STATE_DEPTH_WRITE, BufferState::D3D12_RESOURCE_STATE_GENERIC_READ, srt.get(), RTBufferType::DepthBuffer);

	mRHI->EventEnd();
}

RHI* Render::CreateRHI()
{
#ifdef _RHI_DX12
	return new DX12RHI();
#endif

#ifdef _RHI_OPENGL
	return new OPGLRHI();
#endif
}
