#include "stdafx.h"
#include "Render.h"
#include "Engine.h"

#ifdef _RHI_DX12
#include "DX12RHI.h"
#include "DX12Primitive.h"
#include "DX12Texture.h"
#include "DX12Material.h"
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
	//³¢ÊÔ»æÖÆ
	mRHI->BeginFrame();
	UpdateRenderData();
	ShadowPass();
	BasePass();
	mRHI->EndFrame();
}

void Render::BasePass()
{
	mRHI->ResetViewportsAndScissorRects(RtType::BaseRt);
	mRHI->ChangeResState();
	mRHI->SetRTVAndDSV(RtType::BaseRt);
	auto actors = mSceneRender.GetPrimitive();
	for (auto actor : actors)
	{
		auto Mesh = mSceneRender.GetMesh(actor->GetMeshName());
		if (Mesh == nullptr)
		{
			continue;
		}
		auto Material = mSceneRender.GetMaterial(Mesh->GetMaterialName());
		auto Pipeline = mSceneRender.GetPipeline(Material->GetPipelineName());
		auto Shader = mSceneRender.GetShader(Pipeline->GetShaderName());
		if (Shader == nullptr)
		{
			continue;
		}
		auto Texture = mSceneRender.GetTexture(Material->GetTextures().at(0));
		if (Texture == nullptr)
		{
			continue;
		}
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
		auto dnt = dynamic_cast<DX12Texture*>(NorTex.get());
		mRHI->BindDataTable(4, dt->GetViewOffset(), HeapType::CBV_SRV_UAV);
		mRHI->BindDataTable(6, dnt->GetViewOffset(), HeapType::CBV_SRV_UAV);
		//must fix ,write first
		mRHI->BindDataTable(0, 0, HeapType::SAMPLER);

#endif 
		auto indexCount = Mesh->IndexCount;
		mRHI->DrawMesh(indexCount);
	}
}

void Render::ShadowPass()
{
	mRHI->ResetViewportsAndScissorRects(RtType::ShadowRt);
	mRHI->SetRTVAndDSV(RtType::ShadowRt);
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
