#include "stdafx.h"
#include "Render.h"
#include "DX12RHI.h"
#include "Engine.h"

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

void Render::BuildResource(std::vector<std::shared_ptr<Primitive>> RI)
{
	for (auto actor : RI)
	{
		mRHI->BuildGeo(actor.get());
	}
}






void Render::UpdateRenderData(std::vector<std::shared_ptr<Primitive>> primitives)
{
	Scene* mScene = Engine::GetEngine()->GetScene();
	for (auto actor : primitives)
	{
		mRHI->UpdateTrans(mScene, actor.get());
		mRHI->UpdateLight(*Engine::GetEngine()->GetTimer());
	}
}

void Render::DrawScene(std::vector<std::shared_ptr<Primitive>> primitives)
{
	mRHI->BeginFrame();
	UpdateRenderData(primitives);
	ShadowPass(primitives);
	BasePass(primitives);
	mRHI->EndFrame();
}

void Render::BasePass(std::vector<std::shared_ptr<Primitive>> primitives)
{
	mRHI->ResetViewportsAndScissorRects();
	mRHI->OpenRtv();
	mRHI->ClearRTVAndDSV();
	mRHI->SetRTVAndDSV();
	mRHI->SetRootSignature();
	for (auto actor : primitives)
	{
		mRHI->DrawInstance(actor.get());
	}
}

void Render::ShadowPass(std::vector<std::shared_ptr<Primitive>> primitives)
{
	mRHI->BeginDrawShadow();
	for (auto actor : primitives)
	{
		//mRHI->DrawInstance(actor.get());
		mRHI->DrawItemShadow(actor.get());
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
