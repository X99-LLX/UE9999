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



void Render::Init()
{
	mRHI->ResetViewportsAndScissorRects();
	mRHI->OpenRtv();
	mRHI->ClearRTVAndDSV();
	mRHI->SetRTVAndDSV();
	mRHI->SetRootSignature();
}

void Render::Draw(std::vector<std::shared_ptr<Primitive>> primitives)
{
	Scene* mScene = Engine::GetEngine()->GetScene();
	Init();
	
	for (auto actor : primitives)
	{
		//mRHI->Update(mScene, actor.get());
		mRHI->DrawCall(actor.get());
	}
	mRHI->CloseRtv();
	mRHI->Swapchain();
}

void Render::DrawShadow(std::vector<std::shared_ptr<Primitive>> primitives)
{
	Scene* mScene = Engine::GetEngine()->GetScene();

	mRHI->InitDrawShadow();

	mRHI->UpdateShadowPassCB(*Engine::GetEngine()->GetTimer());

	for (auto actor : primitives)
	{
		mRHI->Update(mScene, actor.get());
		mRHI->DrawallShadow(actor.get());
	}
	mRHI->CloseShadowMapDsv();
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
