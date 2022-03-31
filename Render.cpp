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



void Render::BeginDraw()
{
	mRHI->ResetViewportsAndScissorRects();
	mRHI->OpenRtv();
	mRHI->ClearRTVAndDSV();
	mRHI->SetRTVAndDSV();
	mRHI->SetRootSignature();
}

void Render::UpdatePrimitiveTrans(std::vector<std::shared_ptr<Primitive>> primitives)
{
	Scene* mScene = Engine::GetEngine()->GetScene();
	for (auto actor : primitives)
	{
		mRHI->UpdateTrans(mScene, actor.get());
	}
}

void Render::DrawScene(std::vector<std::shared_ptr<Primitive>> primitives)
{
	UpdatePrimitiveTrans(primitives);
	ShadowPass(primitives);
	ColorPass(primitives);
}

void Render::ColorPass(std::vector<std::shared_ptr<Primitive>> primitives)
{
	BeginDraw();
	for (auto actor : primitives)
	{
		mRHI->DrawInstance(actor.get());
	}
	mRHI->CloseRtv();
	mRHI->Swapchain();
}

void Render::ShadowPass(std::vector<std::shared_ptr<Primitive>> primitives)
{
	mRHI->BeginDrawShadow();
	//test
	mRHI->UpdateLight(*Engine::GetEngine()->GetTimer());

	for (auto actor : primitives)
	{
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
